/**
 * AST to IR compiler 
 */
#ifndef __ZL3_H_
#define __ZL3_H_

struct ZL_CONTEXT {
    struct ZL_CONTEXT* parent;
};

void visit_type(char*, struct AST_NODE*, struct ZL_CONTEXT*);

#endif
