/*
 *	levels.cc
 *	Level loading and saving routines,
 *	global variables used to hold the level data.
 *	BW & RQ sometime in 1993 or 1994.
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
#include "bitvec.h"
#include "dialog.h"
#include "game.h"
#include "levels.h"
#include "wstructs.h"
#include "things.h"
#include "wads.h"


/*
 	FIXME
	All these variables should be turned
	into members of a "Level" class.
*/
MDirPtr Level;			/* master dictionary entry for the level */
int NumThings;			/* number of things */
TPtr Things;			/* things data */
int NumLineDefs;		/* number of line defs */
LDPtr LineDefs;			/* line defs data */
int NumSideDefs;		/* number of side defs */
SDPtr SideDefs;			/* side defs data */
int NumVertices;		/* number of vertexes */
VPtr Vertices;			/* vertex data */
int NumSectors;			/* number of sectors */
SPtr Sectors;			/* sectors data */

// FIXME should be somewhere else
int NumWTexture;		/* number of wall textures */
char **WTexture;		/* array of wall texture names */

// FIXME all the flat list stuff should be put in a separate class
size_t NumFTexture;		/* number of floor/ceiling textures */
flat_list_entry_t *flat_list;	// List of all flats in the directory

int MapMaxX = -32767;		/* maximum X value of map */
int MapMaxY = -32767;		/* maximum Y value of map */
int MapMinX = 32767;		/* minimum X value of map */
int MapMinY = 32767;		/* minimum Y value of map */
Bool MadeChanges;		/* made changes? */
Bool MadeMapChanges;		/* made changes that need rebuilding? */

char Level_name[WAD_NAME + 1];	/* The name of the level (E.G.
				   "MAP01" or "E1M1"), followed by a
				   NUL. If the Level has been created as
				   the result of a "c" command with no
				   argument, an empty string. The name
				   is not necesarily in upper case but
				   it always a valid lump name, not a
				   command line shortcut like "17". */

y_file_name_t Level_file_name;	/* The name of the file in which
				   the level would be saved. If the
				   level has been created as the result
				   of a "c" command, with or without
				   argument, an empty string. */

y_file_name_t Level_file_name_saved;  /* The name of the file in
				   which the level was last saved. If
				   the Level has never been saved yet,
				   an empty string. */

void EmptyLevelData (const char *levelname)
{
Things = 0;
NumThings = 0;
LineDefs = 0;
NumLineDefs = 0;
SideDefs = 0;
NumSideDefs = 0;
Sectors = 0;
NumSectors = 0;
Vertices = 0;
NumVertices = 0;
}


/*
 *	texno_texname
 *	A convenience function when loading Doom alpha levels
 */
static char *tex_list = 0;
static size_t ntex = 0;
static char tex_name[WAD_TEX_NAME + 1];
inline const char *texno_texname (i16 texno)
{
if (texno < 0)
   return "-";
else
   if (yg_texture_format == YGTF_NAMELESS)
      {
      sprintf (tex_name, "TEX%04u", (unsigned) texno);
      return tex_name;
      }
   else
      {
      if (texno < (i16) ntex)
	 return tex_list + WAD_TEX_NAME * texno;
      else
	 return "unknown";
      }
}


/*
   read in the level data
*/

