#ifndef VXGOS_BOOTLOADER_RASPI3B_GPIO_H
#define VXGOS_BOOTLOADER_RASPI3B_GPIO_H

#include "raspi3b/utils.h"

//---
// Macros helpers
//---

#define u32_pin0(name)          \
    u32_union(name,             \
        uint32_t PIN0       :1; \
        uint32_t PIN1       :1; \
        uint32_t PIN2       :1; \
        uint32_t PIN3       :1; \
        uint32_t PIN4       :1; \
        uint32_t PIN5       :1; \
        uint32_t PIN6       :1; \
        uint32_t PIN7       :1; \
        uint32_t PIN8       :1; \
        uint32_t PIN9       :1; \
        uint32_t PIN10      :1; \
        uint32_t PIN11      :1; \
        uint32_t PIN12      :1; \
        uint32_t PIN13      :1; \
        uint32_t PIN14      :1; \
        uint32_t PIN15      :1; \
        uint32_t PIN16      :1; \
        uint32_t PIN17      :1; \
        uint32_t PIN18      :1; \
        uint32_t PIN19      :1; \
        uint32_t PIN20      :1; \
        uint32_t PIN21      :1; \
        uint32_t PIN22      :1; \
        uint32_t PIN23      :1; \
        uint32_t PIN24      :1; \
        uint32_t PIN25      :1; \
        uint32_t PIN26      :1; \
        uint32_t PIN27      :1; \
        uint32_t PIN28      :1; \
        uint32_t PIN29      :1; \
        uint32_t PIN30      :1; \
        uint32_t PIN31      :1; \
    );

#define u32_pin1(name)          \
    u32_union(name,             \
        uint32_t PIN32      :1; \
        uint32_t PIN33      :1; \
        uint32_t PIN34      :1; \
        uint32_t PIN35      :1; \
        uint32_t PIN36      :1; \
        uint32_t PIN37      :1; \
        uint32_t PIN38      :1; \
        uint32_t PIN39      :1; \
        uint32_t PIN40      :1; \
        uint32_t PIN41      :1; \
        uint32_t PIN42      :1; \
        uint32_t PIN43      :1; \
        uint32_t PIN44      :1; \
        uint32_t PIN45      :1; \
        uint32_t PIN46      :1; \
        uint32_t PIN47      :1; \
        uint32_t PIN48      :1; \
        uint32_t PIN49      :1; \
        uint32_t PIN50      :1; \
        uint32_t PIN51      :1; \
        uint32_t PIN52      :1; \
        uint32_t PIN53      :1; \
        uint32_t            :10;\
    );

//---
//
//---

