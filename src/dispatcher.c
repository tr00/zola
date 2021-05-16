/**
 * consists of one huge array for all refrenced objects.
 * a variable lookup will give back the index of the variable (if found)
 * 
 * 
 * 
 *       semantics                       dispatcher
 *
 *       lookup(x, ctx) ------------->  
 *                      <------------   ctx[hash(x)]
 *        declaration
 * 
 *       codegen
 * 
 *       get 4738     idx: 4738 -->
 *                                        
 */