/*
 *	pic2img.cc
 *	Loading Doom-format pictures from a wad file.
 *	See the Unofficial Doom Specs, section [5-1].
 *	AYM 1998-??-??
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
#include "pic2img.h"
#include "wads.h"


/*
 *	LoadPicture
 *	Read a picture from a wad file and store it into a buffer.
 *
 *	LoadPicture() does not allocate the buffer itself. The buffer
 *	and the picture don't have to have the same dimensions. Thanks
 *	to this, it can also be used to compose textures : you allocate
 *	a single buffer for the whole texture and then you call
 *	LoadPicture() once for each patch. LoadPicture() takes care of
 *	all the necessary clipping.
 *
 *	If pic_x_offset == INT_MIN, the picture is centred horizontally.
 *	If pic_y_offset == INT_MIN, the picture is centred vertically.
 */
int LoadPicture (
   game_image_pixel_t *buffer,	/* Buffer to load picture into */
   int buf_width,		/* Dimensions of the buffer */
   int buf_height,
   const char *picname,		/* Picture lump name */
   int pic_x_offset,		/* Coordinates of top left corner of picture */
   int pic_y_offset, 		/* relative to top left corner of buffer. */
   int *pic_width,		/* To return the size of the picture */
   int *pic_height)		/* (can be NULL) */
{
MDirPtr	dir;
i16	_pic_width;
i16	_pic_height;
i16	pic_intrinsic_x_ofs;
i16	pic_intrinsic_y_ofs;
u8	*ColumnData;
u8	*Column;
i32	*NeededOffsets;
i32	CurrentOffset;
int	ColumnInMemory;
long	ActualBufLen;
int	pic_x;
int	pic_x0;
int	pic_x1;
int	pic_y0;
int	pic_y1;
u8      *buf;	/* This variable is set to point to the element of
		   <buffer> where the top of the current column
		   should be pasted. It can be off <buffer>! */

#if 0
c->flags = 0;
#endif

dir = (MDirPtr) FindMasterDir (MasterDir, picname);
if (dir == NULL)
   return 1;

wad_seek (dir->wadfile, dir->dir.start);
wad_read_i16 (dir->wadfile, &_pic_width          );
wad_read_i16 (dir->wadfile, &_pic_height         );
wad_read_i16 (dir->wadfile, &pic_intrinsic_x_ofs);  /* Read but ignored */
wad_read_i16 (dir->wadfile, &pic_intrinsic_y_ofs);  /* Read but ignored */
#if 0
c->width  = _pic_width;
c->height = _pic_height;
c->flags  |= HOOK_SIZE_VALID | HOOK_DISP_SIZE;
#endif

// Centre the picture
if (pic_x_offset == INT_MIN)
   pic_x_offset = (buf_width - _pic_width) / 2;
if (pic_y_offset == INT_MIN)
   pic_y_offset = (buf_height - _pic_height) / 2;

/* AYM 19971202: 17 kB is large enough for 128x128 patches. */
#define TEX_COLUMNBUFFERSIZE ((long) 17 * 1024)
/* AYM 19971202: where does 512 come from ? I think that the worst
case (every second pixel transparent) is 64 * 5 + 1 = 321 */
#define TEX_COLUMNSIZE	     512L

ColumnData    = (u8 *) GetMemory (TEX_COLUMNBUFFERSIZE);
/* FIXME DOS and _pic_width > 16000 */
NeededOffsets = (i32 *) GetMemory ((long) _pic_width * 4);

wad_read_i32 (dir->wadfile, NeededOffsets, _pic_width);

/* Read first column data, and subsequent column data */
if (NeededOffsets[0] != 8 + (long) _pic_width * 4)
  wad_seek (dir->wadfile, dir->dir.start + NeededOffsets[0]);
ActualBufLen = wad_read_bytes2 (dir->wadfile, ColumnData, TEX_COLUMNBUFFERSIZE);

/* Clip the picture horizontally and vertically. */
pic_x0 = - pic_x_offset;
if (pic_x0 < 0)
  pic_x0 = 0;

pic_x1 = buf_width - pic_x_offset - 1;
if (pic_x1 >= _pic_width)
  pic_x1 = _pic_width - 1;

pic_y0 = - pic_y_offset;
if (pic_y0 < 0)
  pic_y0 = 0;

pic_y1 = buf_height - pic_y_offset - 1;
if (pic_y1 >= _pic_height)
  pic_y1 = _pic_height - 1;

/* For each (non clipped) column of the picture... */
for (pic_x = pic_x0,
   buf = buffer + al_amax (pic_x_offset, 0) + buf_width * pic_y_offset;
   pic_x <= pic_x1;
   pic_x++, buf++)
   {
   u8 *filedata;

   CurrentOffset  = NeededOffsets[pic_x];
   ColumnInMemory = CurrentOffset >= NeededOffsets[0]
      && CurrentOffset + TEX_COLUMNSIZE <= NeededOffsets[0] + ActualBufLen;
   if (ColumnInMemory)
      Column = ColumnData + CurrentOffset - NeededOffsets[0];
   else
      {
      Column = (u8 *) GetFarMemory (TEX_COLUMNSIZE);
      wad_seek (dir->wadfile, dir->dir.start + CurrentOffset);
      wad_read_bytes2 (dir->wadfile, Column, TEX_COLUMNSIZE);
      }
   filedata = Column;

   /* we now have the needed column data, one way or another, so write it */

   // For each post of the column...
   {
   register u8 *post;
   for (post = filedata; *post != 0xff;)
      {
      int post_y_offset;/* Y-offset of top of post to origin of buffer */
      int post_height;	/* Height of post */
      int post_pic_y0;	/* Start and end of non-clipped part of post, */
      int post_pic_y1;	/* relative to top of picture */
      int post_y0;	/* Start and end of non-clipped part of post, */
      int post_y1;	/* relative to top of post */

      if (post - filedata > TEX_COLUMNSIZE)
         fatal_error ("Post too long. Wad file might be corrupt.");

      post_y_offset = *post++;
      post_height = *post++;
      post++;  // Skip that dummy byte

      /* Clip the post vertically. */
      post_pic_y0 = post_y_offset;
      if (post_pic_y0 < pic_y0)
	 post_pic_y0 = pic_y0;
      
      post_pic_y1 = post_y_offset + post_height - 1;
      if (post_pic_y1 > pic_y1)
	 post_pic_y1 = pic_y1;

      post_y0 = post_pic_y0 - post_y_offset;
      post_y1 = post_pic_y1 - post_y_offset;

      /* "Paste" the post onto the buffer */
      {
      register game_image_pixel_t *b;
      register int post_y;
      for (b = buf + buf_width * (post_y_offset + post_y0), post_y = post_y0;
	 post_y <= post_y1;
	 b += buf_width, post_y++)
         {
         if (b < buffer)
            fatal_error ("b < buffer");
	 *b = (game_image_pixel_t) post[post_y];
         }
      }

      post += post_height;
      post++;  // Skip the trailing dummy byte
      }  // Post loop
   }

   if (!ColumnInMemory)
      FreeFarMemory (Column);
   }  // Column loop
FreeMemory (ColumnData);
FreeMemory (NeededOffsets);
#if 0
c->flags |= HOOK_DRAWN;
#endif
if (pic_width)
   *pic_width  = _pic_width;
if (pic_height)
   *pic_height = _pic_height;
return 0;
}