struct __bcm2837_gpio
{
    /* [0x00200000-0x00200014] GPIO Function Selection */
    u32_union(GPFSEL0,
        uint32_t FSEL0      :3;
        uint32_t FSEL1      :3;
        uint32_t FSEL2      :3;
        uint32_t FSEL3      :3;
        uint32_t FSEL4      :3;
        uint32_t FSEL5      :3;
        uint32_t FSEL6      :3;
        uint32_t FSEL7      :3;
        uint32_t FSEL8      :3;
        uint32_t FSEL9      :3;
        uint32_t            :2;
    );
    u32_union(GPFSEL1,
        uint32_t FSEL10     :3;
        uint32_t FSEL11     :3;
        uint32_t FSEL12     :3;
        uint32_t FSEL13     :3;
        uint32_t FSEL14     :3;
        uint32_t FSEL15     :3;
        uint32_t FSEL16     :3;
        uint32_t FSEL17     :3;
        uint32_t FSEL18     :3;
        uint32_t FSEL19     :3;
        uint32_t            :2;
    );
    u32_union(GPFSEL2,
        uint32_t FSEL20     :3;
        uint32_t FSEL21     :3;
        uint32_t FSEL22     :3;
        uint32_t FSEL23     :3;
        uint32_t FSEL24     :3;
        uint32_t FSEL25     :3;
        uint32_t FSEL26     :3;
        uint32_t FSEL27     :3;
        uint32_t FSEL28     :3;
        uint32_t FSEL29     :3;
        uint32_t            :2;
    );
    u32_union(GPFSEL3,
        uint32_t FSEL30     :3;
        uint32_t FSEL31     :3;
        uint32_t FSEL32     :3;
        uint32_t FSEL33     :3;
        uint32_t FSEL34     :3;
        uint32_t FSEL35     :3;
        uint32_t FSEL36     :3;
        uint32_t FSEL37     :3;
        uint32_t FSEL38     :3;
        uint32_t FSEL39     :3;
        uint32_t            :2;
    );
    u32_union(GPFSEL4,
        uint32_t FSEL40     :3;
        uint32_t FSEL41     :3;
        uint32_t FSEL42     :3;
        uint32_t FSEL43     :3;
        uint32_t FSEL44     :3;
        uint32_t FSEL45     :3;
        uint32_t FSEL46     :3;
        uint32_t FSEL47     :3;
        uint32_t FSEL48     :3;
        uint32_t FSEL49     :3;
        uint32_t            :2;
    );
    u32_union(GPFSEL5,
        uint32_t FSEL50     :3;
        uint32_t FSEL51     :3;
        uint32_t FSEL52     :3;
        uint32_t FSEL53     :3;
        uint32_t FSEL54     :3;
        uint32_t FSEL55     :3;
        uint32_t FSEL56     :3;
        uint32_t FSEL57     :3;
        uint32_t FSEL58     :3;
        uint32_t FSEL59     :3;
        uint32_t            :2;
    );
    pad(0x04);

    /* [0x0020001c-0x00200020] Pin Output Set Register */
    u32_pin0(GPSET0);
    u32_pin1(GPSET1);
    pad(0x04);

    /* [0x00200028-0x0020030] Pin Output Clear */
    u32_pin0(GPCLR0);
    u32_pin1(GPCLR1);
    pad(0x04);

    /* [0x00200034-0x00200038] Pin Level */
    const u32_pin0(GPLEV0);
    const u32_pin1(GPLEV1);
    pad(0x04);

    /* [0x00200040-0x00200040] Pin Event Detect Status */
    u32_pin0(GPEDS0);
    u32_pin1(GPEDS1);
    pad(0x04);

    /* [0x0020004c-0x00200050] Pin Rising Edge Detect Enable */
    u32_pin0(GPREN0);
    u32_pin1(GPREN1);
    pad(0x04);

    /* [0x00200058-0x0020005c] Pin Falling Edge Detect Enable */
    u32_pin0(GPFEN0);
    u32_pin1(GPFEN1);
    pad(0x04);

    /* [0x00200064-0x00200068] Pin High Detect Enable */
    u32_pin0(GPHEN0);
    u32_pin1(GPHEN1);
    pad(0x04);

    /* [0x00200070-0x00200074] Pin Low Detect Enable */
    u32_pin0(GPLEN0);
    u32_pin1(GPLEN1);
    pad(0x04);

    /* [0x0020007c-0x00200080] Pin Async. Rising Edge Detect */
    u32_pin0(GPAREN0);
    u32_pin1(GPAREN1);
    pad(0x04);

    /* [0x00200088-0x0020008c] Pin Async. Rising Edge Detect */
    u32_pin0(GPAFEN0);
    u32_pin1(GPAFEN1);
    pad(0x04);

    /* [0x00200094] GPIO Pin Pull-up/down Enable */
    u32_union(GPPUD,
        uint32_t PUD            :2; /* GPIO Pin Pull-up/down */
        uint32_t                :30;
    );

    /* [0x00200098-0x0020009c] Pin Pull-up/down Enable Clock */
    u32_pin0(GPPUDCLK0);
    u32_pin1(GPPUDCLK1);
    pad(0x14);

    /* [0x002000b0] Test (unknown) */
    uint32_t _test;
} VPACKED(4);

#define BCM2837_GPIO    (*((volatile struct __bcm2837_gpio*)0x3f200000))

#endif /* VXGOS_BOOTLOADER_RASPI3B_GPIO_H */
