#ifndef VHEX_MODULES_DISPLAY_H
#define VHEX_MODULES_DISPLAY_H 1

#include "vhex/modules/display/color.h"

//---
// Public
//---

/* dclear(): dill the screen with a single color */
extern int dclear(int color);

/* dupdate(): denerate the display frame on the screen */
extern void dupdate(void);

#endif /* VHEX_MODULES_DISPLAY_H */
