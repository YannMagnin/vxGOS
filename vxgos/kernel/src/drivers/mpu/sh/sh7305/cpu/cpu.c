#include <vhex/driver.h>
#include <vhex/driver/mpu/sh/sh7305/cpu.h>

#include <string.h>

/* define the private CPU context structure */
struct cpu_ctx {
    uintptr_t vbr;
    cpu_cpuopm_t cpuopm;
    cpu_sr_t sr;
};

/* hardware configuration call */

/* __cpu_configure() : configure the CPU */
static void __cpu_configure(struct cpu_ctx *state)
{
    extern uintptr_t vhex_vbr;

    memset(state, 0x00, sizeof(struct cpu_ctx));

    /* by default, use the vhex VBR */
    state->vbr = (uintptr_t)&vhex_vbr;

    /* configure the CPU Operation Mode
        <> enable speculative execution (rip Spectre)
        <> enable SR.IMASK update when an interruption occur */
    state->cpuopm.RABD = 0;
    state->cpuopm.INTMU = 1;
    state->cpuopm._HIGH = 0b1111;

    /* configure the Status Register
        <> force the privileged mode
        <> enable DSP
        <> mask interruptions (it will be unmasked in <hypervisor/switch>)*/
    state->sr.MD  = 1;
    state->sr.RB  = 0;
    state->sr.BL  = 0;
    state->sr.DSP = 1;
    state->sr.IMASK = 0b1111;
}

/* hardware hypervisor call */

/* __cpu_hsave() : save hardware information */
static void __cpu_hsave(struct cpu_ctx *state)
{
    state->vbr = cpu_get_vbr();
    state->cpuopm = cpu_get_cpuopm();
    state->sr = cpu_get_sr();
}

/* __cpu_hrestore() : restore hardware information */
static void __cpu_hrestore(struct cpu_ctx *state)
{
    cpu_set_vbr(state->vbr);
    cpu_set_cpuopm(state->cpuopm);
    cpu_set_sr(state->sr);
}

/* declare the CPU driver */

struct vhex_driver drv_cpu = {
    .name       = "CPU",
    .hsave      = (void*)&__cpu_hsave,
    .hrestore   = (void*)&__cpu_hrestore,
    .configure  = (void*)&__cpu_configure,
    .state_size = sizeof(struct cpu_ctx)
};
VHEX_DECLARE_DRIVER(02, drv_cpu);
