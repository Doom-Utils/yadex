/*
 *	prefer.cc
 *	AYM 1998-10-17
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
#include "entry.h"
#include "flats.h"
#include "gfx.h"
#include "levels.h"
#include "oldmenus.h"
#include "prefer.h"


/*
   display a "Preferences" menu (change default textures, etc.)
*/

void Preferences (int x0, int y0)
{
char   *menustr[9];
int     n, val;
char    texname[9];
int     width;
int     height;

width  = 2 * BOX_BORDER + 2 * WIDE_HSPACING + 50 * FONTW;
height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + (int) (10.5 * FONTH);
if (x0 < 0)
   x0 = (ScrMaxX + 1 - width) / 2;
if (y0 < 0)
   y0 = (ScrMaxY + 1 - height) / 2;
for (n = 0; n < 9; n++)
   menustr[n] = (char *) GetMemory (80);
sprintf (menustr[8], "Preferences");
sprintf (menustr[0], "Change default middle texture  (Current: %s)",
   default_middle_texture);
sprintf (menustr[1], "Change default upper texture   (Current: %s)",
   default_upper_texture);
sprintf (menustr[2], "Change default lower texture   (Current: %s)",
   default_lower_texture);
sprintf (menustr[3], "Change default floor texture   (Current: %s)",
   default_floor_texture);
sprintf (menustr[4], "Change default ceiling texture (Current: %s)",
   default_ceiling_texture);
sprintf (menustr[5], "Change default floor height    (Current: %d)",
   default_floor_height);
sprintf (menustr[6], "Change default ceiling height  (Current: %d)",
   default_ceiling_height);
sprintf (menustr[7], "Change default light level     (Current: %d)",
   default_light_level);
val = DisplayMenuArray (x0, y0, menustr[8], 8, NULL, menustr, NULL, NULL, NULL);
for (n = 0; n < 9; n++)
   FreeMemory (menustr[n]);
int subwin_x0 = x0 + BOX_BORDER + WIDE_HSPACING;
int subwin_y0 = y0 + BOX_BORDER + WIDE_VSPACING + (int) ((1.5 + val) * FONTH);
switch (val)
   {
   case 1:
      strcpy (texname, default_middle_texture);
      ChooseWallTexture (subwin_x0, subwin_y0, "Choose a wall texture",
        NumWTexture, WTexture, texname);
      if (strlen (texname) > 0)
	 strcpy (default_middle_texture, texname);
      break;
   case 2:
      strcpy (texname, default_upper_texture);
      ChooseWallTexture (subwin_x0, subwin_y0, "Choose a wall texture",
        NumWTexture, WTexture, texname);
      if (strlen (texname) > 0)
	 strcpy (default_upper_texture, texname);
      break;
   case 3:
      strcpy (texname, default_lower_texture);
      ChooseWallTexture (subwin_x0, subwin_y0, "Choose a wall texture",
        NumWTexture, WTexture, texname);
      if (strlen (texname) > 0)
	 strcpy (default_lower_texture, texname);
      break;
   case 4:
      strcpy (texname, default_floor_texture);
      ChooseFloorTexture (subwin_x0, subwin_y0, "Choose a floor texture",
        NumFTexture, FTexture, texname);
      if (strlen (texname) > 0)
	 strcpy (default_floor_texture, texname);
      break;
   case 5:
      strcpy (texname, default_ceiling_texture);
      ChooseFloorTexture (subwin_x0, subwin_y0, "Choose a ceiling texture",
        NumFTexture, FTexture, texname);
      if (strlen (texname) > 0)
	 strcpy (default_ceiling_texture, texname);
      break;
   case 6:
      val = InputIntegerValue (x0 + 42, subwin_y0, -512, 511, default_floor_height);
      if (val != IIV_CANCEL)
	 default_floor_height = val;
      break;
   case 7:
      val = InputIntegerValue (x0 + 42, subwin_y0, -512, 511, default_ceiling_height);
      if (val != IIV_CANCEL)
	 default_ceiling_height = val;
      break;
   case 8:
      val = InputIntegerValue (x0 + 42, subwin_y0, 0, 255, default_light_level);
      if (val != IIV_CANCEL)
	 default_light_level = val;
      break;
     
   }
}



