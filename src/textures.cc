/*
 *	textures.cc
 *	Trevor Phillips, RQ and Christian Johannes Schladetsch
 *	sometime in 1994.
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2005 André Majorel and others.

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
#include <vector>

#ifdef Y_X11
#include <X11/Xlib.h>
#endif

#include "dialog.h"
#include "game.h"      /* yg_picture_format */
#include "gfx.h"
#include "lists.h"
#include "patchdir.h"
#include "pic2img.h"
#include "sticker.h"
#include "textures.h"
#include "wadfile.h"
#include "wads.h"
#include "wstructs.h"


static int32_t texname2ofs (const char *name, MDirPtr &dir);
static int32_t texname2ofs_doom04 (const char *name, MDirPtr &dir);
static int32_t texname2ofs_doom05 (const char *name, MDirPtr &dir);
static int32_t texname2ofs_normal (const char *name, MDirPtr &dir);

/*
 *	DisplayWallTexture - display a texture at coords (c->x0, c->y0)
 */
void DisplayWallTexture (hookfunc_comm_t *c)
{
  MDirPtr  dir = 0;		// Main directory ptr to the TEXTURE* entries
  int      n;			// General counter
  int16_t  width, height;	// Size of the texture
  int16_t  npatches;		// Number of patches in the texture
  int32_t  texofs;		// Offset in the wad file to the texture data
  char     picname[WAD_PIC_NAME + 1];	// Patch name
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
    return;				// Speedup
#endif

  texofs = texname2ofs (c->name, dir);
  // Texture name not found
  if (texofs == 0)
    return;

  // Read the info for this texture
  int32_t header_ofs;
  if (yg_texture_format == YGTF_NAMELESS)
    header_ofs = texofs;
  else
    header_ofs = texofs + WAD_TEX_NAME;
  dir->wadfile->seek (header_ofs + 4);
  if (dir->wadfile->error ())
  {
    warn ("texture \"%.*s\": cannot seek to definition\n",
      WAD_TEX_NAME, c->name);
    return;
  }
  dir->wadfile->read_i16 (&width);
  dir->wadfile->read_i16 (&height);
  if (have_dummy_bytes)
  {
    int16_t dummy;
    dir->wadfile->read_i16 (&dummy);
    dir->wadfile->read_i16 (&dummy);
  }
  dir->wadfile->read_i16 (&npatches);
  if (dir->wadfile->error ())
  {
    warn ("texture \"%.*s\": cannot read header\n", WAD_TEX_NAME, c->name);
    return;
  }

  c->width    = width;
  c->height   = height;
  c->npatches = npatches;
  c->flags   |= HOOK_SIZE_VALID | HOOK_DISP_SIZE | HOOK_MEDUSA_VALID;

  /* Clip the texture to size. Done *after* setting c->width and
     c->height so that the selector shows the unclipped size. */
  width  = y_min (width,  c->x1 - c->x0 + 1);
  height = y_min (height, c->y1 - c->y0 + 1);

#ifndef Y_X11
  if (have_key ())
    return;				// Speedup
#endif

#ifdef Y_BGI
  // Not really necessary, except when xofs or yofs < 0
  setviewport (c->x0, c->y0, c->x1, c->y1, 1);
#endif  /* FIXME ! */

  // Compose the texture
  c->img.resize (width, height);
  c->img.set_opaque (false);

  /* Paste onto the buffer all the patches that the texture is
     made of. Unless c->npatches is non-zero, in which case we
     paste only the first maxpatches ones. */
  int maxpatches = npatches;
  c->medusa = false;
  std::vector<bool> colpatch (width);
  if (c->maxpatches != 0 && c->maxpatches <= npatches)
    maxpatches = c->maxpatches;
  for (n = 0; n < maxpatches; n++)
  {
    hookfunc_comm_t subc;
    int16_t xofs, yofs;	// Offset in texture space for the patch
    int16_t pnameind;	// Index of patch in PNAMES

    dir->wadfile->seek (header_ofs + header_size + (long) n * item_size);
    dir->wadfile->read_i16 (&xofs);
    dir->wadfile->read_i16 (&yofs);
    dir->wadfile->read_i16 (&pnameind);
    if (have_dummy_bytes)
    {
      int16_t stepdir;
      int16_t colormap;
      dir->wadfile->read_i16 (&stepdir);   // Always 1, unused.
      dir->wadfile->read_i16 (&colormap);  // Always 0, unused.
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
	  WAD_TEX_NAME, c->name, (int) n, (int) pnameind);
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

#ifdef Y_BGI
    /* AYM 1998-07-11
       Is it normal to set x0 and y0 to potentially negative values ? */
    // coords changed because of the "setviewport"
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

    int patchw;
    if (LoadPicture (c->img, picname, loc, xofs, yofs, &patchw, 0))
       warn ("texture \"%.*s\": patch \"%.*s\" not found.\n",
	   WAD_TEX_NAME, c->name, WAD_PIC_NAME, picname);

    // Check for potential medusa effect
    if (! c->medusa)
    {
      int texcol0 = y_max (xofs, 0);
      int texcol1 = y_min (xofs + patchw, width);

      for (int texcol = texcol0; texcol < texcol1; texcol++)
      {
	if (colpatch[texcol])
	{
	  c->medusa = true;
	  break;
	}
	colpatch[texcol] = true;
      }
    }
  }

  // Display the texture
  Sticker sticker (c->img, true);	// Use opaque because it's faster
  sticker.draw (drw, 't', c->x0, c->y0);
  c->flags |= HOOK_DRAWN;
  c->disp_x0 = c->x0 + c->xofs;
  c->disp_y0 = c->y0 + c->yofs;
  c->disp_x1 = c->disp_x0 + width - 1;
  c->disp_y1 = c->disp_y0 + height - 1;

#ifdef Y_BGI
  // Restore the normal viewport
  setviewport (0, 0, ScrMaxX, ScrMaxY, 1);
#endif  /* FIXME ! */
}


