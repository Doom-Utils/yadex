/*
 *	textures.cc
 *	Trevor Phillips, RQ and Christian Johannes Schladetsch
 *	sometime in 1994.
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
#include "dialog.h"
#include "clearimg.h"
#include "dispimg.h"
#include "gfx.h"
#include "pic2img.h"
#include "wads.h"


/*
   display the picture named c->name
   at coords c->x0, c->y0 and not beyond c->x1, c->y1
*/

void DisplayPic (hookfunc_comm_t *c)
{
MDirPtr	dir;
i16	xsize, ysize, xofs, yofs;
u8	*ColumnData;
u8	*Column;
i32	*NeededOffsets;
int	Columns, CurrentColumn;
i32	CurrentOffset;
int	ColumnInMemory;
long	ActualBufLen;

c->flags = 0;

if (have_key ())
   return; /* speedup */

dir = FindMasterDir (MasterDir, c->name);
if (dir == NULL)
   {
   set_colour (DARKGRAY);
   DrawScreenLine (c->x0, c->y0, c->x1, c->y1);
   DrawScreenLine (c->x0, c->y1, c->x1, c->y0);
   return;
   }
wad_seek (dir->wadfile, dir->dir.start);
wad_read_i16 (dir->wadfile, &xsize);
wad_read_i16 (dir->wadfile, &ysize);
wad_read_i16 (dir->wadfile, &xofs );
wad_read_i16 (dir->wadfile, &yofs );
c->width    = xsize;
c->height   = ysize;
c->flags   |= HOOK_SIZE_VALID | HOOK_DISP_SIZE;
/* Ignore the picture offsets (they're always 0 anyway) */
xofs = 0;
yofs = 0;

/* AYM 19971202: 17 kB is large enough for 128x128 patches. */
#define TEX_COLUMNBUFFERSIZE (17L * 1024L)
/* AYM 19971202: where does 512 come from ? I think that the worst
case (every second pixel transparent) is 64 * 5 + 1 = 321
*/
#define TEX_COLUMNSIZE	     512L

Columns = xsize;

/* Note from CJS:
   I tried to use far memory originally, but kept getting out-of-mem errors
   that is really strange - I assume that the wad dir et al uses all
   the far mem, and there is only near memory available. NEVER seen
   this situation before..... I'll keep them huge pointers anyway,
   in case something changes later */
/* AYM 1997-12-02:
   Noticing how slow it was to display mono-patch textures like
   BROWN96, I tried to optimize this function. I supposed that
   putpixel() was the bottleneck so I modified the code to draw
   a whole segment at a time with only one call to putimage().
   I was wrong ; it brought no visible speedup :(
   Is putimage() really slow ? Or is there a large overhead per
   putimage() call ? Or is there something else I overlooked ? */
ColumnData    = (u8 *) GetMemory (TEX_COLUMNBUFFERSIZE+1);
NeededOffsets = (i32 *) GetMemory (Columns * 4L);

wad_read_i32 (dir->wadfile, NeededOffsets, Columns);

/* read first column data, and subsequent column data */
if (NeededOffsets[0] != 8 + 4L * Columns)
   wad_seek (dir->wadfile, dir->dir.start + NeededOffsets[0]);
/* AYM 19971201:
   They used wad_read_bytes() and that was the cause of the bug with
   custom textures ("Error reading from file" when trying to
   display the texture if the patch was within 60 kB of EOF).
   The -deu option in DeuTEX is now pointless :) */
ActualBufLen = wad_read_bytes2 (dir->wadfile, ColumnData+1, TEX_COLUMNBUFFERSIZE);

{
int Column0 = max (0,  - (c->x0 + xofs));
int Column9 = min (Columns, c->x1 + 1 - (c->x0 + xofs));
for (CurrentColumn = Column0; CurrentColumn < Column9; CurrentColumn++)
   {
   CurrentOffset  = NeededOffsets[CurrentColumn];
   ColumnInMemory = CurrentOffset >= NeededOffsets[0]
    && CurrentOffset < (long)(NeededOffsets[0] + ActualBufLen - TEX_COLUMNSIZE);
   if (ColumnInMemory)
      Column = &ColumnData[CurrentOffset - NeededOffsets[0]];
   else
      {
      Column = (unsigned char *) GetFarMemory (TEX_COLUMNSIZE + 1);
      wad_seek (dir->wadfile, dir->dir.start + CurrentOffset);
      wad_read_bytes2 (dir->wadfile, Column + 1, TEX_COLUMNSIZE);
      }

   /* we now have the needed column data, one way or another, so write it */

   /* the column may contain several posts... */
   {
   u8 *Post;
   for (Post = Column; Post[1] != 255; )
      {
      int RowStart;       /* y-offset of segment on texture */
      int PostHeight;     /* height of post */
      int ClippedHeight;  /* part that is actually drawn on screen */
      int x, y;           /* where start of segment is to be displayed */
      int above_y0;       /* length of part of segment that is above c->y0 */
      int below_y1;       /* length of part of segment that is below c->y1 */

      RowStart   = (int) Post[1];
      PostHeight = (int) Post[2];
      x = c->x0 + xofs + CurrentColumn;
      y = c->y0 + yofs + RowStart;

      /* Calculate what parts of the segment shoud be clipped
	 (i.e. are not between c->y0 and c->y1) */
      above_y0 = max (-y, 0);
      below_y1 = (y+PostHeight) - (c->y1+1);
      below_y1 = max (below_y1, 0);
      ClippedHeight = PostHeight - above_y0 - below_y1;
      debmes ("y=%d sh=%d ay0=%d by1=%d ch=%d",
       y, PostHeight, above_y0, below_y1, ClippedHeight);

      /* If clipping leaves nothing to display, skip this segment
	 (yes, it does happen (in BROWN144)) */
      if (ClippedHeight < 1)
	goto cont;

      {
      /* For the sake of putimage(), insert image height and width
	 as two words just before the first pixel to display.
	 The first pixel of the segment is at Post[4]. Therefore,
	 the first pixel to display is at Post[4+above_y0].
	 80x86-ism: assumes that any (char *) is also a valid (int *). */
      unsigned int save1, save2;
      #define BASE ((unsigned int huge *)(Post+4+above_y0))
      save1    = BASE[-2];
      save2    = BASE[-1];
      BASE[-2] = 1             - (GfxMode < -1);
      BASE[-1] = ClippedHeight - (GfxMode < -1);

#if 0
      putimage (x, y+above_y0, BASE-2, COPY_PUT);
#endif

      BASE[-2] = save1;    /* restore what we clobbered */
      BASE[-1] = save2;
      }

      cont:
      Post += PostHeight + 4;
      }
   }

   if (!ColumnInMemory)
      FreeFarMemory (Column);
   }
}
FreeMemory (ColumnData);
FreeMemory (NeededOffsets);
c->flags |= HOOK_DRAWN;
}


