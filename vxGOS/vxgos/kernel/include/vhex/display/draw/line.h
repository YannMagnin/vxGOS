#ifndef __VHEX_DISPLAY_DRAW_LINE__
# define __VHEX_DISPLAY_DRAW_LINE__

#include <vhex/defs/types.h>
#include <vhex/display/shader.h>

//---
// User-level API
//---

/* dline(): Render a straight line

   This function draws a line using a Bresenham-style algorithm and dline() has
   optimizations for horizontal and vertical lines.

   dline() is currently not able to clip arbitrary lines without calculating
   all the pixels, so drawing a line from (-1e6,0) to (1e6,395) will work but
   will be very slow.

   @x1 @y1 @x2 @y2  Endpoints of the line (both included).
   @color           Line color (same values as dpixel() are allowed) */
extern void dline(int color, int x1, int y1, int x2, int y2);

/* dhline() : render an horizontal line */
extern void dhline(int color, int y, int x1, int x2);

/* dvline() : render an vertical line */
extern void dvline(int color, int x, int y1, int y2);


//---
// Kernel-level API
//---

/* dline_render() : real drawing function */
extern void dline_render(dsurface_t *s, int c, int x1, int y1, int x2, int y2);

/* dhline_render() : optimized drawing function for horizontal line */
extern void dhline_render(dsurface_t *s, int color, int y, int x1, int x2);

/* dvline_render() : optimized drawing function for vertical line */
extern void dvline_render(dsurface_t *s, int color, int x, int y1, int y2);

#endif /* __VHEX_DISPLAY_DRAW_LINE__ */
