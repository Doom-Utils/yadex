/*
 *	dispimg.cc
 *	AYM 1998-08-08
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
#ifdef Y_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>  // XDestroyImage
#endif
#include "gcolour2.h"
#include "gfx.h"
#include "dispimg.h"


/*
 *	display_game_image
 *	Display a game image (bi-dimensional array of pixels
 *	encoded with the game palette, as returned by LoadPicture)
 *	on a certain area of the screen.
 */
void display_game_image (const game_image_pixel_t *image_data,
   int image_width, int image_height,
   int screen_x, int screen_y,
   int screen_width, int screen_height)
{
XImage *image;
u8 *buf;
const game_image_pixel_t *image_end;
int buf_bpp;

buf_bpp = win_bpp;
if (buf_bpp == 3)  /* Because, in 24-bit mode, X uses 32 bits per pixel */
   buf_bpp = 4;

/* Allocate a buffer matching the depth of the screen,
   copy the image in it and transcode pixel values to
   match what the screen expects. */
buf = (u8 *) GetMemory ((unsigned long) image_width * image_height * buf_bpp);
if (! buf)
   fatal_error ("Not enough memory to allocate %lu B",
      (unsigned long) image_width * image_height * buf_bpp);
image_end = (const game_image_pixel_t *) image_data + image_width * image_height;

if (buf_bpp == 1)
   {
   register const game_image_pixel_t *image_ptr;
   register u8 *buf_ptr;
   for (image_ptr = image_data, buf_ptr = (u8 *) buf;
        image_ptr < image_end;
        image_ptr++)
      *buf_ptr++ = (u8) game_colour[*image_ptr];
   }

else if (buf_bpp == 2)
   if (cpu_big_endian == x_server_big_endian)
      {
      register const game_image_pixel_t *image_ptr;
      register u16 *buf_ptr;
      for (image_ptr = image_data, buf_ptr = (u16 *) buf;
	   image_ptr < image_end;
	   image_ptr++)
	 *buf_ptr++ = (u16) game_colour[*image_ptr];
      }
   else  // Different endiannesses so swap bytes
      {
      register const game_image_pixel_t *image_ptr;
      register u16 *buf_ptr;
      for (image_ptr = image_data, buf_ptr = (u16 *) buf;
	   image_ptr < image_end;
	   image_ptr++)
	 *buf_ptr++ = (u16) (
	       (game_colour[*image_ptr] >> 8)  // Assume game_colour unsigned
	     | (game_colour[*image_ptr] << 8));
      }

else if (buf_bpp == 4)
   if (cpu_big_endian == x_server_big_endian)
      {
      register const game_image_pixel_t *image_ptr;
      register u32 *buf_ptr;
      for (image_ptr = image_data, buf_ptr = (u32 *) buf;
	   image_ptr < image_end;
	   image_ptr++)
	 *buf_ptr++ = (u32) game_colour[*image_ptr];
      }
   else  // Different endiannesses so swap bytes
      {
      register const game_image_pixel_t *image_ptr;
      register u32 *buf_ptr;
      for (image_ptr = image_data, buf_ptr = (u32 *) buf;
	   image_ptr < image_end;
	   image_ptr++)
	 *buf_ptr++ = (u32) (
	      (game_colour[*image_ptr] >> 24)  // Assume game_colour unsigned
	    | (game_colour[*image_ptr] >> 8) & 0x0000ff00
	    | (game_colour[*image_ptr] << 8) & 0x00ff0000
	    | (game_colour[*image_ptr] << 24));
      }

image = XCreateImage (dpy, win_vis, win_depth, ZPixmap, 0, (char *) buf,
   image_width, image_height, 8, image_width * buf_bpp);
if (! image)
   fatal_error ("XCreateImage returned NULL");
if (image->byte_order == LSBFirst && ! x_server_big_endian
      || image->byte_order == MSBFirst && x_server_big_endian)
   ;  // OK
else
   warn ("image byte_order %d doesn't match X server endianness\n",
	 image->byte_order);
XPutImage (dpy, win, gc, image, 0, 0,
   screen_x, screen_y, image_width, image_height);
XDestroyImage (image);  /* Also frees buf. */
}