/*
   display a wall texture ("TEXTURE1" or "TEXTURE2" object)
   at coords c->x0, c->y0
*/

void DisplayWallTexture (hookfunc_comm_t *c)
{
MDirPtr  dir;		/* main directory pointer to the TEXTURE* entries */
MDirPtr  pdir;		/* main directory pointer to the PNAMES entry */
i32     *offsets;	/* array of offsets to texture names */
int      n;		/* general counter */
i16      xsize, ysize;	/* size of the texture */
i16      xofs, yofs;	/* offset in texture space for the wall patch */
i16      fields;	/* number of wall patches used to build this texture */
i16      pnameind;	/* patch name index in PNAMES table */
i16      stepdir;	/* holds useless data */
i16      colormap;	/* holds useless data */
i32      numtex;	/* number of texture names in TEXTURE* list */
i32      texofs;	/* offset in the wad file to the texture data */
char     tname[9]; 	/* texture name */
char     picname[9];	/* wall patch name */
game_image_pixel_t *texbuf = 0;

c->flags = 0;
#ifndef Y_X11
if (have_key ())
   return; /* speedup */
#endif

/* offset for texture we want. */
texofs = 0;
/* search for c->name in texture1 names */
dir = FindMasterDir (MasterDir, "TEXTURE1");
wad_seek (dir->wadfile, dir->dir.start);
wad_read_i32 (dir->wadfile, &numtex);
/* read in the offsets for texture1 names and info. */
offsets = (i32 *) GetMemory ((long) numtex * 4);
wad_read_i32 (dir->wadfile, offsets, numtex);
for (n = 0; n < numtex && !texofs; n++)
   {
   wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
   wad_read_bytes (dir->wadfile, &tname, 8);
   if (!strnicmp (tname, c->name, 8))
      texofs = dir->dir.start + offsets[n];
   }
FreeMemory (offsets);
if (Registered && texofs == 0)
   {
   /* search for c->name in texture2 names */
   dir = FindMasterDir (MasterDir, "TEXTURE2");
   if (dir != NULL)  /* Doom II has no TEXTURE2 */
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &numtex);
      /* read in the offsets for texture2 names */
      offsets = (i32 *) GetMemory ((long) numtex * 4);
      wad_read_i32 (dir->wadfile, offsets, numtex);
      for (n = 0; n < numtex && !texofs; n++)
	 {
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, &tname, 8);
	 if (!strnicmp (tname, c->name, 8))
	    texofs = dir->dir.start + offsets[n];
	 }
      FreeMemory (offsets);
      }
   }


