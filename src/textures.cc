/*
 *	textures.cc
 *	Trevor Phillips, RQ and Christian Johannes Schladetsch
 *	sometime in 1994.
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
#include "dialog.h"
#include "clearimg.h"
#include "dispimg.h"
#include "game.h"      /* yg_picture_format */
#include "gfx.h"
#include "patchdir.h"
#include "pic2img.h"
#include "wads.h"
#include "wstructs.h"


#if 0
///*
//   display the picture named c->name
//   at coords c->x0, c->y0 and not beyond c->x1, c->y1
//*/
//
//void DisplayPic (hookfunc_comm_t *c)
//{
//MDirPtr	dir;
//i16	xsize, ysize, xofs, yofs;
//u8	*ColumnData;
//u8	*Column;
//i32	*NeededOffsets;
//int	Columns, CurrentColumn;
//i32	CurrentOffset;
//int	ColumnInMemory;
//long	ActualBufLen;
//
//c->flags = 0;
//
//if (have_key ())
//   return; /* speedup */
//
//dir = FindMasterDir (MasterDir, c->name);
//if (dir == NULL)
//   {
//   set_colour (DARKGRAY);
//   DrawScreenLine (c->x0, c->y0, c->x1, c->y1);
//   DrawScreenLine (c->x0, c->y1, c->x1, c->y0);
//   return;
//   }
//wad_seek (dir->wadfile, dir->dir.start);
//wad_read_i16 (dir->wadfile, &xsize);
//wad_read_i16 (dir->wadfile, &ysize);
//wad_read_i16 (dir->wadfile, &xofs );
//wad_read_i16 (dir->wadfile, &yofs );
//c->width    = xsize;
//c->height   = ysize;
//c->flags   |= HOOK_SIZE_VALID | HOOK_DISP_SIZE;
///* Ignore the picture offsets (they're always 0 anyway) */
//xofs = 0;
//yofs = 0;
//
///* AYM 19971202: 17 kB is large enough for 128x128 patches. */
//#define TEX_COLUMNBUFFERSIZE (17L * 1024L)
///* AYM 19971202, updated 1999-05-17 : where does 512 come from ?
//   For a 128-high picture, I think that the worst case (every
//   second pixel transparent except the last) is 63 * 5 + 6 + 1 =
//   322. Anyway, I'll have to bump this up to accomodate the
//   maximum theoretical height which is 510. */
//#define TEX_COLUMNSIZE	     512L
//
//Columns = xsize;
//
///* Note from CJS:
//   I tried to use far memory originally, but kept getting out-of-mem errors
//   that is really strange - I assume that the wad dir et al uses all
//   the far mem, and there is only near memory available. NEVER seen
//   this situation before..... I'll keep them huge pointers anyway,
//   in case something changes later */
///* AYM 1997-12-02:
//   Noticing how slow it was to display mono-patch textures like
//   BROWN96, I tried to optimize this function. I supposed that
//   putpixel() was the bottleneck so I modified the code to draw
//   a whole segment at a time with only one call to putimage().
//   I was wrong ; it brought no visible speedup :(
//   Is putimage() really slow ? Or is there a large overhead per
//   putimage() call ? Or is there something else I overlooked ? */
//ColumnData    = (u8 *) GetMemory (TEX_COLUMNBUFFERSIZE+1);
//NeededOffsets = (i32 *) GetMemory (Columns * 4L);
//
//wad_read_i32 (dir->wadfile, NeededOffsets, Columns);
//
///* read first column data, and subsequent column data */
//if (NeededOffsets[0] != 8 + 4L * Columns)
//   wad_seek (dir->wadfile, dir->dir.start + NeededOffsets[0]);
///* AYM 19971201:
//   They used wad_read_bytes() and that was the cause of the bug
//   with custom textures ("Error reading from file" when trying
//   to display the texture if the patch was within 60 kB of EOF).
//   The -deu option in DeuTex is now pointless :) */
//ActualBufLen = wad_read_bytes2 (dir->wadfile, ColumnData+1, TEX_COLUMNBUFFERSIZE);
//
//{
//int Column0 = y_max (0,  - (c->x0 + xofs));
//int Column9 = y_min (Columns, c->x1 + 1 - (c->x0 + xofs));
//for (CurrentColumn = Column0; CurrentColumn < Column9; CurrentColumn++)
//   {
//   CurrentOffset  = NeededOffsets[CurrentColumn];
//   ColumnInMemory = CurrentOffset >= NeededOffsets[0]
//    && CurrentOffset < (long)(NeededOffsets[0] + ActualBufLen - TEX_COLUMNSIZE);
//   if (ColumnInMemory)
//      Column = &ColumnData[CurrentOffset - NeededOffsets[0]];
//   else
//      {
//      Column = (unsigned char *) GetFarMemory (TEX_COLUMNSIZE + 1);
//      wad_seek (dir->wadfile, dir->dir.start + CurrentOffset);
//      wad_read_bytes2 (dir->wadfile, Column + 1, TEX_COLUMNSIZE);
//      }
//
//   /* we now have the needed column data, one way or another, so write it */
//
//   /* the column may contain several posts... */
//   {
   //u8 *Post;
