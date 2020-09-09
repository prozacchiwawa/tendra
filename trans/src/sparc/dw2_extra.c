/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <assert.h>
#include <stdio.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/exp.h>
#include <local/al.h>
#include <local/dw2_config.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/basicread.h>

#include <flpt/flpt.h>

#include <main/driver.h>
#include <main/flags.h>
#include <main/print.h>

#ifdef TDF_DIAG3
#include <diag3/dg_aux.h>
#endif
#ifdef TDF_DIAG4
#include <diag4/dg_aux.h>
#endif

#include <dwarf2/dw2_codes.h>
#include <dwarf2/dw2_basic.h>
#include <dwarf2/dw2_entries.h>
#include <dwarf2/dw2_locdata.h>

#include <refactor/const.h>

#include "procrec.h"
#include "addr.h"
#include "bits.h"
#include "reg.h"
#include "locate.h"
#include "translate.h"
#include "code_here.h"
#include "eval.h"
#include "proc.h"
#include "regable.h"
#include "regexps.h"
#include "dw2_extra.h"

extern int call_base_reg;	/* declared in locate.c */



#define retaddr_column 64
#define DIAG_FREG 32

long instr_count = -1;
long fde_count = -1;


static long cie_pointer;
static long fde_end;
static long proc_end;

static int calc_length;
static int extra_deref;
static int locate_param;
static int last_param_reg;
static int no_location;

static exp this_proc;

enum loctype { L_INREG = 1, L_REGOFF, L_GLOB, L_INDIRECT, L_SPLIT };

typedef struct {
  int key;
  int reg;
  long off;
} loc_s;



int dw_is_const(exp e)
{
  switch (e->tag) {
    case val_tag:
    case null_tag:
    case real_tag:
      return 1;
    case name_tag:
      if (isdiscarded(e))
	return 0;
      if (isvar(child(e)))
	return 1;
      if ( child(e)->props & defer_bit )
	return dw_is_const (child(child(e)));
      return 0;
#if 0
    case cont_tag:
      return child(e)->tag == name_tag && !isdiscarded(child(e)) &&
		!isvar(child(child(e))) && !isparam(child(child(e)));
#endif
    case reff_tag:
      return 1;
    default:
      return 0;
  }
}

exp dw_has_location(exp e)
{			/* return ident or NULL */
  switch (e->tag) {
    case name_tag: {
      if (isdiscarded(e) || isvar(child(e)))
	return NULL;
      if ( child(e)->props & defer_bit )
	return dw_has_location (child(child(e)));
      return child(e);
    }
    case cont_tag: {
      do {
	e = child(e);
	if (e->tag == name_tag && isdiscarded(e))
	  return NULL;
      }
      while (e->tag != ident_tag || (e->props & defer_bit));
      return e;
    }
    default:
      return NULL;
  }
}


static loc_s find_in_store(exp dc, long off)
{
  loc_s l;
  baseoff b;
  assert (! ( dc->props & defer_bit ));
  b = boff (dc);
  l.reg = b.base;
  l.off = off + b.offset;
  l.key = (isglob(dc) ? L_GLOB : L_REGOFF);
  return l;
}