/*
 *	texname2ofs - find the wad offset of a texture
 */
static int32_t texname2ofs (const char *name, MDirPtr &dir)
{
  // Doom alpha 0.4 : "TEXTURES", no names
  if (yg_texture_lumps == YGTL_TEXTURES
   && yg_texture_format == YGTF_NAMELESS)
  {
    return texname2ofs_doom04 (name, dir);
  }
  // Doom alpha 0.5 : only "TEXTURES"
  else if (yg_texture_lumps == YGTL_TEXTURES
        && (yg_texture_format == YGTF_NORMAL
	 || yg_texture_format == YGTF_STRIFE11))
  {
    return texname2ofs_doom05 (name, dir);
  }
  // Other iwads : "TEXTURE1" and "TEXTURE2"
  else if (yg_texture_lumps == YGTL_NORMAL
        && (yg_texture_format == YGTF_NORMAL
	 || yg_texture_format == YGTF_STRIFE11))
  {
    return texname2ofs_normal (name, dir);
  }
  else
  {
    nf_bug ("Invalid texture_format/texture_lumps combination.");
    return 0;  /* NOTREACHED */
  }
}


/*
 *	texname2ofs_doom04 - find the wad offset of a texture in a Doom alpha
 *				0.4 iwad
 */
static int32_t texname2ofs_doom04 (const char *name, MDirPtr &dir)
{
  const char	*lump_name	= "TEXTURES";
  int32_t	 texofs		= 0;

  dir = FindMasterDir (MasterDir, lump_name);
  if (dir == NULL)
    return 0;

  const Wad_file *wf = dir->wadfile;
  wf->seek (dir->dir.start);
  if (wf->error ())
  {
    warn ("%s: can't seek to lump\n", lump_name);
    goto byebye;
  }

  int32_t numtex;		// Number of texture definitions in TEXTURES
  wf->read_i32 (&numtex);
  if (wf->error ())
  {
    warn ("%s: error reading texture count\n", lump_name);
    goto byebye;
  }
  if (WAD_TEX_NAME < 7)
    nf_bug ("WAD_TEX_NAME too small");  // Sanity
  if (y_strnicmp (name, "TEX", 3) == 0
   && isdigit (name[3])
   && isdigit (name[4])
   && isdigit (name[5])
   && isdigit (name[6])
   && name[7] == '\0')
  {
    long num;

    if (sscanf (name + 3, "%4ld", &num) == 1 && num >= 0 && num < numtex)
    {
      wf->seek (dir->dir.start + 4 + 4 * num);
      if (wf->error ())
      {
	// FIXME print name of offending texture (or #)
	warn ("%s: can't seek to offsets table entry\n", lump_name);
	goto byebye;
      }
      wf->read_i32 (&texofs);
      if (wf->error ())
      {
	warn ("%s: error reading texture offset\n", lump_name);
	goto byebye;
      }
      texofs += dir->dir.start;
    }
  }

  byebye:
  return texofs;
}


/*
 *	texname2ofs_doom05 - find the position of a texture in a Doom alpha
 *				0.5 iwad
 */
