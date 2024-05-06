#include <vhex/driver/mpu/sh/sh7305/usb.h>
#include <vhex/driver/cpu.h>

//---
// Internals
//---

/* internal pipe bitmap used for pipe status snapshot */
static uint16_t pipe_bitmap = 0x0000;

/* sh7305_pipe_reserve() : try to reserve a pipe */
static int sh7305_pipe_reserve(int pipe)
{
    int ret;

    cpu_atomic_start();
    ret = -1;
    if ((pipe_bitmap & (0x0001 << pipe)) == 0x0000) {
        pipe_bitmap |= (0x0001 << pipe);
        ret = 0;
    }
    cpu_atomic_end();

    return ret;
}

//---
// Public API
//---

/* sh7305_usb_pipe_find() : try to find a pipe */
int sh7305_usb_pipe_find(int type)
{
    int min;
    int max;

    max = -1;
    min = -1;

    /* Isochronous transfert: use pipe 1,2 */
    if (type == 1) min=1, max=2;
    /* BULK transfer: use pipe 1..5 */
    if (type == 2) min=1, max=5;
    /* Interrupt transfer: use pipe 6..9 */
    if (type == 3) min=6, max=9;

    /* Start from the end to avoir using Isochronous pipe for BULK */
    for (int pipe = max ; pipe >= min ; pipe--) {
        if (sh7305_pipe_reserve(pipe) == 0)
            return pipe;
    }

    /* no pipe found, return error */
    return -1;
}