int ReadLevelData (const char *levelname) /* SWAP! */
{
int rc = 0;
MDirPtr dir;
int OldNumVertices;
int n;

/* No objects are needed: they may be swapped after they have been read */
ObjectsNeeded (0);

/* Find the various level information from the master directory */
DisplayMessage (-1, -1, "Reading data for level %s...", levelname);
Level = FindMasterDir (MasterDir, levelname);
if (!Level)
   fatal_error ("level data not found");

/* Get the number of vertices */
i32 v_offset = 42;
i32 v_length = 42;
if (yg_level_format == YGLF_ALPHA)  // Doom alpha
   dir = FindMasterDir (Level, "POINTS");
else
   dir = FindMasterDir (Level, "VERTEXES");
if (dir)
   {
   v_offset = dir->dir.start;
   v_length = dir->dir.size;
   if (yg_level_format == YGLF_ALPHA)  // Doom alpha: skip leading count
      {
      v_offset += 4;
      v_length -= 4;
      }
   OldNumVertices = (int) (v_length / WAD_VERTEX_BYTES);
   if ((i32) (OldNumVertices * WAD_VERTEX_BYTES) != v_length)
      warn ("the %s lump has a weird size."
        " The wad might be corrupt.\n",
	yg_level_format == YGLF_ALPHA ? "POINTS" : "VERTEXES");
   }
else
   OldNumVertices = 0;

// Read THINGS
verbmsg ("Reading %s things", levelname);
i32 offset = 42;
i32 length;
dir = FindMasterDir (Level, "THINGS");
if (dir)
   {
   offset = dir->dir.start;
   length = dir->dir.size;
   if (MainWad == Iwad4)  // Hexen mode
      {
      NumThings = (int) (length / WAD_HEXEN_THING_BYTES);
      if ((i32) (NumThings * WAD_HEXEN_THING_BYTES) != length)
         warn ("the THINGS lump has a weird size."
            " The wad might be corrupt.\n");
      }
   else                    // Doom/Heretic/Strife mode
      {
      if (yg_level_format == YGLF_ALPHA)  // Doom alpha: skip leading count
	 {
	 offset += 4;
	 length -= 4;
	 }
      size_t thing_size = yg_level_format == YGLF_ALPHA ? 12 : WAD_THING_BYTES;
      NumThings = (int) (length / thing_size);
      if ((i32) (NumThings * thing_size) != length)
         warn ("the THINGS lump has a weird size."
            " The wad might be corrupt.\n");
      }
   }
else
   NumThings = 0;
if (NumThings > 0)
   {
   Things = (TPtr) GetFarMemory ((unsigned long) NumThings
      * sizeof (struct Thing));
   wad_seek (dir->wadfile, offset);
   if (MainWad == Iwad4)  // Hexen mode
      for (n = 0; n < NumThings; n++)
	 {
         u8 dummy2[6];
	 wad_read_i16   (dir->wadfile);				// Tid
	 wad_read_i16   (dir->wadfile, &(Things[n].xpos ));
	 wad_read_i16   (dir->wadfile, &(Things[n].ypos ));
	 wad_read_i16   (dir->wadfile);				// Height
	 wad_read_i16   (dir->wadfile, &(Things[n].angle));
	 wad_read_i16   (dir->wadfile, &(Things[n].type ));
	 wad_read_i16   (dir->wadfile, &(Things[n].when ));
         wad_read_bytes (dir->wadfile, dummy2, sizeof dummy2);
	 }
   else                   // Doom/Heretic/Strife mode
      for (n = 0; n < NumThings; n++)
	 {
	 wad_read_i16 (dir->wadfile, &(Things[n].xpos ));
	 wad_read_i16 (dir->wadfile, &(Things[n].ypos ));
	 wad_read_i16 (dir->wadfile, &(Things[n].angle));
	 wad_read_i16 (dir->wadfile, &(Things[n].type ));
	 if (yg_level_format == YGLF_ALPHA)
	    wad_read_i16 (dir->wadfile);  // Alpha. Don't know what its for.
	 wad_read_i16 (dir->wadfile, &(Things[n].when ));
	 }
   }

// Read LINEDEFS
if (yg_level_format != YGLF_ALPHA)
   {
   verbmsg (" linedefs");
   dir = FindMasterDir (Level, "LINEDEFS");
   if (dir)
      if (MainWad == Iwad4)  // Hexen mode
	 {
	 NumLineDefs = (int) (dir->dir.size / WAD_HEXEN_LINEDEF_BYTES);
	 if ((i32) (NumLineDefs * WAD_HEXEN_LINEDEF_BYTES) != dir->dir.size)
	    warn ("the LINEDEFS lump has a weird size."
	       " The wad might be corrupt.\n");
	 }
      else                   // Doom/Heretic/Strife mode
	 {
	 NumLineDefs = (int) (dir->dir.size / WAD_LINEDEF_BYTES);
	 if ((i32) (NumLineDefs * WAD_LINEDEF_BYTES) != dir->dir.size)
	    warn ("the LINEDEFS lump has a weird size."
	       " The wad might be corrupt.\n");
	 }
   else
      NumLineDefs = 0;
   if (NumLineDefs > 0)
      {
      LineDefs = (LDPtr) GetFarMemory ((unsigned long) NumLineDefs
	 * sizeof (struct LineDef));
      wad_seek (dir->wadfile, dir->dir.start);
      if (MainWad == Iwad4)  // Hexen mode
	 for (n = 0; n < NumLineDefs; n++)
	    {
	    u8 dummy[6];
	    wad_read_i16   (dir->wadfile, &(LineDefs[n].start   ));
	    wad_read_i16   (dir->wadfile, &(LineDefs[n].end     ));
	    wad_read_i16   (dir->wadfile, &(LineDefs[n].flags   ));
	    wad_read_bytes (dir->wadfile, dummy, sizeof dummy);
	    wad_read_i16   (dir->wadfile, &(LineDefs[n].sidedef1));
	    wad_read_i16   (dir->wadfile, &(LineDefs[n].sidedef2));
	    LineDefs[n].type = dummy[0];
	    }
      else                   // Doom/Heretic/Strife mode
	 for (n = 0; n < NumLineDefs; n++)
	    {
	    wad_read_i16 (dir->wadfile, &(LineDefs[n].start   ));
	    wad_read_i16 (dir->wadfile, &(LineDefs[n].end     ));
	    wad_read_i16 (dir->wadfile, &(LineDefs[n].flags   ));
	    wad_read_i16 (dir->wadfile, &(LineDefs[n].type    ));
	    wad_read_i16 (dir->wadfile, &(LineDefs[n].tag     ));
	    wad_read_i16 (dir->wadfile, &(LineDefs[n].sidedef1));
	    wad_read_i16 (dir->wadfile, &(LineDefs[n].sidedef2));
	    }
      }
   }

// Read SIDEDEFS
verbmsg (" sidedefs");
dir = FindMasterDir (Level, "SIDEDEFS");
if (dir)
   {
   NumSideDefs = (int) (dir->dir.size / WAD_SIDEDEF_BYTES);
   if ((i32) (NumSideDefs * WAD_SIDEDEF_BYTES) != dir->dir.size)
      warn ("the SIDEDEFS lump has a weird size."
         " The wad might be corrupt.\n");
   }
else
   NumSideDefs = 0;
if (NumSideDefs > 0)
   {
   SideDefs = (SDPtr) GetFarMemory ((unsigned long) NumSideDefs
      * sizeof (struct SideDef));
   wad_seek (dir->wadfile, dir->dir.start);
   for (n = 0; n < NumSideDefs; n++)
      {
      wad_read_i16   (dir->wadfile, &(SideDefs[n].xoff));
      wad_read_i16   (dir->wadfile, &(SideDefs[n].yoff));
      wad_read_bytes (dir->wadfile, &(SideDefs[n].tex1), WAD_TEX_NAME);
      wad_read_bytes (dir->wadfile, &(SideDefs[n].tex2), WAD_TEX_NAME);
      wad_read_bytes (dir->wadfile, &(SideDefs[n].tex3), WAD_TEX_NAME);
      wad_read_i16   (dir->wadfile, &(SideDefs[n].sector));
      }
   }

/* Sanity checkings on linedefs: the 1st and 2nd vertices
   must exist. The 1st and 2nd sidedefs must exist or be
   set to -1. */
for (n = 0; n < NumLineDefs; n++)
   {
   if (LineDefs[n].sidedef1 != -1
      && outside (LineDefs[n].sidedef1, 0, NumSideDefs - 1))
      {
      warn ("linedef %d has bad 1st sidedef number %d\n",
	 n, LineDefs[n].sidedef1);
      rc = 1;
      goto byebye;
      }
   if (LineDefs[n].sidedef2 != -1
      && outside (LineDefs[n].sidedef2, 0, NumSideDefs - 1))
      {
      warn ("linedef %d has bad 2nd sidedef number %d\n",
	 n, LineDefs[n].sidedef2);
      rc = 1;
      goto byebye;
      }
   if (outside (LineDefs[n].start, 0, OldNumVertices -1))
      {
      warn ("linedef %d has bad 1st vertex number %d\n",
	 n, LineDefs[n].start);
      rc = 1;
      goto byebye;
      }
   if (outside (LineDefs[n].end, 0, OldNumVertices - 1))
      {
      warn ("linedef %d has bad 2nd vertex number %d\n",
	 n, LineDefs[n].end);
      rc = 1;
      goto byebye;
      }
   }

// Read LINES (Doom alpha only)
if (yg_level_format == YGLF_ALPHA)
   {
   verbmsg (" lines");
   dir = FindMasterDir (Level, "LINES");
   if (dir)
      {
      if ((dir->dir.size - 4) % 36)
	 warn ("the LINES lump has a weird size. The wad might be corrupt.\n");
      const size_t nlines = dir->dir.size / 36;
      NumLineDefs = nlines;
      NumSideDefs = 2 * nlines;  // Worst case. We'll adjust later.
      LineDefs = (LDPtr) GetFarMemory ((unsigned long) NumLineDefs
	 * sizeof (struct LineDef));
      SideDefs = (SDPtr) GetFarMemory ((unsigned long) NumSideDefs
	 * sizeof (struct SideDef));
      // Read TEXTURES
      if (yg_texture_format != YGTF_NAMELESS)
      {
	 ntex = 0;
	 MDirPtr d = FindMasterDir (MasterDir, "TEXTURES");
	 if (! d)
	    warn ("TEXTURES lump not found. Will not be able to"
		  " import texture names.\n");
	 else
	    {
	    wad_seek (d->wadfile, d->dir.start);
	    i32 num;
	    wad_read_i32 (d->wadfile, &num);
	    if (num < 0 || num > 32767)
	       warn ("TEXTURES has bad name count. Ignoring TEXTURES.\n");
	    else
	       {
	       ntex = num;
	       i32 *offset_table = new i32[ntex];
	       for (size_t n = 0; n < ntex; n++)
		  wad_read_i32 (d->wadfile, offset_table + n);
	       tex_list = (char *) GetMemory (ntex * WAD_TEX_NAME);
	       for (size_t n = 0; n < ntex; n++)
		  {
		  wad_seek (d->wadfile, d->dir.start + offset_table[n]);
		  wad_read_bytes (d->wadfile, tex_list + WAD_TEX_NAME * n,
		      WAD_TEX_NAME);
		  }
	       delete[] offset_table;
	       }
	    }

      }

      wad_seek (dir->wadfile, dir->dir.start + 4);
      size_t s = 0;
      for (size_t n = 0; n < nlines; n++)
	 {
	 LDPtr ld = LineDefs + n;
	 ld->start   = wad_read_i16 (dir->wadfile);
	 ld->end     = wad_read_i16 (dir->wadfile);
	 ld->flags   = wad_read_i16 (dir->wadfile);
	               wad_read_i16 (dir->wadfile);  // Unused ?
	 ld->type    = wad_read_i16 (dir->wadfile);
	 ld->tag     = wad_read_i16 (dir->wadfile);
	               wad_read_i16 (dir->wadfile);  // Unused ?
	 i16 sector1 = wad_read_i16 (dir->wadfile);
	 i16 xofs1   = wad_read_i16 (dir->wadfile);
	 i16 tex1m   = wad_read_i16 (dir->wadfile);
	 i16 tex1u   = wad_read_i16 (dir->wadfile);
	 i16 tex1l   = wad_read_i16 (dir->wadfile);
	               wad_read_i16 (dir->wadfile);  // Unused ?
	 i16 sector2 = wad_read_i16 (dir->wadfile);
	 i16 xofs2   = wad_read_i16 (dir->wadfile);
	 i16 tex2m   = wad_read_i16 (dir->wadfile);
	 i16 tex2u   = wad_read_i16 (dir->wadfile);
	 i16 tex2l   = wad_read_i16 (dir->wadfile);
	 if (sector1 >= 0)			// Create first sidedef
	    {
	    ld->sidedef1 = s;
	    SDPtr sd = SideDefs + s;
	    sd->xoff = xofs1;
	    sd->yoff = 0;
	    memcpy (sd->tex1, texno_texname (tex1u), sizeof sd->tex1);
	    memcpy (sd->tex2, texno_texname (tex1l), sizeof sd->tex2);
	    memcpy (sd->tex3, texno_texname (tex1m), sizeof sd->tex3);
	    sd->sector = sector1;
	    s++;
	    }
	 else  // No first sidedef !
	    ld->sidedef1 = -1;
	 if (ld->flags & 0x04)			// Create second sidedef
	    {
	    ld->sidedef2 = s;
	    SDPtr sd = SideDefs + s;
	    sd->xoff = xofs2;
	    sd->yoff = 0;
	    memcpy (sd->tex1, texno_texname (tex2u), sizeof sd->tex1);
	    memcpy (sd->tex2, texno_texname (tex2l), sizeof sd->tex2);
	    memcpy (sd->tex3, texno_texname (tex2m), sizeof sd->tex3);
	    sd->sector = sector2;
	    s++;
	    }
	 else
	    ld->sidedef2 = -1;
	 if (dir->wadfile->error)
	    {
	    warn ("read error in %s\n", dir->wadfile->filename);
	    break;
	    }
	 }
      if (NumSideDefs > s)  // Almost always true.
         {
	 NumSideDefs = s;
         SideDefs = (SDPtr) ResizeFarMemory (SideDefs,
	     (unsigned long) NumSideDefs * sizeof (struct SideDef));
         }
      if (tex_list)
         FreeMemory (tex_list);
      tex_list = 0;
      ntex = 0;
      }
   }

/* Read the vertices. If the wad has been run through a nodes
   builder, there is a bunch of vertices at the end that are not
   used by any linedefs. Those vertices have been created by the
   nodes builder for the segs. We ignore them, because they're
   useless to the level designer. However, we do NOT ignore
   unused vertices in the middle because that's where the
   "string art" bug came from.

   Note that there is absolutely no guarantee that the nodes
   builders add their own vertices at the end, instead of at the
   beginning or right in the middle, AFAIK. It's just that they
   all seem to do that (1). What if some don't ? Well, we would
   end up with many unwanted vertices in the level data. Nothing
   that a good CheckCrossReferences() couldn't take care of. */
{
verbmsg (" vertices");
int last_used_vertex = -1;
for (n = 0; n < NumLineDefs; n++)
   {
   last_used_vertex = y_max (last_used_vertex, LineDefs[n].start);
   last_used_vertex = y_max (last_used_vertex, LineDefs[n].end);
   }
NumVertices = last_used_vertex + 1;
// This block is only here to warn me if (1) is false.
{
  bitvec_c vertex_used (OldNumVertices);
  for (int n = 0; n < NumLineDefs; n++)
     {
     vertex_used.set (LineDefs[n].start);
     vertex_used.set (LineDefs[n].end);
     }
  int unused = 0;
  for (int n = 0; n <= last_used_vertex; n++)
     {
     if (! vertex_used.get (n))
	unused++;
     }
  if (unused > 0)
     {
     warn ("this level has unused vertices in the middle.\n");
     warn ("total %d, tail %d (%d%%), unused %d (",
	OldNumVertices,
	OldNumVertices - NumVertices,
	NumVertices - unused
	   ? 100 * (OldNumVertices - NumVertices) / (NumVertices - unused)
	   : 0,
	unused);
     int first = 1;
     for (int n = 0; n <= last_used_vertex; n++)
        {
	if (! vertex_used.get (n))
	   {
	   if (n == 0 || vertex_used.get (n - 1))
	      {
	      if (first)
	         first = 0;
	      else
	         warn (", ");
	      warn ("%d", n);
	      }
	   else if (n == last_used_vertex || vertex_used.get (n + 1))
	      warn ("-%d", n);
	   }
	}
     warn (")\n");
     }
}
// Now load all the vertices except the unused ones at the end.
if (NumVertices > 0)
   {
   Vertices = (VPtr) GetFarMemory ((unsigned long) NumVertices
      * sizeof (struct Vertex));
   if (yg_level_format == YGLF_ALPHA)  // Doom alpha
      dir = FindMasterDir (Level, "POINTS");
   else
      dir = FindMasterDir (Level, "VERTEXES");
   if (dir)
      {
      wad_seek (dir->wadfile, v_offset);
      MapMaxX = -32767;
      MapMaxY = -32767;
      MapMinX = 32767;
      MapMinY = 32767;
      for (n = 0; n < NumVertices; n++)
	 {
	 i16 val;
	 wad_read_i16 (dir->wadfile, &val);
	 if (val < MapMinX)
	    MapMinX = val;
	 if (val > MapMaxX)
	    MapMaxX = val;
	 Vertices[n].x = val;
	 wad_read_i16 (dir->wadfile, &val);
	 if (val < MapMinY)
	    MapMinY = val;
	 if (val > MapMaxY)
	    MapMaxY = val;
	 Vertices[n].y = val;
	 }
      }
   }
}

// Ignore SEGS, SSECTORS and NODES

// Read SECTORS
verbmsg (" sectors\n");
dir = FindMasterDir (Level, "SECTORS");
if (yg_level_format != YGLF_ALPHA)
   {
   if (dir)
      {
      NumSectors = (int) (dir->dir.size / WAD_SECTOR_BYTES);
      if ((i32) (NumSectors * WAD_SECTOR_BYTES) != dir->dir.size)
	 warn ("the SECTORS lump has a weird size."
	   " The wad might be corrupt.\n");
      }
   else
      NumSectors = 0;
   if (NumSectors > 0)
      {
      Sectors = (SPtr) GetFarMemory ((unsigned long) NumSectors
	 * sizeof (struct Sector));
      wad_seek (dir->wadfile, dir->dir.start);
      for (n = 0; n < NumSectors; n++)
	 {
	 wad_read_i16   (dir->wadfile, &(Sectors[n].floorh ));
	 wad_read_i16   (dir->wadfile, &(Sectors[n].ceilh  ));
	 wad_read_bytes (dir->wadfile, &(Sectors[n].floort ), WAD_FLAT_NAME);
	 wad_read_bytes (dir->wadfile, &(Sectors[n].ceilt  ), WAD_FLAT_NAME);
	 wad_read_i16   (dir->wadfile, &(Sectors[n].light  ));
	 wad_read_i16   (dir->wadfile, &(Sectors[n].special));
	 wad_read_i16   (dir->wadfile, &(Sectors[n].tag    ));
	 }
      }
   }
else  // Doom alpha--a wholly different SECTORS format
   {
   if (dir)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      i32 nsectors;
      i32 nflatnames = 0;
      char *flatnames = 0;
      wad_read_i32 (dir->wadfile, &nsectors);
      NumSectors = nsectors;
      Sectors = (SPtr) GetFarMemory ((unsigned long) NumSectors
	 * sizeof (struct Sector));
      i32 *offset_table = new i32[nsectors];
      for (size_t n = 0; n < nsectors; n++)
	 wad_read_i32 (dir->wadfile, offset_table + n);
      // Load FLATNAME
      {
	 MDirPtr dir2 = FindMasterDir (Level, "FLATNAME");
	 if (dir2)
	    {
	    wad_seek (dir2->wadfile, dir2->dir.start);
	    wad_read_i32 (dir2->wadfile, &nflatnames);
	    if (nflatnames < 0 || nflatnames > 32767)
	       {
	       warn ("FLATNAME has bad name count. Ignoring FLATNAME.\n");
	       nflatnames = 0;
	       }
	    else
	       {
	       flatnames = new char[WAD_FLAT_NAME * nflatnames];
	       wad_read_bytes (dir2->wadfile, flatnames,
		   WAD_FLAT_NAME * nflatnames);
	       }
	    }
      }
      for (size_t n = 0; n < nsectors; n++)
	 {
	 wad_seek (dir->wadfile, dir->dir.start + offset_table[n]);
	 i16 index;
	 wad_read_i16 (dir->wadfile, &(Sectors[n].floorh));
	 wad_read_i16 (dir->wadfile, &(Sectors[n].ceilh ));
	 wad_read_i16 (dir->wadfile, &index);
	 if (nflatnames && flatnames && index >= 0 && index < nflatnames)
	    memcpy (Sectors[n].floort, flatnames + WAD_FLAT_NAME * index,
		WAD_FLAT_NAME);
	 else
	    strcpy (Sectors[n].floort, "unknown");
	 wad_read_i16 (dir->wadfile, &index);
	 if (nflatnames && flatnames && index >= 0 && index < nflatnames)
	    memcpy (Sectors[n].ceilt, flatnames + WAD_FLAT_NAME * index,
		WAD_FLAT_NAME);
	 else
	    strcpy (Sectors[n].ceilt, "unknown");
	 wad_read_i16 (dir->wadfile, &(Sectors[n].light  ));
	 wad_read_i16 (dir->wadfile, &(Sectors[n].special));
	 wad_read_i16 (dir->wadfile, &(Sectors[n].tag));
	 // Don't know what the tail is for. Ignore it.
	 }
      delete[] offset_table;
      if (flatnames)
	 delete[] flatnames;
      }
   }

