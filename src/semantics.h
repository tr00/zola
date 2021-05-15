/**
 * AST to IR compiler 
 */
#ifndef __SEMANTICS_H_
#define __SEMANTICS_H_

struct SEXPR;

struct ZL_CONTEXT {
    struct ZL_CONTEXT* parent;
};

void analyze(struct SEXPR*);
void visit_list(struct SEXPR*, struct ZL_CONTEXT*);
void visit_call(struct SEXPR*, struct ZL_CONTEXT*);
void visit_atom(struct SEXPR*, struct ZL_CONTEXT*);
void visit_node(struct SEXPR*, struct ZL_CONTEXT*);
void assert_type(char*, struct SEXPR*, struct ZL_CONTEXT*);

#endif
