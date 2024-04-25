#ifndef FXCP400_KEYCODE_H
#define FXCP400_KEYCODE_H

/* KEYSCODE_GEN() : generate keycode */
#define KEYCODE_GEN(row, column) \
    (((row & 0x0f) << 4) | ((column & 0x0f) << 0))

/* Define all keycode */
typedef enum
{
    KEY_F1     = 0x41,
    KEY_F2     = 0x42,
    KEY_F3     = 0x43,
    KEY_F4     = 0x44,
    KEY_F5     = 0x45,
    KEY_F6     = 0x46,

    KEY_SHIFT  = 0x49,
    KEY_OPTN   = 0x4a,
    KEY_VARS   = 0x4b,
    KEY_MENU   = 0x4c,
    KEY_LEFT   = 0x4d,
    KEY_UP     = 0x4e,

    KEY_ALPHA  = 0x31,
    KEY_SQUARE = 0x32,
    KEY_POWER  = 0x33,
    KEY_EXIT   = 0x34,
    KEY_DOWN   = 0x35,
    KEY_RIGHT  = 0x36,

    KEY_XOT    = 0x39,
    KEY_LOG    = 0x3a,
    KEY_LN     = 0x3b,
    KEY_SIN    = 0x3c,
    KEY_COS    = 0x3d,
    KEY_TAN    = 0x3e,

    KEY_FRAC   = 0x21,
    KEY_FD     = 0x22,
    KEY_LEFTP  = 0x23,
    KEY_RIGHTP = 0x24,
    KEY_COMMA  = 0x25,
    KEY_ARROW  = 0x26,

    KEY_7      = 0x29,
    KEY_8      = 0x2a,
    KEY_9      = 0x2b,
    KEY_DEL    = 0x2c,

    KEY_4      = 0x11,
    KEY_5      = 0x12,
    KEY_6      = 0x13,
    KEY_MUL    = 0x14,
    KEY_DIV    = 0x15,

    KEY_1      = 0x19,
    KEY_2      = 0x1a,
    KEY_3      = 0x1b,
    KEY_PLUS   = 0x1c,
    KEY_MINUS  = 0x1d,

    KEY_0      = 0x01,
    KEY_DOT    = 0x02,
    KEY_EXP    = 0x03,
    KEY_NEG    = 0x04,
    KEY_EXE    = 0x05,

    KEY_UNUSED = 0xff,
    KEY_NONE   = 0xfe,
} KEY_t;



#endif /* FXCP400_KEYCODE_H */
