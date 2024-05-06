#include <vhex/driver/cpu.h>
#include <vhex/driver/mpu/sh/sh7305/cpu.h>

/* Value of IMASK when atomic mode is entered */
static int saved_IMASK = 0;
/* Number of atomic mode levels */
static unsigned int atomic_level = 0;

/* cpu_atomic_start(): Enter atomic mode */
void cpu_atomic_start(void)
{
    /* There is no access problem to IMASK here because interrupts must
       preserve and restore it */
    cpu_sr_t SR = cpu_get_sr();
    cpu_sr_t SR2 = SR;
    SR2.IMASK = 15;
    cpu_set_sr(SR2);

    /* Now that we're alone, atomically update the atomic level */
    if(atomic_level == 0) saved_IMASK = SR.IMASK;
    atomic_level++;
}

/* cpu_atomic_end(): Exit atomic mode */
void cpu_atomic_end(void)
{
    cpu_sr_t SR = cpu_get_sr();

    /* Update atomic_level before restoring interrupts */
    atomic_level--;
    if(atomic_level == 0) {
        SR.IMASK = saved_IMASK;
        saved_IMASK = 0;
    }

    cpu_set_sr(SR);
}
