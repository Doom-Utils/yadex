/*
 *	highlt.cc
 *	AYM 1998-09-20
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
#include "highlt.h"


highlight_c::highlight_c (void)
{
obj_no      = OBJ_NO_NONE;
obj_no_disp = OBJ_NO_NONE;
}


void highlight_c::draw (void)
{
if (! is_obj (obj_no_disp) && is_obj (obj_no))
   {
   HighlightObject (obj_type, obj_no, YELLOW);
   obj_no_disp   = obj_no;
   obj_type_disp = obj_type;
   }
}


void highlight_c::undraw (void)
{
if (is_obj (obj_no_disp)
   && (obj_no_disp != obj_no || obj_type_disp != obj_type))
   {
   HighlightObject (obj_type_disp, obj_no_disp, YELLOW);
   obj_no_disp = OBJ_NO_NONE;
   }
}


