/*
 *	sapc.c
 *	al_sapc
 */


/*
This file is part of Atclib.

Atclib is Copyright � 1995-1998 Andr� Majorel.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307, USA.
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