static loc_s find_loc(exp e)
{
  loc_s l;
  switch ( e->tag ) {

    case name_tag : {
      if (isdiscarded(e) || (isglob(child(e)) && no(child(e)) == 0 &&
				!(nextg(child(e))->extnamed) )) {
	l.key = L_INREG;
	l.reg = 0;
	no_location = 1;
	return l;
      }
      if ( isvar(child(e)) )
	extra_deref--;
      if ( child(e)->props & defer_bit ) {
	l = find_loc (child(child(e)));
	l.off += (no(e)/8);
      }
      else
      if (locate_param) {
	int r = no(child(child(e)))/32;
	if (!isparam(child(e))) {
	  if (child(child(e))->tag == chvar_tag ||
		child(child(e))->tag == chfl_tag)
	    return find_loc (child(child(child(e))));
	  error(ERR_INTERNAL, "param inconsistency");
	}
	if (r > last_param_reg) {
	  l.key = L_REGOFF;
	  l.reg = R_SP;
	  l.off = (no(child(child(e))) + no(e))/8 + 68;
	}
	else
	if (shape_size(sh(child(child(e)))) > 32) {
	  l.key = L_SPLIT;
	  l.reg = r;
	}
	else {
	  l.key = L_INREG;
	  l.reg = r + R_O0;
	}
      }
      else
      if ( l.reg = regofval(e), l.reg >= 0 && l.reg < R_NO_REG ) {
	l.key = L_INREG;
      }
      else
      if ( l.reg = fregofval(e), l.reg >= 0 && l.reg < R_NO_REG ) {
	l.reg = ((l.reg << 1) + DIAG_FREG);
	l.key = L_INREG;
      }
      else
	l = find_in_store (child(e), no(e)/8);
      break;
    }

    case cont_tag :
    case contvol_tag : {
      if (child(e)->tag == name_tag && (isdiscarded(child(e)) ||
			(isglob(child(child(e))) && no(child(child(e))) == 0 &&
			 !(nextg(child(child(e)))->extnamed) ))) {
	l.key = L_INREG;
	l.reg = 0;
	no_location = 1;
	return l;
      }
      if ( child(e)->tag != name_tag || !isvar(child(child(e))) ) {
	l = find_loc (child(e));
	if (l.key == L_INREG) {
	  l.key = L_REGOFF;
	  l.off = 0;
	}
	else
	  l.key = L_INDIRECT;
      }
      else
      if ( child(child(e))->props & defer_bit ) {
	l = find_loc (child(child(child(e))));
	l.off += (no(child(e))/8);
      }
      else
      if (locate_param) {
	int r = no(child(child(child(e))))/32;
	if (!isparam(child(child(e)))) {
	  if (child(child(child(e)))->tag == chvar_tag ||
		child(child(child(e)))->tag == chfl_tag)
	    return find_loc (child(child(child(child(e)))));
	  error(ERR_INTERNAL, "param inconsistency");
	}
	if (r > last_param_reg) {
	  l.key = L_REGOFF;
	  l.reg = R_SP;
	  l.off = (no(child(child(child(e)))) + no(child(e)))/8 + 68;
	}
	else
	if (shape_size(sh(child(child(child(e))))) > 32) {
	  l.key = L_SPLIT;
	  l.reg = r;
	}
	else {
	  l.key = L_INREG;
	  l.reg = r + R_O0;
	}
      }
      else
      if ( l.reg = - regofval(child(e)), l.reg >= 0 ) {
	l.key = L_INREG;
      }
      else
      if ( l.reg = fregofval(child(e)), l.reg < R_NO_REG ) {
	l.reg = ((l.reg << 1) + DIAG_FREG);
	l.key = L_INREG;
      }
      else
	l = find_in_store (child(child(e)), no(child(e))/8);
      break;
    }

    case reff_tag : {
      l = find_loc (child(e));
      if (l.key == L_GLOB || l.key == L_REGOFF)
	l.off += (no(e)/8);
      else
      if (l.key == L_INREG) {
	l.key = L_REGOFF;
	l.off = (no(e)/8);
	extra_deref--;
      }
      else
	l.key = L_INDIRECT;
      break;
    }

    default:
      error(ERR_INTERNAL, "unimplemented location condition");
  }
  return l;
}

static int inreg_length(int r, int more)
{
  int ans = 1 + more;
  if (r >= 32)
    ans += uleb128_length((unsigned long)r);
  return ans;
}

