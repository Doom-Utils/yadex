/*
 *	savepic.cc
 *	This module contains the function SavePic() which is called
 *	whenever you press shift-F1 in the image viewers. It is only a
 *	wrapper for the gifsave library.
 *	AYM 1998-02-12
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
#if defined Y_UNIX
#elif defined Y_DOS
#include <gifsave.h>
#endif


#define PLAYPALNUM  0  /* palette no. to use when saving */

#ifdef Y_DOS
static int GetPixel (int x, int y);
#endif


void SavePic (int x0, int y0, int width, int height, const char *name)
#if defined Y_UNIX
{
return;  /* FIXME */
}
#elif defined Y_DOS
{
int n;
int r;
MDirPtr             dir;
unsigned char huge *dpal;
al_fspec_t filename;

strcpy (filename, name);
strlwr (filename);

/* Replace illegal file name characters by "_" */
#if defined Y_DOS
while (filename[n = strcspn (filename, "\"[\\]/*?.<>|+=:;^")])
  filename[n] = '_';
#elif defined Y_UNIX
while (filename[n = strcspn (filename, "/")])
  filename[n] = '_';
#endif

strcat (filename, ".gif");
r = GIF_Create (filename, width, height, 256, 8);
if (r != GIF_OK)
  LogMessage ("GIF_Create error %d\n", r);
if (PLAYPALNUM < 0 || PLAYPALNUM > 13)
  return;
dir = FindMasterDir (MasterDir, "PLAYPAL");
if (dir)
  {
  dpal = GetFarMemory (768);
  wad_seek (dir->wadfile, dir->dir.start + 3 * DOOM_COLOURS * PLAYPALNUM);
  wad_read_bytes (dir->wadfile, dpal, 3 * DOOM_COLOURS);
  for (n = 0; n < 256; n++)
    GIF_SetColor (n, dpal[3*n], dpal[3*n+1], dpal[3*n+2]);
  FreeFarMemory (dpal);
  }
r = GIF_CompressImage (x0, y0, width, height, GetPixel);
if (r != GIF_OK)
  LogMessage ("GIF_CompressImage error %d\n", r);
r = GIF_Close ();
LogMessage ("GIF_Close returned %d\n", r);
}
#endif


#ifdef Y_DOS
static int GetPixel (int x, int y)
{
return getpixel (x, y);
}
#endif

