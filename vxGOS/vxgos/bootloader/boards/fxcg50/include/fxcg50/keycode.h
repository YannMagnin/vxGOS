#ifndef FXCG50_KEYCODE_H
#define FXCG50_KEYCODE_H

/* KEYSCODE_GEN() : generate keycode */
#define KEYCODE_GEN(row, column) \
    (((row & 0x0f) << 4) | ((column & 0x0f) << 0))

/* Define all keycode */
typedef enum
{
    KEY_F1     = 0x4e,
    KEY_F2     = 0x4d,
    KEY_F3     = 0x4c,
    KEY_F4     = 0x4b,
    KEY_F5     = 0x4a,
    KEY_F6     = 0x49,

    KEY_SHIFT  = 0x46,
    KEY_OPTN   = 0x45,
    KEY_VARS   = 0x44,
    KEY_MENU   = 0x43,
    KEY_LEFT   = 0x42,
    KEY_UP     = 0x41,

    KEY_ALPHA  = 0x3e,
    KEY_SQUARE = 0x3d,
    KEY_POWER  = 0x3c,
    KEY_EXIT   = 0x3b,
    KEY_DOWN   = 0x3a,
    KEY_RIGHT  = 0x39,

    KEY_XOT    = 0x36,
    KEY_LOG    = 0x35,
    KEY_LN     = 0x34,
    KEY_SIN    = 0x33,
    KEY_COS    = 0x32,
    KEY_TAN    = 0x31,

    KEY_FRAC   = 0x2e,
    KEY_FD     = 0x2d,
    KEY_LEFTP  = 0x2c,
    KEY_RIGHTP = 0x2b,
    KEY_COMMA  = 0x2a,
    KEY_ARROW  = 0x29,

    KEY_7      = 0x26,
    KEY_8      = 0x25,
    KEY_9      = 0x24,
    KEY_DEL    = 0x23,

    KEY_4      = 0x1e,
    KEY_5      = 0x1d,
    KEY_6      = 0x1c,
    KEY_MUL    = 0x1b,
    KEY_DIV    = 0x1a,

    KEY_1      = 0x16,
    KEY_2      = 0x15,
    KEY_3      = 0x14,
    KEY_PLUS   = 0x13,
    KEY_MINUS  = 0x12,

    KEY_0      = 0x0e,
    KEY_DOT    = 0x0d,
    KEY_EXP    = 0x0c,
    KEY_NEG    = 0x0b,
    KEY_EXE    = 0x0a,

    KEY_ACON   = 0x00,
    KEY_UNUSED = 0xff,
    KEY_NONE   = 0xfe,
} KEY_t;

#endif /* FXCG50_KEYCODE_H */