//   for (Post = Column; Post[1] != 255; )
//      {
//      int RowStart;       /* y-offset of segment on texture */
//      int PostHeight;     /* height of post */
//      int ClippedHeight;  /* part that is actually drawn on screen */
//      int x, y;           /* where start of segment is to be displayed */
//      int above_y0;       /* length of part of segment that is above c->y0 */
//      int below_y1;       /* length of part of segment that is below c->y1 */
//
//      RowStart   = (int) Post[1];
//      PostHeight = (int) Post[2];
//      x = c->x0 + xofs + CurrentColumn;
//      y = c->y0 + yofs + RowStart;
//
//      /* Calculate what parts of the segment should be clipped
//	 (i.e. are not between c->y0 and c->y1) */
//      above_y0 = y_max (-y, 0);
//      below_y1 = (y+PostHeight) - (c->y1+1);
//      below_y1 = y_max (below_y1, 0);
//      ClippedHeight = PostHeight - above_y0 - below_y1;
//      debmes ("y=%d sh=%d ay0=%d by1=%d ch=%d",
//       y, PostHeight, above_y0, below_y1, ClippedHeight);
//
//      /* If clipping leaves nothing to display, skip this segment
//	 (yes, it does happen (in BROWN144)) */
//      if (ClippedHeight < 1)
//	goto cont;
//
//      {
//      /* For the sake of putimage(), insert image height and width
//	 as two words just before the first pixel to display.
//	 The first pixel of the segment is at Post[4]. Therefore,
//	 the first pixel to display is at Post[4+above_y0].
//	 80x86-ism: assumes that any (char *) is also a valid (int *). */
//      unsigned int save1, save2;
//#define BASE ((unsigned int huge *)(Post+4+above_y0))
//      save1    = BASE[-2];
//      save2    = BASE[-1];
//      BASE[-2] = 1             - (GfxMode < -1);
//      BASE[-1] = ClippedHeight - (GfxMode < -1);
//
//#if 0
//      putimage (x, y+above_y0, BASE-2, COPY_PUT);
//#endif
//
//      BASE[-2] = save1;    /* restore what we clobbered */
//      BASE[-1] = save2;
//      }
//
      //cont:
//      Post += PostHeight + 4;
//      }
//   }
//
//   if (!ColumnInMemory)
//      FreeFarMemory (Column);
//   }
//}
//FreeMemory (ColumnData);
//FreeMemory (NeededOffsets);
//c->flags |= HOOK_DRAWN;
//}
#endif


