/*
 *	sprites.cc
 *	AYM 1998-??-??
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
#include "disppic.h"
#include "gfx.h"


static int sort_sprites (const void *a, const void *b);


/*
   choose a "sprite"
*/

void ChooseSprite (int x0, int y0, const char *prompt, const char *sname)
{
MDirPtr dir;
int listsize;
char **list;
char name[WAD_PIC_NAME + 1];

/* Get a list of the sprites into list[] */
dir = FindMasterDir (MasterDir, "S_START");
if (dir)
   {
   dir = dir->next;
   // How many sprites are there ?
   for (listsize = 0; dir && strcmp (dir->dir.name, "S_END"); listsize++)
      dir = dir->next;

   // Get their names into list, sorted.
   dir = FindMasterDir (MasterDir, "S_START");
   dir = dir->next;
   list = (char **) GetMemory (listsize * sizeof (char *));
   for (int n = 0; n < listsize; n++)
      {
      list[n] = (char *) GetMemory (WAD_NAME + 1);
      al_scps (list[n], dir->dir.name, WAD_NAME);
      dir = dir->next;
      }
   qsort (list, listsize, sizeof (char *), sort_sprites);
   }
else  // S_START not found (should not happen with valid iwad)
   {
   // Make a list with only one element.
   // I think an empty list would make
   // InputNameFromListWithFunc() barf.
   listsize = 1;
   list = (char **) GetMemory (sizeof (char *));
   *list = (char *) GetMemory (6);
   strcpy (*list, "bogus");
   }

if (sname != NULL)
   al_scps (name, sname, WAD_NAME);
else
   al_scps (name, list[0], WAD_NAME);
HideMousePointer ();
SwitchToVGA256 ();
/* If we only have a 320x200x256 VGA driver, we must change x0 and y0.
   Yuck! */
if (GfxMode > -2)
   {
   x0 = 0;
   y0 = -1;
   }
InputNameFromListWithFunc (x0, y0, prompt, (size_t) listsize, list, 11, name,
  320, 200, display_pic, HOOK_DISP_SIZE);
SwitchToVGA16 ();
ShowMousePointer ();
for (int n = 0; n < listsize; n++)
   FreeMemory (list[n]);
FreeMemory (list);
}


/*
 *	sort_sprites
 *	Function used by qsort() to sort the sprite names.
 */
static int sort_sprites (const void *a, const void *b)
{
return y_strnicmp (*((const char *const *)a), *((const char *const *)b),
    WAD_PIC_NAME);
}



/*
 *	sprite_by_root
 *	Return the first sprite that has a name
 *	that starts with <root>. Oh my god, does
 *	this routine suck. Yet another QADH.
 *	Excuse me for a moment, I have to go vomit.
 */
const char *sprite_by_root (const char *root)
{
MDirPtr dir;
size_t root_len = strlen (root);

// How many sprites are there ?
dir = FindMasterDir (MasterDir, "S_START");
dir = dir->next;
while (dir && strcmp (dir->dir.name, "S_END"))
   {
   if (! y_strnicmp (root, dir->dir.name, root_len))
      return dir->dir.name;
   dir = dir->next;
   }
return NULL;
}



