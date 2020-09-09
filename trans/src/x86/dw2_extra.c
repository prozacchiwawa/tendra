/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <reader/exp.h>

#include <local/al.h>
#include <local/code.h>
#include <local/dw2_config.h>
#include <local/exp.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/basicread.h>
#include <refactor/const.h>

#include <construct/machine.h>
#include <construct/reg_result.h>
#include <construct/label_ops.h>

#include <flpt/flpt.h>

#include <main/flags.h>
#include <main/print.h>

#ifdef TDF_DIAG3
#include <diag3/dg_aux.h>
#include <diag3/dg_types.h>
#endif
#ifdef TDF_DIAG4
#include <diag4/dg_aux.h>
#include <diag4/dg_types.h>
#endif

#include <dwarf2/dw2_codes.h>
#include <dwarf2/dw2_entries.h>
#include <dwarf2/dw2_basic.h>
#include <dwarf2/dw2_info.h>
#include <dwarf2/dw2_locdata.h>

#include "operand.h"
#include "ops.h"
#include "instr.h"
#include "dw2_extra.h"
#include "operand.h"
#include "reg_record.h"
#include "make_code.h"


extern int locals_offset; /* declared in cproc.c */


static long dwarfreg[8] = {0, 2, 1, 3, 7, 6, 5, 4};
#define dw_sp 4
#define dw_fp 5
#define retaddr 8

static long cie_pointer;
static long fde_end;
static long proc_end;

enum loctype { L_INREG = 1, L_REGOFF, L_GLOB, L_INDIRECT };

typedef struct {
  enum loctype key;
  int reg;
  char* s;
  long off;
} loc_s;

static int extra_deref;
static int locate_param;
static int has_fp;
static int no_location;




