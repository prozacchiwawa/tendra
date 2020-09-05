#ifndef _INITIALISER_GROUP_H_
#define _INITIALISER_GROUP_H_

struct EXP;

/* One or the other should be non-null. */
typedef struct _InitialiserGroupPathElement_t {
  char *ig_ident_name;
  EXP ig_target;
  int ig_after; /* Represents the index of an undesignated initialiser after
                 * the given coordinate */
} InitialiserGroupPathElement_t;

/* represents a group of initializers with the same path up to some level. */
typedef struct _InitialiserGroup_t {
  InitialiserGroupPathElement ig_path_element;
  InitialiserGroup_t *ig_subinits;
  TYPE t; /* Type at this level */
  EXP ig_this_initialiser;
} InitialiserGroup_t;

extern InitialiserGroupPathElement_t ig_pe_null;

typedef enum _InitialiserGroupMemberIteratorStage_t {
  IG_Unstarted,
  IG_Parent,
  IG_Members,
  IG_Array
} InitialiserGroupMemberIteratorStage_t;

typedef struct _InitialiserGroupMemberIterator_t {
  InitialiserGroupMemberIteratorStage_t ig_stage;
  TYPE ig_t;
  CLASS_TYPE ig_ct;
  NAMESPACE ig_ns;
  MEMBER ig_mem;
  GRAPH ig_gr;
  InitialiserGroupMemberIterator_t *ig_parent;
  InitialiserGroupPathElement_t *ig_member_path;
} InitialiserGroupMemberIterator_t;

/* Move ahead with iniitialization member iterator */
bool ig_next_initialisable_member(InitialiserGroupMemberIterator_t *iter);

/* Produce an EXP that represents an initialised object of type parent */
EXP ig_aggregate_initialise(TYPE parent, LIST(EXP) initialisers);

#endif/*_INITIALISER_GROUP_H_*/
