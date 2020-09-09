/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * refactor_id tries to apply transformations to improve identity and
 * variable declarations.
 *
 * refactor_id delivers 1 if it makes any change, 0 otherwise.
 *
 * used_in delivers 0 if the identifier declared by vardec is unused in
 * the exp piece, 1 if it is used for contents operation only, 3 if it is
 * used otherwise.
 *
 * simple_const tests whether e is used as a simple constant in whole.
 * This is true in the following circumstances only:
 *
 *  1) e is a constant.
 *  2) e is an identity declaration(not a variable) and the declaration is
 *     external to whole.
 *  3) e is the contents of a variable, and the variable is not used
 *     in whole as the destination of an assignment, and the variable
 *     is only used (anywhere) as the destination of assignment or
 *     argument of contents (ie there is no alias for it).
 *
 * no_ass is true iff there are no assignments to things that might
 * be aliased during the evaluation of whole. (beware procedure calls!)
 */

#include <stddef.h>

#include <shared/bool.h>
#include <shared/check.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/externs.h>

#include <construct/exp.h>
#include <construct/installglob.h>
#include <construct/install_fns.h>
#include <construct/me_fns.h>

#include <main/flags.h>

#include <refactor/refactor.h>
#include <refactor/refactor_id.h>

#ifdef TDF_DIAG4
#include <diag4/dg_aux.h>
#endif

#if defined(TRANS_X86) || defined(TRANS_HPPA)
#include <local/exp.h>
#endif

#if TRANS_M68K
extern int check_anyway(exp);
#endif

/*
 * make_onearg makes up an exp with the given tag (n), shape (sha)
 * and single argument (a).
 */
exp
hc(exp e, exp t)
{
	t->last = true;
	next(t) = e;
	return hold_refactor(e);
}

static exp
make_onearg(unsigned char n, shape sha, exp a)
{
	exp r = getexp(sha, NULL, 0, a, NULL, 0, 0, n);
	return hc(r, a);
}

/*
 * make_twoarg makes up an exp with the given tag (n), shape (sha)
 * and two arguments (a,b) in that order.
 */
static exp
make_twoarg(unsigned char n, shape sha, exp a, exp b)
{
	exp r = getexp(sha, NULL, 0, a, NULL, 0, 0, n);
	next(a) = b;
	a->last = false;
	return hc(r, b);
}

/*
 * used_in delivers 0 if the identifier declared by vardec is unused in
 * the exp piece, 1 if it is used for contents operation only, 3 if it is
 * used otherwise.
 */
static int
used_in(exp vardec, exp piece)
{
	int  res = 0;
	exp t = pt(vardec);
	exp q;
	exp upwards = t;

	/* test each use of the identifier */
	do {
		q = t;

		while (q != NULL && q != piece && q != vardec &&
		       !q->parked && (q->tag != ident_tag || !isglob(q))) {
			upwards = q;
			q = next(q);
		}

		/* ascend from the use until we reach either vardec or piece */
		if (upwards->last && q == piece) {
			/* the use was in piece */
			res = 1;
			if ((t->last || !next(t)->last || next(next(t))->tag != 0)) {
				/* the use was not contents or in diagnostics*/
				if (!t->last || next(t)->tag != cont_tag) {
					res = 3;
				}
			}
		}

		t = pt(t);
	} while (t != NULL && res != 3);

	return res;
}

/*
 * simple_const tests whether e is used as a simple constant in whole.
 * This is true in the following circumstances only:
 *
 *  1) e is a constant.
 *  2) e is an identity declaration(not a variable) and the declaration is
 *     external to whole.
 *  3) e is the contents of a variable, and the variable is not used
 *     in whole as the destination of an assignment, and the variable
 *     is only used (anywhere) as the destination of assignment or
 *     argument of contents (ie there is no alias for it).
 *
 * no_ass is true iff there are no assignements to things that might
 * be aliased during the evaluation of whole. (ware procedure calls!)
 */
int
simple_const(exp whole, exp e, int decl, int no_ass)
{
	if (e->tag == val_tag || e->tag == real_tag || e->tag == null_tag) {
		return 1;
	}

	if (e->tag == name_tag && !isvar(child(e)) &&
	    (decl || !internal_to(whole, child(e)))) {
		return 1;
	}

	if (e->tag == reff_tag) {
		e = child(e);
	}

	if (e->tag == cont_tag && child(e)->tag == name_tag &&
	    !isparam(child(child(e))) && isvar(child(child(e)))) {
		exp var = child(child(e));
		int  u = used_in(var, whole);
		if (u != 3 && (iscaonly(var) || no_ass)) {
			return 1;
		}

		return 0;
	}

	return 0;
}

