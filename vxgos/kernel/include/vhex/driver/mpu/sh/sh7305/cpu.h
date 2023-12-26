#ifndef __VHEX_ARCH_SH7305_CPU__
# define __VHEX_ARCH_SH7305_CPU__

#include <vhex/defs/types.h>
#include <vhex/defs/attributes.h>

//---
// Complet CPU contexte
//---

struct sh7305_cpu_ctx {
    uintptr_t pr;
    uintptr_t spc;
    uintptr_t ssr;
    uintptr_t mach;
    uintptr_t macl;
    uintptr_t gbr;
    uintptr_t sgr;
    uintptr_t r[16];
};

//---
// Access to CPU registers
//---

/* cpu_get_vbr() : get the VBR register */
extern uintptr_t cpu_get_vbr(void);

/* cpu_set_vbr() : set the new VBR address */
extern void cpu_set_vbr(uint32_t VBR);




/* cpu_cpuopm_t : Bits of the CPU Operation Mode */
typedef lword_union(cpu_cpuopm_t,
    uint32_t        :22;
    uint32_t _HIGH  :4;    /* SOULD be 1 */
    uint32_t RABD   :1;    /* Speculative fetching */
    uint32_t        :1;
    uint32_t INTMU  :1;    /* Interrupt mode */
    uint32_t        :3;
);

/* cpu_get_cpuopm() : read the CPU Operation Mode register.  */
extern cpu_cpuopm_t cpu_get_cpuopm(void);

/* cpu_set_cpuopm() : Update the CPU Operation Mode register.
   After a write, the register is re-read and an (icbi) instruction is executed
   to apply the change. Non-writable bits should be left to their initial value
   during a write. */
extern void cpu_set_cpuopm(cpu_cpuopm_t cpuopm);




/* cpu_str_t: Bits of the Status Register */
typedef lword_union(cpu_sr_t,
    uint32_t        :1;
    uint32_t MD     :1;    /* Processing mode */
    uint32_t RB     :1;    /* Register bank */
    uint32_t BL     :1;    /* Exception / Interrupt Block */
    uint32_t RC     :12;   /* Repeat counter */
    uint32_t        :3;
    uint32_t DSP    :1;    /* Enable DSP */
    uint32_t DMY    :1;    /* Y pointer modulo addressing */
    uint32_t DMX    :1;    /* X pointer module addressing */
    uint32_t M      :1;    /* M bit (used in div0u, ...) */
    uint32_t Q      :1;    /* Q bit (used in div0u, ...) */
    uint32_t IMASK  :4;    /* Interrupt Mask */
    uint32_t RF     :2;    /* Repeat flags */
    uint32_t S      :1;    /* S bit (used with MAC operation) */
    uint32_t T      :1;    /* T but (true/false, condition, ...) */
);

/* cpu_get_sr() : get the Status Register */
extern cpu_sr_t cpu_get_sr(void);

/* cpu_set_st() : write the SR register.
   When writing, only "permanent" bits are set:
   * MD, RB, BL, DSP, IMASK are set.
   * M, Q, S and T are not set to preserve the behavior of ongoing divisions
     and tests. You can change T with (sett) and (clrt).
   * RC, DMY, DMX and DF are not set: use (setrc), (setdmx), (setdmy), and
     (clrdmxy). DF is preserved for old-style (setrc) loops to work. */
extern void cpu_set_sr(cpu_sr_t sr);



#endif /* __VHEX_ARCH_SH7305_CPU__ */
