/*
 *	flats.cc
 *	AYM 1998-??-??
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
#include "flats.h"
#include "gfx.h"
#include "dispimg.h"


/*
   choose a floor or ceiling texture
*/

void ChooseFloorTexture (int x0, int y0, const char *prompt, int listsize, char **list, char *name)
{
HideMousePointer ();

SwitchToVGA256 ();
/* If we only have a 320x200x256 VGA driver, we must change x0 and y0.
   Yuck! */
if (GfxMode > -2)
   {
   x0 = -1;
   y0 = -1;
   }
InputNameFromListWithFunc (x0, y0, prompt, listsize, list, 5, name,
  64, 64, DisplayFloorTexture);
SwitchToVGA16 ();

ShowMousePointer ();
}


/*
   display a floor or ceiling texture at coords c->x0, c->y0
   and not beyond c->x1, c->y1
*/

void DisplayFloorTexture (hookfunc_comm_t *c)
{
MDirPtr        dir;	/* main directory pointer to the texture entry */
unsigned char *pixels;	/* array of pixels that hold the image */

c->width  = DOOM_FLAT_WIDTH;  // Big deal !
c->height = DOOM_FLAT_HEIGHT;
c->flags  = HOOK_SIZE_VALID;

/* AYM 1997-08-17
   Now handles replaced textures by searching for the texture c->name
   in all the F1_START/F1_END F2_START/F2_END and FF_START/F_END groups
   in the main directory.
   Bugfix: not confused anymore by non-texture lumps of same name. */
{
char state;
MDirPtr wdir;

dir = 0;
for (wdir = MasterDir, state = 0; wdir; wdir = wdir->next)
   {
   if (! state)
      {
      if (! strncmp (wdir->dir.name, "F1_START", 8))
	 state = '1';
      else if (! strncmp (wdir->dir.name, "F2_START", 8))
	 state = '2';
      else if (! strncmp (wdir->dir.name, "F3_START", 8))
	 state = '3';
      else if (! strncmp (wdir->dir.name, "FF_START", 8))
	 state = 'f';
      }
   else if (state == '1' && ! strncmp (wdir->dir.name, "F1_END", 8))
      state = 0; 
   else if (state == '2' && ! strncmp (wdir->dir.name, "F2_END", 8))
      state = 0; 
   else if (state == '3' && ! strncmp (wdir->dir.name, "F3_END", 8))
      state = 0; 
   else if (state == 'f' && ! strncmp (wdir->dir.name, "F_END", 8))
      state = 0; 
   else if (state && ! strncmp (wdir->dir.name, c->name, 8))
      dir = wdir;
   }
if (state)
   fatal_error ("Reached end of master directory with state=\"%c\"", state);
}
  
if (! dir)
   {
   set_colour (WINFG_DIM);
   DrawScreenLine (c->x0, c->y0, c->x1, c->y1);
   DrawScreenLine (c->x0, c->y1, c->x1, c->y0);
   return;
   }

wad_seek (dir->wadfile, dir->dir.start);

#if defined Y_X11

pixels = (unsigned char huge *) GetFarMemory (c->width * c->height);
wad_read_bytes (dir->wadfile, pixels, (long) c->width * c->height);
display_game_image (pixels, c->width, c->height, c->x0, c->y0,
   c->x1 - c->x0 + 1, c->y1 - c->y0 + 1);

#elif defined Y_BGI

pixels = GetFarMemory (c->width * c->height + 4);
wad_read_bytes (dir->wadfile, pixels + 4, (long) c->width * c->height);
if (GfxMode < -1)
   {
   /* Probably a bug in the VESA driver...    */
   /* It requires "size-1" instead of "size"! */
   ((unsigned short huge *)pixels)[0] = c->width - 1;
   ((unsigned short huge *)pixels)[1] = c->height - 1;
   }
else
   {
   ((unsigned short huge *)pixels)[0] = c->width;
   ((unsigned short huge *)pixels)[1] = c->height;
   }
putimage (c->x0, c->y0, pixels, COPY_PUT);

#endif

FreeFarMemory (pixels);
c->flags |= HOOK_DRAWN;
}


/*
 *	display_flat_depressed
 *	Display a flat inside a hollow box
 */
void display_flat_depressed (hookfunc_comm_t *c)
{
draw_box_border (c->x0, c->y0, c->x1 - c->x0 + 1, c->y1 - c->y0 + 1,
   HOLLOW_BORDER, 0);
c->x0 += HOLLOW_BORDER;
c->y0 += HOLLOW_BORDER;
c->x1 -= HOLLOW_BORDER;
c->y1 -= HOLLOW_BORDER;
DisplayFloorTexture (c);
c->x0 -= HOLLOW_BORDER;
c->y0 -= HOLLOW_BORDER;
c->x1 += HOLLOW_BORDER;
c->y1 += HOLLOW_BORDER;
}



