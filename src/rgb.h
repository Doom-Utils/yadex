/*
 *	rgb.h
 *	AYM 1998-11-28
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1998 André Majorel.

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


class rgb_c
   {
   public :
      inline rgb_c ()
         {
         }

      inline rgb_c (u8 r, u8 g, u8 b)
         {
         set (r, g, b);
         }

      inline void set (u8 r, u8 g, u8 b)
         {
         this->r = r;
         this->g = g;
         this->b = b;
         }

      inline int operator == (const rgb_c rgb2)
         {
         return rgb2.r == r && rgb2.g == g && rgb2.b == b;
         }

      inline int operator - (const rgb_c rgb2)
         {
         return abs (rgb2.r - r) + abs (rgb2.g - g) + abs (rgb2.b - b);
         }

      u8 r;
      u8 g;
      u8 b;
   };


