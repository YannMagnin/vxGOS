//---
//  vhex:mmu - Memory Management Unit
//---

#ifndef __VHEX_MMU__
# define __VHEX_MMU__

#include <vhex/defs/types.h>

/* mmu_translate(): Get the physical address for a virtual page
   Looks for a translation with the specified virtual address as start, and
   returns the corresponding physical address. Only works if the argument is
   page-aligned. */
extern uintptr_t mmu_translate(uintptr_t page, size_t *size);

/* mmu_uram_size(): Get size of user RAM area

   Returns the size of the static memory at 0x08100000, whose address is
   returned by mmu_uram(). This is typically 8k on SH3 fx-9860G, 32k on SH4
   fx-9860G, and 512k on fx-CG 50. */
extern size_t mmu_uram_size(void);

#endif /*__VHEX_MMU__*/
