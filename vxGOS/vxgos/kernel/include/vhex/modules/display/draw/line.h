#ifndef VHEX_MODULES_DISPLAY_DRAW_LINE_H
#define VHEX_MODULES_DISPLAY_DRAW_LINE_H 1

//---
// Public
//---

/* dline(): render a straight line */
extern void dline(int color, int x1, int y1, int x2, int y2);

/* dhline() : render an horizontal line */
extern void dhline(int color, int y, int x1, int x2);

/* dvline() : render an vertical line */
extern void dvline(int color, int x, int y1, int y2);

#endif /* VHEX_MODULES_DISPLAY_DRAW_LINE_H */
