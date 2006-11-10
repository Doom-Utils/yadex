/*
 *	sanity.cc
 *	Sanity checks
 *	AYM 1998-06-14
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2005 André Majorel and others.

This program is free software; you can redistribute it and/or modify it under
the terms of version 2 of the GNU Library General Public License as published
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#include "yadex.h"
#include "sanity.h"
#include "wstructs.h"


/*
 *	check_types
 *
 *	Sanity checks about the sizes and properties of certain types.
 *	Useful when porting.
 */

#define assert_size(type,size)						\
  do									\
  {									\
    if (sizeof (type) != size)						\
      fatal_error ("sizeof " #type " is %d (should be " #size ")",	\
	(int) sizeof (type));						\
  }									\
  while (0)
   
#define assert_wrap(type,high,low)					\
  do									\
  {									\
    type n = high;							\
    if (++n != low)							\
      fatal_error ("Type " #type " wraps around to %lu (should be " #low ")",\
	(unsigned long) n);						\
  }									\
  while (0)

/* This one is used for int32_t. The C standard says that the most
   negative value an int32_t is guaranteed to hold is -2,147,483,647,
   not -2,147,483,648 (perhaps to accommodate one's complement or sign
   bit platforms). Hence GCC warns that -2147483648 is "only unsigned in
   C90".

   assert_wrap2() does += 2, which on two's complement platforms should
   wrap to -2,147_483,647 which is a "safe" constant. If this test
   fails, your platform probably doesn't use two's complement. Need a
   code review to know whether this would break Yadex. */
#define assert_wrap2(type,high,low)					\
  do									\
  {									\
    type n = high;							\
    n += 2;								\
    if (n != low)							\
      fatal_error ("Type " #type " wraps around to %lu (should be " #low ")",\
	(unsigned long) n);						\
  }									\
  while (0)

void check_types ()
{
  assert_size (uint8_t,  1);
  assert_size (int8_t,   1);
  assert_size (uint16_t, 2);
  assert_size (int16_t,  2);
  assert_size (uint32_t, 4);
  assert_size (int32_t,  4);
  assert_size (struct LineDef, 14);
  assert_size (struct Sector,  26);
  assert_size (struct SideDef, 30);
  assert_size (struct Thing,   10);
  assert_size (struct Vertex,   4);
  assert_wrap (uint8_t,          255,           0);
  assert_wrap (int8_t,           127,        -128);
  assert_wrap (uint16_t,       65535,           0);
  assert_wrap (int16_t,        32767,      -32768);
  assert_wrap (uint32_t, 4294967295u,           0);
  assert_wrap2 (int32_t,  2147483647, -2147483647);
}


/*
 *	check_charset
 *
 *	If this test fails on your platform, send me a postcard
 *	from your galaxy.
 */
void check_charset ()
{
  /* Always false, unless your platform uses a
     character set even worse than EBCDIC (yet
     unseen). */
  if (   '0' + 1 != '1'
      || '1' + 1 != '2'
      || '2' + 1 != '3'
      || '3' + 1 != '4'
      || '4' + 1 != '5'
      || '5' + 1 != '6'
      || '6' + 1 != '7'
      || '7' + 1 != '8'
      || '8' + 1 != '9'
      || 'a' + 1 != 'b'
      || 'b' + 1 != 'c'
      || 'c' + 1 != 'd'
      || 'd' + 1 != 'e'
      || 'e' + 1 != 'f'
      || 'A' + 1 != 'B'
      || 'B' + 1 != 'C'
      || 'C' + 1 != 'D'
      || 'D' + 1 != 'E'
      || 'E' + 1 != 'F')
    fatal_error ("This platform uses a terminally broken character set");
}


