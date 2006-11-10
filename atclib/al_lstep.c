/*
 *	lstep.c
 *	al_lstep()
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


#define AL_AILLEGAL_ACCESS
#include "atclib.h"


int al_lstep (al_llist_t *l)
{
al_lcheckmagic (l);
if (l->current == NULL || l->ateol)
  {
  al_aerrno = AL_AEOL;
  return AL_AEOL;
  }
l->prev = l->current;
l->curno++;
if (l->current->next == NULL)
  l->ateol = 1;
else
  l->current = l->current->next;
return 0;
}