/*
   display a wall texture ("TEXTURE1" or "TEXTURE2" object)
   at coords c->x0, c->y0
*/
void DisplayWallTexture (hookfunc_comm_t *c)
{
MDirPtr  dir = 0;	/* main directory pointer to the TEXTURE* entries */
i32     *offsets;	/* array of offsets to texture names */
int      n;		/* general counter */
i16      width, height;	/* size of the texture */
i16      npatches;	/* number of wall patches used to build this texture */
i32      numtex;	/* number of texture names in TEXTURE* list */
i32      texofs;	/* offset in the wad file to the texture data */
char     tname[WAD_TEX_NAME + 1];	/* texture name */
char     picname[WAD_PIC_NAME + 1];	/* wall patch name */
game_image_pixel_t *texbuf = 0;
bool     have_dummy_bytes;
int      header_size;
int      item_size;

// So that, on failure, the caller clears the display area
c->disp_x0 = c->x1;
c->disp_y0 = c->y1;
c->disp_x1 = c->x0;
c->disp_y1 = c->y0;

// Iwad-dependant details
if (yg_texture_format == YGTF_NAMELESS)
   {
   have_dummy_bytes = true;
   header_size      = 14;
   item_size        = 10;
   }
else if (yg_texture_format == YGTF_NORMAL)
   {
   have_dummy_bytes = true;
   header_size      = 14;
   item_size        = 10;
   }
else if (yg_texture_format == YGTF_STRIFE11)
   {
   have_dummy_bytes = false;
   header_size      = 10;
   item_size        = 6;
   }
else
   {
   nf_bug ("Bad texture format %d.", (int) yg_texture_format);
   return;
   }

#ifndef Y_X11
if (have_key ())
   return; /* speedup */
#endif

/* offset for texture we want. */
texofs = 0;
// Doom alpha 0.4 : "TEXTURES", no names
if (yg_texture_lumps == YGTL_TEXTURES && yg_texture_format == YGTF_NAMELESS)
   {
   dir = FindMasterDir (MasterDir, "TEXTURES");
   if (dir != NULL)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &numtex);
      if (WAD_TEX_NAME < 7) nf_bug ("WAD_TEX_NAME too small");  // Sanity
      if (! y_strnicmp (c->name, "TEX", 3)
	    && isdigit (c->name[3])
	    && isdigit (c->name[4])
	    && isdigit (c->name[5])
	    && isdigit (c->name[6])
	    && c->name[7] == '\0')
	 {
	 long num;
	 if (sscanf (c->name + 3, "%4ld", &num) == 1
	       && num >= 0 && num < numtex)
	    {
	    wad_seek (dir->wadfile, dir->dir.start + 4 + 4 * num);
	    wad_read_i32 (dir->wadfile, &texofs);
	    texofs += dir->dir.start;
	    }
	 }
      }
   }
// Doom alpha 0.5 : only "TEXTURES"
else if (yg_texture_lumps == YGTL_TEXTURES
   && (yg_texture_format == YGTF_NORMAL || yg_texture_format == YGTF_STRIFE11))
   {
   // Is it in TEXTURES ?
   dir = FindMasterDir (MasterDir, "TEXTURES");
   if (dir != NULL)  // (Theoretically, it should always exist)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &numtex);
      /* read in the offsets for texture1 names and info. */
      offsets = (i32 *) GetMemory ((long) numtex * 4);
      wad_read_i32 (dir->wadfile, offsets, numtex);
      for (n = 0; n < numtex && !texofs; n++)
	 {
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, &tname, WAD_TEX_NAME);
	 if (!y_strnicmp (tname, c->name, WAD_TEX_NAME))
	    texofs = dir->dir.start + offsets[n];
	 }
      FreeMemory (offsets);
      }
   }
// Other iwads : "TEXTURE1" and "TEXTURE2"
else if (yg_texture_lumps == YGTL_NORMAL
   && (yg_texture_format == YGTF_NORMAL || yg_texture_format == YGTF_STRIFE11))
   {
   // Is it in TEXTURE1 ?
   dir = FindMasterDir (MasterDir, "TEXTURE1");
   if (dir != NULL)  // (Theoretically, it should always exist)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &numtex);
      /* read in the offsets for texture1 names and info. */
      offsets = (i32 *) GetMemory ((long) numtex * 4);
      wad_read_i32 (dir->wadfile, offsets, numtex);
      for (n = 0; n < numtex && !texofs; n++)
	 {
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, &tname, WAD_TEX_NAME);
	 if (!y_strnicmp (tname, c->name, WAD_TEX_NAME))
	    texofs = dir->dir.start + offsets[n];
	 }
      FreeMemory (offsets);
      }
   // Well, then is it in TEXTURE2 ?
   if (texofs == 0)
      {
      dir = FindMasterDir (MasterDir, "TEXTURE2");
      if (dir != NULL)  // Doom II has no TEXTURE2
	 {
	 wad_seek (dir->wadfile, dir->dir.start);
	 wad_read_i32 (dir->wadfile, &numtex);
	 /* read in the offsets for texture2 names */
	 offsets = (i32 *) GetMemory ((long) numtex * 4);
	 wad_read_i32 (dir->wadfile, offsets, numtex);
	 for (n = 0; n < numtex && !texofs; n++)
	    {
	    wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	    wad_read_bytes (dir->wadfile, &tname, WAD_TEX_NAME);
	    if (!y_strnicmp (tname, c->name, WAD_TEX_NAME))
	       texofs = dir->dir.start + offsets[n];
	    }
	 FreeMemory (offsets);
	 }
      }
   }
else
   nf_bug ("Invalid texture_format/texture_lumps combination.");

/* texture name not found */
if (texofs == 0)
   return;

/* read the info for this texture */
i32 header_ofs;
if (yg_texture_format == YGTF_NAMELESS)
   header_ofs = texofs;
