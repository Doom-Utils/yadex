/*
 *	mkpalette.cc
 *	Generate palette files from lump PLAYPAL.
 *	AYM 1998-12-29
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-2005 Andr� Majorel and others.

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
#include "mkpalette.h"
#include "gfx.h"
#include "rgb.h"
#include "wadfile.h"
#include "wads.h"


/*
 *	make_gimp_palette
 *	Generate a Gimp palette file for the <playpalnum>th
 *	palette in the PLAYPAL entry.
 *	Return 0 on success, non-zero on failure.
 */
int make_gimp_palette (int playpalnum, const char *filename)
{
  int     rc        = 0;
  MDirPtr dir;
  uint8_t *dpal      = 0;
  FILE	 *output_fp = 0;

  const char *lump_name = "PLAYPAL";
  dir = FindMasterDir (MasterDir, lump_name);
  if (dir == 0)
  {
    warn ("%s: lump not found\n",lump_name);
    return 1;
  }

  int playpal_count = dir->dir.size / (3 * DOOM_COLOURS);
  if (playpalnum < 0 || playpalnum >= playpal_count)
  {
    warn ("playpalnum %d out of range (0-%d), using #0 instead\n",
      playpalnum, playpal_count - 1);
    playpalnum = 0;
  }

  output_fp = fopen (filename, "w");
  if (output_fp == 0)
  {
    warn ("%s: %s\n", filename, strerror (errno));
    return 1;
  }
  fprintf (output_fp,
     "GIMP Palette\n"
     "# Generated by Yadex %s\n", yadex_version);

  dpal = (uint8_t *) GetFarMemory (3 * DOOM_COLOURS);
  const Wad_file *wf = dir->wadfile;
  wf->seek (dir->dir.start + (long) playpalnum * 3 * DOOM_COLOURS);
  if (wf->error ())
  {
    err ("%s: seek error", lump_name);
    rc = 1;
    goto byebye;
  }
  wf->read_bytes (dpal, 3 * DOOM_COLOURS);
  if (wf->error ())
  {
    err ("%s: read error", lump_name);
    rc = 1;
    goto byebye;
  }
  for (size_t n = 0; n < DOOM_COLOURS; n++)
    fprintf (output_fp, "%3d %3d %3d  Index = %d (%02Xh)   RGB = %d, %d, %d\n",
      dpal[3 * n],
      dpal[3 * n + 1],
      dpal[3 * n + 2],
      n,
      n,
      dpal[3 * n],
      dpal[3 * n + 1],
      dpal[3 * n + 2]);

  byebye:
  if (dpal != 0)
    FreeFarMemory (dpal);
  if (output_fp != 0)
    if (fclose (output_fp))
      return 1;
  return rc;
}


/*
 *	make_palette_ppm
 *	Generate a 256 x 128 raw PPM image showing all the
 *	colours in the palette.
 *	Return 0 on success, non-zero on failure.
 */
