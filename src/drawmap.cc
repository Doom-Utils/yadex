/*
 *	drawmap.cc
 *	AYM 1998-09-06
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2000 André Majorel.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include "yadex.h"
#include <X11/Xlib.h>
#include "_edit.h"
#include "drawmap.h"
#include "gfx.h"
#include "levels.h"
#include "things.h"


/*
  draw the actual game map
*/

void draw_map (edit_t *e) /* SWAP! */
{
/* Draw the grid.
   Optimization: set_colour is not called unnecessarily. */
if (e->grid_shown)
   {
   int current_colour = INT_MIN;  // Some impossible colour number
   int new_colour;
   int mapx0;
   int mapx1;
   int mapy0;
   int mapy1;
   int grid_step2;
#ifdef NEW_GRID
   XPoint *points;
   int npoints;
#endif

   mapx0 = MAPX (0)                        & ~(e->grid_step - 1);
   mapx1 = (MAPX (ScrMaxX) + e->grid_step) & ~(e->grid_step - 1);
   mapy0 = (MAPY (ScrMaxY))                & ~(e->grid_step - 1);
   mapy1 = (MAPY (0)       + e->grid_step) & ~(e->grid_step - 1);

#ifdef NEW_GRID
   npoints = (mapx1 - mapx0) / e->grid_step + 1;
   points = (XPoint *) malloc (npoints * sizeof *points);
   points[0].x = SCREENX (mapx0);
   for (int n = 1; n < npoints; n++)
      {
      points[n].x = SCREENX (mapx0 + n * e->grid_step)
                  - SCREENX (mapx0 + (n - 1) * e->grid_step);
      points[n].y = 0;
      }
#endif

   for (grid_step2 = 64; grid_step2 < 4 * e->grid_step; grid_step2 *= 4)
      ;

   for (int i = mapx0; i <= mapx1; i += e->grid_step)
      {
      new_colour = i % grid_step2 ? GRID1 : GRID2;
      if (new_colour != current_colour)
	set_colour (current_colour = new_colour);
#ifdef NEW_GRID
      if (i % (4 * e->grid_step) == 0)
#endif
         DrawMapLine (i, mapy0, i, mapy1);
      }
   for (int j = mapy0; j <= mapy1; j += e->grid_step)
      {
      new_colour = j % grid_step2 ? GRID1 : GRID2;
      if (new_colour != current_colour)
	set_colour (current_colour = new_colour);
#ifdef NEW_GRID
      if (j % (4 * e->grid_step) == 0)
#endif
         DrawMapLine (mapx0, j, mapx1, j);
#ifdef NEW_GRID
      else
         {
	 set_colour (current_colour = GRID3);
         points[0].y = SCREENY (j);
	 XDrawPoints (dpy, drw, gc, points, npoints, CoordModePrevious);
         }
#endif
      }

#ifdef NEW_GRID
   free (points);
#endif
   }

int mapx0 = MAPX (0);
int mapx9 = MAPX (ScrMaxX);
int mapy0 = MAPY (ScrMaxY);
int mapy9 = MAPY (0);
int n, m;

/* Draw the linedefs to form the map.
   Optimization: off-screen lines are not drawn
   and set_colour is not called unnecessarily. */
switch (e->obj_type)
   {
   case OBJ_THINGS:
      {
      int current_colour = INT_MIN;  /* Some impossible colour no. */
      int new_colour;
      
      ObjectsNeeded (OBJ_LINEDEFS, OBJ_VERTICES, 0);
      for (n = 0; n < NumLineDefs; n++)
	 {
	 register int x1 = Vertices[LineDefs[n].start].x;
	 register int x2 = Vertices[LineDefs[n].end  ].x;
	 register int y1 = Vertices[LineDefs[n].start].y;
	 register int y2 = Vertices[LineDefs[n].end  ].y;
	 if (x1 < mapx0 && x2 < mapx0
	  || x1 > mapx9 && x2 > mapx9
	  || y1 < mapy0 && y2 < mapy0
	  || y1 > mapy9 && y2 > mapy9)
	    continue;
	 if (LineDefs[n].flags & 1)
	    new_colour = WHITE;
	 else
	    new_colour = LIGHTGRAY;
	 if (new_colour != current_colour)
	    set_colour (current_colour = new_colour);
	 DrawMapLine (x1, y1, x2, y2);
	 }
      break;
      }
   case OBJ_VERTICES:
     ObjectsNeeded (OBJ_LINEDEFS, OBJ_VERTICES, 0);
     set_colour (LIGHTGRAY);
     for (n = 0; n < NumLineDefs; n++)
	{
	register int x1 = Vertices[LineDefs[n].start].x;
	register int x2 = Vertices[LineDefs[n].end  ].x;
	register int y1 = Vertices[LineDefs[n].start].y;
	register int y2 = Vertices[LineDefs[n].end  ].y;
	if (x1 < mapx0 && x2 < mapx0
	 || x1 > mapx9 && x2 > mapx9
	 || y1 < mapy0 && y2 < mapy0
	 || y1 > mapy9 && y2 > mapy9)
	   continue;
	DrawMapVector (x1, y1, x2, y2);
	}
     break;
   case OBJ_LINEDEFS:
      {
      int current_colour = INT_MIN;  /* Some impossible colour no. */
      int new_colour;

      ObjectsNeeded (OBJ_LINEDEFS, OBJ_VERTICES, 0);
      for (n = 0; n < NumLineDefs; n++)
	 {
	 register int x1 = Vertices[LineDefs[n].start].x;
	 register int x2 = Vertices[LineDefs[n].end  ].x;
	 register int y1 = Vertices[LineDefs[n].start].y;
	 register int y2 = Vertices[LineDefs[n].end  ].y;
	 if (x1 < mapx0 && x2 < mapx0
	  || x1 > mapx9 && x2 > mapx9
	  || y1 < mapy0 && y2 < mapy0
	  || y1 > mapy9 && y2 > mapy9)
	    continue;
	 if (LineDefs[n].type != 0)  /* AYM 19980207: was "> 0" */
	    {
	    if (LineDefs[n].tag != 0)  /* AYM 19980207: was "> 0" */
	       new_colour = LIGHTMAGENTA;
	    else
	       new_colour = LIGHTGREEN;
	    }
	 else if (LineDefs[n].tag != 0)  /* AYM 19980207: was "> 0" */
	    new_colour = LIGHTRED;
	 else if (LineDefs[n].flags & 1)
	    new_colour = WHITE;
	 else
	    new_colour = LIGHTGRAY;
	 if (new_colour != current_colour)
	    set_colour (current_colour = new_colour);
	 DrawMapLine (x1, y1, x2, y2);
	 }
      break;
      }
   case OBJ_SECTORS:
      {
      int current_colour = INT_MIN;  /* Some impossible colour no. */
      int new_colour;

      ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
      for (n = 0; n < NumLineDefs; n++)
	 {
	 register int x1 = Vertices[LineDefs[n].start].x;
	 register int x2 = Vertices[LineDefs[n].end  ].x;
	 register int y1 = Vertices[LineDefs[n].start].y;
	 register int y2 = Vertices[LineDefs[n].end  ].y;
	 if (x1 < mapx0 && x2 < mapx0
	  || x1 > mapx9 && x2 > mapx9
	  || y1 < mapy0 && y2 < mapy0
	  || y1 > mapy9 && y2 > mapy9)
	    continue;
	 if ((m = LineDefs[n].sidedef1) < 0 || (m = SideDefs[m].sector) < 0)
	    new_colour = LIGHTRED;
	 else
	    {
	    if (Sectors[m].tag != 0)  /* AYM 19980207: was "> 0" */
	       new_colour = LIGHTGREEN;
	    else if (Sectors[m].special > 0)
	       new_colour = LIGHTCYAN;
	    else if (LineDefs[n].flags & 1)
	       new_colour = WHITE;
	    else
	       new_colour = LIGHTGRAY;
	    if ((m = LineDefs[n].sidedef2) >= 0)
	       {
	       if ((m = SideDefs[m].sector) < 0)
		  new_colour = LIGHTRED;
	       else if (Sectors[m].tag != 0)  /* AYM 19980207: was "> 0" */
		  new_colour = LIGHTGREEN;
	       else if (Sectors[m].special > 0)
		  new_colour = LIGHTCYAN;
	       }
	    }
	 ObjectsNeeded (OBJ_LINEDEFS, OBJ_VERTICES, 0);
	 if (new_colour != current_colour)
	    set_colour (current_colour = new_colour);
	 DrawMapLine (x1, y1, x2, y2);
	 }
      break;
      }
   }

/* Draw in the vertices.
   Optimization: off-screen vertices are not drawn. */
if (e->obj_type == OBJ_VERTICES)
   {
   int r = (int) (OBJSIZE * y_min (Scale, 1));
   set_colour (LIGHTGREEN);
   for (n = 0; n < NumVertices; n++)
      {
      int mapx = Vertices[n].x;
      int mapy = Vertices[n].y;
      if (mapx >= mapx0 && mapx <= mapx9 && mapy >= mapy0 && mapy <= mapy9)
	 {
	 register int scrx = SCREENX (mapx);
	 register int scry = SCREENY (mapy);
	 DrawScreenLine (scrx - r, scry - r, scrx + r, scry + r);
	 DrawScreenLine (scrx + r, scry - r, scrx - r, scry + r);
	 }
      }
   if (e->show_object_numbers && e->obj_type == OBJ_VERTICES)
      {
      for (n = 0; n < NumVertices; n++)
         {
	 int mapx = Vertices[n].x;
	 int mapy = Vertices[n].y;
	 if (mapx >= mapx0 && mapx <= mapx9 && mapy >= mapy0 && mapy <= mapy9)
	    {
	    int x = SCREENX (mapx) + 2 * r;
	    int y = SCREENY (mapy) + 2;
	    set_colour (BLACK);
            DrawScreenText (x + 1, y,     "%d", n);
            DrawScreenText (x - 1, y,     "%d", n);
            DrawScreenText (x,     y + 1, "%d", n);
            DrawScreenText (x,     y - 1, "%d", n);
	    set_colour (CYAN);
            DrawScreenText (x,     y,     "%d", n);
	    }
         }
      }
   }

/* Draw in the things.
   Optimization: off-screen things are not drawn. */
ObjectsNeeded (OBJ_THINGS, 0);
if (e->obj_type != OBJ_THINGS)
  set_colour (LIGHTGRAY);

// The radius of the largest thing.
int max_radius = get_max_thing_radius ();

// A thing is guaranteed to be totally off-screen
// if its centre is more than <max_radius> units
// beyond the edge of the screen.
mapx0 -= max_radius;
mapx9 += max_radius;
mapy0 -= max_radius;
mapy9 += max_radius;

for (n = 0; n < NumThings; n++)
   {
   int mapx = Things[n].xpos;
   int mapy = Things[n].ypos;
   int corner_x;
   int corner_y;
   if (mapx < mapx0 || mapx > mapx9 || mapy < mapy0 || mapy > mapy9)
      continue;
   if (e->obj_type == OBJ_THINGS)
      {
      m = get_thing_radius (Things[n].type);
      set_colour (get_thing_colour (Things[n].type));
#ifdef ROUND_THINGS
      DrawMapLine (mapx - m, mapy,     mapx + m, mapy    );
      DrawMapLine (mapx,     mapy - m, mapx,     mapy + m);
      DrawMapCircle (mapx, mapy, m);
#else
      DrawMapLine (mapx - m, mapy - m, mapx + m, mapy - m);
      DrawMapLine (mapx + m, mapy - m, mapx + m, mapy + m);
      DrawMapLine (mapx + m, mapy + m, mapx - m, mapy + m);
      DrawMapLine (mapx - m, mapy + m, mapx - m, mapy - m);
#endif
      }
   else
     {
     // If Yadex is too slow, you can replace this by sth like "m = 16;"
     m = get_thing_radius (Things[n].type);
#ifdef ROUND_THINGS
     DrawMapLine (mapx - OBJSIZE, mapy, mapx+ OBJSIZE, mapy);
     DrawMapLine (mapx, mapy - OBJSIZE, mapx, mapy+ OBJSIZE);
#else
     DrawMapLine (mapx - m, mapy - m, mapx + m, mapy - m);
     DrawMapLine (mapx + m, mapy - m, mapx + m, mapy + m);
     DrawMapLine (mapx + m, mapy + m, mapx - m, mapy + m);
     DrawMapLine (mapx - m, mapy + m, mapx - m, mapy - m);
#endif
     }
   switch (Things[n].angle)
      {
      case 0 :
                 corner_x =  m;  corner_y =  0;  break;
      case 45 :
                 corner_x =  m;  corner_y =  m;  break;
      case 90 :
                 corner_x =  0;  corner_y =  m;  break;
      case 135 :
                 corner_x = -m;  corner_y =  m;  break;
      case 180 :
                 corner_x = -m;  corner_y =  0;  break;
      case 225 :
                 corner_x = -m;  corner_y = -m;  break;
      case 270 :
                 corner_x =  0;  corner_y = -m;  break;
      case 315 :
                 corner_x =  m;  corner_y = -m;  break;
      default :
                 corner_x =  0;  corner_y =  0;  break;
      }
#ifndef ROUND_THINGS
   DrawMapLine (mapx, mapy, mapx + corner_x, mapy + corner_y);
#endif
   }

/* Draw the things numbers */
if (e->show_object_numbers && e->obj_type == OBJ_THINGS)
   {
   for (n = 0; n < NumThings; n++)
      {
      int mapx = Things[n].xpos;
      int mapy = Things[n].ypos;
      if (mapx < mapx0 || mapx > mapx9 || mapy < mapy0 || mapy > mapy9)
	continue;
      int x = SCREENX (mapx) + 10;
      int y = SCREENY (mapy) + 2;
      set_colour (BLACK);
      DrawScreenText (x + 1, y,     "%d", n);
      DrawScreenText (x - 1, y,     "%d", n);
      DrawScreenText (x,     y + 1, "%d", n);
      DrawScreenText (x,     y - 1, "%d", n);
      set_colour (CYAN);
      DrawScreenText (x, y, "%d", n);
      }
   }
}