else
   header_ofs = texofs + WAD_TEX_NAME;
wad_seek (dir->wadfile, header_ofs + 4);
wad_read_i16 (dir->wadfile, &width);
wad_read_i16 (dir->wadfile, &height);
if (have_dummy_bytes)
   {
   i16 dummy;
   wad_read_i16 (dir->wadfile, &dummy);
   wad_read_i16 (dir->wadfile, &dummy);
   }
wad_read_i16 (dir->wadfile, &npatches);
c->width    = width;
c->height   = height;
c->npatches = npatches;
c->flags   |= HOOK_SIZE_VALID | HOOK_DISP_SIZE;

#ifndef Y_X11
if (have_key ())
   return; /* speedup */
#endif

if (c->x1 - c->x0 > width)
   c->x1 = c->x0 + width;
if (c->y1 - c->y0 > height)
   c->y1 = c->y0 + height;
#ifdef Y_BGI
/* not really necessary, except when xofs or yofs < 0 */
setviewport (c->x0, c->y0, c->x1, c->y1, 1);
#endif  /* FIXME ! */

/* Compose the texture */
texbuf =
   (game_image_pixel_t *) GetMemory ((unsigned long) width * height);
/* Fill the buffer with whatever pixel value is to appear
   through transparent textures. That pixel value is the
   "transparent colour". */
clear_game_image (texbuf, width, height);

/* Paste onto the buffer all the patches that the texture is
   made of. */
for (n = 0; n < npatches; n++)
   {
   hookfunc_comm_t subc;
   i16 xofs, yofs;	// offset in texture space for the patch
   i16 pnameind;	// index of patch in PNAMES

   wad_seek (dir->wadfile, header_ofs + header_size + (long) n * item_size);
   wad_read_i16 (dir->wadfile, &xofs);
   wad_read_i16 (dir->wadfile, &yofs);
   wad_read_i16 (dir->wadfile, &pnameind);
   if (have_dummy_bytes)
      {
      i16 stepdir;
      i16 colormap;
      wad_read_i16 (dir->wadfile, &stepdir);   // Always 1, unused.
      wad_read_i16 (dir->wadfile, &colormap);  // Always 0, unused.
      }

   /* AYM 1998-08-08: Yes, that's weird but that's what Doom
      does. Without these two lines, the few textures that have
      patches with negative y-offsets (BIGDOOR7, SKY1, TEKWALL1,
      TEKWALL5 and a few others) would not look in the texture
      viewer quite like in Doom. This should be mentioned in
      the UDS, by the way. */
   if (yofs < 0)
      yofs = 0;

   Lump_loc loc;
   {
     wad_pic_name_t *name = patch_dir.name_for_num (pnameind);
     if (name == 0)
       {
       warn ("texture \"%.*s\": patch %2d has bad index %d.\n",
	  WAD_TEX_NAME, tname, (int) n, (int) pnameind);
       continue;
       }
     patch_dir.loc_by_name ((const char *) *name, loc);
     *picname = '\0';
     strncat (picname, (const char *) *name, sizeof picname - 1);
#ifdef DEBUG
     printf ("Texture \"%.*s\": Patch %2d: #%3d %-8.8s (%d, %d)\n",
       c->name, (int) n, (int) pnameind, picname, (int) xofs, (int) yofs);
#endif
   }

   /* coords changed because of the "setviewport" */
#ifdef Y_BGI
   /* AYM 1998-07-11
      Is it normal to set x0 and y0 to potentially negative values ? */
   subc.x0 = xofs;
   subc.y0 = yofs;
   subc.x1 = c->x1 - c->x0;
   subc.y1 = c->y1 - c->y0;
#else
   subc.x0 = y_max (c->x0, c->x0 + xofs);
   subc.y0 = y_min (c->y0, c->y0 + yofs);
   subc.x1 = c->x1;
   subc.x1 = c->y1;
#endif
   subc.name = picname;

   if (LoadPicture (texbuf, width, height, picname, loc, xofs, yofs, 0, 0))
      warn ("texture \"%.*s\": patch \"%.*s\" not found.\n",
	  WAD_TEX_NAME, tname, WAD_PIC_NAME, picname);
   }

/* Display the texture */
display_game_image (texbuf, width, height, c->x0, c->y0,
   c->x1 + 1 - c->x0, c->y1 + 1 - c->y0);
c->flags |= HOOK_DRAWN;
c->disp_x0 = c->x0 + c->xofs;
c->disp_y0 = c->y0 + c->yofs;
c->disp_x1 = c->disp_x0 + width - 1;
c->disp_y1 = c->disp_y0 + height - 1;
free (texbuf);

