/*
 *	sanity.cc
 *	Sanity checks
 *	AYM 1998-06-14
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
#include "sanity.h"
#include "wstructs.h"


/*
 *	check_types
 *	Sanity checks about the sizes and properties of certain types.
 *	Useful when porting.
 */

#define assert_size(type,size)\
  do {\
  if (sizeof (type) != size)\
    fatal_error ("sizeof " #type " is %d (should be " #size ")",\
      (int) sizeof (type));\
  } while (0)
   
#define assert_wrap(type,high,low)\
  do {\
  type n = high;\
  if (++n != low)\
    fatal_error ("Type " #type " wraps around to %lu (should be " #low ")",\
      (unsigned long) n);\
  } while (0)

void check_types ()
{
assert_size (u8,  1);
assert_size (i8,  1);
assert_size (u16, 2);
assert_size (i16, 2);
assert_size (u32, 4);
assert_size (i32, 4);
assert_size (struct LineDef, 14);
assert_size (struct Sector,  26);
assert_size (struct SideDef, 30);
assert_size (struct Thing,   10);
assert_size (struct Vertex,   4);
assert_wrap (u8,         255,           0);
assert_wrap (i8,         127,        -128);
assert_wrap (u16,      65535,           0);
assert_wrap (i16,      32767,      -32768);
assert_wrap (u32, 4294967295,           0);
assert_wrap (i32, 2147483647, -2147483648);
}


