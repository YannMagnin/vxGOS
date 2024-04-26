#ifndef FXCP400_KEYCODE_H
#define FXCP400_KEYCODE_H

/* KEYSCODE_GEN() : generate keycode */
#define KEYCODE_GEN(row, column) \
    (((row & 0x0f) << 4) | ((column & 0x0f) << 0))

/* Define all keycode */
typedef enum
{
    // hardware key

    KEY_KEYBOARD    = 0x2f,
    KEY_SHIFT       = 0x0f,
    KEY_LEFT        = 0x1e,
    KEY_UP          = 0x27,
    KEY_DOWN        = 0x26,
    KEY_RIGHT       = 0x1f,
    KEY_ASSIGN      = 0x17,
    KEY_CLEAR       = 0x01,
    KEY_EQUAL       = 0x37,
    KEY_X           = 0x36,
    KEY_Y           = 0x2e,
    KEY_Z           = 0x1d,
    KEY_POWER       = 0x16,
    KEY_DIV         = 0x0e,
    KEY_LEFTP       = 0x35,
    KEY_7           = 0x2d,
    KEY_8           = 0x25,
    KEY_9           = 0x15,
    KEY_MUL         = 0x0d,
    KEY_RIGHTP      = 0x34,
    KEY_4           = 0x2c,
    KEY_5           = 0x24,
    KEY_6           = 0x14,
    KEY_MINUS       = 0x0c,
    KEY_COMMA       = 0x33,
    KEY_1           = 0x2b,
    KEY_2           = 0x23,
    KEY_3           = 0x13,
    KEY_PLUS        = 0x0b,
    KEY_NEG         = 0x32,
    KEY_0           = 0x2a,
    KEY_DOT         = 0x22,
    KEY_EXP         = 0x12,
    KEY_EXE         = 0x0a,

    // aliases

    KEY_SPACE       = KEY_DOT,
    KEY_DELETE      = KEY_ASSIGN,
    KEY_ENTER       = KEY_EXE,
    KEY_MAJ         = KEY_SHIFT,

    KEY_UNUSED      = 0xff,
    KEY_NONE        = 0xfe,
} KEY_t;



#endif /* FXCP400_KEYCODE_H */
