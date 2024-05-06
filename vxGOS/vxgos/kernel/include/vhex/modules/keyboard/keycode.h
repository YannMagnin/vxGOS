#ifndef __VHEX_KEYBOARD_KEYCODE__
# define __VHEX_KEYBOARD_KEYCODE__

/* VKEYSCODE_GEN() : generate keycode */
#define VKEYCODE_GEN(row, column) \
    (((row & 0x0f) << 4) | ((column & 0x0f) << 0))

/* Define all keycode */
typedef enum
{
    VKEY_F1     = 0x41,
    VKEY_F2     = 0x42,
    VKEY_F3     = 0x43,
    VKEY_F4     = 0x44,
    VKEY_F5     = 0x45,
    VKEY_F6     = 0x46,

    VKEY_SHIFT  = 0x49,
    VKEY_OPTN   = 0x4a,
    VKEY_VARS   = 0x4b,
    VKEY_MENU   = 0x4c,
    VKEY_LEFT   = 0x4d,
    VKEY_UP     = 0x4e,

    VKEY_ALPHA  = 0x31,
    VKEY_SQUARE = 0x32,
    VKEY_POWER  = 0x33,
    VKEY_EXIT   = 0x34,
    VKEY_DOWN   = 0x35,
    VKEY_RIGHT  = 0x36,

    VKEY_XOT    = 0x39,
    VKEY_LOG    = 0x3a,
    VKEY_LN     = 0x3b,
    VKEY_SIN    = 0x3c,
    VKEY_COS    = 0x3d,
    VKEY_TAN    = 0x3e,

    VKEY_FRAC   = 0x21,
    VKEY_FD     = 0x22,
    VKEY_LEFTP  = 0x23,
    VKEY_RIGHTP = 0x24,
    VKEY_COMMA  = 0x25,
    VKEY_ARROW  = 0x26,

    VKEY_7      = 0x29,
    VKEY_8      = 0x2a,
    VKEY_9      = 0x2b,
    VKEY_DEL    = 0x2c,

    VKEY_4      = 0x11,
    VKEY_5      = 0x12,
    VKEY_6      = 0x13,
    VKEY_MUL    = 0x14,
    VKEY_DIV    = 0x15,

    VKEY_1      = 0x19,
    VKEY_2      = 0x1a,
    VKEY_3      = 0x1b,
    VKEY_PLUS   = 0x1c,
    VKEY_MINUS  = 0x1d,

    VKEY_0      = 0x01,
    VKEY_DOT    = 0x02,
    VKEY_EXP    = 0x03,
    VKEY_NEG    = 0x04,
    VKEY_EXE    = 0x05,

    VKEY_UNUSED = 0xff,
    VKEY_NONE   = 0xfe,
} vkey_t;

#endif /* __VHEX_KEYBOARD_KEYCODE__ */
