/*
 *	warn.cc
 *	AYM 1999-08-15
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


/*
 *	warn
 *	printf a warning message to stdout.
 *	If the format string of the previous invocation did not
 *	end with a '\n', do not prepend the "Warning: " string.
 *
 *	FIXME should handle cases where stdout is not available
 *	(BGI version when in graphics mode).
 */
void warn (const char *fmt, ...)
{
  static bool start_of_line = true;
  va_list args;

  if (start_of_line)
    fputs ("Warning: ", stdout);
  va_start (args, fmt);
  vprintf (fmt, args);
  size_t len = strlen (fmt);
  start_of_line = len > 0 && fmt[len - 1] == '\n';
}


