/*
 *	s_swapf.cc
 *	Swap floor and ceiling flats of sectors
 *	AYM 2000-08-12
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
#include "levels.h"
#include "selectn.h"


void swap_flats (SelPtr list)
{
  for (SelPtr cur = list; cur != NULL; cur = cur->next)
  {
    wad_flat_name_t tmp;
    struct Sector *s = Sectors + cur->objnum;

    memcpy (tmp,       s->floort, sizeof tmp);
    memcpy (s->floort, s->ceilt,  sizeof s->floort);
    memcpy (s->ceilt,  tmp,       sizeof s->ceilt);
    MadeChanges = 1;
  }
}