/*
 * replace declaration by sequence of definition and body.
 * Done if the identifier is not used.
 */
static void
repbyseq(exp e)
{
	exp def = child(e);
	exp body = hold_refactor(next(def));
	exp seq, s;

#ifdef TDF_DIAG4
	exp t = pt(e);
	while (t != NULL) {
		if (isdiaginfo(t)) {
			setdiscarded(t);
		}
		t = pt(t);
	}
#endif

	if (child(def) == NULL) {
#ifdef TDF_DIAG4
		if (diag != DIAG_NONE) {
			dg_whole_comp(e, body);
		}
#endif

		replace(e, body, e);
		retcell(def);
		return;
	}

	seq = getexp(f_bottom, NULL, 0, def, NULL, 0, 0, 0);
	next(def) = seq;
	def->last = true;
	s = hold_refactor(make_twoarg(seq_tag, sh(body), seq, body));

#ifdef TDF_DIAG4
	if (diag != DIAG_NONE) {
		dg_whole_comp(e, s);
	}
#endif

	replace(e, s, e);
}

/*
 * propagate looks right and upwards from plc through the tree, looking
 * for contents operations applied to the variable defined by vardec.
 *
 * The assumption is that plc made an assignment to the variable defined
 * by vardec, and this scan looks forward from this point, marking any
 * contents operations on that variable for later modification to use the
 * value assigned. The variable is previously checked to make sure there
 * is no alias for it.
 *
 * The scan terminates if ende is reached or when it is no longer safe
 * to propagate the value forward. 1 is delivered if ende was reached
 * while propagation was still safe, 0 otherwise.
 */
static int
propagate(exp vardec, exp ende, exp plc, int bfirst)
{
	exp p = plc;		/* starting place */
	int good = 1;		/* good is set to 0 when we find a place where we must stop */
	int bb = bfirst;	/* if bb is 1, avoid the first up */

up: /* up ascends the tree */

	if (bb) {
		bb = 0;
	} else {
		if (p == ende) {		/* finished */
			return good;
		}

		if (!p->last) {
			p = next(p);
			if (p->tag == labst_tag) {
				/* can't go further */
				return 0;
			}
		} else {
			if (next(p)->tag == proc_tag ||
				next(p)->tag == labst_tag ||
				next(p)->tag == condassign_tag) {
				/* can't go further */
				return 0;
			}

			p = next(p);
			if ((p->tag == ass_tag ||
				 p->tag == assvol_tag) &&
				child(p)->tag == name_tag &&
				child(child(p)) == vardec) {
				return 0;
			}

			goto up;
		}
	}

rep: /* rep processes an exp */

	if (p->tag == ass_tag || p->tag == assvol_tag) {
		if (child(p)->tag == name_tag && child(child(p)) == vardec) {
			/* just process the value */
			p = next(child(p));
			goto rep;
		} else {
			/* assignment to something else */
			p = child(p);
			goto rep;
		}
	}

	if (p->tag == cont_tag) {
		if (child(p)->tag == name_tag && child(child(p)) == vardec) {
			set_propagate(p);		/* mark it */
			goto up;
		} else {
			p = child(p);
			goto rep;
		}
	}

	if (p->tag == name_tag || p->tag == env_offset_tag) {
		goto up;
	}

	if (p->tag == apply_tag || p->tag == apply_general_tag) {
		if (isglob(vardec)) {
			/* vardec is global */
			return 0;
		} else {
			/* not aliased so OK */
			p = child(p);
			goto rep;
		}
	}

	if (p->tag == rep_tag) {
		return 0;
	}

	if (p->tag == cond_tag) {
		if (propagate(vardec, child(p), child(p), 1)) {
			good = propagate(vardec, next(child(next(child(p)))),
			                 next(child(next(child(p)))), 1);
			/* if we can propagate right through the first of the
			 * cond we can go into the alt. This condition is
			 * stronger than needed. */
			if (good) {
				goto up;
			} else {
				return good;
			}
		} else {
			return 0;
		}
	}

	if (p->tag == solve_tag) {
		IGNORE propagate(vardec, child(p), child(p), 1);
		/* give up after trying the first element */
		return 0;
	}

	if (p->tag == case_tag) {
		if (propagate(vardec, child(p), child(p), 1)) {
			goto up;
		}
		return 0;
	}

	if (child(p) == NULL) {
		goto up;
	}

	p = child(p);
	goto rep;
}