static void out_inreg(int r, int more)
{
  if (!more) {
    if (r < 32)
      asm_printf("%d", DW_OP_reg0 + r);
    else {
      asm_printf("%d, ", DW_OP_regx); uleb128((unsigned long)r);
    }
  }
  else {
    if (r < 32)
      asm_printf("%d", DW_OP_breg0 + r);
    else {
      asm_printf("%d, ", DW_OP_bregx); uleb128((unsigned long)r);
    }
    asm_printf(", %d", 0);
  }
}

static int regoff_length(loc_s l)
{
  assert (l.reg >= 0 && l.reg < 32);
  return 1 + sleb128_length (l.off);
}

static void out_regoff(loc_s l)
{
  asm_printf("%d, ", l.reg == R_FP ? DW_OP_fbreg : DW_OP_breg0 + l.reg);
  sleb128 (l.off);
}

static int split_length(loc_s l)
{
  int ans = inreg_length (l.reg + R_O0, 0) +4;
  if (l.reg == last_param_reg) {
    l.key = L_REGOFF;
    l.off = (l.reg * 4) + 72;
    l.reg = R_SP;
    ans += regoff_length (l);
  }
  else
    ans += inreg_length (l.reg + R_O1, 0);
  return ans;
}

static void out_split(loc_s l)
{
  out_inreg (l.reg + R_O0, 0);
  asm_printf(", %d", DW_OP_piece);
  asm_printf(", %d, ", 4);
  if (l.reg == last_param_reg) {
    l.key = L_REGOFF;
    l.off = (l.reg * 4) + 72;
    l.reg = R_SP;
    out_regoff (l);
  }
  else
    out_inreg (l.reg + R_O1, 0);
  asm_printf(", %d", DW_OP_piece);
  asm_printf(", %d", 4);
}

static int glob_length(loc_s l)
{
  UNUSED(l);

  if (needs_debug_align) {
    calc_length = 0;
  }
  return 5;
}

static void out_glob(loc_s l)
{
  asm_printf("%d\n", DW_OP_addr);
  out32 (); outlab (l.reg);
  if (l.off) {
    asm_printf(" + %ld", l.off);
  }
}

static int indirect_length(exp e)
{
  int length;
  loc_s l;

  if (needs_debug_align && !calc_length)
    return 0;

  switch (e->tag) {
    case cont_tag: {
      length = 1;
      break;
    }
    case reff_tag: {
      if (no(e) >= 0)
	length = 1 + uleb128_length ((unsigned long)(no(e)/8));
      else
	length = 2 + uleb128_length ((unsigned long)(-no(e)/8));
      break;
    }
    case name_tag: {
      if (child(e)->props & defer_bit) {
	return indirect_length (child(child(e)));
      }
	  FALL_THROUGH;
    }
    default: {
      error(ERR_INTERNAL, "unimplemented dwarf locate");
      return 0;
    }
  }
  l = find_loc (child(e));
  switch (l.key) {
    case L_INREG: {
      length += inreg_length (l.reg, 1);
      break;
    }
    case L_REGOFF: {
      length += regoff_length(l);
      break;
    }
    case L_SPLIT: {
      length += split_length(l);
      break;
    }
    case L_GLOB: {
      length += glob_length(l);
      break;
    }
    case L_INDIRECT: {
      length += indirect_length(child(e));
      break;
    }
  }
  return length;
}

static void out_indirect(exp e)
{
  loc_s l;
  if (e->tag == name_tag) {
    assert (child(e)->props & defer_bit);
    out_indirect (child(child(e)));
    return;
  }
  l = find_loc (child(e));
  switch (l.key) {
    case L_INREG:    out_inreg(l.reg, 1);  asm_printf(", "); break;
    case L_REGOFF:   out_regoff(l);        asm_printf(", "); break;
    case L_SPLIT:    out_split(l);         asm_printf(", "); break;
    case L_GLOB:     out_glob(l); asm_printf("\n"); out8(); break;
    case L_INDIRECT: out_indirect(child(e)); asm_printf(", "); break;
  }
  switch (e->tag) {
    case cont_tag: {
      asm_printf("%d", DW_OP_deref);
      break;
    }
    case reff_tag: {
      if (no(e) >= 0) {
	asm_printf("%d, ", DW_OP_plus_uconst);
	uleb128 ((unsigned long)(no(e)/8));
      }
      else {
	asm_printf("%d, ", DW_OP_constu);
	uleb128 ((unsigned long)(-no(e)/8)); asm_printf(", ");
	asm_printf("%d", DW_OP_minus);
      }
      break;
    }
  }
}


