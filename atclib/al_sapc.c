/*
 *	sapc.c
 *	al_sapc
 */


/*
This file is part of Atclib.

Atclib is Copyright © 1995-1999 André Majorel.

This library is free software; you can redistribute it and/or
modify it under the terms of version 2 of the GNU Library General
Public License as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.
*/


#include <string.h>
#include "atclib.h"


int al_sapc (char *dest, char source, size_t maxlen)
{
register size_t len;

len = strlen (dest);
if (len >= maxlen)
  return 1;
dest[len]   = source;
dest[len+1] = '\0';
return 0;
}

/* eof - sapc.c */
