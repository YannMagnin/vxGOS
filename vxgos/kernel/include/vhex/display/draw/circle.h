#ifndef __VHEX_DISPLAY_DRAW_CIRCLE__
# define __VHEX_DISPLAY_DRAW_CIRCLE__

#include <vhex/display/shader.h>
#include <vhex/display/types.h>

/* dcircle_filled() : draw a filled circle

   This function will add a filled circle drawing in the internal "draw queue"
   handled by vhex display core. The "draw ID" will be returned and shader can
   be added to the circle afterward.

  Args:
  ( in) x     : X position (horizontal middle of the circle)
  ( in) y     : Y position (vertical   middle of the circle)
  ( in) size  : diameter of the circle
  ( in) color : filled color for the circle

  Return:
  > the "draw ID", which can be used to apply shader afterward */
//extern did_t dcircle(int x, int y, size_t size, int mode, dshader_t *list);

enum {
    /* Horizontal settings: default in dcircle*() is DCIRCLE_LEFT */
    DCIRCLE_LEFT   = 0x01,
    DCIRCLE_CENTER = 0x02,
    DCIRCLE_RIGHT  = 0x04,
    /* Vertical settings: default in dcircle() is DCIRCLE_TOP */
    DCIRCLE_TOP    = 0x10,
    DCIRCLE_MIDDLE = 0x20,
    DCIRCLE_BOTTOM = 0x40,
};

/* dcircle_filled() : draw a filled circle */
extern void dcircle_filled(int x, int y, size_t radius, int mode, int color);



#endif /* __VHEX_DISPLAY_DRAW_CIRCLE__ */
