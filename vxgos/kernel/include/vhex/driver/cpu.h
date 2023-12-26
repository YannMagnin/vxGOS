#ifndef __VHEX_DRIVERS_CPU__
# define __VHEX_DRIVERS_CPU__

#include <stddef.h>
#include <stdint.h>

/* cpu_atomic_start(): Enter atomic mode

   This function enters "atomic mode", a mode where distractions to the CPU
   (mainly interrupts) are disabled. This is useful when doing critical
   operations on the hardware, because it ensures that no other code will see
   any intermediate state between the start and end of the atomic mode, thereby
   making the sequence atomic to other code.

   Atomic mode disables interrupts with IMASK=15, however it does not set BL=1
   because exceptions never occur on their own (and it is desirable to have
   panic reports if the atomic code is buggy), and TLB misses are almost always
   desirable. If you want to set BL=1, you can do so with cpu_setSR().

   This function uses a mutex so atomic mode can be started within atomic code;
   every cpu_atomic_start() must be paired with exactly one cpu_atomic_end().
   Entering atomic mode several times does not affect the CPU state, however
   atomic mode will be exited only after all exits have been completed.

   Once atomic mod is exited the original value of IMASK at the first call to
   cpu_atomic_start() is restored. */
extern void cpu_atomic_start(void);

/* cpu_atomic_end(): Exit atomic mode
   There should be exactly one cpu_atomic_end() for each cpu_atomic_start(). */
extern void cpu_atomic_end(void);


/* sleep(): Put the processor to sleep

   This function uses the [sleep] instruction to put the processor in sleep
   mode. This allows reduced energy consumption while waiting for interrupts.

   In some certain situations, sleeping would block the process that is being
   waited for (generally when on-chip memory is involved). When this occurs,
   this function will not sleep at all and instead return instantly.

   The indented use for sleep() is *always* in some sort of loop, as there is
   no guarantee about time or interrupts elapsed before this function
   returns. */
extern void cpu_sleep(void);

#endif /* __VHEX_DRIVERS_CPU__ */
