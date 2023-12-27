//---
//  vhex:mpu:pfc - Pin Function Controller
//
//  The Pin Function Controller has a simple register interface, the main
//  difficulty is still understanding the role of its pins.
//---

#ifndef __VHEX_MPU_SH7305_PFC__
# define __VHEX_MPU_SH7305_PFC__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

//---
//  SH7305 Pin Function Controller. Refer to:
//    "Renesas SH7705 Group Hardware Manual"
//    Section 19: "Pin Function Controller"
//
// Note that we don't have any reliable information concerning this module, this
// is why we don't havec any bitfields here.
//---

struct sh7305_pfc
{
    /* Control registers */
    uint16_t PACR;
    uint16_t PBCR;
    uint16_t PCCR;
    uint16_t PDCR;
    uint16_t PECR;
    uint16_t PFCR;
    uint16_t PGCR;
    uint16_t PHCR;
    uint16_t PJCR;
    uint16_t PKCR;
    uint16_t PLCR;
    uint16_t SCPCR;         /* Port SC control register */
    uint16_t PMCR;
    uint16_t PNCR;

    pad(4);

    /* Data registers */
    uint8_t PADR;
    pad(1);
    uint8_t PBDR;
    pad(1);
    uint8_t PCDR;
    pad(1);
    uint8_t PDDR;
    pad(1);
    uint8_t PEDR;
    pad(1);
    uint8_t PFDR;
    pad(1);
    uint8_t PGDR;
    pad(1);
    uint8_t PHDR;
    pad(1);
    uint8_t PJDR;
    pad(1);
    uint8_t PKDR;
    pad(1);
    uint8_t PLDR;
    pad(1);
    uint8_t SCPDR;      /* Port SC data register */
    pad(1);
    uint8_t PMDR;
    pad(1);
    uint8_t PNDR;
    pad(1);

} VPACKED(4);

#define SH7305_PFC (*((volatile struct sh7305_pfc *)0xa4000100))


#endif /*__VHEX_MPU_SH7305_PFC__*/