static int32_t texname2ofs_doom05 (const char *name, MDirPtr &dir)
{
  const char	*lump_name	= "TEXTURES";
  int32_t	 texofs		= 0;

  dir = FindMasterDir (MasterDir, lump_name);
  if (dir == NULL)  // (Theoretically, it should always exist)
    return 0;
  int32_t *offsets = NULL;  // Array of offsets to texture names
  const Wad_file *wf = dir->wadfile;

  wf->seek (dir->dir.start);
  if (wf->error ())
  {
    warn ("%s: can't seek to lump\n", lump_name);
    goto byebye;
  }

  int32_t numtex;			// Number of texture names in TEXTURES
  wf->read_i32 (&numtex);
  if (wf->error ())
  {
    warn ("%s: error reading texture count\n", lump_name);
    goto byebye;
  }

  offsets = (int32_t *) GetMemory ((long) numtex * 4);
  wf->read_i32 (offsets, numtex);
  if (wf->error ())
  {
    warn ("%s: error reading offsets table\n", lump_name);
    goto byebye;
  }
  for (long n = 0; n < numtex; n++)
  {
    char tname[WAD_TEX_NAME + 1];

    wf->seek (dir->dir.start + offsets[n]);
    if (wf->error ())
    {
      warn ("%s: error seeking to definition of texture #%ld\n", lump_name, n);
      break;
    }
    wf->read_bytes (tname, WAD_TEX_NAME);
    if (wf->error ())
    {
      warn ("%s: error reading name of texture #%ld\n", lump_name, n);
      break;
    }
    if (y_strnicmp (tname, name, WAD_TEX_NAME) == 0)
    {
      texofs = dir->dir.start + offsets[n];
      break;
    }
  }

  byebye:
  if (offsets != NULL)
    FreeMemory (offsets);
  return texofs;
}


/*
 *	texname2ofs_normal - find the position of a texture in a "normal" iwad
 *				(normal = not alpha)
 */
static int32_t texname2ofs_normal (const char *name, MDirPtr &dir)
{
  const char *lump_names[] =
  {
    "TEXTURE1",
    "TEXTURE2",				// Only in the registered Doom iwad
    NULL
  };
  int32_t texofs = 0;

  for (const char **lump_name = lump_names; *lump_name != NULL; lump_name++)
  {
    int32_t *offsets = NULL;
    const Wad_file *wf;

    dir = FindMasterDir (MasterDir, *lump_name);
    if (dir == NULL)
    {
      if (strcmp (*lump_name, "TEXTURE1") == 0)
	warn ("%s lump not found, what iwad is this ?\n", *lump_name);
      goto done;
    }

    wf = dir->wadfile;
    wf->seek (dir->dir.start);
    if (wf->error ())
    {
      warn ("%s: can't seek to lump\n", *lump_name);
      goto done;
    }

    int32_t numtex;			// Number of texture names in TEXTURE*
    wf->read_i32 (&numtex);
    if (wf->error ())
    {
      warn ("%s: error reading texture count\n", *lump_name);
      goto done;
    }

    // The next thing in the TEXTURE* lump is a table of numtex offsets,
    // each pointing to a texture definition.
    offsets = (int32_t *) GetMemory ((long) numtex * 4);
    wf->read_i32 (offsets, numtex);
    if (wf->error ())
    {
      warn ("%s: error reading offsets table\n", *lump_name);
      goto done;
    }
    for (long n = 0; n < numtex; n++)
    {
      char tname[WAD_TEX_NAME + 1];

      wf->seek (dir->dir.start + offsets[n]);
      if (wf->error ())
      {
	warn ("%s: error seeking to definition of texture #%ld\n",
	  *lump_name, n);
	break;
      }
      wf->read_bytes (tname, WAD_TEX_NAME);
      if (wf->error ())
      {
	warn ("%s: error reading name of texture #%ld\n", *lump_name, n);
	break;
      }
      if (y_strnicmp (tname, name, WAD_TEX_NAME) == 0)
      {
	texofs = dir->dir.start + offsets[n];
	break;
      }
    }

    done:
    if (offsets != NULL)
      FreeMemory (offsets);
    if (texofs != 0)
      break;
  }

  return texofs;
}


/*
 *	texname2size - get width and height of a texture
 *
 *	If the texture was found and its definition could be read,
 *	*width and *height are set accordingly. Otherwise, *width and
 *	*height and set to -1.
 */
void texname2size (int16_t *width, int16_t *height, const char *texname)
{
  MDirPtr dir = 0;			// Pointer in main directory to texname
  int32_t texofs;			// Offset in wad for the texture data

  *width  = -1;
  *height = -1;

  texofs = texname2ofs (texname, dir);
  if (texofs == 0)			// Texture not found
    return;

  // Read this texture's header
  int32_t header_ofs;
  int16_t w;
  int16_t h;
  if (yg_texture_format == YGTF_NAMELESS)
    header_ofs = texofs;
  else
    header_ofs = texofs + WAD_TEX_NAME;
  dir->wadfile->seek     (header_ofs + 4);
  if (dir->wadfile->error ())
  {
    warn ("texture \"%.*s\": cannot seek to definition\n",
      WAD_TEX_NAME, texname);
    return;
  }
  dir->wadfile->read_i16 (&w);
  dir->wadfile->read_i16 (&h);
  if (dir->wadfile->error ())
  {
    warn ("texture \"%.*s\": cannot read header\n", WAD_TEX_NAME, texname);
    return;
  }
  *width = w;
  *height = h;
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
    512, 256, DisplayWallTexture);
  SwitchToVGA16 ();

  ShowMousePointer ();
}

