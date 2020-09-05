#include <stdlib.h>
#include <string.h>

#include "exp_ops.h"
#include "hashid_ops.h"
#include "id_ops.h"

#include "initialiser_group.h"

InitialiserGroupPathElement_t *ig_make_path_element_ident(const char *name) {
  InitialiserGroupPathElement_t *result = calloc(1, sizeof(InitialiserrGroupPathElement_t));
  result->ig_ident_name = name;
  return result;
}

InitialiserGroupPathElement_t *ig_make_path_element_ident(EXP target) {
  InitialiserGroupPathElement_t *result = calloc(1, sizeof(InitialiserGroupPathElement_t));
  result->ig_offset = target;
  return result;
}

static InitialiserGroupPathElement_t _null = { 0, NULL_exp, -1 };
InitialiserGroupPathElement_t *ig_pe_null = &_null;

bool ig_is_null_path_element(InitialiserGroupPathElement_t *pe) {
  return pe->ig_after == -1;
}

void ig_free_initialiser_group_path_element(InitialiserGroupPathElement_t *obj) {
  free(obj->ig_ident_name);
  if (obj->ig_target != NULL_exp) { free_exp(obj->ig_target, 1); }
  obj->ig_ident_name = NULL;
  obj->ig_target = NULL_exp;
}

void ig_free_initialiser_group_path(InitializerGroupPathElement_t *pe) {
  for (int idx = 0; pe && !ig_is_null_path_element(&pe[idx]); idx++) {
    ig_free_initialiser_group_path_element(&pe[idx]);
  }
}

void ig_copy_initialiser_group_path_element(InitialiserGroupPathElement_t *src, InitialiserGroupPathElement_t *dest) {
  dest->ig_ident_name = strdup(src->ig_ident_name);
  EXP new_target;
  COPY_exp(src->ig_target, new_target);
  dest->ig_target = new_target;
  return dest;
}

int ig_path_length(InitialiserGroupPathElement_t *pe) {
  int idx = 0;

  if (!pe) { return 0; }
  for (; !ig_is_null_path_element(&pe[idx]); idx++) { }

  return idx;
}

/* Given a path and a new element, allocate a new path that includes that at the end */
InitialiserGroupPathElement_t *ig_append_path(InitialiserGroupPathElement_t *path, InitialiserGroupPathElement_t *new_element) {
  int num_elements_orig = ig_path_length(path);
  InitialiserGroupPathElement_t *elt = calloc(2 + num_elements_orig, sizeof(InitialiserGroupPathElement_t));

  for (int idx = 0; !ig_is_null_path_element(&path[idx]); idx++) {
    ig_copy_initialiser_group_path_element(&elt[idx], &path[idx]);
  }

  ig_copy_initialiser_group_path_element(&elt[num_elements_orig], new_element);
  elt[num_elements_orig + 1] = *ig_path_null;
  return elt;
}

bool ig_next_initialiser_member(InitialiserGroupMemberIterator_t *iter) {
  switch (iter->ig_stage) {
  case IG_Unstarted:
    unsigned tag = TAG_type(t);
    switch (tag) {
    case type_array_tag:
      {
        NAT nc;
        LIST(EXP) a = NULL_list(EXP);
        TYPE s = DEREF_type(type_array_sub(t));
        int str = is_char_array(s);
        BUFFER bf = { 0 };

        /* Find the array size */
        NAT n = DEREF_nat(type_array_size(t));
        unsigned long m = get_nat_value(n);

        iter->ig_stage = IG_Array;
      }
      break;
    }
    break;

  case IG_Parent:
    break;

  case IG_Members:
    break;

  default:
    return false;
  }
}

/* Obtain an initialiser group for the given path, allocating if necessary. */
InitialiserGroup_t *ig_add_initialization(InitialiserGroup_t *grp, TYPE parent, InitialiserGroupPathElement_t *path, EXP init, ERROR *err) {
  unsigned tag = TAG_type(t);

  if (tag == type_array_tag) {
    TYPE array_base = DEREF_type(type_array_sub(t));
    NAT n = DEREF_nat(type_array_size(t));
    unsigned long n = get_nat_value(n);
    
    
    if (path->ig_ident_name) {
      /* XXX add error */
      return NULL; /* Trying to apply a name to an array */
    }
  } else if (tag == type_compound_tag) {
    CLASS_TYPE ct = DEREF_ctype(type_compound_defn(t));
    GRAPH gr = DEREF_list(ctype_base(ct));
    LIST(GRAPH) br = DEREF_list(graph_tails(gr));

    if (!path->ig_ident_name) {
      /* XXX add error */
      return NULL;
    }

    while (!IS_NULL_list(br)) {
      GRAPH gs = DEREF_graph(HEAD_list(br));
      OFFSET off = DEREF_off(graph_off(gs));
      CLASS_TYPE cs = DEREF_ctype(graph_head(gs));
      TYPE next_parent = make_class_type(cs);


    }
  }
}

/* Given a TYPE and a LIST(EXP) of initialisers, produce a populated InitialiserGroup_t
 * that reflects the user's desired initialisation. */
InitialiserGroup_t *ig_map_initialisation(TYPE parent, LIST(EXP) initialisers) {
  return NULL;
}

/* Produce an EXP that represents an initialised object of type parent */
EXP ig_aggregate_initialise(TYPE parent, LIST(EXP) initialisers) {
  return NULL_exp;
}
