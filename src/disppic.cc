/*
 *	disppic.cc
 *	AYM 1998-08-30
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1999 André Majorel.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307, USA.
*/


#include "yadex.h"
#include "clearimg.h"
#include "dispimg.h"
#include "pic2img.h"
#include "spectimg.h"


/*
 *	display_pic()
 *	Load a picture and display it.
 *	A wrapper for LoadPicture() and display_game_image().
 */
void display_pic (hookfunc_comm_t *c)
{
game_image_pixel_t *pixels;
int width  = c->x1 - c->x0 + 1;
int height = c->y1 - c->y0 + 1;

pixels = (game_image_pixel_t *) GetFarMemory ((unsigned long) width * height);
clear_game_image (pixels, width, height);
LoadPicture (pixels,
   width,
   height,
   c->name,
   INT_MIN,  // Not very clean, should use c->xofs but *WithFunc doesn't set it
   INT_MIN,  // Not very clean, should use c->yofs but *WithFunc doesn't set it
   &c->width,
   &c->height);
c->flags |= HOOK_SIZE_VALID;
if (c->flags & HOOK_SPECTRAL)
  spectrify_game_image (pixels, width, height);
display_game_image (pixels, width, height, c->x0, c->y0, width, height);
c->flags |= HOOK_DRAWN;
FreeMemory (pixels);
}


