#ifndef __BUILTINS_H_
#define __BUILTINS_H_

enum {
// arithmetic
    __ADD__,
    __SUB__,
    __MUL__,
    __DIV__, __UDIV__,
    __REM__, __UREM__,
    __SHL__,
    __SHR__,
    __SAR__,
    __AND__,
    __OR__,
    __XOR__,

// comparisons
    __CEQL__, __CEQW__,
    __CNEL__, __CNEW__,
    __CSLEL__, __CSLEW__,
    __CULEL__, __CULEW__,
    __CSLTL__, __CSLTW__,
    __CULTL__, __CULTW__,
    __CSGEL__, __CSGEW__,
    __CUGEL__, __CUGEW__,
    __CSGTL__, __CSGTW__,
    __CUGTL__, __CUGTW__,

    __CEQD__, __CEQS__,
    __CNED__, __CNES__,
    __CLED__, __CLES__,
    __CLTD__, __CLTS__,
    __CGED__, __CGES__,
    __CGTD__, __CGTS__,
    __COD__, __COS__, /* not NaN */
    __CUOD__, __CUOS__, /* is NaN */

// conversion

    __DTOSI__, __STOSI__,
    __SLTOF__, __SWTOF__,
    __TRUNCD__, __EXTS__,

    __COPY__, __CAST__,

};

#endif