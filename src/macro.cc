/*
 *	macro.cc
 *	Expanding macros
 *	AYM 1999-11-30
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
#include "macro.h"


/*
 *	macro_expand, vmacro_expand - expand macro references in a string
 *
 *	Copies the NUL-terminated string in <fmt> into the
 *	buffer <buf>, replacing certain strings by other
 *	strings. <buf> must be at least (<size> + 1) characters
 *	long.
 *
 *	The macro definitions are provided by the optional
 *	arguments that follow <fmt>. You must pass two (const
 *	char *) for each macro. The first is the name of the
 *	macro, NUL-terminated. The second is the value of the
 *	macro, also NUL-terminated. The last macro value must be
 *	followed by a null pointer ((const char *) 0).
 *
 *	Matching is case-sensitive. The first match is used.
 *	For example,
 *
 *	  macro_expand (buf, 99, "a%bcd", "%b", "1", "%bc", "2", (char *) 0);
 *
 *	puts "a1cd" in buf.
 *
 *	If the value of a macro to expand is a null pointer, the
 *	macro expands to an empty string.
 *
 *	If <fmt> expands to more than <size> characters, only
 *	the first <size> characters of the expansion are stored
 *	in <buf>. After the function returns, <buf> is
 *	guaranteed to be NUL-terminated.
 *
 *	Return 0 if successful, non-zero if one of the macros
 *	could not be expanded (value was a null pointer). In
 *	that case, the actual number returned is the number of
 *	the first macro that could not be expanded, starting
 *	from 1.
 */
int macro_expand (char *buf, size_t size, const char *fmt, ...)
{
  va_list list;
  va_start (list, fmt);
  return vmacro_expand (buf, size, fmt, list);
}


int vmacro_expand (char *buf, size_t size, const char *fmt, va_list list)
{
  int rc = 0;
  int macro_no = 0;

  *buf = '\0';
  // This is awful, but who cares ?
  while (*fmt)
  {
    va_list l = list;
    const char *macro_name = 0;
    const char *macro_value = 0;
    while (macro_name = va_arg (l, const char *))
    {
      macro_value = va_arg (l, const char *);
      size_t len1 = strlen (fmt);
      size_t len2 = strlen (macro_name);
      if (len1 >= len2 && ! memcmp (fmt, macro_name, len2))
	break;
    }
    if (macro_name)
    {
      macro_no++;
      if (macro_value == 0)
      {
	if (rc == 0)
	  rc = macro_no;
      }
      else
        al_saps (buf, macro_value, size);
      fmt += strlen (macro_name);
    }
    else
    {
      al_sapc (buf, *fmt, size);
      fmt++;
    }
  }
  return rc;
}


