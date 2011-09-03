/*
    		 Crown Copyright (c) 1997

    This TenDRA(r) Computer Program is subject to Copyright
    owned by the United Kingdom Secretary of State for Defence
    acting through the Defence Evaluation and Research Agency
    (DERA).  It is made available to Recipients with a
    royalty-free licence for its use, reproduction, transfer
    to other parties and amendment for any purpose not excluding
    product development provided that any such use et cetera
    shall be deemed to be acceptance of the following conditions:-

        (1) Its Recipients shall ensure that this Notice is
        reproduced upon any copies or amended versions of it;

        (2) Any amended version of it shall be clearly marked to
        show both the nature of and the organisation responsible
        for the relevant amendment or amendments;

        (3) Its onward transfer from a recipient to another
        party shall be deemed to be that party's acceptance of
        these conditions;

        (4) DERA gives no warranty or assurance as to its
        quality or suitability for any purpose and DERA accepts
        no liability whatsoever in relation to any use to which
        it may be put.
*/


/* ****************************************************************
                              out_ba.c
   Procs for outputting binary assembly files.
   Each record in binasm is 16 bytes, coded in various ways depending
   on class of instruction or directtive.
*****************************************************************/
#include "config.h"
#include "ibinasm.h"
#include "out_ba.h"

extern  FILE * ba_file;

static  binasm ba;



#define  OUT out_one(  ba.data)
#define C(x)  ba.x.symno = symno; ba.x.fill07 =0;\
ba.x.formextn = 0; ba.x.asmtype = asmtype

void
out_one(char *d)
{	/* output one binasm record */
  int   i;
  for (i = 0; i < 16; i++) {
    putc (d[i], ba_file);
    d[i] = 0;
  }
  return;
}

void
out_data(char *s, int size)
{
				/* output string as set of binasm records
				*/
  int   i;
  int   j = ((size + 15) >> 4) << 4;
  for (i = 0; i < size; i++) {
    putc (s[i], ba_file);
  }
  for (i = size; i < j; i++)
    putc (' ', ba_file);
}




void
out_common(asmsym symno, unsigned asmtype)
{
				/* things like label settings */
  C (common);
  OUT;
}

void
out_ent(asmsym symno, unsigned asmtype, unsigned lexlev)
{
				/* output enter binasm record */
  C (ent);
  ba.ent.lexlev = lexlev;
  OUT;
}

void
out_frame(asmsym symno, unsigned asmtype, asmint frameoffset, asmreg framereg, asmreg pcreg)
{/* output frame binasm record */
  C (frame);
  ba.frame.frameoffset = frameoffset;
  ba.frame.framereg = framereg;
  ba.frame.pcreg = pcreg;
  OUT;
}

void
out_mask(asmsym symno, unsigned asmtype, unsigned regmask, asmint regoffset)
{		/* output mask binasm record */
  C (mask);
  ba.mask.regmask = regmask;
  ba.mask.regoffset = regoffset;
  OUT;
}


void
out_verstamp(int majornumber, int minornumber)
{
				/* output version stamp binasm record */
  ba.verstamp.symno = 0;
  ba.verstamp.fill07 = 0; ba.verstamp.formextn = 0;
  ba.verstamp.asmtype = iverstamp;
  ba.verstamp.majornumber = majornumber;
  ba.verstamp.minornumber = minornumber;
  OUT;
}

void
out_loc(unsigned filenumber, unsigned linenumber)
{
				/* output line-no binasm record */
  ba.loc.symno = 0;
  ba.loc.fill07 = 0; ba.loc.formextn = 0;
  ba.loc.asmtype = iloc;
  ba.loc.filenumber = filenumber;
  ba.loc.linenumber = linenumber;
  OUT;
  fflush (ba_file);
}

void
out_chars(asmsym symno, unsigned asmtype, asmint expression, unsigned short repeat)
{	/* output some data directive binasm
				   record */
  C (value);
  ba.value.expression = expression;
  ba.value.repeat = repeat;
  OUT;
}

void
out_option(int optype, int opint)
{
				/* output option binasm record */
  ba.voption.symno = 0;
  ba.voption.fill07 = 0; ba.voption.formextn = 0;
  ba.voption.asmtype = ioption;
  ba.voption.option = optype;
  ba.voption.opt_int_value = opint;
  OUT;
}

void
out_value(asmsym symno, unsigned asmtype, asmint expression, unsigned long repeat)
{	/* output some data directive binasm
  				   record */
    C (value);
    ba.value.expression = expression;
    ba.value.repeat = (repeat>0xffff)?0xffff:repeat;
    OUT;
    if (repeat>0xffff) out_value(symno, asmtype, expression, repeat-0xffff);
}

void
out_alias(asmsym symno, unsigned asmtype, asmreg basereg1, asmreg basereg2)
{		/* output alias binasm record */
  C (alias);
  ba.alias.basereg1 = basereg1;
  ba.alias.basereg2 = basereg2;
  OUT;
}

void
out_cpload(asmsym symno, asmreg reg)
{
  ba.rinst.symno = symno;
  ba.rinst.asmtype = icpload;
  ba.rinst.fill07 = 0; ba.rinst.formextn = 0;
  ba.rinst.fill03 = 0;
  ba.rinst.op = 0;
  ba.rinst.reg1 = reg;
  ba.rinst.reg2 = 0;
  ba.rinst.form = 0;
  ba.rinst.reg3 = 0;
  OUT;
}
void
out_rinst(asmsym symno, unsigned char opcode, asmreg reg1, asmreg reg2, asmformat form, asmreg reg3)
{/* output binasm for instruction with no
				   immediate operand */
  ba.rinst.symno = symno;
  ba.rinst.asmtype = iocode;
  ba.rinst.fill07 = 0; ba.rinst.formextn = 0;
  ba.rinst.fill03 = 0;
  ba.rinst.op = opcode;
  ba.rinst.reg1 = reg1;
  ba.rinst.reg2 = reg2;
  ba.rinst.form = form;
  ba.rinst.reg3 = reg3;
  OUT;
}

void
out_iinst(asmsym symno, unsigned char opcode, asmreg reg1, asmreg reg2, asmformat form, asmint immediate)
{
				/* output binasm for instruction with
				   immediate operand */
  ba.iinst.symno = symno;
  ba.iinst.asmtype = iocode;
  ba.iinst.fill07 = 0; ba.iinst.formextn = 0;
  ba.iinst.fill03 = 0;
  ba.iinst.op = opcode;
  ba.iinst.reg1 = reg1;
  ba.iinst.reg2 = reg2;
  ba.iinst.form = form;
  ba.iinst.mem_tag = 0; /*????*/
  ba.iinst.immediate = immediate;
  OUT;
}