int dw_is_const
(exp e)
{
  switch (e->tag) {
    case val_tag:
    case null_tag:
    case real_tag:
      return 1;
    case name_tag:
      return !isdiscarded(e) && isvar(child(e));
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

exp dw_has_location
(exp e)
{
  switch (e->tag) {
    case name_tag: {
      if (isdiscarded(e) || isvar(child(e)))
	return NULL;
      return child(e);
    }
    case cont_tag: {
      do {
	e = child(e);
	if (e->tag == name_tag && isdiscarded(e))
	  return NULL;
      }
      while (e->tag!= ident_tag);
      return e;
    }
    default:
      return NULL;
  }
}


static loc_s name_to_loc(exp e);

static loc_s find_param
(exp e)
{
  switch (e->tag) {
    case name_tag:
      if (isvar(child(e)))
	extra_deref--;
      if (isparam(child(e)))
        return name_to_loc(e);
      return find_param(child(child(e)));
    case cont_tag:
      extra_deref++;
	  FALL_THROUGH;
    case chvar_tag:
    case chfl_tag:
      return find_param(child(e));
    default:
      break;
  }
  error(ERR_INTERNAL, "parameter inconsistency");
  return find_param(e);
}

static loc_s name_to_loc
(exp e)
{
  loc_s l;
  exp id = child(e);
  long n_off = no(e) /8;
  if (locate_param && !isparam(id)) {
    return find_param(child(id));
#if 0
    if (child(id)->tag == name_tag && isloadparam(child(id)) && isparam(child(child(id))))
      return name_to_loc(child(id));
    if (child(id)->tag == cont_tag && child(child(id))->tag == name_tag &&
		isparam(child(child(child(id)))))
      return name_to_loc(child(child(id)));
    error(ERR_INTERNAL, "parameter inconsistency");
#endif
  }
  if (isglob(id)) {
    l.key = L_GLOB;
    l.s = nextg(id) -> name;
    l.off = n_off;
  }
  else {
    switch (ptno(id)) {
      case local_pl: {
	l.key = L_REGOFF;
	l.reg = -1;
	l.off = (no(id) /8) + n_off;
	if (id->tag)
	  l.off -= locals_offset;
	else {			/* env_off modification */
	  if (locate_param) {
	    l.reg = dw_sp;
	  }
	}
	if (has_fp && l.reg < 0)
	  l.off += 4;
	break;
      }
      case callstack_pl: {
	error(ERR_INTERNAL, "callstack arg - location list needed");
	break;
      }
      case par_pl: {
	l.key = L_REGOFF;
	l.reg = (locate_param ? dw_sp : -1);
	l.off = (no(id) /8) +4 + n_off;
	break;
      }
      case reg_pl: {
	int z = get_reg_no(no(id));
	l.key = L_INREG;
	l.reg = (z >= first_fl_reg ? 100 + z - first_fl_reg :
					(int)dwarfreg[z]);
	break;
      }
      default:
	error(ERR_INTERNAL, "no location for id");
	SET(l);
    }
    if (has_fp && l.reg < 0)
      l.off += 4;
  }
  return l;
}

static loc_s find_loc
(exp e)
{
  loc_s l;
  switch (e->tag) {

    case name_tag: {
      if (isdiscarded(e) || (isglob(child(e)) && no(child(e)) == 0 &&
				!(nextg(child(e)) ->extnamed))) {
	l.key = L_INREG;
	l.reg = 0;
	no_location = 1;
	return l;
      }
      if (isvar(child(e)))
	extra_deref--;
      return name_to_loc(e);
    }

    case cont_tag:
    case contvol_tag: {
      if (child(e)->tag == name_tag) {
	if (isdiscarded(child(e)) ||
			(isglob(child(child(e))) && no(child(child(e))) == 0 &&
			 !(nextg(child(child(e))) ->extnamed))) {
	  l.key = L_INREG;
	  l.reg = 0;
	  no_location = 1;
	  return l;
	}
	if (isvar(child(child(e))))
	  return name_to_loc(child(e));
      }
      l = find_loc(child(e));
      if (l.key == L_INREG) {
	l.key = L_REGOFF;
	l.off = 0;
      }
      else
	l.key = L_INDIRECT;
      break;
    }

    case reff_tag: {
      l = find_loc(child(e));
      if (l.key == L_GLOB || l.key == L_REGOFF)
	l.off += (no(e) /8);
      else
      if (l.key == L_INREG) {
	l.key = L_REGOFF;
	l.off = (no(e) /8);
	extra_deref--;
      }
      else
	l.key = L_INDIRECT;
      break;
    }

    case chvar_tag: {
      l = find_loc(child(e));
      break;
    }

    default:
      error(ERR_INTERNAL, "unimplemented location condition");
      SET(l);
  }
  return l;
}

static int inreg_length
(int r, int more)
{
  int ans = 1 + more;
  if (r >= 32)
    ans += uleb128_length((unsigned long)r);
  return ans;
}

static void out_inreg
(int r, int more)
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

static int regoff_length
(loc_s l)
{
  return 1 + sleb128_length(l.off);
}

static void out_regoff
(loc_s l)
{
  asm_printf("%d, ", l.reg < 0 ? DW_OP_fbreg : DW_OP_breg0 + l.reg);
  sleb128(l.off);
}

static int glob_length
(loc_s l)
{
  UNUSED(l);
  return 5;
}

static void out_glob
(loc_s l)
{
  asm_printf("%d\n", DW_OP_addr);
  out32(); asm_printf("%s", l.s);
  if (l.off) {
    asm_printf(" + %ld", l.off);
  }
}

static int indirect_length
(exp e)
{
  int length;
  loc_s l;
  switch (e->tag) {
    case cont_tag: {
      length = 1;
      break;
    }
    case reff_tag: {
      if (no(e) >= 0)
	length = 1 + uleb128_length((unsigned long)(no(e) /8));
      else
	length = 2 + uleb128_length((unsigned long)(-no(e) /8));
      break;
    }
    default: {
      error(ERR_INTERNAL, "unimplemented dwarf locate");
      return 0;
    }
  }
  l = find_loc(child(e));
  switch (l.key) {
    case L_INREG: {
      length += inreg_length(l.reg, 1);
      break;
    }
    case L_REGOFF: {
      length += regoff_length(l);
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

static void out_indirect
(exp e)
{
  loc_s l;
  l = find_loc(e);
  switch (l.key) {
    case L_INREG: {
      out_inreg(l.reg, 1);
      asm_printf(", ");
      break;
    }
    case L_REGOFF: {
      out_regoff(l);
      asm_printf(", ");
      break;
    }
    case L_GLOB: {
      out_glob(l);
      asm_printf("\n"); out8();
      break;
    }
    case L_INDIRECT: {
      out_indirect(child(e));
      asm_printf(", ");
      break;
    }
  }
  switch (e->tag) {
    case cont_tag: {
      asm_printf("%d", DW_OP_deref);
      break;
    }
    case reff_tag: {
      if (no(e) >= 0) {
	asm_printf("%d, ", DW_OP_plus_uconst);
	uleb128((unsigned long)(no(e) /8));
      }
      else {
	asm_printf("%d, ", DW_OP_constu);
	uleb128((unsigned long)(-no(e) / 8));
	asm_printf(", %d", DW_OP_minus);
      }
      break;
    }
  }
}


void dw2_locate_exp
(exp e, int locate_const, int cx)
{
  loc_s l;
  int length;
  int within_loclist = (cx & 1);
  locate_param = (cx & 2);
  extra_deref = locate_const;
  no_location = 0;
  l = find_loc (e);	/* may reduce extra_deref */
  length = extra_deref;
  switch (l.key) {
    case L_INREG: {
      length += inreg_length(l.reg, extra_deref);
      break;
    }
    case L_REGOFF: {
      length += regoff_length(l);
      break;
    }
    case L_GLOB: {
      length += glob_length(l);
      break;
    }
    case L_INDIRECT: {
      length += indirect_length(e);
      break;
    }
  }
  if (no_location)
    length = 0;

  if (within_loclist) {
    out16();
    asm_printf("%d\n", length);
    if (no_location)
      return;
    out8();
  }
  else {
    out8();
    asm_printf("%d", length);
    if (no_location) {
      asm_printf("\n");
      return;
    }
    asm_printf(", ");
  }
  switch (l.key) {
    case L_INREG: {
      out_inreg(l.reg, extra_deref);
      break;
    }
    case L_REGOFF: {
      out_regoff(l);
      break;
    }
    case L_GLOB: {
      out_glob(l);
      break;
    }
    case L_INDIRECT: {
      out_indirect(e);
      break;
    }
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
    asm_printf(", %d", DW_OP_deref);
    extra_deref--;
  }
  asm_printf("\n");
}


void dw2_prepare_locate
(exp id)
{
			/* set local proc conditions for local locations */
  exp p = child(id);	/* proc or general proc */
  locals_offset = no(p);
  has_fp = proc_has_fp(p);
}

void dw_at_procdetails
(void)
{
			/* return address and frame base */
  out8(); asm_printf("%d, %d, ", 2, DW_OP_fbreg);
  if (has_fp) {
    asm_printf("%d\n", 4);
    out8(); asm_printf("%d, ", 1);
    asm_printf("%d\n", DW_OP_reg0+dw_fp);
  }
  else {
    asm_printf("%d\n", 0);
    out8(); asm_printf("%d\n", 0);
  }
}


void dw2_locate_result
(shape sha)
{
  long length = 1;
  int reg = 0;
  int indirect = 0;
  int multi = 0;
  if (is_floating(sha->tag)) {
    reg = 100;
    length = inreg_length(reg, 0);
  }
  else
  if (!reg_result(sha)) {
    indirect = 1;
    length = 2;
  }
  else
  if (shape_size(sha) > 32) {
    multi = 1;
    length = 6;
  }
  out8(); asm_printf("%ld, ", length);
  if (multi) {
    asm_printf("%d, ", DW_OP_reg0);
    asm_printf("%d, %d, ", DW_OP_piece, 4);
    asm_printf("%d, ", DW_OP_reg0+2);
    asm_printf("%d, %d", DW_OP_piece, 4);
  }
  else
  if (indirect) {
    asm_printf("%d, %d", DW_OP_breg0, 0);
  }
  else {
    out_inreg(reg, 0);
  }
  asm_printf("\n");
}


void dw2_locate_val
(dg_where v)
{
  out8();
  switch (v.k) {
    case NO_WH: {
      asm_printf("%d", 0);
      break;
    }
    case WH_STR: {
      loc_s l = { 0 };
      l.key = L_GLOB;
      l.s = v.u.s;
      l.off = v.o;
      asm_printf("%d, ", glob_length(l));
      out_glob(l);
      break;
    }
    case WH_REG: {
      int r = (int)dwarfreg[v.u.l];
      asm_printf("%d, ", inreg_length(r, 0));
      out_inreg(r, 0);
      break;
    }
    case WH_REGOFF: {
      loc_s l;
      l.key = L_REGOFF;
      l.reg = (int)v.u.l;
      l.off = v.o;
      if (l.reg == -2) {
	l.reg = -1;
	l.off -= locals_offset;
      }
      asm_printf("%d, ", regoff_length(l));
      out_regoff(l);
      break;
    }
    default:
      error(ERR_INTERNAL, "unexpected locate val");
  }
  asm_printf("\n");
}


static int dw_eval_exp
(exp e, int line_started)
{
  if (line_started)
    asm_printf(", ");
  else {
    out8();
    line_started = 1;
  }
  switch (e->tag) {
    case name_tag:
    case cont_tag:
    case contvol_tag:
    case reff_tag: {
      loc_s l;
      locate_param = extra_deref = no_location = 0;
      l = find_loc(e);
      if (no_location || extra_deref)
	error(ERR_INTERNAL, "value unobtainable by DWARF expression");
      switch (l.key) {
	case L_INREG: {
	  out_inreg(l.reg, extra_deref);
	  break;
	}
	case L_REGOFF: {
	  out_regoff(l);
	  break;
	}
	case L_GLOB: {
	  out_glob(l);
	  asm_printf("\n");
	  line_started = 0;
	  break;
	}
	case L_INDIRECT: {
	  out_indirect(e);
	  break;
	}
      }
      break;
    }
    case val_tag:
    case null_tag: {
      if (isbigval(e)) {
	flt64 x;
	int ov;
	x = flt_to_f64(no(e), is_signed(sh(e)), &ov);
	asm_printf("%d\n", is_signed(sh(e))? DW_OP_const8s : DW_OP_const8u);
	out32(); asm_printf("%d, %d\n", x.small, x.big);
	line_started = 0;
      }
      else
      if (no(e) >= 0 && no(e) < 32)
	asm_printf("%ld", DW_OP_lit0 + no(e));
      else
      if (is_signed(sh(e))) {
	asm_printf("%d, ", DW_OP_consts);
	sleb128((long)no(e));
      }
      else {
	asm_printf("%d, ", DW_OP_constu);
	uleb128((unsigned long)no(e));
      }
      break;
    }
    case plus_tag:
    case offset_add_tag: {
      line_started = dw_eval_exp(child(e), line_started);
      if (next(child(e))->tag == val_tag && !is_signed(sh(e)) && !isbigval(next(child(e)))) {
	if (line_started)
	  asm_printf(", ");
	else {
	  out8();
	  line_started = 1;
	}
	asm_printf("%d, ", DW_OP_plus_uconst);
	uleb128((unsigned long)no(e));
      }
      else {
	line_started = dw_eval_exp(next(child(e)), line_started);
	if (line_started)
	  asm_printf(", ");
	else {
	  out8();
	  line_started = 1;
	}
	asm_printf("%d", DW_OP_plus);
      }
      break;
    }
    case minus_tag:
    case offset_subtract_tag: {
      line_started = dw_eval_exp(child(e), line_started);
      line_started = dw_eval_exp(next(child(e)), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8();
	line_started = 1;
      }
      asm_printf("%d", DW_OP_minus);
      break;
    }
    case neg_tag:
    case offset_negate_tag: {
      line_started = dw_eval_exp(child(e), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8();
	line_started = 1;
      }
      asm_printf("%d", DW_OP_neg);
      break;
    }
    case mult_tag:
    case offset_mult_tag: {
      line_started = dw_eval_exp(child(e), line_started);
      line_started = dw_eval_exp(next(child(e)), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8();
	line_started = 1;
      }
      asm_printf("%d", DW_OP_mul);
      break;
    }
    case div0_tag:
    case div1_tag:
    case div2_tag:
    case offset_div_by_int_tag:
    case offset_div_tag: {
      line_started = dw_eval_exp(child(e), line_started);
      line_started = dw_eval_exp(next(child(e)), line_started);
      if (line_started)
	asm_printf(", ");
      else {
	out8();
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


void dw2_offset_exp
(exp e)
{
  long block_end = next_dwarf_label();
  if (sh(e)->tag!= offsethd)
    error(ERR_INTERNAL, "wrong shape for offset expression");
  dw_at_form(DW_FORM_block2); asm_printf("\n");
  out16(); out_dwf_dist_to_label(block_end); asm_printf("\n");
  if (dw_eval_exp(e, 0))
    asm_printf("\n");
  if (sh(e)->tag == offsethd && al2(sh(e)) < 8) {
    out8(); asm_printf("%d, %d\n", DW_OP_lit0 + 8, DW_OP_mul);
  }
  out_dwf_label(block_end, 1);
}


void dw2_cie
(void)
{
  long cie_end;
  cie_pointer = next_dwarf_label();
  cie_end = next_dwarf_label();
  enter_section("debug_frame");
  asm_comment("Common Information Entry");
  out_dwf_label(cie_pointer, 1);
  out32(); out_dwf_dist_to_label(cie_end); asm_printf("\n");
  out32(); asm_printf("%d\n", DW_CIE_id);
  out8(); asm_printf("%d\n", DW_CIE_MOD_VERSION);
  out_string("DERA/DDC-I");
  out8(); uleb128((unsigned long)framecode_factor); asm_printf("\n");
  out8(); sleb128((long)framedata_factor); asm_printf("\n");
  out8 (); asm_printf("%d\n", retaddr); /* return address column */
  out8(); asm_printf("%d", DW_CFA_def_cfa);
	asm_printf(", "); uleb128((unsigned long)dw_sp);
	asm_printf(", "); uleb128 ((unsigned long)0);	/* CFA is sp at entry point */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_undefined);
	asm_printf(", "); uleb128 ((unsigned long)dwarfreg [0]);	/* eax */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_undefined);
	asm_printf(", "); uleb128 ((unsigned long)dwarfreg [1]);	/* edx */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_undefined);
	asm_printf(", "); uleb128 ((unsigned long)dwarfreg [2]);	/* ecx */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_same_value);
	asm_printf(", "); uleb128 ((unsigned long)dwarfreg [3]);	/* ebx */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_same_value);
	asm_printf(", "); uleb128 ((unsigned long)dwarfreg [4]);	/* edi */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_same_value);
	asm_printf(", "); uleb128 ((unsigned long)dwarfreg [5]);	/* esi */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_same_value);
	asm_printf(", "); uleb128 ((unsigned long)dwarfreg [6]);	/* ebp */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_DD_location);
	asm_printf(", "); uleb128 ((unsigned long)dw_sp);		/* esp */
	asm_printf(", %d", 2);	/* location block */
	asm_printf(", %d", DW_OP_plus_uconst);
	asm_printf(", "); uleb128 ((unsigned long)4);	/* virtual pop return address */
	asm_printf("\n");
  out8(); asm_printf("%d", DW_CFA_offset + retaddr);
	asm_printf(", "); uleb128 ((unsigned long)0);	/* return address */
	asm_printf("\n");
  dot_align(PTR_SZ/8);
  out_dwf_label(cie_end, 1);
  exit_section();
}

static void short_advance
(long lo, long hi)
{
  if (lo) {
    out8(); out_dwf_label(hi, 0);  asm_printf(" - "); out_dwf_label(lo, 0);
    asm_printf(" + %d\n", DW_CFA_advance_loc);
  }
  else {
    out8(); asm_printf("%d\n", DW_CFA_set_loc);
    out32(); out_dwf_label(hi, 0); asm_printf("\n");
  }
}

long dw2_start_fde
(long proc_start, long fblab)
{
  long hold_pos;
  fde_end = next_dwarf_label();
  proc_end = next_dwarf_label();
  enter_section("debug_frame");
  asm_comment("Frame Descriptor Entry");
  out32(); out_dwf_dist_to_label(fde_end); asm_printf("\n");
  out32(); out_dwf_label(cie_pointer, 0); asm_printf("\n");
  out32(); out_dwf_label(proc_start, 0); asm_printf("\n");
  out32(); out_dwf_labdiff(proc_start, proc_end); asm_printf("\n");

  if (callee_size >= 0) {
    unsigned long pop_bytes = 4;
    int n = (remove_struct_ref && has_struct_res(crt_proc_exp))? 32 : 0;
    if ((n += callee_size)!= 0) {
      pop_bytes += (unsigned long)(n/8);
      out8(); asm_printf("%d", DW_CFA_DD_location);
	asm_printf(", "); uleb128((unsigned long)dw_sp);
	asm_printf(", %d", 1 + uleb128_length(pop_bytes));
	asm_printf(", %d", DW_OP_plus_uconst);
	asm_printf(", "); uleb128 (pop_bytes);	/* adjust virtual pops */
	asm_printf("\n");
    }
  }
  else {	/* var_callees */
    int extra = (remove_struct_ref && has_struct_res(crt_proc_exp))? 2 : 0;
    out8(); asm_printf("%d", DW_CFA_DD_location);
	asm_printf(", "); uleb128((unsigned long)dw_sp);
	asm_printf(", %d", 3 + extra);	/* location block length */
	asm_printf(", %d", DW_OP_plus_uconst);
	asm_printf(", "); uleb128 ((unsigned long)4);	/* virtual pop return address */
	asm_printf(", %d", DW_OP_deref);	/* over callees */
	if (extra) {
	  asm_printf(", %d", DW_OP_plus_uconst);
	  asm_printf(", "); uleb128 ((unsigned long)4);	/* pop struct_res */
	}
	asm_printf("\n");
  }

  if (!no_frame) {
    short_advance(proc_start, fblab);
    out8(); asm_printf("%d", DW_CFA_def_cfa);
	asm_printf(", "); uleb128((unsigned long)dwarfreg[6]);
	asm_printf(", "); uleb128 ((unsigned long)4);	/* CFA now relative to %ebp */
	asm_printf("\n");
    out8 (); asm_printf("%ld, ", DW_CFA_offset + dwarfreg[6]); /* %ebp */
	uleb128((unsigned long)1); asm_printf("\n");
  }

  hold_pos = out_tell_pos();
  asm_printf("%350s\n", "");

  exit_section();
  return hold_pos;
}

void dw2_fde_entry
(long dwl0, long dwl1, long dwl2, long dwl3, long dwl4, long dwl8, int space)
{
  long here = dwl0;
  unsigned long up = 0;
  if (!no_frame) {
    here = dwl1;
    ++up;
  }
  if (min_rfree & 0x8) {
    short_advance(here, dwl2);
    here = dwl2;
    out8();
    if (no_frame) {
	asm_printf("%d, ", DW_CFA_DD_def_cfa_inc_offset);
    }
	asm_printf("%ld, ", DW_CFA_offset + dwarfreg[3]); /* %ebx */
	uleb128(++up); asm_printf("\n");
  }
  if (min_rfree & 0x10) {
    short_advance(here, dwl3);
    here = dwl3;
    out8();
    if (no_frame) {
	asm_printf("%d, ", DW_CFA_DD_def_cfa_inc_offset);
    }
	asm_printf("%ld", DW_CFA_offset + dwarfreg[4]); /* %edi */
	asm_printf(", "); uleb128(++up); asm_printf("\n");
  }
  if (min_rfree & 0x20) {
    short_advance(here, dwl4);
    here = dwl4;
    out8();
    if (no_frame) {
	asm_printf("%d, ", DW_CFA_DD_def_cfa_inc_offset);
    }
	asm_printf("%ld, ", DW_CFA_offset + dwarfreg[5]); /* %esi */
	uleb128(++up); asm_printf("\n");
  }
  if (no_frame && (min_rfree & 0x40)) {
    short_advance(here, dwl1);
    here = dwl1;
    out8(); asm_printf("%d, ", DW_CFA_DD_def_cfa_inc_offset);
	asm_printf("%ld, ", DW_CFA_offset + dwarfreg[6]); /* %ebp */
	uleb128(++up); asm_printf("\n");
  }
  if (space && no_frame) {
    short_advance(here, dwl8);
    out8(); asm_printf("%d, ", DW_CFA_def_cfa_offset);
	uleb128((unsigned long)space); asm_printf("\n");
  }
}

void dw2_untidy_return
(void)
{
	/* we have pushed the return address */
  long here = set_dw_text_label();
  enter_section("debug_frame");
  out8(); asm_printf("%d\n", DW_CFA_set_loc);
  out32(); out_dwf_label(here, 0); asm_printf("\n");
  out8(); asm_printf("%d, ", DW_CFA_remember_state);
  if (no_frame) {
    asm_printf("%d\n", DW_CFA_DD_def_cfa_inc_offset);
  }
  exit_section();
}

long dw2_prep_fde_restore_args
(int untidy)
{
  long here;
  if (!untidy) {
    long pos;
    enter_section("debug_frame");
    out8(); asm_printf("%d\n", DW_CFA_remember_state);
    pos = out_tell_pos();
    asm_printf("%300s\n", "");
    asm_printf("\n");
    exit_section();
    return pos;
  }
	/* otherwise */
  here = set_dw_text_label();
  enter_section("debug_frame");
  short_advance(0, here);
  out8(); asm_printf("%ld\n", DW_CFA_restore + dwarfreg[3]);
  out8(); asm_printf("%ld\n", DW_CFA_restore + dwarfreg[4]);
  out8(); asm_printf("%ld\n", DW_CFA_restore + dwarfreg[5]);
  out8(); asm_printf("%ld\n", DW_CFA_restore + dwarfreg[6]);

  if (!no_frame) {	/* %ebp restored, return address pushed */
    out8(); asm_printf("%d", DW_CFA_def_cfa);
	asm_printf(", "); uleb128((unsigned long)dw_sp);
	asm_printf(", "); uleb128 ((unsigned long)0);	/* temp CFA */
	asm_printf("\n");
    out8(); asm_printf("%d", DW_CFA_undefined);
	asm_printf(", "); uleb128((unsigned long)dw_sp);
	asm_printf("\n");
  }
  exit_section();
  return 0;
}

void dw2_fde_restore_args
(long dwl0, long dwl1, long dwl2, long dwl3, long dwl4, int space)
{
  long here = 0;
  if (no_frame && dwl0) {
    short_advance(here, dwl0);
    here = dwl0;
    out8(); asm_printf("%d, ", DW_CFA_def_cfa_offset);
	uleb128((unsigned long)space); asm_printf("\n");
  }
  if (no_frame && (min_rfree & 0x40)) {
    short_advance(here, dwl1);
    here = dwl1;
    out8(); asm_printf("%d, ", DW_CFA_DD_def_cfa_dec_offset);
	asm_printf("%ld\n", DW_CFA_restore + dwarfreg[6]); /* %ebp */
  }
  if (min_rfree & 0x20) {
    short_advance(here, dwl2);
    here = dwl2;
    out8();
    if (no_frame) {
	asm_printf("%d, ", DW_CFA_DD_def_cfa_dec_offset);
    }
	asm_printf("%ld\n", DW_CFA_restore + dwarfreg[5]); /* %esi */
  }
  if (min_rfree & 0x10) {
    short_advance(here, dwl3);
    here = dwl3;
    out8();
    if (no_frame) {
	asm_printf("%d, ", DW_CFA_DD_def_cfa_dec_offset);
    }
	asm_printf("%ld\n", DW_CFA_restore + dwarfreg[4]); /* %edi */
  }
  if (min_rfree & 0x8) {
    short_advance(here, dwl4);
    here = dwl4;
    out8();
    if (no_frame) {
	asm_printf("%d, ", DW_CFA_DD_def_cfa_dec_offset);
    }
	asm_printf("%ld\n", DW_CFA_restore + dwarfreg[3]); /* %ebx */
  }
  if (!no_frame) {
    short_advance(here, dwl1);
    here = dwl1;
    out8 (); asm_printf("%ld", DW_CFA_restore + dwarfreg[6]); /* %ebp */
	asm_printf(", %d", DW_CFA_def_cfa);
	asm_printf(", "); uleb128((unsigned long)dw_sp);
	asm_printf(", "); uleb128 ((unsigned long)0);	/* CFA is sp at entry point */
	asm_printf("\n");
  }
  UNUSED(here);
}

void dw2_after_fde_exit
(long here)
{
  out_dwf_label(here, 1);
  enter_section("debug_frame");
  short_advance(0, here);
  out8(); asm_printf("%d\n", DW_CFA_restore_state);
  exit_section();
}

void dw2_track_push
(void)
{
  long here = set_dw_text_label();
  enter_section("debug_frame");
  short_advance(0, here);
  out8(); asm_printf("%d\n", DW_CFA_DD_def_cfa_inc_offset);
  exit_section();
}

void dw2_track_pop
(void)
{
  long here = set_dw_text_label();
  enter_section("debug_frame");
  short_advance(0, here);
  out8(); asm_printf("%d\n", DW_CFA_DD_def_cfa_dec_offset);
  exit_section();
}

void dw2_track_sp
(void)
{
  long here = set_dw_text_label();
  enter_section("debug_frame");
  short_advance(0, here);
  out8(); asm_printf("%d\n", DW_CFA_DD_def_cfa_fixed_offset);
  out32(); asm_printf("%d+%sdisp%d\n", (extra_stack - stack_dec) / 8, local_prefix, crt_proc_id);
  exit_section();
}


void dw2_complete_fde
(void)
{
  out_dwf_label(proc_end, 1);
  enter_section("debug_frame");
  dot_align(PTR_SZ/8);
  out_dwf_label(fde_end, 1);
  exit_section();
}


void dw2_start_extra_bit
(exp body)
{
  dg_info di = new_dg_info(DGA_EXTRA);
  di->data.i_scope.start = next_dwarf_label();
  di->data.i_scope.end = next_dwarf_label();
  di->more = dgf(body);
  dgf(body) = di;
}

void dw2_end_extra_bit
(exp body)
{
  UNUSED(body);
}


static exp lab_mark_list;

static void mark_lab
(exp labst)
{
  if (!dg_labmark(labst)) {
    set_dg_labmark(labst);
    if (child(child(labst))!= NULL)
      error(ERR_INTERNAL, "strange labst");
    child(child(labst)) = lab_mark_list;
    lab_mark_list = labst;
  }
}

static void trace_branch_aux
(exp whole, exp e)
{
  exp t;
  switch (e->tag) {
    case test_tag:
    case goto_tag: {
      if (!intnl_to(whole, pt(e)))
	mark_lab(pt(e));
      break;
    }
    case case_tag: {
      t = next(child(e));
      for (;;) {
	if (!intnl_to(whole, pt(t)))
	  mark_lab(pt(t));
	if (t->last)break;
	t = next(t);
      }
      break;
    }
    case labst_tag: {
      t = final_dest(e);
      if (!intnl_to(whole, t))
	mark_lab(t);
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
      trace_branch_aux(whole, t);
      if (t->last || e->tag == case_tag)break;
      t = next(t);
    }
  }
}

void trace_dw_branch_exits
(exp e)
{
  lab_mark_list = NULL;
  trace_branch_aux(e, e);
  while (lab_mark_list) {
    exp labst = lab_mark_list;
    exp dest = final_dest(labst);
    clear_dg_labmark(labst);
    lab_mark_list = child(child(labst));
    child(child(labst)) = NULL;
    IGNORE dw_entry(dwe_break, 0);
    out32(); out_code_label((long)ptno(pt(child(dest)))); asm_printf("\n");
  }
}



int dw_loc_equivalence
(exp a, exp b)
{
  return eq_where_exp(a, b, 1, 0);
}

typedef struct
{
  dg_name	nm;
  long		start;
  long		end;
} dw_regassn;

typedef struct
{
  dg_name	alloc;
  void *	share_set;
  dw_regassn	assn[2];
} dw_regdata;

#define TRACKREGS no_fixed_regs

static dw_regdata regassns[TRACKREGS];


void dw_allocated
(dg_name nm, exp id)
{
  int reg = no(id);
  if (!isglob(id) && ptno(id) == reg_pl && reg < TRACKREGS) {
    dw_close_regassn(reg, 0);
    dw_close_regassn(reg, 1);
    regassns[reg].alloc = nm;
    regassns[reg].share_set = NULL;
  }
}

void dw_deallocated
(dg_name nm)
{
  int i;
  for (i=0; i<TRACKREGS; i++) {
    if (regassns[i].alloc == nm) {
      dw_close_regassn(i, 0);
      dw_close_regassn(i, 1);
      regassns[i].alloc = NULL;
      regassns[i].share_set = NULL;
    }
  }
}

void dw_all_deallocated		/* initialisation */
(void)
{
  int i;
  for (i=0; i<TRACKREGS; i++) {
    dw_regassn * a = & (regassns[i].assn[0]);
    dw_regassn * b = & (regassns[i].assn[1]);
    regassns[i].alloc = NULL;
    regassns[i].share_set = NULL;
    a->start = a->end = b->start = b->end = 0;
  }
}


int dw_ignore_used_regassn = 0;

void dw_init_regassn
(int reg, int x)
{
  if (reg < TRACKREGS) {
    dg_name nm = find_equiv_object((!x ? crt_reg_record[reg].first_dest
				: crt_reg_record[reg].second_dest), 0);
    if (nm) {
      dw_regassn * a = & (regassns[reg].assn[x]);
      a->nm = nm;
      a->start = set_dw_text_label();
      a->end = 0;
    }
  }
}

void dw_used_regassn
(int reg, int x)
{
  if (reg < TRACKREGS && regassns[reg].assn[x].start)
    regassns[reg].assn[x].end = set_dw_text_label();
}

void dw_close_regassn
(int reg, int x)
{
  dw_regassn * a;
  if (dw_ignore_used_regassn || reg >= TRACKREGS)
    return;
  a = & (regassns[reg].assn[x]);
  if (a->end) {
    if (!regassns[reg].share_set) {
      regassns[reg].share_set = (void *)
		dw_new_regshare(regassns[reg].alloc, dwarfreg[reg]);
    }
    dw_add_regshare(regassns[reg].share_set, a->nm,
		a->start, a->end);
    a->end = 0;
    if (!regassns[reg].alloc && !regassns[reg].assn[1-x].start)
      regassns[reg].share_set = NULL;
  }
  a->start = 0;
}

