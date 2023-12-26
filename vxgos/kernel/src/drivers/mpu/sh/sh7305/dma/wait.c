#include <vhex/driver/mpu/sh/sh7305/dma.h>

//---
// Internal functions
//---

static int sh7305_dma_wait_core(sh7305_dma_id_t id, int atomic)
{
    sh7305_dma_channel_t *ch;

    ch = sh7305_dma_channel_find(id);
    if (ch == NULL)
        return -1;

    if (ch->id.KEYFRAME == id.KEYFRAME)
        sh7305_dma_channel_wait(ch, atomic);
    return 0;
}

//---
// User-level API
//---

/* dma_wait() : wait the end of the DMA channel transfer */
int sh7305_dma_wait(dma_id_t id)
{
    sh7305_dma_id_t _id;

    _id.lword = id;
    return sh7305_dma_wait_core(_id, 0);
}

/* dma_wait() : wait the end of the DMA channel transfer without interruption */
int sh7305_dma_spinwait(dma_id_t id)
{
    sh7305_dma_id_t _id;

    _id.lword = id;
    return sh7305_dma_wait_core(_id, 1);
}
