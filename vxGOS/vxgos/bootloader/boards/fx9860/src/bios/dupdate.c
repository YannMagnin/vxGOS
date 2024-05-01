#include <stdint.h>

//---
// Internals
//---

// Device specification sheet

/* This version number is 1 for the old T6K11 (T6K73A (2005-11-30)) that
 * everyone knows, and 2 for the newer one found in the Graph 35+E II (ML9801A
 * (2018-07-05)).
 * Documentation is available only for the T6K11 (version 1 is close).
 * Dumps of Bdisp_PutDisp_DD() are used to drive version 2. */
static int t6k11_version = -1;

/* Screen registers on the original T6K11. Registers 8..11 and 13..15 are test
   registers and must not be used! */
enum {
    reg_display     = 0,
    reg_counter     = 1,
    reg_analog      = 2,
    reg_alternate   = 3,

    reg_yaddr       = 4, /* These two use the same register */
    reg_xaddr       = 4, /* (interpretation depends on count mode) */

    reg_zaddr       = 5,
    reg_contrast    = 6,
    reg_data        = 7,
    reg_daconv      = 12,
};

/* Automatic counter increment during read/write */
enum {
    cnt_ydown       = 0,
    cnt_yup         = 1,
    cnt_xdown       = 2,
    cnt_xup         = 3,
};

// Device communication primitives

/* I/O may be performed either with RS = 0 or RS = 1.
 * The memory-mapping of the device I/O maps bit 16 of the address to pin RS.
 * */

/* RS = 0: Register selection */
static volatile uint8_t *sel = (void *)0xb4000000;
/* RS = 1: Command data or vram data */
static volatile uint8_t *cmd = (void *)0xb4010000;

/* command() - send a command to set the value of a register */
static void command(uint8_t reg, uint8_t data)
{
    *sel = reg;
    *cmd = data;
}

/* write_row() - send 16 bytes to the display driver */
static void write_row(const uint8_t *buf)
{
    /* Unroll the loop for more speed */
    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;

    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;

    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;

    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;
    *cmd = *buf++;
}

// Driver functions

/* t6k11_display() - send vram data to the LCD device */
static void t6k11_display_t6k73(const void *vram)
{
    for(int y = 0; y < 64; y++)
    {
        command(reg_xaddr, y | 0xc0);
        command(reg_counter, cnt_yup);
        command(reg_yaddr, 0);

        *sel = reg_data;
        write_row(vram);
        vram += 128 / 8;
    }
}

/* t6k11_display() - send vram data to the LCD device */
static void t6k11_display_ml9801(const void *vram)
{
    for(int y = 0; y < 64; y++)
    {
        command(8, y | 0x80);
        command(8, 4);

        *sel = 10;
        write_row(vram);
        vram += 128 / 8;
    }
}

//---
// Public
//---

/* expose VRAM here */
uint8_t vram[(128 * 64) / 8];

/* _bios_dupdate() : small T6K73A and ML9801A driver */
void _bios_dupdate(void)
{
    extern uint8_t vram[];
    extern int t6k11_version;

    /* Tell if it is a Graph 35+E II using the OS version (this is accurate
     * unless someone tweaks an OS file enough to de-correlate the version
     * of the OS and the version of the display and storage memory drivers,
     * which, let's be real, is enough for now. */
    if (t6k11_version == -1) {
        char *version = (void *)0x80010020;
        t6k11_version = (version[1] == '3') ? 2 : 1;
    }

    /* branch to the real driver */
    if(t6k11_version == 1) t6k11_display_t6k73(vram);
    if(t6k11_version == 2) t6k11_display_ml9801(vram);
}
