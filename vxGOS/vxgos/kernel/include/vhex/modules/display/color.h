#ifndef VHEX_MODULES_DISPLAY_COLOR_H
#define VHEX_MODULES_DISPLAY_COLOR_H 1


//FIXME: this is plateform-specific :/
enum {
    C_WHITE  = 0xffff,
    C_LIGHT  = 0xad55,
    C_DARK   = 0x528a,
    C_BLACK  = 0x0000,

    C_RED    = 0xf800,
    C_GREEN  = 0x07e0,
    C_BLUE   = 0x001f,

    C_NONE   = -1,
    C_INVERT = -2,
};

#endif /* VHEX_MODULES_DISPLAY_COLOR_H */
