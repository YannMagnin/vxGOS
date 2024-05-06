#ifndef VHEX_KERNEL_H
#define VHEX_KERNEL_H 1

//---
// Public
//---

/* kernel_initialise() : generic kernel initialisation
 * (theorically, should never be called) */
extern void kernel_initialise(void);

/* kernel_panic() : kernel panic
 * (provided by the selected board) */
extern _Noreturn void kernel_panic(char const * const text);

#endif /* VHEX_KERNEL_H */