/* texture name not found */
if (texofs == 0)
   return;

/* read the info for this texture */
wad_seek (dir->wadfile, texofs + 12);
wad_read_i16 (dir->wadfile, &xsize);
wad_read_i16 (dir->wadfile, &ysize);
wad_seek (dir->wadfile, texofs + 20);
wad_read_i16 (dir->wadfile, &fields);
c->width    = xsize;
c->height   = ysize;
c->npatches = fields;
c->flags   |= HOOK_SIZE_VALID | HOOK_DISP_SIZE;

#ifndef Y_X11
if (have_key ())
   return; /* speedup */
#endif

if (c->x1 - c->x0 > xsize)
   c->x1 = c->x0 + xsize;
if (c->y1 - c->y0 > ysize)
   c->y1 = c->y0 + ysize;
/* not really necessary, except when xofs or yofs < 0 */
#ifdef Y_BGI
setviewport (c->x0, c->y0, c->x1, c->y1, 1);
#endif  /* FIXME ! */

/* Compose the texture */
texbuf =
   (game_image_pixel_t *) GetMemory ((unsigned long) xsize * ysize);
/* Fill the buffer with whatever pixel value is to appear through
   transparent textures. That pixel value is the "transparent colour". */
clear_game_image (texbuf, xsize, ysize);
for (n = 0; n < fields; n++)
   {
   hookfunc_comm_t subc;
   wad_seek (dir->wadfile, texofs + 22L + n * 10L);
   wad_read_i16 (dir->wadfile, &xofs);
   wad_read_i16 (dir->wadfile, &yofs);
   wad_read_i16 (dir->wadfile, &pnameind);
   wad_read_i16 (dir->wadfile, &stepdir);   /* Always 1, unused. */
   wad_read_i16 (dir->wadfile, &colormap);  /* Always 0, unused. */

   /* AYM 1998-08-08: Yes, that's weird but that's what Doom
      does. Without these two lines, the few textures that have
      patches with negative y-offsets (BIGDOOR7, SKY1, TEKWALL1,
      TEKWALL5 and a few others) would not look in the texture
      viewer quite like in Doom. This should be mentioned in
      the UDS, by the way. */
   if (yofs < 0)
      yofs = 0;

   /* OK, now look up the pic's name in the PNAMES entry. */
   pdir = FindMasterDir (MasterDir, "PNAMES");
   wad_seek (pdir->wadfile, pdir->dir.start + 4L + pnameind * 8L);
   wad_read_bytes (pdir->wadfile, &picname, 8);
   picname[8] = '\0';
#ifdef Y_DOS
   strupr (picname);  /* FIXME ? */
#endif
   /* coords changed because of the "setviewport" */
#ifdef Y_BGI
   /* AYM 1998-07-11
      Is it normal to set x0 and y0 to potentially negative values ? */
   subc.x0 = xofs;
   subc.y0 = yofs;
   subc.x1 = c->x1 - c->x0;
   subc.y1 = c->y1 - c->y0;
#else
   subc.x0 = max (c->x0, c->x0 + xofs);
   subc.y0 = min (c->y0, c->y0 + yofs);
   subc.x1 = c->x1;
   subc.x1 = c->y1;
#endif
   subc.name = picname;

   LoadPicture (texbuf, xsize, ysize, picname, xofs, yofs, 0, 0);
   }