/* Sanity checking on sidedefs: the sector must exist. I don't
   make this a fatal error, though, because it's not exceptional
   to find wads with unused sidedefs with a sector# of -1. Well
   known ones include dyst3 (MAP06, MAP07, MAP08), mm (MAP16),
   mm2 (MAP13, MAP28) and requiem (MAP03, MAP08, ...). */
for (n = 0; n < NumSideDefs; n++)
   {
   if (outside (SideDefs[n].sector, 0, NumSectors - 1))
      warn ("sidedef %d has bad sector number %d\n",
	 n, SideDefs[n].sector);
   }

// Ignore REJECT and BLOCKMAP

// Silly statistics
verbmsg ("  %d things, %d vertices, %d linedefs, %d sidedefs, %d sectors\n",
   (int) NumThings, (int) NumVertices, (int) NumLineDefs,
   (int) NumSideDefs, (int) NumSectors);
verbmsg ("  Map: (%d,%d)-(%d,%d)\n", MapMinX, MapMinY, MapMaxX, MapMaxY);

byebye:
return rc;
}



/*
   forget the level data
*/

void ForgetLevelData () /* SWAP! */
{
/* forget the things */
ObjectsNeeded (OBJ_THINGS, 0);
NumThings = 0;
if (Things)
   FreeFarMemory (Things);
Things = 0;

/* forget the vertices */
ObjectsNeeded (OBJ_VERTICES, 0);
NumVertices = 0;
if (Vertices)
   FreeFarMemory (Vertices);
Vertices = 0;

/* forget the linedefs */
ObjectsNeeded (OBJ_LINEDEFS, 0);
NumLineDefs = 0;
if (LineDefs)
   FreeFarMemory (LineDefs);
LineDefs = 0;

/* forget the sidedefs */
ObjectsNeeded (OBJ_SIDEDEFS, 0);
NumSideDefs = 0;
if (SideDefs)
   FreeFarMemory (SideDefs);
SideDefs = 0;

/* forget the sectors */
ObjectsNeeded (OBJ_SECTORS, 0);
NumSectors = 0;
if (Sectors)
   FreeFarMemory (Sectors);
Sectors = 0;
ObjectsNeeded (0);
}