int make_palette_ppm (int playpalnum, const char *filename)
{
  int     rc        = 0;
  MDirPtr dir;
  uint8_t *dpal      = 0;
  FILE	 *output_fp = 0;

  const char *lump_name = "PLAYPAL";
  dir = FindMasterDir (MasterDir, lump_name);
  if (dir == 0)
  {
    warn ("%s: lump not found\n", lump_name);
    return 1;
  }

  int playpal_count = dir->dir.size / (3 * DOOM_COLOURS);
  if (playpalnum < 0 || playpalnum >= playpal_count)
  {
    warn ("playpalnum %d out of range (0-%d), using #0 instead\n",
      playpalnum, playpal_count - 1);
    playpalnum = 0;
  }

  output_fp = fopen (filename, "wb");
  if (output_fp == 0)
  {
    warn ("%s: %s\n", filename, strerror (errno));
    return 1;
  }

  const int width = 128;
  const int height = 128;
  const int columns = 16;

  fputs ("P6", output_fp);
  fnewline (output_fp);
  fprintf (output_fp, "# Generated by Yadex %s", yadex_version);
  fnewline (output_fp);
  fprintf (output_fp, "%d %d", width, height);
  fnewline (output_fp);
  fputs ("255\n", output_fp);  // Always \n (must be a single character)

  int rect_w = width / columns;
  int rect_h = height / (DOOM_COLOURS / columns);

  dpal = (uint8_t *) GetFarMemory (3 * DOOM_COLOURS);
  const Wad_file *wf = dir->wadfile;
  wf->seek (dir->dir.start + (long) playpalnum * 3 * DOOM_COLOURS);
  if (wf->error ())
  {
    err ("%s: seek error", lump_name);
    rc = 1;
    goto byebye;
  }
  wf->read_bytes (dpal, 3 * DOOM_COLOURS);
  if (wf->error ())
  {
    err ("%s: read error", lump_name);
    rc = 1;
    goto byebye;
  }
  for (size_t n = 0; n < DOOM_COLOURS; n += columns)
    for (int subrow = 0; subrow < rect_h; subrow++)
      for (int c = 0; c < columns; c++)
	for (int subcol = 0; subcol < rect_w; subcol++)
	{
	  if (subrow == 0 && subcol == 0)
	  {
	    putc (0, output_fp);
	    putc (0, output_fp);
	    putc (0, output_fp);
	  }
	  else
	  {
	    putc (dpal[3 * (n + c)],     output_fp);
	    putc (dpal[3 * (n + c) + 1], output_fp);
	    putc (dpal[3 * (n + c) + 2], output_fp);
	  }
	}

  byebye:
  if (dpal != 0)
    FreeFarMemory (dpal);
  if (output_fp != 0)
    if (fclose (output_fp))
      return 1;
  return rc;
}


/*
 *	make_palette_ppm_2
 *	Make a wide PPM containing all the colours in the palette
 */

int make_palette_ppm_2 (int playpalnum, const char *filename)
{
  int     rc        = 0;
  MDirPtr dir;
  uint8_t	 *dpal      = 0;
  FILE	 *output_fp = 0;

  const char *lump_name = "PLAYPAL";
  dir = FindMasterDir (MasterDir, lump_name);
  if (dir == 0)
  {
    warn ("%s: lump not found", lump_name);
    return 1;
  }

  int playpal_count = dir->dir.size / (3 * DOOM_COLOURS);
  if (playpalnum < 0 || playpalnum >= playpal_count)
  {
    warn ("playpalnum %d out of range (0-%d), using #0 instead",
      playpalnum, playpal_count - 1);
    playpalnum = 0;
  }

  output_fp = fopen (filename, "wb");
  if (output_fp == 0)
  {
    warn ("%s: %s\n", filename, strerror (errno));
    return 1;
  }

  const int width = DOOM_COLOURS;
  const int height = DOOM_COLOURS;

  fputs ("P6", output_fp);
  fnewline (output_fp);
  fprintf (output_fp, "# Generated by Yadex %s", yadex_version);
  fnewline (output_fp);
  fprintf (output_fp, "%d %d", width, height);
  fnewline (output_fp);
  fputs ("255\n", output_fp);  // Always \n (must be a single character)

  dpal = (uint8_t *) GetFarMemory (3 * DOOM_COLOURS);
  const Wad_file *wf = dir->wadfile;
  wf->seek (dir->dir.start + (long) playpalnum * 3 * DOOM_COLOURS);
  if (wf->error ())
  {
    err ("%s: seek error", lump_name);
    rc = 1;
    goto byebye;
  }
  wf->read_bytes (dpal, 3 * DOOM_COLOURS);
  if (wf->error ())
  {
    err ("%s: read error", lump_name);
    rc = 1;
    goto byebye;
  }
  for (int l = 0; l < height; l++)
    for (int c = 0; c < width; c++)
    {
      putc (dpal[3 * ((c + l) % DOOM_COLOURS)    ], output_fp);
      putc (dpal[3 * ((c + l) % DOOM_COLOURS) + 1], output_fp);
      putc (dpal[3 * ((c + l) % DOOM_COLOURS) + 2], output_fp);
    }

  byebye:
  if (dpal != 0)
    FreeFarMemory (dpal);
  if (output_fp != 0)
    if (fclose (output_fp))
      return 1;
  return rc;
}


