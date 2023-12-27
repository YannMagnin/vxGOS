#ifndef __VHEX_HARDWARE_H__
# define __VHEX_HARDWARE_H__


/* For compatibility with ASM, include the following bits only in C code */
#ifndef ASM_SOURCE

#include <vhex/defs/types.h>

/* hw_detect(): Basic hardware detection */
extern void hw_detect(void);

/* Most of the information here is going to be stored in (key, value) pairs for
   predetermined keys and 32-bits values that are often integers or a set of
   flags. The data will be filled by vhex or its drivers. */
#define HW_KEYS 16
extern uintptr_t vhex[HW_KEYS];

#endif /* ASM_SOURCE */

/*
**  Key list
*/

#define HWMPU                   0 /* MPU type */
#define HWCPUVR                 1 /* CPU Version Register */
#define HWCPUPR                 2 /* CPU Product Register */
#define HWDEVICE                3 /* Device model */
#define HWRAM_PHY_USER_START    4 /* Physical RAM address (user start) */
#define HWRAM_PHY_USER_END      5 /* Physical RAM address (user end) */
#define HWRAM_PHY_ORIGIN        6 /* Physical RAM address (origin) */
#define HWRAM_PHY_SIZE          7 /* Physical RAM size */
#define HWRAM_PHY_END           8 /* Physical RAM address (end) */

#endif /*__VHEX_HARDWARE_H__*/