void dw2_locate_exp(exp e, int locate_const, int cx)
{
  loc_s l;
  int length;
  int within_loclist = (cx & 1);
  long over_lab;

  if (needs_debug_align) {
    calc_length = 1;
  }

  locate_param = (cx & 2);
  extra_deref = locate_const;
  no_location = 0;
  l = find_loc (e);	/* may reduce extra_deref */
  length = extra_deref;
  switch (l.key) {
    case L_INREG:    length += inreg_length(l.reg, extra_deref); break;
    case L_REGOFF:   length += regoff_length(l);                 break;
    case L_SPLIT:    length += split_length(l);                  break;
    case L_GLOB:     length += glob_length(l);                   break;
    case L_INDIRECT: length += indirect_length(e);               break;
  }
  if (no_location)
    length = 0;

  if (within_loclist)
    out16 ();
  else
    out8 ();

  if (needs_debug_align && !calc_length) {
    over_lab = next_dwarf_label();
    out_dwf_label (over_lab, 0);
    if (within_loclist)
      asm_printf(" - . - 2");
    else
      asm_printf(" - . - 1");
  }
  else
    asm_printf("%d", length);
  asm_printf("\n");
  if (no_location)
    return;
  out8 ();
  switch (l.key) {
    case L_INREG:    out_inreg(l.reg, extra_deref); break;
    case L_REGOFF:   out_regoff(l);                 break;
    case L_SPLIT:    out_split(l);                  break;
    case L_GLOB:     out_glob(l);                   break;
    case L_INDIRECT: out_indirect(e);               break;
  }
  while (extra_deref) {
    if (extra_deref < 0) {
      error(ERR_INTERNAL, "miscalculated location");
      break;
    }
#if 0
    if (locate_const)
      error(ERR_INTERNAL, "constant location???");
#endif
    asm_printf(", %s", DW_OP_deref);
    extra_deref--;
  }
  asm_printf("\n");

  if (needs_debug_align && !calc_length)
    out_dwf_label (over_lab, 1);
}

static long current_pprops;

void dw2_prepare_locate(exp id)
{
			/* set local proc conditions for local locations */
  exp e = child(id);	/* proc or general proc */
  procrec *pr = &procrecs [ no ( e ) ] ;
  needs *ndpr = &pr->needsproc ;
  spacereq *sppr = &pr->spacereqproc ;
  long pprops = ( long ) ( ndpr->propneeds ) ;
  /* bool leaf = ( bool ) ( ( pprops & anyproccall ) == 0 ) ;	*/
  long maxargs = ndpr->maxargs ;/* maxargs of proc body in bits */
  long st = sppr->stack ;		/* space for locals in bits */

  Has_vcallees = (e->tag == general_proc_tag) && proc_has_vcallees(e);
	/* callee_start_reg as initialisation */
  call_base_reg = R_NO_REG;	/* needs reset at apply_general */
  proc_state.params_offset = PARAMS_OFFSET;
  st = ( st + 63 ) & ~63 ;
  proc_state.callee_size = ndpr->callee_size;
  proc_state.locals_space = st ;
  proc_state.locals_offset = 0 ;
  proc_state.frame_size = maxargs + st ;
  proc_state.maxargs = maxargs ;
  last_param_reg = 5;
  if (proc_may_have_callees(e))
    last_param_reg = (Has_vcallees ? 3 : 4);
  this_proc = e;
  current_pprops = pprops;
}


