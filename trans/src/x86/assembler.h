/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <reader/exp.h>

#include <construct/installtypes.h>

#include <main/flags.h>

void outbyte(void);
void outshort(void);
void outlong(void);

void align_label(int f, exp jr);
void eval_postlude(char *s, exp c);
void out_readonly_section(void);
void out_dot_comm(char *name, shape sha);
void out_dot_lcomm(char *name, shape sha);

void pic_prelude(void);

void out_switch_jump(int tab, where a, int min);
void out_switch_table (int tab, int min, int max, int *v, int absent);

void proc_size(char *s);
void proc_type(char *s);
void dot_align(int n);
void outend(void);

void outopenbr(void);
void outclosebr(void);

void out_initialiser(char *name);
void out_main_prelude(void);
void out_main_postlude(void);

#endif /* ASSEMBLER_H */