/* Display the texture */
display_game_image (texbuf, xsize, ysize, c->x0, c->y0,
   c->x1 + 1 - c->x0, c->y1 + 1 - c->y0);
free (texbuf);

/* restore the normal viewport */
#ifdef Y_BGI
setviewport (0, 0, ScrMaxX, ScrMaxY, 1);
#endif  /* FIXME ! */
c->flags |= HOOK_DRAWN;
}



/*
   Function to get the size of a wall texture
*/

void GetWallTextureSize (i16 *xsize_r, i16 *ysize_r, const char *texname)
{
MDirPtr  dir;		/* pointer in main directory to texname */
i32    *offsets;	/* array of offsets to texture names */
int      n;		/* general counter */
i32      numtex;	/* number of texture names in TEXTURE* list */
i32      texofs;	/* offset in doom.wad for the texture data */
char     tname[9];	/* texture name */

/* offset for texture we want. */
texofs = 0;
/* search for texname in texture1 names */
dir = FindMasterDir (MasterDir, "TEXTURE1");
wad_seek (dir->wadfile, dir->dir.start);
wad_read_i32 (dir->wadfile, &numtex);
/* read in the offsets for texture1 names and info. */
offsets = (i32 *) GetMemory ((long) numtex * 4);
wad_read_i32 (dir->wadfile, offsets, numtex);
for (n = 0; n < numtex && !texofs; n++)
   {
   wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
   wad_read_bytes (dir->wadfile, &tname, 8);
   if (!strnicmp (tname, texname, 8))
      texofs = dir->dir.start + offsets[n];
   }
FreeMemory (offsets);
if (Registered && texofs == 0)
   {
   /* search for texname in texture2 names */
   dir = FindMasterDir (MasterDir, "TEXTURE2");
   if (dir != NULL)  /* Doom II has no TEXTURE2 */
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &numtex);
      /* read in the offsets for texture2 names */
      offsets = (i32 *) GetMemory ((long) numtex * 4);
      wad_read_i32 (dir->wadfile, offsets);
      for (n = 0; n < numtex && !texofs; n++)
	 {
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, &tname, 8);
	 if (!strnicmp (tname, texname, 8))
	    texofs = dir->dir.start + offsets[n];
	 }
      FreeMemory (offsets);
      }
   }

if (texofs != 0)
   {
   /* read the info for this texture */
   wad_seek (dir->wadfile, texofs + 12L);
   wad_read_i16 (dir->wadfile, xsize_r);
   wad_read_i16 (dir->wadfile, ysize_r);
   }
else
   {
   /* texture data not found */
   *xsize_r = -1;
   *ysize_r = -1;
   }
}




/*
   choose a wall texture
*/

void ChooseWallTexture (int x0, int y0, const char *prompt, int listsize,
   char **list, char *name)
{
HideMousePointer ();

SwitchToVGA256 ();
/* If we only have a 320x200x256 VGA driver, we must change x0 and y0.
   Yuck! */
if (GfxMode > -2)
   {
   x0 = 0;
   y0 = -1;
   }
InputNameFromListWithFunc (x0, y0, prompt, listsize, list, 11, name,
  256, 128, DisplayWallTexture);
SwitchToVGA16 ();

ShowMousePointer ();
}



