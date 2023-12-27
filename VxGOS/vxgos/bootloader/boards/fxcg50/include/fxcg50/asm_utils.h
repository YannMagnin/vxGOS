#ifndef FXCG50_BOOTLOADER_ASM_UTILS_H
#define FXCG50_BOOTLOADER_ASM_UTILS_H 1

/* function() : define a function using special indication
 *
 * - the function's name must start with a '_'
 * - the function should start in a 4-aligned address in order to benefit from
 *   the ILP (Instruction Level Parallelism) */
#define function(name)  \
.balign 4               ;\
.global _ ## name       ;\
_ ## name

/* FXCG50_SYSCALL_TRAMPOLINE - Casio's syscall trampoline address */
#define FXCG50_SYSCALL_TRAMPOLINE   0x80020070

/* syscall() : helper used to quickly define Casio syscall invokation
 *
 * - the syscall trampoline code is common for all syscall
 * - r0 contain the syscall ID */
#define syscall(id)                     \
    mov.l   1f, r2                      ;\
    mov.l   2f, r0                      ;\
    jmp     @r2                         ;\
    nop                                 ;\
.balign 4                               ;\
1: .long    FXCG50_SYSCALL_TRAMPOLINE   ;\
2: .long    id

#endif /* FXCG50_BOOTLOADER_ASM_UTILS_H */
