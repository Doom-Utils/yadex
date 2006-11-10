/*
 *	ytime.cc
 *	AYM 1999-11-11
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
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "ytime.h"


/*
 *	Return the current time in ms
 */
unsigned long y_milliseconds ()
{
#ifdef Y_GETTIMEOFDAY
  struct timeval tv;
  struct timezone tz;
  if (gettimeofday (&tv, &tz))
  {
    nf_bug ("gettimeofday() error (%s)", strerror (errno));
    return 0;
  }
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else  /* Sucks ! */
  static const double ms_per_clock = 1000.0 / CLOCKS_PER_SEC;
  return (unsigned long) (clock () * ms_per_clock);
#endif
}



