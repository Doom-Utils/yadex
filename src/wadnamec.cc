/*
 *	wadnamec.cc
 *	AYM 2000-04-11
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-2005 Andr� Majorel and others.

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
#include "wadnamec.h"


Wad_name_c::Wad_name_c (const char *string)
{
  char *p = name;
  char *pmax = name + 8;
  while (*string && p < pmax)
    *p++ = toupper (*string++);
  *p = '\0';
}


Wad_name_c::Wad_name_c (const char *fmt, ...)
{
  va_list list;
  va_start (list, fmt);
  y_vsnprintf (name, sizeof name, fmt, list);
  va_end (list);
  y_strupr (name);
}


int Wad_name_c::cmp (wad_name_t name)
{
  return y_strnicmp (this->name, name, WAD_NAME);
}


