#ifndef VXGOS_BOOTLOADER_RASPI3B_MAILBOX_H
#define VXGOS_BOOTLOADER_RASPI3B_MAILBOX_H

#include "raspi3b/utils.h"

//---
// Public API
//---

/* MBOX_REQUEST_* : mailbox request tag */
enum {
    MBOX_REQUEST_CODE       = 0x00000000,
    MBOX_REQUEST_SUCCEED    = 0x80000000,
    MBOX_REQUEST_FAILED     = 0x80000001,
};

/* MBOX_TAG_* : mailbox supported tags
 * @note
 * <> some obscure TAG are missing
 * <> some obscure TAG can change with other raspi board
 * */
enum {
    // Request
    MBOX_TAG_END                    = 0x00000000,

    // Video Core
    MBOX_TAG_GET_FIRMWARE_REV       = 0x00000001,

    // Hardware
    MBOX_TAG_GET_BOARD_MODEL        = 0x00010001,
    MBOX_TAG_GET_BOARD_REV          = 0x00010002,
    MBOX_TAG_GET_BOARD_MAC          = 0x00010003,
    MBOX_TAG_GET_BOARD_SERIAL       = 0x00010004,
    MBOX_TAG_GET_ARM_MEMORY         = 0x00010005,
    MBOX_TAG_GET_VC_MEMORY          = 0x00010006,
    MBOX_TAG_GET_CLOCKS             = 0x00010007,

    // Config
    MBOX_TAG_GET_CMDLINE            = 0x00050001,

    // Shared Ressource Management
    MBOX_TAG_GET_DMA_CHANNELS       = 0x00060001,

    // Power
    MBOX_TAG_GET_POWER_STATE        = 0x00020001,
    MBOX_TAG_SET_POWER_STATE        = 0x00028001,
    MBOX_TAG_GET_POWER_TIMING       = 0x00020002,

    // Clocks
    MBOX_TAG_GET_CLOCK_STATE        = 0x00030001,
    MBOX_TAG_SET_CLOCK_STATE        = 0x00038001,
    MBOX_TAG_GET_CLOCK_RATE         = 0x00030002,
    MBOX_TAG_SET_CLOCK_RATE         = 0x00038002,
    MBOX_TAG_GET_CLOCK_RATE_MAX     = 0x00030004,
    MBOX_TAG_GET_CLOCK_RATE_MIN     = 0x00030007,
    MBOX_TAG_GET_CLOCK_RATE_REAL    = 0x00030047,
    MBOX_TAG_GET_TURBO              = 0x00030009,
    MBOX_TAG_SET_TURBO              = 0x00038009,

    // Leds
    MBOX_TAG_GET_BOARD_LED_STATUS   = 0x00030041,
    MBOX_TAG_TEST_BOARD_LED_STATUS  = 0x00034041,
    MBOX_TAG_SET_BOARD_LED_STATUS   = 0x00038041,

    // Voltage
    MBOX_TAG_GET_VOLTAGE            = 0x00030003,
    MBOX_TAG_SET_VOLTAGE            = 0x00038003,
    MBOX_TAG_GET_VOLTAGE_MAX        = 0x00030005,
    MBOX_TAG_GET_VOLTAGE_MIN        = 0x00030008,

    // Temperature
    MBOX_TAG_GET_TEMPERATURE        = 0x00030006,
    MBOX_TAG_GET_TEMPERATURE_MAX    = 0x0003000a,

    // Memory management
    MBOX_TAG_MEMORY_ALLOC           = 0x0003000c,
    MBOX_TAG_MEMORY_LOCK            = 0x0003000d,
    MBOX_TAG_MEMORY_UNLOCK          = 0x0003000e,
    MBOX_TAG_MEMORY_FREE            = 0x0003000f,

