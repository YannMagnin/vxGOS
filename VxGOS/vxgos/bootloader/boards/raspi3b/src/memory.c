#include "raspi3b/memory.h"
#include "raspi3b/mailbox.h"
#include "raspi3b/uart.h"

//---
// Internals
//---

/* memory_alloc_reserve() : reserve a memory area */
static int memory_alloc_reserve(
    uint32_t *handle,
    volatile uint32_t *mbox,
    size_t size,
    int align,
    int flags
) {
    /* request memory area
     * @note
     * <> mbox[5] = handle
     * */
    mbox[0] = 9 * 4;
    mbox[1] = MBOX_REQUEST_CODE;
    mbox[2] = MBOX_TAG_MEMORY_ALLOC;
    mbox[3] = 12;
    mbox[4] = 12;
    mbox[5] = size;
    mbox[6] = align;
    mbox[7] = flags;
    mbox[8] = MBOX_TAG_END;

    if (mailbox_send(MBOX_CHANNEL_PROP) == 0)
        return -1;

    *handle = mbox[5];
    return 0;
}

/* memory_alloc_lock() : lock the reserved area and fetch its address */
static int memory_alloc_lock(
    uintptr_t *ptr,
    volatile uint32_t *mbox,
    uint32_t handle
) {
    /* request memory area
     * @note
     * <> mbox[5] = handle
     * */
    mbox[0] = 7 * 4;
    mbox[1] = MBOX_REQUEST_CODE;
    mbox[2] = MBOX_TAG_MEMORY_LOCK;
    mbox[3] = 4;
    mbox[4] = 4;
    mbox[5] = handle;
    mbox[6] = MBOX_TAG_END;

    if (mailbox_send(MBOX_CHANNEL_PROP) == 0)
        return -1;

    *ptr = mbox[5];
    return 0;
}

//---
// Public API
//---

/* memory_alloc() : malloc using GPU firmware request */
void *memory_alloc(size_t size, int align, int flags)
{
    volatile uint32_t *mbox;
    uint32_t handle;
    uintptr_t ptr;

    if (mailbox_get(&mbox) != 0) {
        uart_puts("[GPU]: memory_alloc: unable to fetch mbox\n");
        return NULL;
    }

    if (memory_alloc_reserve(&handle, mbox, size, align, flags) != 0) {
        uart_puts("[GPU]: memory_alloc: unable to fetch mbox\n");
        return NULL;
    }
    if (memory_alloc_lock(&ptr, mbox, handle) != 0) {
        uart_puts("[GPU]: memory_alloc: unable to fetch mbox\n");
        return NULL;
    }

    return (void *)ptr;
}

/* memory_free() : free using GPU firmware request */
void memory_free(void *ptr)
{
    uart_puts("[GPU] : FIXME : memory_free not implemented yet!\n");
    (void)ptr;
}
