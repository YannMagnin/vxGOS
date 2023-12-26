#include "raspi3b/mailbox.h"

//---
// Internals
//---

/* mbox : mailbox request information (MUST be 16-aligned) */
__attribute__((aligned(16)))
volatile uint32_t mailbox_buffer[36];

//---
// Public
//---

/* mailbox_get() : return the mailbox request buffer */
int mailbox_get(volatile uint32_t **mbox)
{
    if (mbox == NULL)
        return -1;
    *mbox = mailbox_buffer;
    return 0;
}

/* mailbox_send() : mailbox wrapper */
int mailbox_send(int channel)
{
    uintptr_t watermark_request;

    /* wait until we can write to the mailbox */
    do{
        __asm__ volatile("nop");
    } while(BCM2837_MAILBOX.MBOX_STATUS.FULL);

    /* Indicate a new request */
    watermark_request = (uintptr_t)mailbox_buffer | (channel & 0x0f);
    BCM2837_MAILBOX.MBOX_WRITE.lword = watermark_request;

    /* now wait for the response */
    while(1)
    {
        /* is there a response? */
        do{
            __asm__ volatile("nop");
        } while(BCM2837_MAILBOX.MBOX_STATUS.EMPTY);

        /* check if the GPU responded to us */
        if (BCM2837_MAILBOX.MBOX_READ.lword == watermark_request)
            return mailbox_buffer[1] == 0x80000000;
    }
    return 0;
}