    // Screen (HDMI/DVI)
    MBOX_TAG_SCREEN_BUFF_ALLOC              = 0x00040001,
    MBOX_TAG_SCREEN_BUFF_FREE               = 0x00048001,
    MBOX_TAG_SCREEN_BLINK                   = 0x00040002,
    MBOX_TAG_SCREEN_GET_PHYS_SIZE           = 0x00040003,
    MBOX_TAG_SCREEN_SET_PHYS_SIZE           = 0x00048003,
    MBOX_TAG_SCREEN_TEST_PHYS_SIZE          = 0x00044003,
    MBOX_TAG_SCREEN_GET_VIRT_SIZE           = 0x00040004,
    MBOX_TAG_SCREEN_SET_VIRT_SIZE           = 0x00048004,
    MBOX_TAG_SCREEN_TEST_VIRT_SIZE          = 0x00044004,
    MBOX_TAG_SCREEN_GET_PIXEL_DEPTH         = 0x00040005,
    MBOX_TAG_SCREEN_SET_PHYS_DEPTH          = 0x00048005,
    MBOX_TAG_SCREEN_TEST_PIXEL_DEPTH        = 0x00044005,
    MBOX_TAG_SCREEN_GET_PIXEL_ORDER         = 0x00040006,
    MBOX_TAG_SCREEN_SET_PIXEL_ORDER         = 0x00048006,
    MBOX_TAG_SCREEN_TEST_PIXEL_ORDER        = 0x00044006,
    MBOX_TAG_SCREEN_GET_PIXEL_ALPHA_MODE    = 0x00040007,
    MBOX_TAG_SCREEN_SET_PIXEL_ALPHA_MODE    = 0x00048007,
    MBOX_TAG_SCREEN_TEST_PIXEL_ALPHA_MODE   = 0x00044007,
    MBOX_TAG_SCREEN_GET_PITCH               = 0x00040008,
    MBOX_TAG_SCREEN_GET_VIRTUAL_OFFSET      = 0x00040009,
    MBOX_TAG_SCREEN_SET_VIRTUAL_OFFSET      = 0x00048009,
    MBOX_TAG_SCREEN_TEST_VIRTUAL_OFFSET     = 0x00044009,
    MBOX_TAG_SCREEN_GET_OVERSCAN            = 0x0004000a,
    MBOX_TAG_SCREEN_SET_OVERSCAN            = 0x0004800a,
    MBOX_TAG_SCREEN_TEST_OVERSCAN           = 0x0004400a,
    MBOX_TAG_SCREEN_GET_PALETTE             = 0x0004000b,
    MBOX_TAG_SCREEN_SET_PALETTE             = 0x0004800b,
    MBOX_TAG_SCREEN_TEST_PALETTE            = 0x0004400b,
    MBOX_TAG_SCREEN_SET_CURSOR_INFO         = 0x00008010,
    MBOX_TAG_SCREEN_SET_CURSOR_STATE        = 0x00008011,
    MBOX_TAG_SCREEN_SET_GAMMA               = 0x00008012,
};

/* MBOX_CHANNEL_* : mailbox channel ID */
enum {
    MBOX_CHANNEL_POWER          = 0,
    MBOX_CHANNEL_FRAMEBUFFER    = 1,
    MBOX_CHANNEL_VUART          = 2,
    MBOX_CHANNEL_VCHIQ          = 3,
    MBOX_CHANNEL_LEDS           = 4,
    MBOX_CHANNEL_BTNS           = 5,
    MBOX_CHANNEL_TOUCH          = 6,
    MBOX_CHANNEL_COUNT          = 7,
    MBOX_CHANNEL_PROP           = 8,
};

/* mailbox_get() : return the mailbox request buffer */
extern int mailbox_get(volatile uint32_t **mbox);

/* mailbox_send() : send request to the mailbox */
extern int mailbox_send(int channel);

//---
// Mailbox mecanism
//
// @note
// <> mailbox seems to be the only way to communicate with the VideoCore
//      firmware which seems to contains a lot of critical primitive. The
//      firmware is in closed source, so all information written here are
//      glanned using multiple source:
//
//      - https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes
//      - https://github.com/bztsrc/raspi3-tutorial
//      - https://github.com/raspberrypi/firmware/blob/master/hardfp/opt/vc/bin/vcmailbox
//      - https://www.rpi4os.com/part5-framebuffer/
//---

/* __bcm2837_mailbox : mailbox module structure */
struct __bcm2837_mailbox
{
    /* [0x0020b880] Read "register" */
    u32_union(MBOX_READ,
        uint32_t    CHANNEL     :4;
        uint32_t    DATA        :28;
    );
    pad(0x0c);

    /* [0x0020b890] Unknown register (rpi4os.com/part5-framebuffer) */
    const uint32_t MBOX_POLL;
    /* [0x0020b894] Unknown register (rpi4os.com/part5-framebuffer) */
    const uint32_t MBOX_SENDER;

    /* [0x0020b898] Status regsiter */
    u32_union(MBOX_STATUS,
        uint32_t                :24;
        uint32_t EMPTY          :1;     /* indicate if the MBOX is empty */
        uint32_t FULL           :1;     /* indicate if the MBOX is full */
    );

    /* [0x0020b89c] Configuration register */
    //TODO : bitfields
    uint32_t MBOX_CONFIG;

    /* [0x0020b8a0] Write register */
    u32_union(MBOX_WRITE,
        uint32_t CHANNEL        :4;
        uint32_t DATA           :28;
    );
} VPACKED(4);

#define BCM2837_MAILBOX (*((volatile struct __bcm2837_mailbox*)0x3f00b880))

#endif /* VXGOS_BOOTLOADER_RASPI3B_MAILBOX_H */
