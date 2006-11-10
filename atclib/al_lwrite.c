/*
 *	lwrite.c
 *	al_lwrite()
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


#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

#define AL_AILLEGAL_ACCESS
#include "atclib.h"


int al_lwrite (al_llist_t *l, const void *buf)
{
int r;
r = al_lpoke (l, buf);
if (r == 0)
  r = al_lstep (l);
return r;
}

