/*
 *	highlt.cc
 *	AYM 1998-09-20
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
#include "highlt.h"
#include "objects.h"
#include "objid.h"


highlight_c::highlight_c (void)
{
obj.nil ();
obj_disp.nil ();
}


void highlight_c::draw (void)
{
if (! obj_disp () && obj ())
   {
   HighlightObject (obj.type, obj.num, YELLOW);
   obj_disp = obj;
   }
}


void highlight_c::undraw (void)
{
if (obj_disp () && ! (obj_disp == obj))
   {
   HighlightObject (obj_disp.type, obj_disp.num, YELLOW);
   obj_disp.nil ();
   }
}


