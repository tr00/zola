/**
 * AST to IR compiler 
 */
#ifndef __SEMANTICS_H_
#define __SEMANTICS_H_

struct SEXPR;

struct ZL_CONTEXT {
    struct ZL_CONTEXT* parent;
};

struct SEXPR* analyze(struct SEXPR*);
struct SEXPR* visit_list(struct SEXPR*, struct ZL_CONTEXT*);
struct SEXPR* visit_call(struct SEXPR*, struct ZL_CONTEXT*);
struct SEXPR* visit_atom(struct SEXPR*, struct ZL_CONTEXT*);
struct SEXPR* visit_node(struct SEXPR*, struct ZL_CONTEXT*);
void assert_type(char*, struct SEXPR*, struct ZL_CONTEXT*);

#endif
