#include <stdint.h>
#include <stddef.h>

#include "raspi3b/board.h"
#include "raspi3b/uart.h"
#include "raspi3b/mailbox.h"

//---
// Internals
//---

/* board_get_generic_u32() : generic mailbox call for u32 response */
static uint32_t board_get_generic_u32(uint32_t tag)
{
    volatile uint32_t *mbox;

    if (mailbox_get(&mbox) != 0)
        return 0x55555555;

    mbox[0] = 7 * 4;
    mbox[1] = MBOX_REQUEST_CODE;
    mbox[2] = tag;
    mbox[3] = 4;
    mbox[4] = MBOX_REQUEST_CODE;
    mbox[5] = 0;
    mbox[6] = MBOX_TAG_END;

    if (mailbox_send(MBOX_CHANNEL_PROP) == 0)
        return 0x77777777;

    return mbox[5];
}

/* board_get_revision() : request the board revision */
static uint32_t board_get_revision(void)
{
    return board_get_generic_u32(MBOX_TAG_GET_BOARD_REV);
}

/* board_get_model() : request the board model */
static uint32_t board_get_model(void)
{
    return board_get_generic_u32(MBOX_TAG_GET_BOARD_MODEL);
}

/* board_get_firmware_ver() : request the GPU firmware revision */
static uint32_t board_get_firmware_ver(void)
{
    return board_get_generic_u32(MBOX_TAG_GET_FIRMWARE_REV);
}

//---
// Public
//---

/* board_init() : initialize board information */
int board_init(void)
{
    uart_putf("- board model          = %p\n", board_get_model());
    uart_putf("- board revision       = %p\n", board_get_revision());
    uart_putf("- GPU firmware version = %p\n", board_get_firmware_ver());
    return 0;
}