/*
 * change_cont looks at all the cont uses of the variable defined by
 * vardec. If they have been marked by propagate or if force is 1,
 * the cont(var) is replaced by val.
 */
static exp
change_shape(exp e, shape sha)
{
	if (e->tag == val_tag) {
		no(e) = dochvar(no(e), sha);
	}

	sh(e) = sha;

	return e;
}

static int
change_cont(exp vardec, exp val, int force)
{
	exp t;
	exp bh = hold(next(child(vardec)));
	int ch = 0;
	int go = 1;
	int defsize = shape_size(sh(child(vardec)));

	while (go) {
		t = pt(vardec);
		go = 0;
		while (!go && t != NULL) {
			if (t->last && next(t)->tag == cont_tag &&
#ifdef TDF_DIAG4
			    !isdiaginfo(t) &&
#endif
			    (to_propagate(next(t)) || force))
		{
				if (defsize == shape_size(sh(next(t)))) {
					exp p = next(t);
					exp c = change_shape(copy(val), sh(p));
					kill_exp(t, child(bh));
					replace(p, c, child(bh));
					retcell(p);
					t = pt(vardec);
					ch = 1;
					go = 1;
				} else {
					clear_propagate(next(t));
				}
			} else {
				t = pt(t);
			}
		}
	}

	next(child(vardec)) = child(bh);
	next(child(vardec))->last = true;
	next(next(child(vardec))) = vardec;
	retcell(bh);

	return ch;
}

/*
 * checks identity and variable declarations.
 */
