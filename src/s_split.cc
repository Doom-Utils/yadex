/*
 *	s_split.cc
 *	Split sectors
 *	AYM 1998-02-03
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
#include "dialog.h"
#include "levels.h"
#include "selectn.h"


/*
   split a Sector in two, adding a new LineDef between the two Vertices
*/

void SplitSector (int vertex1, int vertex2) /* SWAP! */
{
SelPtr llist;
int    curv, s, l, sd;
char   msg1[80], msg2[80];

/* AYM 1998-08-09 : FIXME : I'm afraid this test is not relevant
   if the sector contains subsectors. I should ask Jim (Flynn),
   he did something about that in DETH. */
/* Check if there is a Sector between the two Vertices (in the middle) */
s = GetCurObject (OBJ_SECTORS,
                  (Vertices[vertex1].x + Vertices[vertex2].x) / 2,
                  (Vertices[vertex1].y + Vertices[vertex2].y) / 2,
                  0);
if (s < 0)
   {
   Beep ();
   sprintf (msg1, "There is no sector between vertex #%d and vertex #%d",
     vertex1, vertex2);
   Notify (-1, -1, msg1, NULL);
   return;
   }

/* Check if there is a closed path from vertex1 to vertex2,
   along the edge of the Sector s */
ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
llist = NULL;
curv = vertex1;
while (curv != vertex2)
   {
   //printf ("%d -> %d\t", curv, vertex2);
   for (l = 0; l < NumLineDefs; l++)
      {
      sd = LineDefs[l].sidedef1;
      if (sd >= 0 && SideDefs[sd].sector == s && LineDefs[l].start == curv)
         {
	 curv = LineDefs[l].end;
	 SelectObject (&llist, l);
	 break;
         }
      sd = LineDefs[l].sidedef2;
      if (sd >= 0 && SideDefs[sd].sector == s && LineDefs[l].end == curv)
         {
	 curv = LineDefs[l].start;
	 SelectObject (&llist, l);
	 break;
         }
      }
   if (l >= NumLineDefs)
      {
      Beep ();
      sprintf (msg1, "Cannot find a closed path from vertex #%d to vertex #%d",
        vertex1, vertex2);
      if (curv == vertex1)
	 sprintf (msg2, "There is no sidedef starting from Vertex #%d"
	   " on Sector #%d", vertex1, s);
      else
	 sprintf (msg2, "Check if Sector #%d is closed"
	   " (cannot go past Vertex #%d)", s, curv);
      Notify (-1, -1, msg1, msg2);
      ForgetSelection (&llist);
      return;
      }
   if (curv == vertex1)
      {
      Beep ();
      sprintf (msg1, "Vertex #%d is not on the same sector (#%d)"
        " as Vertex #%d", vertex2, s, vertex1);
      Notify (-1, -1, msg1, NULL);
      ForgetSelection (&llist);
      return;
      }
   }
/* now, the list of LineDefs for the new Sector is in llist */

/* add the new Sector, LineDef and SideDefs */
InsertObject (OBJ_SECTORS, s, 0, 0);
InsertObject (OBJ_LINEDEFS, -1, 0, 0);
LineDefs[NumLineDefs - 1].start = vertex1;
LineDefs[NumLineDefs - 1].end = vertex2;
LineDefs[NumLineDefs - 1].flags = 4;
InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
SideDefs[NumSideDefs - 1].sector = s;
strncpy (SideDefs[NumSideDefs - 1].tex3, "-", 8);
InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
strncpy (SideDefs[NumSideDefs - 1].tex3, "-", 8);
ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs - 2;
LineDefs[NumLineDefs - 1].sidedef2 = NumSideDefs - 1;

/* bind all LineDefs in llist to the new Sector */
while (llist)
{
   sd = LineDefs[llist->objnum].sidedef1;
   if (sd < 0 || SideDefs[sd].sector != s)
      sd = LineDefs[llist->objnum].sidedef2;
   SideDefs[sd].sector = NumSectors - 1;
   UnSelectObject (&llist, llist->objnum);
}

/* second check... useful for Sectors within Sectors */
ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
for (l = 0; l < NumLineDefs; l++)
{
   sd = LineDefs[l].sidedef1;
   if (sd >= 0 && SideDefs[sd].sector == s)
   {
      curv = GetOppositeSector (l, 1);
      ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
      if (curv == NumSectors - 1)
	 SideDefs[sd].sector = NumSectors - 1;
   }
   sd = LineDefs[l].sidedef2;
   if (sd >= 0 && SideDefs[sd].sector == s)
   {
      curv = GetOppositeSector (l, 0);
      ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
      if (curv == NumSectors - 1)
	 SideDefs[sd].sector = NumSectors - 1;
   }
}

MadeChanges = 1;
MadeMapChanges = 1;
}



/*
   split two LineDefs, then split the Sector and add a new LineDef between the new Vertices
*/

void SplitLineDefsAndSector (int linedef1, int linedef2) /* SWAP! */
{
SelPtr llist;
int    s1, s2, s3, s4;
char   msg[80];

/* check if the two LineDefs are adjacent to the same Sector */
ObjectsNeeded (OBJ_LINEDEFS, 0);
s1 = LineDefs[linedef1].sidedef1;
s2 = LineDefs[linedef1].sidedef2;
s3 = LineDefs[linedef2].sidedef1;
s4 = LineDefs[linedef2].sidedef2;
ObjectsNeeded (OBJ_SIDEDEFS, 0);
if (s1 >= 0)
   s1 = SideDefs[s1].sector;
if (s2 >= 0)
   s2 = SideDefs[s2].sector;
if (s3 >= 0)
   s3 = SideDefs[s3].sector;
if (s4 >= 0)
   s4 = SideDefs[s4].sector;
if ((s1 < 0 || (s1 != s3 && s1 != s4)) && (s2 < 0 || (s2 != s3 && s2 != s4)))
{
   Beep ();
   sprintf (msg, "Linedefs #%d and #%d are not adjacent to the same sector",
     linedef1, linedef2);
   Notify (-1, -1, msg, NULL);
   return;
}
/* split the two LineDefs and create two new Vertices */
llist = NULL;
SelectObject (&llist, linedef1);
SelectObject (&llist, linedef2);
SplitLineDefs (llist);
ForgetSelection (&llist);
/* split the Sector and create a LineDef between the two Vertices */
SplitSector (NumVertices - 1, NumVertices - 2);
}



