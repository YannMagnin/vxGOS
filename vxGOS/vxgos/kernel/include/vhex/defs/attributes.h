//---
//  vhex:defs:attributes - Macros for compiler-specific attributes
//---

#ifndef __VHEX_DEFS_ATTRIBUTES__
# define __VHEX_DEFS_ATTRIBUTES__

/* Objects from specific sections */
#define VSECTION(x)         __attribute__((section(x)))

/* Disable Stack Protector */
#define VNO_STACK_PROTECTOR __attribute__((no_stack_protector))

/* Unused parameters or variables */
#define VUNUSED             __attribute__((unused))
/* Functions that *must* be inlined */
#define VINLINE             __attribute__((always_inline)) inline

/* Aligned variables */
#define VALIGNED(x)         __attribute__((aligned(x)))
/* Packed structures. I require explicit alignment because if it's
 * unspecified, GCC cannot optimize access size, and reads to memory-mapped
 * I/O with invalid access sizes silently fail - honestly you don't want
 * this to happen */
#define VPACKED(x)          __attribute__((packed, aligned(x)))
/* Packed enumerations */
#define VPACKED_ALONE       __attribute__((packed))
/* Transparent unions */
#define VTRANSPARENT        __attribute__((transparent_union))

/* Weak symbols */
#define VWEAK               __attribute__((weak))

/* Functions that do not return */
#define VNORETURN           __attribute__((noreturn))

#endif /*__VHEX_DEFS_ATTRIBUTES__*/
