/*
 *	help2.cc
 *	AYM 1998-08-17
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
#include "gfx.h"
#include "help1.h"  // what()
#include "help2.h"


/*
   display the help screen
 */

static const char *help_text[] =
  {
  "Keyboard :",
  "q     Quit, saving changes      PgUp  Scroll up",
  "Esc   Quit without saving       PgDn  Scroll down",
  "                                Home  Scroll left",
  "Tab   Next mode                 End   Scroll right",
  "ShiftTab Previous mode          '     Go to centre of map",
  "l     Linedefs & sidedefs mode  `     Show whole map",
  "s     Sectors mode              n,>   Jump to next object",
  "t     Things mode               p,<   Jump to previous object",
  "v     Vertices mode             j,#   Jump to object #N",
  "&     Show/hide object numbers",
  "                                +/-   Zoom in/out",
  "Ins   Insert a new object       g/G   Decr./incr. the grid step",
  "Del   Delete the object(s)      h     Hide/show the grid",
  "Enter Edit object properties    H     Reset grid step to the max",
  "x/w   Spin things cw/ccw        z     Lock the grid step",
  "a     Set things/ld flags       y     Snap to grid on/off",
  "b     Toggle things/ld flags    Space Toggle extra zoom",
  "c     Clear things/ld flags",
  "F8    Misc. operations          F5    Preferences",
  "F9    Insert compound object    F10   Checks"
  "",
  "Mouse :",
  "- The map scrolls when you move the pointer near the edge of the",
  "  window.",
  "- Clicking on an object with the left button selects it (and",
  "  unselects everything else unless [Ctrl] is pressed).",
  "- Clicking on an already selected object with the left button with",
  "  [Ctrl] pressed unselects it.",
  "- Double clicking on an object allows to change its properties.",
  "- You can also drag objects with the left button.",
  "- Clicking on an empty space with the left button and moving draws",
  "  a rectangular selection box. Releasing the button selects",
  "  everything in that box (and unselects everything else unless",
  "  [Ctrl] is pressed).",
  "- Wheel or buttons 4 and 5: zoom in and out",
  NULL
  };

void DisplayHelp () /* SWAP! */
{
int x0;
int y0;
int width;
int height;
size_t maxlen = 0;
int lines = 4;

for (const char **str = help_text; *str; str++)
   {
   size_t len = strlen (*str);
   maxlen = max (maxlen, len);
   lines++;
   }
width  = (maxlen + 4) * FONTW + 2 * BOX_BORDER;
height = lines * FONTH + 2 * BOX_BORDER;
x0 = (ScrMaxX + 1 - width) / 2;
y0 = (ScrMaxY + 1 - height) / 2;
HideMousePointer ();
/* put in the instructions */
DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
set_colour (LIGHTCYAN);
DrawScreenText (x0 + BOX_BORDER + (width - 5 * FONTW) / 2,
                y0 + BOX_BORDER + FONTH / 2,
                "Yadex");
set_colour (WINFG);
DrawScreenText (x0 + BOX_BORDER + 2 * FONTW, y0 + BOX_BORDER + FONTH, "");
for (const char **str = help_text; *str; str++)
   DrawScreenText (-1, -1, *str);
set_colour (YELLOW);
DrawScreenText (-1, -1, "Press any key to return to the editor...");
get_key_or_click ();
ShowMousePointer ();
}


/*
 *	about_yadex()
 *	The name says it all.
 */
void about_yadex ()
{
int width  = 2 * BOX_BORDER + 2 * WIDE_HSPACING + 42 * FONTW;
int height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + 14 * FONTH;
int x0 = (ScrMaxX + 1 - width) / 2;
int y0 = (ScrMaxY + 1 - height) / 2;

HideMousePointer ();
DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
set_colour (WINFG);
DrawScreenText (x0 + BOX_BORDER + WIDE_HSPACING,
                y0 + BOX_BORDER + WIDE_VSPACING, what ());
DrawScreenText (-1, -1, "");
DrawScreenText (-1, -1, "Copyright © 1997-1998 André Majorel");
DrawScreenText (-1, -1, "Yadex is derived from DEU 5.21 by");
DrawScreenText (-1, -1, "Raphaël Quinet and Brendon Wyber.");
DrawScreenText (-1, -1, "");
DrawScreenText (-1, -1, "The home page of Yadex is :");
DrawScreenText (-1, -1, "http://www.teaser.fr/~amajorel/yadex/");
DrawScreenText (-1, -1, "");
DrawScreenText (-1, -1, "To contact the maintainer, send email to :");
DrawScreenText (-1, -1, "André Majorel <amajorel@teaser.fr>");
DrawScreenText (-1, -1, "");
DrawScreenText (-1, -1, "");
set_colour (YELLOW);
DrawScreenText (-1, -1, "Press any key to return to the editor...");
get_key_or_click ();
ShowMousePointer ();
}