/*
 *	Save the level data to a pwad file
 *	The name of the level is always obtained from
 *	<level_name>, whether or not the level was created from
 *	scratch.
 *
 *	The previous contents of the pwad file are lost. Yes, it
 *	sucks but it's not easy to fix.
 *
 *	The lumps are always written in the same order, the same
 *	as the one in the Doom iwad. The length field of the
 *	marker lump is always set to 0. Its offset field is
 *	always set to the offset of the first lump of the level
 *	(THINGS).
 *
 *	If the level has been created by editing an existing
 *	level and has not been changed in a way that calls for a
 *	rebuild of the nodes, the VERTEXES, SEGS, SSECTORS,
 *	NODES, REJECT and BLOCKMAP lumps are copied from the
 *	original level. Otherwise, they are created with a
 *	length of 0 bytes and an offset equal to the offset of
 *	the previous lump plus its length.
 *
 *	Returns 0 on success and non-zero on failure (see errno).
 */

#ifdef NEW_SAVE_METHOD
int SaveLevelData (const char *outfile, const char *level_name) /* SWAP! */
{
FILE   *file;
MDirPtr dir;
int     n;
long	lump_offset[WAD_LL__];
size_t	lump_size[WAD_LL__];
wad_level_lump_no_t l;

if (yg_level_format == YGLF_HEXEN || ! strcmp (Game, "hexen"))
   {
   Notify (-1, -1, "I refuse to save. Hexen mode is still",
                   "too badly broken. You would lose data.");
   return 1;
   }
if (! level_name || ! levelname2levelno (level_name))
   {
   nf_bug ("SaveLevelData called with bad level_name.\n"
       "Using \"E1M1\" instead.");
   level_name = "E1M1";
   }
DisplayMessage (-1, -1, "Saving data to \"%s\"...", outfile);
LogMessage (": Saving data to \"%s\"...\n", outfile);
if ((file = fopen (outfile, "wb")) == NULL)
   fatal_error ("Unable to open file \"%s\"", outfile);

// Write the pwad header
WriteBytes (file, "PWAD", 4);		// Pwad file
file_write_i32 (file, WAD_LL__);	// Number of entries = 11
file_write_i32 (file, 0);		// Fix this up later
if (Level)
   dir = Level->next;
else
   dir = 0;  // Useless except to trap accidental dereferences

// The label (EnMm or MAPnm)
l = WAD_LL_LABEL;
lump_offset[l] = ftell (file);	// By definition
lump_size[l]   =  0;		// By definition
 
// Write the THINGS lump
l = WAD_LL_THINGS;
lump_offset[l] = ftell (file);
ObjectsNeeded (OBJ_THINGS, 0);
for (n = 0; n < NumThings; n++)
   {
   file_write_i16 (file, Things[n].xpos );
   file_write_i16 (file, Things[n].ypos );
   file_write_i16 (file, Things[n].angle);
   file_write_i16 (file, Things[n].type );
   file_write_i16 (file, Things[n].when );
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the LINEDEFS lump
l = WAD_LL_LINEDEFS;
lump_offset[WAD_LL_LINEDEFS] = ftell (file);
ObjectsNeeded (OBJ_LINEDEFS, 0);
for (n = 0; n < NumLineDefs; n++)
   {
   file_write_i16 (file, LineDefs[n].start   );
   file_write_i16 (file, LineDefs[n].end     );
   file_write_i16 (file, LineDefs[n].flags   );
   file_write_i16 (file, LineDefs[n].type    );
   file_write_i16 (file, LineDefs[n].tag     );
   file_write_i16 (file, LineDefs[n].sidedef1);
   file_write_i16 (file, LineDefs[n].sidedef2);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the SIDEDEFS lump
l = WAD_LL_SIDEDEFS;
lump_offset[l] = ftell (file);
ObjectsNeeded (OBJ_SIDEDEFS, 0);
for (n = 0; n < NumSideDefs; n++)
   {
   file_write_i16 (file, SideDefs[n].xoff);
   file_write_i16 (file, SideDefs[n].yoff);
   WriteBytes     (file, &(SideDefs[n].tex1), WAD_TEX_NAME);
   WriteBytes     (file, &(SideDefs[n].tex2), WAD_TEX_NAME);
   WriteBytes     (file, &(SideDefs[n].tex3), WAD_TEX_NAME);
   file_write_i16 (file, SideDefs[n].sector);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the VERTEXES lump
l = WAD_LL_VERTEXES;
lump_offset[WAD_LL_VERTEXES] = ftell (file);
if (! Level || MadeMapChanges)
   {
   /* Write the vertices */
   ObjectsNeeded (OBJ_VERTICES, 0);
   for (n = 0; n < NumVertices; n++)
      {
      file_write_i16 (file, Vertices[n].x);
      file_write_i16 (file, Vertices[n].y);
      }
   }
else
   {
   /* Copy the vertices */
   ObjectsNeeded (0);
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, dir->dir.size);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the SEGS, SSECTORS and NODES lumps
for (n = 0; n < 3; n++)
   {
   if (n == 0)
      l = WAD_LL_SEGS;
   else if (n == 1)
      l = WAD_LL_SSECTORS;
   else if (n == 2)
      l = WAD_LL_NODES;
   lump_offset[l] = ftell (file);
   if (Level && ! MadeMapChanges
       && yg_level_format != YGLF_ALPHA)  // Doom alpha lacks those lumps
      {
      wad_seek (dir->wadfile, dir->dir.start);
      CopyBytes (file, dir->wadfile->fd, dir->dir.size);
      }
   lump_size[l] = ftell (file) - lump_offset[l];
   if (Level)
      dir = dir->next;
   }

// Write the SECTORS lump
l = WAD_LL_SECTORS;
lump_offset[l] = ftell (file);
ObjectsNeeded (OBJ_SECTORS, 0);
for (n = 0; n < NumSectors; n++)
   {
   file_write_i16 (file, Sectors[n].floorh);
   file_write_i16 (file, Sectors[n].ceilh );
   WriteBytes     (file, Sectors[n].floort, WAD_FLAT_NAME);
   WriteBytes     (file, Sectors[n].ceilt,  WAD_FLAT_NAME);
   file_write_i16 (file, Sectors[n].light  );
   file_write_i16 (file, Sectors[n].special);
   file_write_i16 (file, Sectors[n].tag    );
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the REJECT lump
l = WAD_LL_REJECT;
lump_offset[l] = ftell (file);
if (Level && ! MadeMapChanges
    && yg_level_format != YGLF_ALPHA)  // Doom alpha lacks that lump
   {
   /* Copy the REJECT data */
   ObjectsNeeded (0);
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, dir->dir.size);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the BLOCKMAP lump
l = WAD_LL_BLOCKMAP;
lump_offset[l] = ftell (file);
if (Level && ! MadeMapChanges
    && yg_level_format != YGLF_ALPHA)  // Doom alpha lacks that lump
   {
   ObjectsNeeded (0);
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, dir->dir.size);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the actual directory
long dir_offset = ftell (file);
for (int L = 0; L < (int) WAD_LL__; L++)
   {
   file_write_i32 (file, lump_offset[L]);
   file_write_i32 (file, lump_size[L]);
   if (L == (int) WAD_LL_LABEL)
      file_write_name (file, level_name);
   else
      file_write_name (file, wad_level_lump[L].name);
   }

/* Fix up the directory start information */
if (fseek (file, 8, SEEK_SET))
   fatal_error ("error writing to file");
file_write_i32 (file, dir_offset);

/* Close the file */
fclose (file);

/* The file is now up to date */
if (! Level || MadeMapChanges)
   remind_to_build_nodes = 1;
MadeChanges = 0;
MadeMapChanges = 0;
ObjectsNeeded (0);

/* Update pointers in Master Directory */
OpenPatchWad (outfile);

/* This should free the old "*.bak" file */
CloseUnusedWadFiles ();

/* Update MapMinX, MapMinY, MapMaxX, MapMaxY */
// Probably not necessary anymore -- AYM 1999-04-05
ObjectsNeeded (OBJ_VERTICES, 0);
MapMaxX = -32767;
MapMaxY = -32767;
MapMinX = 32767;
MapMinY = 32767;
for (n = 0; n < NumVertices; n++)
   {
   if (Vertices[n].x < MapMinX)
      MapMinX = Vertices[n].x;
   if (Vertices[n].x > MapMaxX)
      MapMaxX = Vertices[n].x;
   if (Vertices[n].y < MapMinY)
      MapMinY = Vertices[n].y;
   if (Vertices[n].y > MapMaxY)
      MapMaxY = Vertices[n].y;
   }
return 0;
}
#else
int SaveLevelData (const char *outfile, const char *level_name) /* SWAP! */
{
FILE   *file;
MDirPtr dir;
int     n;
long	lump_offset[WAD_LL__];
size_t	lump_size[WAD_LL__];
wad_level_lump_no_t l;

if (yg_level_format == YGLF_HEXEN || ! strcmp (Game, "hexen"))
   {
   Notify (-1, -1, "I refuse to save. Hexen mode is still",
                   "too badly broken. You would lose data.");
   return 1;
   }
if (! level_name || ! levelname2levelno (level_name))
   {
   nf_bug ("SaveLevelData: bad level_name \"%s\", using \"E1M1\" instead.",
       level_name);
   level_name = "E1M1";
   }
DisplayMessage (-1, -1, "Saving data to \"%s\"...", outfile);
LogMessage (": Saving data to \"%s\"...\n", outfile);
if ((file = fopen (outfile, "wb")) == NULL)
   fatal_error ("Unable to open file \"%s\"", outfile);

/* Can we reuse the old nodes ? Not if this is a new level from
   scratch or if the structure of the level has changed. If the
   level comes from an alpha version of Doom, we can't either
   because that version of Doom didn't have SEGS, NODES, etc. */
bool reuse_nodes = Level
  && ! MadeMapChanges
  && yg_level_format != YGLF_ALPHA;

// Write the pwad header
WriteBytes (file, "PWAD", 4);		// Pwad file
file_write_i32 (file, WAD_LL__);	// Number of entries = 11
file_write_i32 (file, 0);		// Fix this up later
if (Level)
   dir = Level->next;
else
   dir = 0;  // Useless except to trap accidental dereferences

// The label (EnMm or MAPnm)
l = WAD_LL_LABEL;
lump_offset[l] = ftell (file);	// By definition
lump_size[l]   =  0;		// By definition
 
// Write the THINGS lump
l = WAD_LL_THINGS;
lump_offset[l] = ftell (file);
ObjectsNeeded (OBJ_THINGS, 0);
for (n = 0; n < NumThings; n++)
   {
   file_write_i16 (file, Things[n].xpos );
   file_write_i16 (file, Things[n].ypos );
   file_write_i16 (file, Things[n].angle);
   file_write_i16 (file, Things[n].type );
   file_write_i16 (file, Things[n].when );
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the LINEDEFS lump
l = WAD_LL_LINEDEFS;
lump_offset[WAD_LL_LINEDEFS] = ftell (file);
ObjectsNeeded (OBJ_LINEDEFS, 0);
for (n = 0; n < NumLineDefs; n++)
   {
   file_write_i16 (file, LineDefs[n].start   );
   file_write_i16 (file, LineDefs[n].end     );
   file_write_i16 (file, LineDefs[n].flags   );
   file_write_i16 (file, LineDefs[n].type    );
   file_write_i16 (file, LineDefs[n].tag     );
   file_write_i16 (file, LineDefs[n].sidedef1);
   file_write_i16 (file, LineDefs[n].sidedef2);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the SIDEDEFS lump
l = WAD_LL_SIDEDEFS;
lump_offset[l] = ftell (file);
ObjectsNeeded (OBJ_SIDEDEFS, 0);
for (n = 0; n < NumSideDefs; n++)
   {
   file_write_i16 (file, SideDefs[n].xoff);
   file_write_i16 (file, SideDefs[n].yoff);
   WriteBytes     (file, &(SideDefs[n].tex1), WAD_TEX_NAME);
   WriteBytes     (file, &(SideDefs[n].tex2), WAD_TEX_NAME);
   WriteBytes     (file, &(SideDefs[n].tex3), WAD_TEX_NAME);
   file_write_i16 (file, SideDefs[n].sector);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the VERTEXES lump
l = WAD_LL_VERTEXES;
lump_offset[WAD_LL_VERTEXES] = ftell (file);
if (reuse_nodes)
   {
   /* Copy the vertices */
   ObjectsNeeded (0);
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, dir->dir.size);
   }
else
   {
   /* Write the vertices */
   ObjectsNeeded (OBJ_VERTICES, 0);
   for (n = 0; n < NumVertices; n++)
      {
      file_write_i16 (file, Vertices[n].x);
      file_write_i16 (file, Vertices[n].y);
      }
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the SEGS, SSECTORS and NODES lumps
for (n = 0; n < 3; n++)
   {
   if (n == 0)
      l = WAD_LL_SEGS;
   else if (n == 1)
      l = WAD_LL_SSECTORS;
   else if (n == 2)
      l = WAD_LL_NODES;
   lump_offset[l] = ftell (file);
   if (reuse_nodes)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      CopyBytes (file, dir->wadfile->fd, dir->dir.size);
      }
   lump_size[l] = ftell (file) - lump_offset[l];
   if (Level)
      dir = dir->next;
   }

// Write the SECTORS lump
l = WAD_LL_SECTORS;
lump_offset[l] = ftell (file);
ObjectsNeeded (OBJ_SECTORS, 0);
for (n = 0; n < NumSectors; n++)
   {
   file_write_i16 (file, Sectors[n].floorh);
   file_write_i16 (file, Sectors[n].ceilh );
   WriteBytes     (file, Sectors[n].floort, WAD_FLAT_NAME);
   WriteBytes     (file, Sectors[n].ceilt,  WAD_FLAT_NAME);
   file_write_i16 (file, Sectors[n].light  );
   file_write_i16 (file, Sectors[n].special);
   file_write_i16 (file, Sectors[n].tag    );
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the REJECT lump
l = WAD_LL_REJECT;
lump_offset[l] = ftell (file);
if (reuse_nodes)
   {
   /* Copy the REJECT data */
   ObjectsNeeded (0);
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, dir->dir.size);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the BLOCKMAP lump
l = WAD_LL_BLOCKMAP;
lump_offset[l] = ftell (file);
if (reuse_nodes)
   {
   ObjectsNeeded (0);
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, dir->dir.size);
   }
lump_size[l] = ftell (file) - lump_offset[l];
if (Level)
   dir = dir->next;

// Write the actual directory
long dir_offset = ftell (file);
for (int L = 0; L < (int) WAD_LL__; L++)
   {
   file_write_i32 (file, lump_offset[L]);
   file_write_i32 (file, lump_size[L]);
   if (L == (int) WAD_LL_LABEL)
      file_write_name (file, level_name);
   else
      file_write_name (file, wad_level_lump[L].name);
   }

/* Fix up the directory start information */
if (fseek (file, 8, SEEK_SET))
   fatal_error ("error writing to file");
file_write_i32 (file, dir_offset);

/* Close the file */
fclose (file);

/* The file is now up to date */
if (! Level || MadeMapChanges)
   remind_to_build_nodes = 1;
MadeChanges = 0;
MadeMapChanges = 0;
ObjectsNeeded (0);

/* Update pointers in Master Directory */
OpenPatchWad (outfile);

/* This should free the old "*.bak" file */
CloseUnusedWadFiles ();

/* Update MapMinX, MapMinY, MapMaxX, MapMaxY */
// Probably not necessary anymore -- AYM 1999-04-05
ObjectsNeeded (OBJ_VERTICES, 0);
MapMaxX = -32767;
MapMaxY = -32767;
MapMinX = 32767;
MapMinY = 32767;
for (n = 0; n < NumVertices; n++)
   {
   if (Vertices[n].x < MapMinX)
      MapMinX = Vertices[n].x;
   if (Vertices[n].x > MapMaxX)
      MapMaxX = Vertices[n].x;
   if (Vertices[n].y < MapMinY)
      MapMinY = Vertices[n].y;
   if (Vertices[n].y > MapMaxY)
      MapMaxY = Vertices[n].y;
   }
return 0;
}
#endif  /* NEW_SAVE_METHOD */


/*
 *	flat_list_entry_cmp
 *	Function used by qsort() to sort the flat_list_entry array
 *	by ascending flat name.
 */
static int flat_list_entry_cmp (const void *a, const void *b)
{
return y_strnicmp (
    ((const flat_list_entry_t *) a)->name,
    ((const flat_list_entry_t *) b)->name,
    WAD_FLAT_NAME);
}


/*
   function used by qsort to sort the texture names
*/
static int SortTextures (const void *a, const void *b)
{
return y_strnicmp (*((const char *const *)a), *((const char *const *)b),
    WAD_TEX_NAME);
}


/*
   read the texture names
*/
void ReadWTextureNames ()
{
MDirPtr dir;
i32 *offsets;
int n;
i32 val;

verbmsg ("Reading texture names\n");

// Doom alpha 0.4 : "TEXTURES", no names
if (yg_texture_lumps == YGTL_TEXTURES
 && yg_texture_format == YGTF_NAMELESS)
   {
   dir = FindMasterDir (MasterDir, "TEXTURES");
   if (dir != NULL)
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &val);
      NumWTexture = (int) val + 1;
      WTexture = (char **) GetMemory ((long) NumWTexture * sizeof *WTexture);
      WTexture[0] = (char *) GetMemory (WAD_TEX_NAME + 1);
      strcpy (WTexture[0], "-");
      if (WAD_TEX_NAME < 7) nf_bug ("WAD_TEX_NAME too small");  // Sanity
      for (long n = 0; n < val; n++)
	 {
	 WTexture[n + 1] = (char *) GetMemory (WAD_TEX_NAME + 1);
	 if (n > 9999)
	    {
	    warn ("more than 10,000 textures. Ignoring excess.\n");
	    break;
	    }
	 sprintf (WTexture[n + 1], "TEX%04ld", n);
	 }
      }
   }

// Doom alpha 0.5 : only "TEXTURES"
else if (yg_texture_lumps == YGTL_TEXTURES
      && (yg_texture_format == YGTF_NORMAL
	  || yg_texture_format == YGTF_STRIFE11))
   {
   dir = FindMasterDir (MasterDir, "TEXTURES");
   if (dir != NULL)  // In theory it always exists, though
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &val);
      NumWTexture = (int) val + 1;
      /* read in the offsets for texture1 names */
      offsets = (i32 *) GetMemory ((long) NumWTexture * 4);
      wad_read_i32 (dir->wadfile, offsets + 1, NumWTexture - 1);
      /* read in the actual names */
      WTexture = (char **) GetMemory ((long) NumWTexture * sizeof (char *));
      WTexture[0] = (char *) GetMemory (WAD_TEX_NAME + 1);
      strcpy (WTexture[0], "-");
      for (n = 1; n < NumWTexture; n++)
	 {
	 WTexture[n] = (char *) GetMemory (WAD_TEX_NAME + 1);
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, WTexture[n], WAD_TEX_NAME);
	 WTexture[n][WAD_TEX_NAME] = '\0';
	 }
      FreeMemory (offsets);
      }
   else
      warn ("TEXTURES lump not found. Iwad might be corrupt.\n");
   }
// Other iwads : "TEXTURE1" and possibly "TEXTURE2"
else if (yg_texture_lumps == YGTL_NORMAL
      && (yg_texture_format == YGTF_NORMAL
	  || yg_texture_format == YGTF_STRIFE11))
   {
   dir = FindMasterDir (MasterDir, "TEXTURE1");
   if (dir != NULL)  // In theory it always exists, though
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &val);
      NumWTexture = (int) val + 1;
      /* read in the offsets for texture1 names */
      offsets = (i32 *) GetMemory ((long) NumWTexture * 4);
      wad_read_i32 (dir->wadfile, offsets + 1, NumWTexture - 1);
      /* read in the actual names */
      WTexture = (char **) GetMemory ((long) NumWTexture * sizeof (char *));
      WTexture[0] = (char *) GetMemory (WAD_TEX_NAME + 1);
      strcpy (WTexture[0], "-");
      for (n = 1; n < NumWTexture; n++)
	 {
	 WTexture[n] = (char *) GetMemory (WAD_TEX_NAME + 1);
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, WTexture[n], WAD_TEX_NAME);
	 WTexture[n][WAD_TEX_NAME] = '\0';
	 }
      FreeMemory (offsets);
      }
   {
   dir = FindMasterDir (MasterDir, "TEXTURE2");
   if (dir)  /* Doom II has no TEXTURE2 */
      {
      wad_seek (dir->wadfile, dir->dir.start);
      wad_read_i32 (dir->wadfile, &val);
      /* read in the offsets for texture2 names */
      offsets = (i32 *) GetMemory ((long) val * 4);
      wad_read_i32 (dir->wadfile, offsets, val);
      /* read in the actual names */
      WTexture = (char **) ResizeMemory (WTexture,
	 (NumWTexture + val) * sizeof (char *));
      for (n = 0; n < val; n++)
	 {
	 WTexture[NumWTexture + n] = (char *) GetMemory (WAD_TEX_NAME + 1);
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, WTexture[NumWTexture + n], WAD_TEX_NAME);
	 WTexture[NumWTexture + n][WAD_TEX_NAME] = '\0';
	 }
      NumWTexture += val;
      FreeMemory (offsets);
      }
   }
   }
