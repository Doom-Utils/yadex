/*
 *	gotoobj.cc
 *	AYM 1998-09-06
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1999 André Majorel.

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
#include "gfx.h"
#include "gotoobj.h"
#include "levels.h"


/*
  center the map around the given coords
*/

void CenterMapAroundCoords (int xpos, int ypos)
{
OrigX = xpos;
OrigY = ypos;
/* AYM 1998-09-20 This is a DOS-ism. IIRC, the ICCCM says an
   application program should generally not mess with the
   pointer. */
#if 0
is.x = ScrCenterX;
is.y = ScrCenterY;
#endif
}


/*
  center the map around the object and zoom in if necessary
*/

void GoToObject (int objtype, int objnum) /* SWAP! */
{
int   xpos, ypos;
int   xpos2, ypos2;
int   n;
int   sd1, sd2;
float oldscale;

GetObjectCoords (objtype, objnum, &xpos, &ypos);
CenterMapAroundCoords (xpos, ypos);
oldscale = Scale;

/* zoom in until the object can be selected */
while (Scale < 8.0
   && GetCurObject (objtype, MAPX (is.x), MAPY (is.y), 4) != objnum)
   {
   if (Scale < 1.0)
      Scale = 1.0 / ((1.0 / Scale) - 1.0);
   else
      Scale = Scale * 2.0;
   }

/* Special case for Sectors: if several Sectors are one inside another, then
   zooming in on the center won't help. So I choose a LineDef that borders the
   sector, move a few pixels towards the inside of the Sector, then zoom in. */
if (objtype == OBJ_SECTORS
 && GetCurObject (OBJ_SECTORS, OrigX, OrigY, 1) != objnum)
   {
   /* restore the Scale */
   Scale = oldscale;
   for (n = 0; n < NumLineDefs; n++)
      {
      ObjectsNeeded (OBJ_LINEDEFS, 0);
      sd1 = LineDefs[n].sidedef1;
      sd2 = LineDefs[n].sidedef2;
      ObjectsNeeded (OBJ_SIDEDEFS, 0);
      if (sd1 >= 0 && SideDefs[sd1].sector == objnum)
	 break;
      if (sd2 >= 0 && SideDefs[sd2].sector == objnum)
	 break;
      }
   if (n < NumLineDefs)
      {
      GetObjectCoords (OBJ_LINEDEFS, n, &xpos2, &ypos2);
      n = ComputeDist (abs (xpos - xpos2), abs (ypos - ypos2)) / 7;
      if (n <= 1)
	n = 2;
      xpos = xpos2 + (xpos - xpos2) / n;
      ypos = ypos2 + (ypos - ypos2) / n;
      CenterMapAroundCoords (xpos, ypos);
      /* zoom in until the sector can be selected */
      while (Scale > 8.0
          && GetCurObject (OBJ_SECTORS, OrigX, OrigY, 1) != objnum)
         {
	 if (Scale < 1.0)
	    Scale = 1.0 / ((1.0 / Scale) - 1.0);
	 else
	    Scale = Scale / 2.0;
         }
      }
   }
if (UseMouse)
   SetMouseCoords (is.x, is.y);
}



