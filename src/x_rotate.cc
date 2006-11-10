/*
 *	x_rotate.cc
 *	Rotate and scale a group of objects
 *	AYM 1998-02-07
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
#include <math.h>
#include "l_vertices.h"
#include "levels.h"
#include "objid.h"
#include "s_vertices.h"
#include "selectn.h"
#include "t_centre.h"
#include "v_centre.h"
#include "x_rotate.h"


/*
   move (x, y) to a new position: rotate and scale around (0, 0)
*/

#if 0
inline void RotateAndScaleCoords (int *x, int *y, double angle, double scale)
{
  double r, theta;

  r = hypot ((double) *x, (double) *y);
  theta = atan2 ((double) *y, (double) *x);
  *x = (int) (r * scale * cos (theta + angle) + 0.5);
  *y = (int) (r * scale * sin (theta + angle) + 0.5);
}
#endif


/*
   rotate and scale a group of objects around the centre of gravity
*/

void RotateAndScaleObjects (int objtype, SelPtr obj, double angle, double scale) /* SWAP! */
{
  int    dx, dy;
  int    centerx, centery;
  SelPtr cur, vertices;

  if (obj == NULL)
    return;
  ObjectsNeeded (objtype, 0);
  bool unit = fabs (scale - 1) < 1e-4;

  switch (objtype)
  {
    case OBJ_THINGS:
      centre_of_things (obj, &centerx, &centery);
      if (unit && fabs (angle) < 1e-4)				// 0 degrees
	break;
      if (unit && fabs (angle - HALFPI) < 1e-4)			// 90 degrees
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Thing *t = Things + cur->objnum;
	  dx = t->xpos - centerx;
	  dy = t->ypos - centery;
	  t->xpos = centerx - dy;
	  t->ypos = centery + dx;
	}
      }
      else if (unit && fabs (angle - ONEPI) < 1e-4)		// 180 degrees
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Thing *t = Things + cur->objnum;
	  t->xpos = 2 * centerx - t->xpos;
	  t->ypos = 2 * centery - t->ypos;
	}
      }
      else if (unit && fabs (angle - (ONEPI + HALFPI)) < 1e-4)	// 270 degrees
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Thing *t = Things + cur->objnum;
	  dx = t->xpos - centerx;
	  dy = t->ypos - centery;
	  t->xpos = centerx + dy;
	  t->ypos = centery - dx;
	}
      }
      else
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Thing *t = Things + cur->objnum;
	  dx = t->xpos - centerx;
	  dy = t->ypos - centery;
	  RotateAndScaleCoords (&dx, &dy, angle, scale);
	  t->xpos = centerx + dx;
	  t->ypos = centery + dy;
	}
      }
      MadeChanges = 1;
      break;

    case OBJ_VERTICES:
      centre_of_vertices (obj, &centerx, &centery);
      if (unit && fabs (angle) < 1e-4)				// 0 degrees
	break;
      if (unit && fabs (angle - HALFPI) < 1e-4)			// 90 degrees
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Vertex *v = Vertices + cur->objnum;
	  dx = v->x - centerx;
	  dy = v->y - centery;
	  v->x = centerx - dy;
	  v->y = centery + dx;
	}
      }
      else if (unit && fabs (angle - ONEPI) < 1e-4)		// 180 degrees
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Vertex *v = Vertices + cur->objnum;
	  v->x = 2 * centerx - v->x;
	  v->y = 2 * centery - v->y;
	}
      }
      else if (unit && fabs (angle - (ONEPI + HALFPI)) < 1e-4)	// 270 degrees
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Vertex *v = Vertices + cur->objnum;
	  dx = v->x - centerx;
	  dy = v->y - centery;
	  v->x = centerx + dy;
	  v->y = centery - dx;
	}
      }
      else
      {
	for (cur = obj; cur != 0; cur = cur->next)
	{
	  struct Vertex *v = Vertices + cur->objnum;
	  dx = v->x - centerx;
	  dy = v->y - centery;
	  RotateAndScaleCoords (&dx, &dy, angle, scale);
	  v->x = centerx + dx;
	  v->y = centery + dy;
	}
      }
      MadeChanges = 1;
      MadeMapChanges = 1;
      break;

    case OBJ_LINEDEFS:
      vertices = list_vertices_of_linedefs (obj);
      RotateAndScaleObjects (OBJ_VERTICES, vertices, angle, scale);
      ForgetSelection (&vertices);
      break;

    case OBJ_SECTORS:
      ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
      vertices = list_vertices_of_sectors (obj);
      RotateAndScaleObjects (OBJ_VERTICES, vertices, angle, scale);
      ForgetSelection (&vertices);
      break;
  }
}

