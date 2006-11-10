/*
 *	t_centre.cc
 *	AYM 1998-11-22
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-2005 Andr� Majorel and others.

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
#include "bitvec.h"
#include "levels.h"
#include "selectn.h"
#include "t_centre.h"


/*
 *	centre_of_things
 *	Return the coordinates of the centre of a group of things.
 */
void centre_of_things (SelPtr list, int *x, int *y)
{
SelPtr cur;
int nitems;
long x_sum;
long y_sum;

x_sum = 0;
y_sum = 0;
for (nitems = 0, cur = list; cur; cur = cur->next, nitems++)
   {
   x_sum += Things[cur->objnum].xpos;
   y_sum += Things[cur->objnum].ypos;
   }
if (nitems == 0)
   {
   *x = 0;
   *y = 0;
   }
else
   {
   *x = (int) (x_sum / nitems);
   *y = (int) (y_sum / nitems);
   }
}


