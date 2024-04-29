#ifndef FXCG50_KERNEL_ASM_UTILS_H
#define FXCG50_KERNEL_ASM_UTILS_H 1

/* function() : define a function using special indication
 *
 * - the function's name must start with a '_'
 * - the function should start in a 4-aligned address in order to benefit
 *   of the ILP (Instruction Level Parallelism) */
#define function(name)  \
.balign 4               ;\
.global _ ## name       ;\
_ ## name

#endif /* FXCG50_KERNEL_ASM_UTILS_H */