void dw2_locate_result(shape sha)
{
  out8 ();
  if ( !valregable (sha) && sha->tag != tophd &&
	( !is_floating ( sha->tag ) || shape_size(sha) > 64 )) {
      /* structure or union result, address of space to [ %fp+64 ] */
    loc_s l;
    l.key = L_REGOFF;
    l.reg = R_FP;
    l.off = 64;
    asm_printf("%d, ", regoff_length (l));
    out_regoff (l);
  }
  else {
    int r;
    if ( is_floating ( sha->tag ) ) {
	/* proc has real result */
      r = R_F0 + DIAG_FREG;
    }
    else if ( sha->tag > tophd  ) {
      r = R_I0;
    }
    else {
	/* no result */
      error(ERR_INTERNAL, "inconsistent result");
      r = R_G0;
    }
    asm_printf("%d, ", inreg_length (r, 0));
    out_inreg (r, 0);
  }
  asm_printf("\n");
}

void dw_at_procdetails(void)
{			/* return address and frame base */
  out8(); asm_printf("%d, ", 2);
  asm_printf("%d, ", DW_OP_breg0 + R_I7);
  asm_printf("%d\n", has_struct_res (this_proc) ? 12 : 8);
  out8(); asm_printf("%d, ", 1);
  asm_printf("%d\n", DW_OP_reg0 + R_SP);
}


void dw2_locate_val(dg_where v)
{
  long over_lab;

  if (needs_debug_align) {
    calc_length = 1;
  }

  out8 ();
  switch (v.k) {
    case WH_CODELAB: {
      int length;
      loc_s l;
      l.key = L_GLOB;
      l.reg = v.u.l;
      l.off = v.o;
      length = glob_length (l);
      if (needs_debug_align && !calc_length) {
	over_lab = next_dwarf_label();
	out_dwf_label (over_lab, 0);
	asm_printf(" - . - 1");
      }
      else
        asm_printf("%d", length);
      asm_printf("\n");
      out8 ();
      out_glob (l);
      break;
    }
    case WH_REG: {
      int r = v.u.l;
      asm_printf("%d, ", inreg_length (r, 0));
      out_inreg (r, 0);
      break;
    }
    case WH_REGOFF: {
      loc_s l;
      l.key = L_REGOFF;
      l.reg = v.u.l;
      l.off = v.o;
      asm_printf("%d, ", regoff_length (l));
      out_regoff (l);
      break;
    }
    default:
      error(ERR_INTERNAL, "unexpected locate val");
  }
  asm_printf("\n");

  if (needs_debug_align && !calc_length)
    out_dwf_label (over_lab, 1);
}


