/*
 *	spectimg.cc
 *	AYM 1999-03-09
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-1999 Andr� Majorel.

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
#include "spectimg.h"


/*
 *	spectrify_game_image
 *	Make a game image look vaguely like a spectre
 */
void spectrify_game_image (game_image_pixel_t *buf, int width, int height)
{
int x,y;
for (y = 0; y < height; y++)
   {
   game_image_pixel_t *row = buf + y * width;
   for (x = 0; x < width; x++)
      if (row[x] != 0)  // FIXME hard-coded 0
         row[x] = 104 + (rand () >> 6) % 7;  // FIXME hard-coded 104
   }
}