/* restore the normal viewport */
#ifdef Y_BGI
setviewport (0, 0, ScrMaxX, ScrMaxY, 1);
#endif  /* FIXME ! */
}


/*
   Function to get the size of a wall texture
*/
void GetWallTextureSize (i16 *width, i16 *height, const char *texname)
{
MDirPtr  dir = 0;	/* pointer in main directory to texname */
i32    *offsets;	/* array of offsets to texture names */
int      n;		/* general counter */
i32      numtex;	/* number of texture names in TEXTURE* list */
i32      texofs;	/* offset in doom.wad for the texture data */
char     tname[WAD_TEX_NAME + 1];  /* texture name */

/* offset for texture we want. */
texofs = 0;
// search for texname in TEXTURE1 (or TEXTURES)

if (yg_texture_lumps == YGTL_TEXTURES && yg_texture_format == YGTF_NAMELESS)
   {
   dir = FindMasterDir (MasterDir, "TEXTURES");
   if (dir != NULL)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &numtex);
      if (WAD_TEX_NAME < 7) nf_bug ("WAD_TEX_NAME too small");  // Sanity
      if (! y_strnicmp (texname, "TEX", 3)
	    && isdigit (texname[3])
	    && isdigit (texname[4])
	    && isdigit (texname[5])
	    && isdigit (texname[6])
	    && texname[7] == '\0')
	 {
	 long num;
	 if (sscanf (texname + 3, "%4ld", &num) == 1
	       && num >= 0 && num < numtex)
	    {
	    wad_seek (dir->wadfile, dir->dir.start + 4 + 4 * num);
	    wad_read_i32 (dir->wadfile, &texofs);
	    }
	 }
      }
   }
else if (yg_texture_format == YGTF_NORMAL
    || yg_texture_format == YGTF_STRIFE11)
   {
   if (yg_texture_lumps == YGTL_TEXTURES)
      dir = FindMasterDir (MasterDir, "TEXTURES");  // Doom alpha 0.5
   else if (yg_texture_lumps == YGTL_NORMAL)
      dir = FindMasterDir (MasterDir, "TEXTURE1");
   else
      {
      dir = 0;
      nf_bug ("Invalid texture_format/texture_lumps combination.");
      }
   if (dir != NULL)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &numtex);
      /* read in the offsets for texture1 names and info. */
      offsets = (i32 *) GetMemory ((long) numtex * 4);
      wad_read_i32 (dir->wadfile, offsets, numtex);
      for (n = 0; n < numtex && !texofs; n++)
	 {
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, &tname, WAD_TEX_NAME);
	 if (!y_strnicmp (tname, texname, WAD_TEX_NAME))
	    texofs = dir->dir.start + offsets[n];
	 }
      FreeMemory (offsets);
      }
   if (texofs == 0 && yg_texture_lumps == YGTL_NORMAL)
      {
      // Search for texname in TEXTURE2
      dir = FindMasterDir (MasterDir, "TEXTURE2");
      if (dir != NULL)  // Doom II has no TEXTURE2
	 {
	 wad_seek (dir->wadfile, dir->dir.start);
	 wad_read_i32 (dir->wadfile, &numtex);
	 /* read in the offsets for texture2 names */
	 offsets = (i32 *) GetMemory ((long) numtex * 4);
	 wad_read_i32 (dir->wadfile, offsets);
	 for (n = 0; n < numtex && !texofs; n++)
	    {
	    wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	    wad_read_bytes (dir->wadfile, &tname, WAD_TEX_NAME);
	    if (!y_strnicmp (tname, texname, WAD_TEX_NAME))
	       texofs = dir->dir.start + offsets[n];
	    }
	 FreeMemory (offsets);
	 }
      }
   }
else
   nf_bug ("Invalid texture_format/texture_lumps combination.");

if (texofs != 0)
   {
   /* read the info for this texture */
   if (yg_texture_format == YGTF_NAMELESS)
      wad_seek (dir->wadfile, texofs + 4L);
   else
      wad_seek (dir->wadfile, texofs + 12L);
   wad_read_i16 (dir->wadfile, width);
   wad_read_i16 (dir->wadfile, height);
   }
else
   {
   /* texture data not found */
   *width  = -1;
   *height = -1;
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
InputNameFromListWithFunc (x0, y0, prompt, listsize, list, 9, name,
  256, 128, DisplayWallTexture);
SwitchToVGA16 ();

ShowMousePointer ();
}