int
refactor_id(exp e, exp scope)
{
	bool is_var = isvar(e);
	bool is_vis = all_variables_visible || isvis(e);
	exp def = child(e);
	exp body = next(def);
	int looping;
	exp t1;

	if (no(e) == 0) {
		if (!isvis(e) && !isenvoff(e) && !isglob(e) && !isparam(e)) {
			/* the variable is not used */
			repbyseq(e);

			return 1;
		} else {
			if (isparam(e)) {
				setcaonly(e);
			}

			return 0;
		}
	}

	if (load_ptr_pars) {
		if (!is_vis && is_var && isparam(e) && no(e) > 1 && sh(def)->tag == ptrhd
#if TRANS_M68K
		    && check_anyway(e)
#endif
		   ) {
			int ch_load = 1;
			int sz = shape_size(sh(def));
			t1 = pt(e);
			looping = 1;

			do {
#ifdef TDF_DIAG4
				if (!isdiaginfo(t1)) {
#endif

					if (!t1->last && next(t1)->last && next(next(t1))->tag == ass_tag &&
					    shape_size(sh(next(t1))) == sz) {
						;
					} else if (!t1->last || next(t1)->tag != cont_tag ||
					           shape_size(sh(next(t1))) != sz) {
						ch_load = 0;
					}

#ifdef TDF_DIAG4
				}
#endif
				if (pt(t1) == NULL) {
					looping = 0;
				} else {
					t1 = pt(t1);
				}
			} while (looping && ch_load);

			if (ch_load) {
				exp old_pt_list = pt(e);
				int old_uses = no(e);
				exp new_var;
				exp new_n;
				exp real_body;

				t1 = e;

				while (next(child(t1))->tag == ident_tag && isparam(next(child(t1)))) {
					t1 = next(child(t1));
				}

				real_body = next(child(t1));

				new_n   = getexp(sh(def), real_body, 0, e, NULL, 0, 0, name_tag);
				new_var = getexp(sh(e), NULL, 0, new_n, old_pt_list, 1, old_uses, ident_tag);

				setloadparam(new_n);
				setfather(new_var, real_body);
				pt(e) = new_n;
				no(e) = 1;

				clearvar(e);
				while (old_pt_list != NULL) {
					child(old_pt_list) = new_var;
					old_pt_list = pt(old_pt_list);
				}

				new_var = hold_refactor(new_var);

				next(child(t1)) = new_var;
				setfather(t1, new_var);

				return 1;
			}
		}
	}

	if (!is_vis && !is_var &&
	    (!load_ptr_pars || ((def->tag != name_tag || !isloadparam(def)))) &&
	    (def->tag == val_tag ||
	     (load_ptr_pars
	      ? ((def->tag == name_tag &&
	          (!isparam(child(def)) || sh(def)->tag == ptrhd)))
	      : def->tag == name_tag )
	     ||
#if TRANS_X86
	     (def->tag == name_tag && isparam(child(def)) && !isvar(child(def)) &&
	      shape_size(sh(def)) < shape_size(sh(child(child(def)))) &&
	      sh(def)->tag <= ulonghd) ||
#endif

	     /*
	      * Substitute the definitions of identity declarations into body
	      * if it seems cheaper to do so.
	      */
	     (def->tag == reff_tag && child(def)->tag == cont_tag &&
	      child(child(def))->tag == name_tag && isvar(child(child(child(def)))) &&
	      !isglob(child(child(child(def)))) &&
	      used_in(child(child(child(def))), body) != 3) ||
	     (def->tag == reff_tag && child(def)->tag == name_tag &&
	      isvar(child(child(def))) && !isglob(child(child(def))) &&
	      used_in(child(child(def)), body) != 3) || def->tag == null_tag ||
	     def->tag == real_tag)) {

		/* identifying a constant or named value */
		if (optim & OPTIM_SUBSTPARAMS || def->tag != name_tag || !isparam(child(def)) ||
		    isvar(child(def))) {
			exp bh = hold(body);

#ifdef TDF_DIAG4
			dg_info dgh = dgf(def);
			/* don't copy line info to all uses */
			dgf(def) = NULL;
#endif

			while (pt(e) != NULL) {
				exp mem = pt(e);
				exp cp;
				pt(e) = pt(mem);
				cp = copy(def);

#ifdef TDF_DIAG4
				if (isdiaginfo(mem)) {
					IGNORE diaginfo_exp(cp);
				} else {
					--no(e);
				}
#else
				--no(e);
#endif

				if (cp->tag == name_tag) {
					no(cp) += no(mem);
				}

				if (sh(cp) != sh(mem)) {
					if (sh(cp)->tag <= u64hd) {
						cp = hold_refactor(me_u3(sh(mem), cp, chvar_tag));
					} else {
						sh(cp) = sh(mem);
					}
				}

#ifdef TDF_DIAG4
				if (diag != DIAG_NONE) {
					dg_whole_comp(mem, cp);
				}
#endif

				replace(mem, cp, body);
			}

#ifdef TDF_DIAG4
			dgf(def) = dgh;
#endif

			next(def) = child(bh);
			next(next(def)) = e;
			next(def)->last = true;
			retcell(bh);
			IGNORE refactor(e, scope);

			return 1;
		}
	}

	/*
	 * Also substitute identity definitions which are references to bitfields.
	 */
	if (!is_vis && !is_var && def->tag == reff_tag && al1(sh(def)) == 1) {
		exp t = pt(e);
		int n = no(def);
		shape sha = sh(def);
		shape shb = sh(child(def));
		exp q, k;

#ifdef TDF_DIAG4
		if (diag != DIAG_NONE) {
			dg_whole_comp(def, child(def));
		}
#endif
		replace(def, child(def), child(def));

		for (;;) {
			k = pt(t);
			q = getexp(sha, NULL, 0, copy(t), NULL, 0, n, reff_tag);
			sh(child(q)) = shb;
			q = hc(q, child(q));
			replace(t, q, q);
			kill_exp(t, t);
			if (k == NULL) {
				return 1;
			}

			t = k;
		}
	}

	/* and substitute strings */
	if (!is_vis && !is_var && def->tag == string_tag) {
		exp t = pt(e);
		int all_chars = 1;

		for (;;) {
			if (sh(t)->tag > ucharhd) {
				all_chars = 0;
				break;
			}
			if (t->last) {
				/* Surely this is wrong ??? */
				break;
			}
			t = pt(t);
		}

		if (all_chars) {
			char *str = nostr(def);

			t = pt(e);
			for (;;) {
				/* Surely this is wrong ??? */
				int l = (int)t->last;

				exp n = next(t);
				int  v = str[no(t) / 8];
				exp c;
				if (sh(t)->tag == ucharhd) {
					v = v & 0xff;
				}
				c = getexp(sh(t), NULL, 0, NULL, NULL, 0,
				           v, val_tag);
				replace(t, c, c);
				kill_exp(t, t);
				if (l) {
					break;
				}
				t = n;
			}

			if (no(e) == 0) {
				replace(e, next(child(e)), scope);
				return 1;
			}

			return 0;
		}
	}

	if (!is_vis && !is_var && body->tag == seq_tag &&
	    child(child(body))->tag == ass_tag && next(child(body))->tag == name_tag) {
		exp tb = next(child(child(child(body))));
		if (tb->tag == name_tag && child(tb) == e &&
		    child(next(child(body))) == e && child(child(body))->last &&
		    sh(tb) == sh(def) && sh(tb) == sh(next(child(body)))) {
			/* e=id(def, seq(ass(tz, n(e)), n(e)) -> seq(ass(tz, def), cont(tz)) */
			exp ass = child(child(body));
			exp tz = child(ass);
			exp r, s, c;
			exp cz = copy(tz);

			next(tz) = def;
			ass = hc(ass, def);
			r = getexp(f_top, NULL, 0, ass, NULL, 0, 0, 0);
			ass->last = true;
			next(ass) = r;

			s = getexp(sh(body), NULL, 0, r, NULL, 0, 0, seq_tag);
			c = getexp(sh(body), s, 1, cz, NULL, 0, 0, cont_tag);
			setnext(r, hc(c, cz));
			replace(e, hc(s, next(child(s))), e);

			return 1;
		}
	}

	/*
	 * Look to see if we can replace variable definitions by identities.
	 * This can be done if there are only contents operations and no aliasing.
	 */
	if (!is_vis && is_var) {
		/* variable declaration */
		int all_c = 1;	/* every use is a contents operation */
		int all_a = 1;	/* every use is an assignment operation */
		int not_aliased = 1;
		int ca = 0;		/* there is an assignment of a constant */

		/* there is an assignment of a variable (not its contents) (lhvalue in C terms). */
		int vardecass = 0;

		exp assd_val;	/* the assigned value */
		int conversion = 0;
		int biggest_assigned_const = 0;
		exp tc = pt(e);
		int defsize = shape_size(sh(def));

		do {
			/* scan the uses of the variable */
			if (tc->last && (next(tc)->tag == hold_tag ||
			                 next(tc)->tag == hold2_tag)) {
				if (diag != DIAG_NONE) {
					setvis(e);
					return 0;
				}
			} else {
				if (tc->last && next(tc)->tag == cont_tag && no(tc) == 0 &&
#ifdef TDF_DIAG4
				    !isdiaginfo(tc) &&
#endif
				    (sh(next(tc))->tag < shrealhd || sh(next(tc))->tag > doublehd ||
				     (sh(def)->tag >= shrealhd && sh(def)->tag <= doublehd))) {
					int qq = shape_size(sh(next(tc)));
					/* contents op so not all assignments */
					all_a = 0;

					if (father(next(tc))->tag != test_tag) {
						conversion = -1;
					}

					if ((defsize != qq) && (sh(def)->tag < shrealhd)) {
#if TRANS_X86
						if (!isparam(e) || no(e) != 1) {
							if (no(tc) == 0 && defsize <= 32) {
								if (qq == 8) {
									setbyteuse(e);
								}
							} else {
								all_c = 0;
								not_aliased = 0;
							}
						}
#else
						all_c = 0;
						not_aliased = 0;
#endif
					}
				} else {
					if (!tc->last && next(tc)->last && no(tc) == 0 &&
#ifdef TDF_DIAG4
					    !isdiaginfo(tc) &&
#endif
					    next(next(tc))->tag == ass_tag) {
						/* assignment op */
						all_c = 0;		/* not all contents */
						assd_val = next(tc);

						if (assd_val->tag == val_tag) {
							if (no(assd_val) < 0) {
								conversion = -1;
							}
							if (no(assd_val) > biggest_assigned_const) {
								biggest_assigned_const = no(assd_val);
							}
						} else if (assd_val->tag == chvar_tag &&
						           sh(child(assd_val))->tag <= uwordhd &&
						           is_signed(sh(child(assd_val)))) {
							int sz1 = shape_size(sh(child(assd_val)));
							if (conversion == 0) {
								conversion = sz1;
							} else if (conversion != sz1) {
								conversion = -1;
							}
						} else {
							conversion = -1;
						}

						if (defsize != shape_size(sh(assd_val))) {
#if TRANS_X86
							if (no(tc) == 0 && defsize <= 32) {
								if (shape_size(sh(next(tc))) == 8) {
									setbyteuse(e);
								}
							} else {
								all_a = 0;
								not_aliased = 0;
							}
#else
							all_a = 0;
							not_aliased = 0;
#endif
						}

						if (assd_val->tag == val_tag || assd_val->tag == real_tag ||
						    assd_val->tag == null_tag ||
						    (assd_val->tag == name_tag && isglob(child(assd_val)))) {
							ca = 1;		/* assigning a constant */
						} else {
							if (assd_val->tag == ident_tag && isvar(assd_val)) {
								vardecass = 1;
							}
						}
					} else
#ifdef TDF_DIAG4
						if (!isdiaginfo(tc))
#endif
						{
							if (isreallyass(tc)) {
								all_c = 0;
								all_a = 0; /* so that we dont remove the proc call */
							} else {
								/* something else */
								exp dad = father(tc);
								all_c = 0;
								all_a = 0;
								if (!((dad->tag == addptr_tag ||
								       dad->tag == subptr_tag) &&
								      ((!dad->last && next(dad)->last &&
								        next(next(dad))->tag == ass_tag) ||
								       (dad->last && next(dad)->tag == cont_tag))) ||
								    (sh(def)->tag == realhd &&
								     sh(next(dad))->tag != realhd) ||
								    (sh(def)->tag == doublehd &&
								     sh(next(dad))->tag != doublehd)) {
									/* not an assignment to element of array */
									not_aliased = 0;
								} else {
									setvis(e);
									uses_loc_address = true;
								}
							}
						}
				}
			}

			tc = pt(tc);
		} while (tc != NULL);

		if (not_aliased || iscaonly(e)) {
			/* set no alias flag if nothing but cont and ass */
			setcaonly (e);
		} else {
			/* set visible flag if there is an alias */
			setvis (e);
			uses_loc_address = true;
		}

		if (all_c) {
			/*
			 * If only cont operations replace by an identity declaration and
			 * change the uses accordingly.
			 */
			exp bh = hold(body);
			int i, j;
			setid(e);
			tc = e;

			do {
				tc = pt(tc);
				if (next(tc)->tag == cont_tag) {
					sh(tc) = sh(next(tc));
#ifdef TDF_DIAG4
					if (diag != DIAG_NONE) {
						dg_whole_comp(next(tc), tc);
					}
#endif
					replace(next(tc), tc, tc);
				}
			} while (pt(tc) != NULL);

			if (no(e) < 100) {
				for (i = 0; i < no(e); ++i) {
					tc = e;
					for (j = 0; tc != NULL && j <= i; ++j) {
						tc = pt(tc);
#ifdef TDF_DIAG4
						while (tc != NULL && isdiaginfo(tc)) {
							tc = pt(tc);
						}
#endif
					}
					altered(tc, child(bh));
				}
			}

			next(def) = child(bh);
			next(next(def)) = e;
			next(def)->last = true;
			retcell(bh);

			IGNORE refactor(e, scope);
			return 1;
		}

#if TRANS_X86 || TRANS_HPPA
		/* look for places where we can avoid sign extending */
		if (not_aliased && sh(def)->tag == slonghd &&
		    conversion == 16 && /* not 8 because of 80x86 regs */
		    (biggest_assigned_const & ((conversion == 8) ? (int) 0xffffff80 :
		                               (int) 0xffff8000)) == 0 &&
		    def->tag == clear_tag) {
			exp temp, next;
			shape ish = (conversion == 8) ? scharsh : swordsh;
			setse_opt(e);

			for (temp = pt(e); temp != NULL; temp = next) {
				next = pt(temp);

				if (temp->last) {
					if ((next(temp)->last || next(next(temp))->tag != val_tag) &&
					    next(temp)->tag != hold_tag) {
						exp x = me_u3(slongsh, copy(next(temp)), chvar_tag);
						sh(child(x)) = ish;
						replace(next(temp), x, x);
						IGNORE refactor(father(x), father(x));
						kill_exp(next(temp), next(temp));
					}
				} else {
					if (next(temp)->tag == val_tag) {
						sh(next(temp)) = ish;
					} else {
						next(child(next(temp))) = next(next(temp));
						next(temp) = child(next(temp));
#if TRANS_HPPA
						sh(next(temp)) = (conversion == 8) ? ucharsh : uwordsh;
#endif
					}
				}
			}

			replace(def, me_shint(slongsh, 0), def);
		}
#endif

		if (not_aliased && no(e) < 1000 &&
		    (sh(def)->tag < shrealhd || sh(def)->tag > doublehd) &&
		    (ca || vardecass || def->tag == val_tag ||
		     child(e)->tag == real_tag || def->tag == null_tag))
		{
			/* propagate constant assignment forward from the place where they occur */
			int  no_ass;
			int chv;

			if (def->tag == val_tag || child(e)->tag == real_tag ||
			    def->tag == null_tag
			    /*
			       ||
			       (def->tag == name_tag &&
			       isglob (child(def)))
			     */
			   ) {
				do {
					body = next(def);
					IGNORE propagate(e, e, body, 1);
				} while (change_cont(e, def, 0));
			}

			body = next(def);

			do {
				chv = 0;
				no_ass = 0;
				tc = pt(e);

				while (!chv && tc != NULL) {
					if (!tc->last &&
#ifdef TDF_DIAG4
					    !isdiaginfo(tc) &&
#endif
					    sh(next(tc)) == sh(child(child(tc))) && next(tc)->last &&
					    next(next(tc))->tag == ass_tag)
					{
						exp var = next(tc);
						exp va, df, bd;

						if (eq_shape(sh(next(tc)), sh(child(e))) &&
						    (next(tc)->tag == val_tag || next(tc)->tag == real_tag ||
						     next(tc)->tag == null_tag
						     /*
						        ||
						        (next(tc)->tag == name_tag &&
						        isglob (child(next(tc))))
						      */
						    ))
						{
							IGNORE propagate(e, e, next(next(tc)), 0);
							chv = change_cont(e, next(tc), 0);
							body = next(def);
							++no_ass;
						} else {
							va = child(tc);
							df = child(var);

							if (df != NULL && (bd = next(df)) != NULL &&
							    !isinlined(e) && !isglob(va) && isvar(va) &&
							    bd->tag == seq_tag && next(child(bd))->tag == cont_tag &&
							    child(next(child(bd)))->tag == name_tag &&
							    child(child(next(child(bd)))) == var &&
							    isvar(var) && used_in(va, bd) == 0) {
								exp a = child(next(var));
								exp prev_uses, ass, seq_hold, s;
								kill_exp(next(child(bd)), body);
								prev_uses = pt(va);
								tc = var;
								pt(va) = pt(var);

								do {
									child(pt(tc)) = va;
									++no(va);
									tc = pt(tc);
								} while (pt(tc) != NULL);
								pt(tc) = prev_uses;

								if (df->tag == clear_tag) {
									ass = getexp(f_top, NULL, 0, NULL, NULL, 0, 0,
									             top_tag);
								} else {
									ass = getexp(f_top, NULL, 0, a, NULL, 0, 0, ass_tag);
									next(a) = df;
									next(df) = ass;
									df->last = true;
								}

								seq_hold = make_onearg(0, f_bottom, ass);
								s = make_twoarg(seq_tag, f_top, seq_hold, child(child(bd)));
								replace(next(var), s, body);
								chv = 1;
							}
						}
					}

					tc = pt(tc);
				}
			} while (chv);

#ifdef TDF_DIAG4
			if (no(e) == no_ass && !isparam(e)) {
				int diagonly = 1;
				tc = pt(e);

				while (tc != NULL) {
					if (!isdiaginfo(tc)) {
						if (diag != DIAG_NONE) {
							dg_rem_ass(next(next(tc)));
						}
						replace(next(next(tc)), next(tc), next(def));
						diagonly = 0;
					}
					tc = pt(tc);
				}

				if (!diagonly) {
					repbyseq(e);
				}
			}
#else
			if (no(e) == no_ass && pt(e) != NULL && !isparam(e)) {
				tc = pt(e);
				while (replace(next(next(tc)), next(tc), next(def)), pt(tc) != NULL) {
					tc = pt(tc);
				}
				repbyseq(e);
			}
#endif
			return 1;
		}

		if (!isparam(e) && def->tag == clear_tag && body->tag == seq_tag &&
		    child(child(body))->tag == ass_tag &&
		    child(child(child(body)))->tag == name_tag &&
		    child(child(child(child(body)))) == e &&
		    eq_shape(sh(def), sh(next(child(child(child(body)))))))
		{
			/* definition is clear and first assignment is to this variable */
			exp val = next(child(child(child(body))));/* assigned value */
			if (!used_in(e, val)) {
				child(e) = val;		/* put it in as initialisation */
				val->last = false;
				next(val) = body;
				/* kill the use of var */
				kill_exp(child(child(child(body))), child(child(child(body))));
				/* replace assignment by void */
				replace(child(child(body)), getexp(f_top, NULL, 0, NULL, NULL, 0, 0, top_tag), body);
				return 1;
			}
		}

		if (all_a && !isparam(e) && diag == DIAG_NONE) {
			/*
			 * If only assignments replace them by evaluating the value assigned
			 * and discarding it. Replace the declaration by a sequence of
			 * definition and body.
			 */
			tc = pt(e);

			for (;;) {
				if (!tc->last && next(next(tc))->tag == ass_tag) {
#ifdef TDF_DIAG4
					if (diag != DIAG_NONE) {
						dg_rem_ass(next(next(tc)));
					}
#endif
					replace(next(next(tc)), next(tc), body);
				}
				tc = pt(tc);
				if (tc == NULL) {
					break;
				}
			}

			repbyseq(e);
			return 1;
		}

	}

	if (!is_var && !is_vis && no(e) == 1 && !isparam(e) &&
	    body->tag == ident_tag && child(body)->tag == name_tag &&
	    child(child(body)) == e && shape_size(def) == shape_size(child(body))) {

#ifdef TDF_DIAG4
		if (diag != DIAG_NONE) {
			exp t = pt(e);
			while (t) {
				if (isdiaginfo(t)) {
					setdiscarded(t);
				}
				t = pt(t);
			}
		}
#endif

		replace(child(body), def, def);
#ifdef TDF_DIAG4
		if (diag != DIAG_NONE) {
			dg_whole_comp(e, body);
		}
#endif

		replace(e, body, scope);
		return 1;
	}

	if (!is_var && !is_vis && def->tag == compound_tag) {
		exp c = child(def);
		int nuses = no(e);
		int changed = 0;

		for (;;) {
			int n = next(c)->tag;

			if (n == val_tag || n == real_tag || n == name_tag || n == null_tag) {
				exp u = pt(e);
				for (; nuses != 0 && u != NULL;) {
					exp nextu = pt(u);
#ifdef TDF_DIAG4
					if (!isdiaginfo(u) && no(u) == no(c) &&
					    eq_shape(sh(u), sh(next(c)))) {
#else
					if (no(u) == no(c) && eq_shape(sh(u), sh(next(c)))) {
#endif
						replace(u, copy(next(c)), next(def));
						nextu = pt(u); /* it could have changed */
						kill_exp(u, next(def));
						nuses--;
						changed = 1;
					}
					u = nextu;
				}
			}

			if (nuses == 0 || next(c)->last) {
				break;
			}

			c = next(next(c));
		}

		if ((no(e) == 0 || pt(e) == NULL) && !isenvoff(e)) {
			repbyseq(e);
			return 1;
		}

		return changed;
	}

	if (!is_var && !is_vis && def->tag == nof_tag) {
		exp c = child(def);
		int changed = 0;
		int nuses = no(e);
		int sz = rounder(shape_size(sh(c)), shape_align(sh(c)));
		int nd = 0;

		for (;;) {
			int n = c->tag;

			if (n == val_tag || n == real_tag || n == name_tag || n == null_tag) {
				exp u = pt(e);

				for (; nuses != 0 && u != NULL;) {
					exp nextu = pt(u);
#ifdef TDF_DIAG4
					if (!isdiaginfo(u) && no(u) == nd && eq_shape(sh(u), sh(c))) {
#else
					if (no(u) == nd && eq_shape(sh(u), sh(c))) {
#endif
						replace(u, copy(c), next(def));
						nextu = pt(u); /* it could have changed */
						kill_exp(u, next(def));
						nuses--;
						changed = 1;
					}
					u = nextu;
				}
			}

			if (nuses == 0 || c->last) {
				break;
			}

			c = next(c);
			nd += sz;
		}

		if ((no(e) == 0 || pt(e) == NULL) && !isenvoff(e)) {
			repbyseq(e);
			return 1;
		}

		return changed;
	}

	return 0;
}