static int dw_eval_exp(exp e, int line_started)
{
  if (line_started)
    asm_printf(", ");
  else {
    out8 ();
    line_started = 1;
  }
  switch (e->tag) {
    case name_tag:
    case cont_tag:
    case contvol_tag:
    case reff_tag: {
      loc_s l;
      locate_param = extra_deref = no_location = 0;
      l = find_loc (e);
      if (no_location || extra_deref)
	error(ERR_INTERNAL, "value unobtainable by DWARF expression");
      switch (l.key) {
	case L_INREG:    out_inreg(l.reg, extra_deref);            break;
	case L_REGOFF:   out_regoff(l);                            break;
	case L_GLOB:     out_glob(l); asm_printf("\n"); line_started = 0; break;
	case L_INDIRECT: out_indirect(e);                          break;
      }
      break;
    }
    case val_tag:
    case null_tag : {
      if (isbigval(e)) {
	flt64 x;
	int ov;
	x = flt_to_f64(no(e), is_signed(sh(e)), &ov);
	asm_printf("%d\n", is_signed(sh(e)) ? DW_OP_const8s : DW_OP_const8u);
	out32(); asm_printf("%d, %d\n", x.small, x.big);
	line_started = 0;
      }
      else
      if (no(e) >= 0 && no(e) < 32)
	asm_printf("%ld", DW_OP_lit0 + no(e));
      else
      if (is_signed(sh(e))) {
	asm_printf("%d, ", DW_OP_consts);
	sleb128 (no(e));
      }
      else {
	asm_printf("%d, ", DW_OP_constu);
	uleb128 ((unsigned long)no(e));
      }
      break;
    }
    case plus_tag:
    case offset_add_tag : {
      line_started = dw_eval_exp (child(e), line_started);
      if (next(child(e))->tag == val_tag && !is_signed(sh(e)) && !isbigval(next(child(e)))) {
	if (line_started)
	  asm_printf(", ");
	else {
	  out8 ();
	  line_started = 1;
	}
	asm_printf("%d, ", DW_OP_plus_uconst);
	uleb128 ((unsigned long)no(e));
      }
      else {
	line_started = dw_eval_exp (next(child(e)), line_started);
	if (line_started)
	  asm_printf(", ");
	else {
	  out8 ();
	  line_started = 1;
	}
	asm_printf("%d", DW_OP_plus);
      }
      break;
    }
    case minus_tag:
    case offset_subtract_tag : {
      line_started = dw_eval_exp (child(e), line_started);
      line_started = dw_eval_exp (next(child(e)), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8 ();
	line_started = 1;
      }
      asm_printf("%d", DW_OP_minus);
      break;
    }
    case neg_tag:
    case offset_negate_tag : {
      line_started = dw_eval_exp (child(e), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8 ();
	line_started = 1;
      }
      asm_printf("%d", DW_OP_neg);
      break;
    }
    case mult_tag:
    case offset_mult_tag : {
      line_started = dw_eval_exp (child(e), line_started);
      line_started = dw_eval_exp (next(child(e)), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8 ();
	line_started = 1;
      }
      asm_printf("%d", DW_OP_mul);
      break;
    }
    case div0_tag :
    case div1_tag :
    case div2_tag :
    case offset_div_by_int_tag :
    case offset_div_tag : {
      line_started = dw_eval_exp (child(e), line_started);
      line_started = dw_eval_exp (next(child(e)), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8 ();
	line_started = 1;
      }
      asm_printf("%d", DW_OP_div);
      break;
    }
    default:
      error(ERR_INTERNAL, "unsupported operation for DWARF expression");
  }
  return line_started;
}


void dw2_offset_exp(exp e)
{
  long block_end = next_dwarf_label ();
  if (sh(e)->tag != offsethd)
    error(ERR_INTERNAL, "wrong shape for offset expression");
  dw_at_form (DW_FORM_block2); asm_printf("\n");
  out16 (); out_dwf_dist_to_label (block_end); asm_printf("\n");
  if (dw_eval_exp (e, 0))
    asm_printf("\n");
  if (sh(e)->tag == offsethd && al2(sh(e)) < 8 ) {
    out8 (); asm_printf("%d, %d\n", DW_OP_lit0 + 8, DW_OP_mul);
  }
  out_dwf_label (block_end, 1);
}


void dw2_cie(void)
{
  long cie_end;
  int i;
  cie_pointer = next_dwarf_label();
  cie_end = next_dwarf_label();
  enter_section ("debug_frame");
  asm_commentln("Common Information Entry");
  out_dwf_label (cie_pointer, 1);
  out32 (); out_dwf_dist_to_label (cie_end); asm_printf("\n");
  out32 (); asm_printf("%d\n", DW_CIE_id);
  out8 (); asm_printf("%d\n", DW_CIE_MOD_VERSION);
  out_string ("DERA/DDC-I");
  out8 (); uleb128 ((unsigned long)framecode_factor); asm_printf("\n");
  out8 (); sleb128 (framedata_factor); asm_printf("\n");
  out8 (); asm_printf("%d\n", retaddr_column);	/* return address column */

  out8 (); asm_printf("%d, ", DW_CFA_DD_sparc_restore_regwindow);
	sleb128 (8); asm_printf("\n");		/* sparc entry rules, ret offset 8 */

  for (i = R_FIRST; i <= R_LAST; i++) {
    if ((i > R_G0 && i <= (R_G0 + g_reg_max)) ||
		(i >= R_O0 && i <= R_O7 && i != R_SP)) {
	out8 (); asm_printf("%d, ", DW_CFA_undefined);
	uleb128 ((unsigned long)i); asm_printf("\n");
    }
  }
  dot_align (PTR_SZ/8);
  out_dwf_label (cie_end, 1);
  exit_section ();
}


