#include "raspi3b/framebuffer.h"
#include "raspi3b/mailbox.h"
#include "raspi3b/uart.h"

//---
// Internals
//---

/* frambuffer - internal framebuffer information */
static struct framebuffer framebuffer = {
    .watermark = 0x00000000,
    .vram = {
        .gpu   = (uintptr_t)NULL,
        .cpu   = (uintptr_t)NULL,
        .size  = 0,
        .pitch = 0,
        .align = 0,
    },
    .phys = {
        .width = 0,
        .height = 0,
    },
    .virt = {
        .width = 0,
        .height = 0,
    },
};

/* framebuffer_setup_request() : perpare mailbox request */
static void framebuffer_setup_prepare(volatile uint32_t *mbox)
{
    /* prepare mailbox request
     * @note
     * <> 1024x768
     * <> 32bits per pixel (TODO: palette ?)
     * <> start offset at x=0 and y=0
     * */
    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST_CODE;

    /* set the physical screen size
     * @note
     * <> 1024x768
     * <> mbox[5] = FrameBufferInfo.width
     * <> mbox[6] = FrameBufferInfo.height
     * */
    mbox[2] = MBOX_TAG_SCREEN_SET_PHYS_SIZE;
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;
    mbox[6] = 768;

    /* set the virtual screen size
     * @note
     * <> 1024x768
     * <> mbox[10] = FrameBufferInfo.virtual_width
     * <> mbox[11] = FrameBufferInfo.virtual_height
     * */
    mbox[7] = MBOX_TAG_SCREEN_SET_VIRT_SIZE;
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;
    mbox[11] = 768;

    /* set the virtual X/Y offset
     * @note
     * <> x=0 and y=0
     * <> mbox[14] = FrameBufferInfo.x_offset
     * <> mbox[15] = FrameBufferInfo.y_offset
     * */
    mbox[12] = MBOX_TAG_SCREEN_SET_VIRTUAL_OFFSET;
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;
    mbox[16] = 0;

    /* set the pixel depth (bits per pixels)
     * @note
     * <> for now, use classic RGB(A?) 32bits encoding
     * <> mbox[20] = FrameBufferInfo.depth
     * */
    mbox[17] = MBOX_TAG_SCREEN_SET_PHYS_DEPTH;
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;

    /* set pixel order (BGR or RGB)
     * <> use RGB
     * <> mbox[24] = FrameBufferInfo.
     * */
    mbox[21] = MBOX_TAG_SCREEN_SET_PIXEL_ORDER;
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;

    /* request a framebuffer
     * @note
     * <> mbox[28] = request buffer alignement to 4k
     * <> mbox[28] = FrameBufferInfo.addr
     * <> mbox[29] = FramebufferInfo.size
     * */
    mbox[25] = MBOX_TAG_SCREEN_BUFF_ALLOC;
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;
    mbox[29] = 0;

    /* get pixel pitch (number of pixel per line)
     * @note
     * <> mbox[33] = FrameBufferInfo.pitch
     * */
    mbox[30] = MBOX_TAG_SCREEN_GET_PITCH;
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;

    /* indicate the end of the request */
    mbox[34] = MBOX_TAG_END;
}

/* framebuffer_setup_request() : perform mailbox request and fetch info */
static int framebuffer_setup_request(volatile uint32_t *mbox)
{
    if (framebuffer.watermark == FRAMEBUFFER_WATERMARK)
        return -1;

    framebuffer_setup_prepare(mbox);
    if (mailbox_send(MBOX_CHANNEL_PROP) == 0)
        return -2;

    framebuffer.vram.gpu    = mbox[28];
    framebuffer.vram.cpu    = mbox[28] & 0x3fffffff;
    framebuffer.vram.size   = mbox[29];
    framebuffer.vram.pitch  = mbox[33];
    framebuffer.vram.align  = 4096;
    framebuffer.phys.width  = mbox[5];
    framebuffer.phys.height = mbox[6];
    framebuffer.virt.width  = mbox[10];
    framebuffer.virt.height = mbox[11];

    if (framebuffer.vram.gpu == (uintptr_t)NULL)
        return -3;

    framebuffer.watermark = FRAMEBUFFER_WATERMARK;
    return 0;
}

//---
// Public
//---

/* framebuffer_init() : initialize framebuffer */
int framebuffer_init(void)
{
    volatile uint32_t *mbox;

    if (mailbox_get(&mbox) != 0)
        return -1;
    if (framebuffer_setup_request(mbox) != 0)
        return -2;
    uart_puts("- OK\n");
    return 0;
}

/* framebuffer_get() : fetch the framebuffer information if available */
int framebuffer_get(struct framebuffer **framebuffer_out)
{
    if (framebuffer_out == NULL)
        return -1;
    if (framebuffer.watermark != FRAMEBUFFER_WATERMARK)
        return -2;
    *framebuffer_out = &framebuffer;
    return 0;
}

/* framebuffer_debug() : display framebuffer information */
void framebuffer_debug(void)
{
    uart_puts("[+] debug framebuffer information:\n");
    if (framebuffer.watermark != FRAMEBUFFER_WATERMARK) {
        uart_puts("- not initialized\n");
        return;
    }
    uart_putf("- VRAM GPU address    : %p\n", framebuffer.vram.gpu);
    uart_putf("- VRAM CPU address    : %p\n", framebuffer.vram.cpu);
    uart_putf("- VRAM size           : %d\n", framebuffer.vram.size);
    uart_putf("- VRAM pitch          : %d\n", framebuffer.vram.pitch);
    uart_putf("- VRAM align          : %d\n", framebuffer.vram.align);
    uart_putf("- SCREEN phys. width  : %d\n", framebuffer.phys.width);
    uart_putf("- SCREEN phys. height : %d\n", framebuffer.phys.height);
    uart_putf("- SCREEN virt. width  : %d\n", framebuffer.virt.width);
    uart_putf("- SCREEN virt. height : %d\n", framebuffer.virt.height);
}
