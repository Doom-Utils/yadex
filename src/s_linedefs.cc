/*
 *	s_linedefs.cc
 *	AYM 1998-11-22
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-1998 Andr� Majorel.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307, USA.
*/


#include "yadex.h"
#include "bitvec.h"
#include "levels.h"
#include "s_linedefs.h"
#include "selectn.h"


/*
 *	linedefs_of_sectors
 *	Return a bit vector of all linedefs used by the sectors.
 *	It's up to the caller to delete the bit vector after use.
 */
bitvec_c *linedefs_of_sectors (SelPtr list)
{
bitvec_c *sectors;
bitvec_c *linedefs;
int n;

sectors = list_to_bitvec (list, NumSectors);
linedefs = new bitvec_c (NumLineDefs);

for (n = 0; n < NumLineDefs; n++)
   if (   is_sidedef (LineDefs[n].sidedef1)
          && sectors->get (SideDefs[LineDefs[n].sidedef1].sector)
       || is_sidedef (LineDefs[n].sidedef2)
          && sectors->get (SideDefs[LineDefs[n].sidedef2].sector))
      linedefs->set (n);

delete sectors;
return linedefs;
}

