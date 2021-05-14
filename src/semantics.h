/**
 * AST to IR compiler 
 */
#ifndef __SEMANTICS_H_
#define __SEMANTICS_H_

struct AST_NODE;

struct ZL_CONTEXT {
    struct ZL_CONTEXT* parent;
};

void visit_node(struct AST_NODE*, struct ZL_CONTEXT*);
void visit_type(char*, struct AST_NODE*, struct ZL_CONTEXT*);

#endif
