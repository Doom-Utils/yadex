/*
 *	editzoom.cc
 *	Zoom functions
 *	AYM 1998-11-09
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1998 André Majorel.

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
#include "_edit.h"
#include "editzoom.h"
#include "events.h"
#include "gfx.h"


void edit_zoom_in (edit_t *e)
{
if (! e) return;  // Prevent the compiler from emitting warning about unused .p.
if (Scale >= 8.0)
   return;

OrigX += (int) ((is.x - ScrCenterX) / Scale);
OrigY += (int) ((ScrCenterY - is.y) / Scale);
if (Scale < 1.0)
   Scale = 1.0 / ((1.0 / Scale) - 1.0);
else
   Scale = Scale * 2.0;
OrigX -= (int) ((is.x - ScrCenterX) / Scale);
OrigY -= (int) ((ScrCenterY - is.y) / Scale);
send_event (YE_ZOOM_CHANGED);
}


void edit_zoom_out (edit_t *e)
{
if (! e) return;  // Prevent the compiler from emitting warning about unused .p.
if (Scale <= 0.05)
   return;

OrigX += (int) ((is.x - ScrCenterX) / Scale);
OrigY += (int) ((ScrCenterY - is.y) / Scale);
if (Scale < 1.0)
   Scale = 1.0 / ((1.0 / Scale) + 1.0);
else
   Scale = Scale / 2.0;
OrigX -= (int) ((is.x - ScrCenterX) / Scale);
OrigY -= (int) ((ScrCenterY - is.y) / Scale);
send_event (YE_ZOOM_CHANGED);
}


void edit_set_zoom (edit_t *e, double zoom_factor)
{
if (! e) return;  // Prevent the compiler from emitting warning about unused .p.
OrigX += (int) ((is.x - ScrCenterX) / Scale);
OrigY += (int) ((ScrCenterY - is.y) / Scale);
Scale = zoom_factor;
OrigX -= (int) ((is.x - ScrCenterX) / Scale);
OrigY -= (int) ((ScrCenterY - is.y) / Scale);
send_event (YE_ZOOM_CHANGED);
}




