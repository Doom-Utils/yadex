/*
 *	s_door.cc
 *	Make door from sector
 *	AYM 1998-02-03
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
#include "dialog.h"
#include "levels.h"
#include "selectn.h"


/*
   turn a Sector into a door: change the LineDefs and SideDefs
*/

void MakeDoorFromSector (int sector) /* SWAP! */
{
int    sd1, sd2;
int    n, s;
SelPtr ldok, ldflip, ld1s;

ldok = NULL;
ldflip = NULL;
ld1s = NULL;
s = 0;
/* build lists of linedefs that border the Sector */
for (n = 0; n < NumLineDefs; n++)
{
   ObjectsNeeded (OBJ_LINEDEFS, 0);
   sd1 = LineDefs[n].sidedef1;
   sd2 = LineDefs[n].sidedef2;
   if (sd1 >= 0 && sd2 >= 0)
   {
      ObjectsNeeded (OBJ_SIDEDEFS, 0);
      if (SideDefs[sd2].sector == sector)
      {
	 SelectObject (&ldok, n); /* already ok */
	 s++;
      }
      if (SideDefs[sd1].sector == sector)
      {
	 SelectObject (&ldflip, n); /* must be flipped */
	 s++;
      }
   }
   else if (sd1 >= 0 && sd2 < 0)
   {
      ObjectsNeeded (OBJ_SIDEDEFS, 0);
      if (SideDefs[sd1].sector == sector)
	 SelectObject (&ld1s, n); /* wall (one-sided) */
   }
}
/* a normal door has two sides... */
if (s < 2)
{
   Beep ();
   Notify (-1, -1, "The door must be connected to two other Sectors.", NULL);
   ForgetSelection (&ldok);
   ForgetSelection (&ldflip);
   ForgetSelection (&ld1s);
   return;
}
if ((s > 2) && !(Expert || Confirm (-1, -1, "The door will have more than two sides.", "Do you still want to create it?")))
{
   ForgetSelection (&ldok);
   ForgetSelection (&ldflip);
   ForgetSelection (&ld1s);
   return;
}
/* flip the linedefs that have the wrong orientation */
if (ldflip != NULL)
   FlipLineDefs (ldflip, 1);
/* merge the two selection lists */
while (ldflip != NULL)
{
   if (!IsSelected (ldok, ldflip->objnum))
      SelectObject (&ldok, ldflip->objnum);
   UnSelectObject (&ldflip, ldflip->objnum);
}
/* change the linedefs and sidedefs */
while (ldok != NULL)
{
   /* give the "normal door" type and flags to the linedef */
   ObjectsNeeded (OBJ_LINEDEFS, 0);
   n = ldok->objnum;
   LineDefs[n].type = 1;
   LineDefs[n].flags = 0x04;
   sd1 = LineDefs[n].sidedef1; /* outside */
   sd2 = LineDefs[n].sidedef2; /* inside */
   /* adjust the textures for the SideDefs */
   ObjectsNeeded (OBJ_SIDEDEFS, 0);
   if (strncmp (SideDefs[sd1].tex3, "-", 8))
   {
      if (!strncmp (SideDefs[sd1].tex1, "-", 8))
	 strncpy (SideDefs[sd1].tex1, SideDefs[sd1].tex3, 8);
      strncpy (SideDefs[sd1].tex3, "-", 8);
   }
   if (!strncmp (SideDefs[sd1].tex1, "-", 8))
      strncpy (SideDefs[sd1].tex1, "BIGDOOR2", 8);
   strncpy (SideDefs[sd2].tex3, "-", 8);
   UnSelectObject (&ldok, n);
}
while (ld1s != NULL)
{
   /* give the "door side" flags to the linedef */
   ObjectsNeeded (OBJ_LINEDEFS, 0);
   n = ld1s->objnum;
   LineDefs[n].flags = 0x11;
   sd1 = LineDefs[n].sidedef1;
   /* adjust the textures for the SideDef */
   ObjectsNeeded (OBJ_SIDEDEFS, 0);
   if (!strncmp (SideDefs[sd1].tex3, "-", 8))
      strncpy (SideDefs[sd1].tex3, "DOORTRAK", 8);
   strncpy (SideDefs[sd1].tex1, "-", 8);
   strncpy (SideDefs[sd1].tex2, "-", 8);
   UnSelectObject (&ld1s, n);
}
/* adjust the ceiling height */
ObjectsNeeded (OBJ_SECTORS, 0);
Sectors[sector].ceilh = Sectors[sector].floorh;
}



