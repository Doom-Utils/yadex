/*
 *	clearimg.cc
 *	AYM 1998-11-23
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2000 André Majorel.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include "yadex.h"
#include "clearimg.h"


/*
 *	clear_game_image
 *	Clear a game image buffer (fill it
 *	with the game image background colour)
 */
void clear_game_image (game_image_pixel_t *buf, int width, int height)
{
unsigned long bytes_total = (unsigned long) width * height * sizeof *buf;
size_t bytes_this_time = AL_ASIZE_T_MAX;

// Under DOS, memset() is limited to 64 kB.
while (bytes_total > 0)
   {
   if (bytes_total <= AL_ASIZE_T_MAX)
      bytes_this_time = (size_t) bytes_total;
   // FIXME should be parameterized, not hard-coded 0.
   memset (buf, 0, bytes_this_time);
   bytes_total -= bytes_this_time;
   buf += bytes_this_time / sizeof *buf;
   }
}


