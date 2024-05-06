#ifndef __VHEX_DMA__
# define __VHEX_DMA__

#include <vhex/dma/types.h>

/* dma_memcpy() : memcpy-like using the DMA

  As set in the short function description, we use the DMA transfert to perform
  the copy which allow us to do other things in parallele.

  This function act like memcpy except that it will not return the destination
  address but the dma ID that should be used to wait the end of the transfert
  afterwards. */
extern dma_id_t dma_memcpy(void * restrict dst, void * restrict src, size_t sz);

/* dma_memset() : memset using the DMA */
extern dma_id_t dma_memset(void *dst, int c, size_t sz);

/* dma_wait() : wait dma transfert end (dma_memset or dma_memcpy) */
extern int dma_wait(dma_id_t id);

#endif /* __VHEX_DMA__ */