void dw2_start_fde(exp e)
{
  long proc_start = next_dwarf_label();
  fde_end = next_dwarf_label();
  proc_end = next_dwarf_label();
  out_dwf_label (proc_start, 1);
  enter_section ("debug_frame");
  asm_commentln("Frame Descriptor Entry");
  out32 (); out_dwf_dist_to_label (fde_end); asm_printf("\n");
  out32 (); out_dwf_label (cie_pointer, 0); asm_printf("\n");
  out32 (); out_dwf_label (proc_start, 0); asm_printf("\n");
  out32 (); out_dwf_labdiff (proc_start, proc_end); asm_printf("\n");
  this_proc = e;
  if (has_struct_res (this_proc)) {
    out8 (); asm_printf("%d, ", DW_CFA_DD_location);
	uleb128 ((unsigned long)retaddr_column);  asm_printf(", ");
	asm_printf("%d, ", 2 + sleb128_length((long)12));
	asm_printf("%d, ", DW_OP_drop);
	asm_printf("%d, ", DW_OP_breg0 + R_O7);
	sleb128 ((long) 12); asm_printf("\n");	/* return R_O7 offset 12 */
  }
  exit_section ();
  instr_count = -1;
  fde_count = 0;
}

static void fde_advance(long here)
{
  if (fde_count < 0) {
    out8 (); asm_printf("%d\n", DW_CFA_set_loc);
    out32 (); out_dwf_label (here, 0);
  }
  else
  if (fde_count < 0x40) {
    out8 (); asm_printf("%ld", DW_CFA_advance_loc + fde_count);
  }
  else
  if (fde_count < 0x100) {
    out8 (); asm_printf("%d, ", DW_CFA_advance_loc1);
    asm_printf("%ld", fde_count);
  }
  else
  if (fde_count < 0x10000) {
    out8 (); asm_printf("%d\n", DW_CFA_advance_loc2);
    out16 (); asm_printf("%ld", fde_count);
  }
  else {
    out8 (); asm_printf("%d\n", DW_CFA_advance_loc4);
    out32 (); asm_printf("%ld", fde_count);
  }
  asm_printf("\n");
  fde_count = 0;
}

void dw2_fde_save(void)
{
  long here = 0;
  if (fde_count < 0)
    here = set_dw_text_label();
  enter_section ("debug_frame");
  fde_advance (here);
  out8 (); asm_printf("%d, ", DW_CFA_DD_sparc_save_regwindow);
	sleb128 ((long) (has_struct_res (this_proc) ? 12 : 8));
	asm_printf("\n");
  exit_section ();
}

void dw2_complete_fde(void)
{
  out_dwf_label (proc_end, 1);
  enter_section ("debug_frame");
  dot_align (PTR_SZ/8);
  out_dwf_label (fde_end, 1);
  exit_section ();
}


static exp lab_mark_list;

static void mark_lab(exp labst)
{
  if (!dg_labmark (labst)) {
    set_dg_labmark (labst);
    if (child(child(labst)) != NULL)
      error(ERR_INTERNAL, "strange labst");
    child(child(labst)) = lab_mark_list;
    lab_mark_list = labst;
  }
}