else
   nf_bug ("Invalid texture_format/texture_lumps combination.");

/* sort the names */
qsort (WTexture, NumWTexture, sizeof (char *), SortTextures);
}



/*
   forget the texture names
*/

void ForgetWTextureNames ()
{
int n;

/* forget all names */
for (n = 0; n < NumWTexture; n++)
   FreeMemory (WTexture[n]);

/* forget the array */
NumWTexture = 0;
FreeMemory (WTexture);
}



/*
   read the flat names
*/

void ReadFTextureNames ()
{
MDirPtr dir;
int n;

verbmsg ("Reading flat names");
NumFTexture = 0;

for (dir = MasterDir; (dir = FindMasterDir (dir, "F_START", "FF_START"));)
   {
   bool ff_start = ! y_strnicmp (dir->dir.name, "FF_START", WAD_NAME);
   MDirPtr dir0;
   /* count the names */
   dir = dir->next;
   dir0 = dir;
   for (n = 0; dir && y_strnicmp (dir->dir.name, "F_END", WAD_NAME)
	 && (! ff_start || y_strnicmp (dir->dir.name, "FF_END", WAD_NAME));
	 dir = dir->next)
      {
      if (dir->dir.start == 0 || dir->dir.size == 0)
	 {
	 if (! (toupper (dir->dir.name[0]) == 'F'
	     && (dir->dir.name[1] == '1'
	      || dir->dir.name[1] == '2'
	      || dir->dir.name[1] == '3'
	      || toupper (dir->dir.name[1]) == 'F')
	     && dir->dir.name[2] == '_'
	     && (! y_strnicmp (dir->dir.name + 3, "START", WAD_NAME - 3)
		 || ! y_strnicmp (dir->dir.name + 3, "END", WAD_NAME - 3))))
	    warn ("unexpected label \"%.*s\" among flats.\n",
		  WAD_NAME, dir->dir.name);
	 continue;
	 }
      if (dir->dir.size != 4096)
	 warn ("flat \"%.*s\" has weird size %lu."
	     " Using 4096 instead.\n",
	       WAD_NAME, dir->dir.name, (unsigned long) dir->dir.size);
      n++;
      }
   /* If FF_START/FF_END followed by F_END (mm2.wad), advance
      past F_END. In fact, this does not work because the F_END
      that follows has been snatched by OpenPatchWad(), that
      thinks it replaces the F_END from the iwad. OpenPatchWad()
      needs to be kludged to take this special case into
      account. Fortunately, the only consequence is a useless
      "this wad uses FF_END" warning. -- AYM 1999-07-10 */
   if (ff_start && dir && ! y_strnicmp (dir->dir.name, "FF_END", WAD_NAME))
      if (dir->next && ! y_strnicmp (dir->next->dir.name, "F_END", WAD_NAME))
         dir = dir->next;

   verbmsg (" FF_START/%s %d", dir->dir.name, n);
   if (dir && ! y_strnicmp (dir->dir.name, "FF_END", WAD_NAME))
      warn ("this wad uses FF_END. That won't work with Doom."
	 " Use F_END instead.\n");
   /* get the actual names from master dir. */
   flat_list = (flat_list_entry_t *) ResizeMemory (flat_list,
      (NumFTexture + n) * sizeof *flat_list);
   for (size_t m = NumFTexture; m < NumFTexture + n; dir0 = dir0->next)
      {
      // Skip all labels.
      if (dir0->dir.start == 0
	 || dir0->dir.size == 0
	 || (toupper (dir0->dir.name[0]) == 'F'
	     && (dir0->dir.name[1] == '1'
	      || dir0->dir.name[1] == '2'
	      || dir0->dir.name[1] == '3'
	      || toupper (dir0->dir.name[1]) == 'F')
	     && dir0->dir.name[2] == '_'
	     && (! y_strnicmp (dir0->dir.name + 3, "START", WAD_NAME - 3)
		 || ! y_strnicmp (dir0->dir.name + 3, "END", WAD_NAME - 3))
	     ))
	 continue;
      *flat_list[m].name = '\0';
      strncat (flat_list[m].name, dir0->dir.name, sizeof flat_list[m].name - 1);
      flat_list[m].wadfile = dir0->wadfile;
      flat_list[m].offset  = dir0->dir.start;
      m++;
      }
   NumFTexture += n;
   }

verbmsg ("\n");

/* sort the flats by names */
qsort (flat_list, NumFTexture, sizeof *flat_list, flat_list_entry_cmp);

/* AYM 19970817: eliminate all but the last duplicates of a flat */
for (size_t n = 0; n < NumFTexture; n++)
   {
   if (n + 1 < NumFTexture
       && ! flat_list_entry_cmp (flat_list + n, flat_list + n + 1))
      {
      size_t m;
      for (m = n; m + 1 < NumFTexture; m++)
	 flat_list[m] = flat_list[m + 1];

      // Not strictly necessary but could help catching bugs
      strcpy (flat_list[m].name, "deleted");
      flat_list[m].wadfile = NULL;
      flat_list[m].offset  = 0;
      NumFTexture--;       /* I don't bother resizing flat_list */
      }
   }
}


/*
 *	is_flat_name_in_list
 *	FIXME should use bsearch()
 */
int is_flat_name_in_list (const char *name)
{
if (! flat_list)
  return 0;
for (size_t n = 0; n < NumFTexture; n++)
  if (! y_strnicmp (name, flat_list[n].name, WAD_FLAT_NAME))
    return 1;
return 0;
}


/*
   forget the flat names
*/

void ForgetFTextureNames ()
{
NumFTexture = 0;
FreeMemory (flat_list);
flat_list = 0;
}