static void trace_branch_aux(exp whole, exp e)
{
  exp t;
  switch (e->tag) {
    case test_tag:
    case goto_tag: {
      if (!intnl_to (whole, pt(e)))
	mark_lab (pt(e));
      break;
    }
    case case_tag: {
      t = next(child(e));
      for (;;) {
	if (!intnl_to (whole, pt(t)))
	  mark_lab (pt(t));
	if (t->last) break;
	t = next(t);
      }
      break;
    }
    case name_tag:
    case env_offset_tag:
    case general_env_offset_tag:
      return;
  }
  t = child(e);
  if (t) {
    for (;;) {
      trace_branch_aux (whole, t);
      if (t->last || e->tag == case_tag) break;
      t = next(t);
    }
  }
}

void trace_dw_branch_exits(exp e)
{
  lab_mark_list = NULL;
  trace_branch_aux (e, e);
  while (lab_mark_list) {
    exp holder = child(lab_mark_list);
    clear_dg_labmark (lab_mark_list);
    lab_mark_list = child(holder);
    child(holder) = NULL;
    dw_entry (dwe_break, 0);
    out32 (); out_code_label (no(holder)); asm_printf("\n");
  }
}


int dw_loc_equivalence(exp a, exp b)
{
  return (int) sim_exp(a, b);
}

typedef struct
{
  dg_name	alloc;
  void *	share_set;
  dg_name	nm;
  long		start;
  long		end;
} dw_regdata;

#define TRACKREGS 32

static dw_regdata regassns [TRACKREGS];


void dw_allocated(dg_name nm, exp id)
{
  int reg = no(id), i;
  exp x = child(nm->data.n_obj.obtain_val);
  if (!isglob(id) && (id->props & inreg_bits) && reg < TRACKREGS) {
    dw_close_regassn (reg);
    regassns[reg].alloc = nm;
    regassns[reg].share_set = NULL;
  }
  for (i=0; i<TRACKREGS; i++) {
    if (regexps[i].keptexp && 
	(regexps[i].iscont ? (x->tag == cont_tag &&
			sim_exp (child(x), regexps[i].keptexp))
		      : sim_exp (x, regexps[i].keptexp) )) {
      regassns[i].nm = nm;
      regassns[i].start = set_dw_text_label ();
      regassns[i].end = 0;
      break;
    }
  }
}

void dw_deallocated(dg_name nm)
{
  int i;
  for (i=0; i<TRACKREGS; i++) {
    if (regassns[i].alloc == nm) {
      dw_close_regassn (i);
      regassns[i].alloc = NULL;
      regassns[i].share_set = NULL;
    }
  }
}

void dw_all_deallocated(void)		/* initialisation */
{
  int i;
  for (i=0; i<TRACKREGS; i++) {
    regassns[i].alloc = NULL;
    regassns[i].share_set = NULL;
    regassns[i].start = regassns[i].end = 0;
  }
}


void dw_init_regassn(int reg)
{
  if (reg < TRACKREGS) {
    dg_name nm = find_equiv_object (regexps[reg].keptexp,
					regexps[reg].iscont);
    if (nm) {
      regassns[reg].nm = nm;
      regassns[reg].start = set_dw_text_label ();
      regassns[reg].end = 0;
    }
  }
}

void dw_used_regassn(int reg)
{
  if (reg < TRACKREGS && regassns[reg].start)
    regassns[reg].end = set_dw_text_label ();
}

void dw_close_regassn(int reg)
{
  if (reg >= TRACKREGS)
    return;
  if (regassns[reg].end) {
    if (!regassns[reg].share_set) {
      regassns[reg].share_set = (void *)
		dw_new_regshare (regassns[reg].alloc, reg);
    }
    dw_add_regshare (regassns[reg].share_set, regassns[reg].nm,
			regassns[reg].start, regassns[reg].end);
    regassns[reg].end = 0;
    if (!regassns[reg].alloc)
      regassns[reg].share_set = NULL;
  }
  regassns[reg].start = 0;
}

