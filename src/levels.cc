/*
 *	levels.cc
 *	Level loading and saving routines,
 *	global variables used to hold the level data.
 *	BW & RQ sometime in 1993 or 1994.
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
#include "dialog.h"
#include "wstructs.h"
#include "things.h"
#include "wads.h"


/* the global data */
MDirPtr Level = 0;		/* master dictionary entry for the level */
int NumThings = 0;		/* number of things */
TPtr Things;			/* things data */
int NumLineDefs = 0;		/* number of line defs */
LDPtr LineDefs;			/* line defs data */
int NumSideDefs = 0;		/* number of side defs */
SDPtr SideDefs;			/* side defs data */
int NumVertices = 0;		/* number of vertexes */
VPtr Vertices;			/* vertex data */
int NumSectors = 0;		/* number of sectors */
SPtr Sectors;			/* sectors data */
int NumSegs = 0;		/* number of segments */
int NumWTexture = 0;		/* number of wall textures */
char **WTexture;		/* array of wall texture names */
int NumFTexture = 0;		/* number of floor/ceiling textures */
char **FTexture;		/* array of texture names */
int MapMaxX = -32767;		/* maximum X value of map */
int MapMaxY = -32767;		/* maximum Y value of map */
int MapMinX = 32767;		/* minimum X value of map */
int MapMinY = 32767;		/* minimum Y value of map */
Bool MadeChanges = 0;	/* made changes? */
Bool MadeMapChanges = 0;	/* made changes that need rebuilding? */


/*
   read in the level data
*/

void ReadLevelData (const char *levelname) /* SWAP! */
{
MDirPtr dir;
int n, m;
int OldNumVertices;
int *VertexUsed;

/* No objects are needed: they may be swapped after they have been read */
ObjectsNeeded (0);

/* Find the various level information from the master directory */
DisplayMessage (-1, -1, "Reading data for level %s...", levelname);
Level = FindMasterDir (MasterDir, levelname);
if (!Level)
   fatal_error ("level data not found");

/* Get the number of Vertices */
dir = FindMasterDir (Level, "VERTEXES");
if (dir)
   OldNumVertices = (int) (dir->dir.size / 4L);
else
   OldNumVertices = 0;
if (OldNumVertices > 0)
   {
   VertexUsed = (int *) GetMemory (OldNumVertices * sizeof (int));
   for (n = 0; n < OldNumVertices; n++)
      VertexUsed[n] = 0;
   }

/* Read in the Things data */
verbmsg ("Reading %s things", levelname);
dir = FindMasterDir (Level, "THINGS");
if (dir)
   NumThings = (int) (dir->dir.size / 10L);
else
   NumThings = 0;
if (NumThings > 0)
   {
   Things = (TPtr) GetFarMemory ((unsigned long) NumThings
      * sizeof (struct Thing));
   wad_seek (dir->wadfile, dir->dir.start);
   for (n = 0; n < NumThings; n++)
      {
      wad_read_i16 (dir->wadfile, &(Things[n].xpos ));
      wad_read_i16 (dir->wadfile, &(Things[n].ypos ));
      wad_read_i16 (dir->wadfile, &(Things[n].angle));
      wad_read_i16 (dir->wadfile, &(Things[n].type ));
      wad_read_i16 (dir->wadfile, &(Things[n].when ));
      }
   }

/* Read in the LineDef information */
verbmsg (" linedefs");
dir = FindMasterDir (Level, "LINEDEFS");
if (dir)
   NumLineDefs = (int) (dir->dir.size / 14L);
else
   NumLineDefs = 0;
if (NumLineDefs > 0)
   {
   LineDefs = (LDPtr) GetFarMemory ((unsigned long) NumLineDefs
      * sizeof (struct LineDef));
   wad_seek (dir->wadfile, dir->dir.start);
   for (n = 0; n < NumLineDefs; n++)
      {
      wad_read_i16 (dir->wadfile, &(LineDefs[n].start   ));
      VertexUsed[LineDefs[n].start] = 1;
      wad_read_i16 (dir->wadfile, &(LineDefs[n].end     ));
      VertexUsed[LineDefs[n].end] = 1;
      wad_read_i16 (dir->wadfile, &(LineDefs[n].flags   ));
      wad_read_i16 (dir->wadfile, &(LineDefs[n].type    ));
      wad_read_i16 (dir->wadfile, &(LineDefs[n].tag     ));
      wad_read_i16 (dir->wadfile, &(LineDefs[n].sidedef1));
      wad_read_i16 (dir->wadfile, &(LineDefs[n].sidedef2));
      }
   }

/* Read in the SideDef information */
verbmsg (" sidedefs");
dir = FindMasterDir (Level, "SIDEDEFS");
if (dir)
   NumSideDefs = (int) (dir->dir.size / 30L);
else
   NumSideDefs = 0;
if (NumSideDefs > 0)
   {
   SideDefs = (SDPtr) GetFarMemory ((unsigned long) NumSideDefs
      * sizeof (struct SideDef));
   wad_seek (dir->wadfile, dir->dir.start);
   for (n = 0; n < NumSideDefs; n++)
      {
      wad_read_i16 (dir->wadfile, &(SideDefs[n].xoff));
      wad_read_i16 (dir->wadfile, &(SideDefs[n].yoff));
      wad_read_bytes (dir->wadfile, &(SideDefs[n].tex1), 8);
      wad_read_bytes (dir->wadfile, &(SideDefs[n].tex2), 8);
      wad_read_bytes (dir->wadfile, &(SideDefs[n].tex3), 8);
      wad_read_i16 (dir->wadfile, &(SideDefs[n].sector));
      }
   }

/* Read in the Vertices which are all the corners of the level,
   but ignore the Vertices not used in any LineDef (they usually
   are at the end of the list). */
verbmsg (" vertices");
NumVertices = 0;
for (n = 0; n < OldNumVertices; n++)
   if (VertexUsed[n])
      NumVertices++;
if (NumVertices > 0)
   {
   Vertices = (VPtr) GetFarMemory ((unsigned long) NumVertices
      * sizeof (struct Vertex));
   dir = FindMasterDir (Level, "VERTEXES");
   wad_seek (dir->wadfile, dir->dir.start);
   MapMaxX = -32767;
   MapMaxY = -32767;
   MapMinX = 32767;
   MapMinY = 32767;
   m = 0;
   for (n = 0; n < OldNumVertices; n++)
      {
      i16 val;
      wad_read_i16 (dir->wadfile, &val);
      if (VertexUsed[n])
         {
	 if (val < MapMinX)
	    MapMinX = val;
	 if (val > MapMaxX)
	    MapMaxX = val;
	 Vertices[m].x = val;
         }
      wad_read_i16 (dir->wadfile, &val);
      if (VertexUsed[n])
         {
	 if (val < MapMinY)
	    MapMinY = val;
	 if (val > MapMaxY)
	    MapMaxY = val;
	 Vertices[m].y = val;
	 m++;
         }
      }
   if (m != NumVertices)
      fatal_error ("inconsistency in the vertices data\n");
   }

if (OldNumVertices > 0)
   {
   /* Update the Vertex numbers in the LineDefs
      (not really necessary, but...) */
   m = 0;
   for (n = 0; n < OldNumVertices; n++)
      if (VertexUsed[n])
	 VertexUsed[n] = m++;
   ObjectsNeeded (OBJ_LINEDEFS, 0);
   for (n = 0; n < NumLineDefs; n++)
      {
      LineDefs[n].start = VertexUsed[LineDefs[n].start];
      LineDefs[n].end = VertexUsed[LineDefs[n].end];
      }
   ObjectsNeeded (0);
   FreeMemory (VertexUsed);
   }

/* Ignore the Segs, SSectors and Nodes */

/* Read in the Sectors information */
verbmsg (" sectors\n");
dir = FindMasterDir (Level, "SECTORS");
if (dir)
   NumSectors = (int) (dir->dir.size / 26L);
else
   NumSectors = 0;
if (NumSectors > 0)
   {
   Sectors = (SPtr) GetFarMemory ((unsigned long) NumSectors
      * sizeof (struct Sector));
   wad_seek (dir->wadfile, dir->dir.start);
   for (n = 0; n < NumSectors; n++)
      {
      wad_read_i16 (dir->wadfile, &(Sectors[n].floorh ));
      wad_read_i16 (dir->wadfile, &(Sectors[n].ceilh  ));
      wad_read_bytes (dir->wadfile, &(Sectors[n].floort ), 8);
      wad_read_bytes (dir->wadfile, &(Sectors[n].ceilt  ), 8);
      wad_read_i16 (dir->wadfile, &(Sectors[n].light  ));
      wad_read_i16 (dir->wadfile, &(Sectors[n].special));
      wad_read_i16 (dir->wadfile, &(Sectors[n].tag    ));
      }
   }

/* Ignore the last entries (Reject & BlockMap) */

/* Silly statistics */
verbmsg ("  %d things, %d vertices, %d linedefs, %d sidedefs, %d sectors\n",
   (int) NumThings, (int) NumVertices, (int) NumLineDefs,
   (int) NumSideDefs, (int) NumSectors);
verbmsg ("  Map: %d,%d-%d,%d\n", MapMinX, MapMinY, MapMaxX, MapMaxY);
}



/*
   forget the level data
*/

void ForgetLevelData () /* SWAP! */
{
/* forget the Things */
ObjectsNeeded (OBJ_THINGS, 0);
NumThings = 0;
if (Things)
   FreeFarMemory (Things);
Things = 0;

/* forget the Vertices */
ObjectsNeeded (OBJ_VERTICES, 0);
NumVertices = 0;
if (Vertices)
   FreeFarMemory (Vertices);
Vertices = 0;

/* forget the LineDefs */
ObjectsNeeded (OBJ_LINEDEFS, 0);
NumLineDefs = 0;
if (LineDefs)
   FreeFarMemory (LineDefs);
LineDefs = 0;

/* forget the SideDefs */
ObjectsNeeded (OBJ_SIDEDEFS, 0);
NumSideDefs = 0;
if (SideDefs)
   FreeFarMemory (SideDefs);
SideDefs = 0;

/* forget the Sectors */
ObjectsNeeded (OBJ_SECTORS, 0);
NumSectors = 0;
if (Sectors)
   FreeFarMemory (Sectors);
Sectors = 0;
ObjectsNeeded (0);
}


/*
   save the level data to a pwad file
*/

void SaveLevelData (const char *outfile) /* SWAP! */
{
FILE   *file;
MDirPtr dir;
i32     counter;
int     n;
i32     size;
i32     dirstart;
i32     blocksize;
i32     rejectsize;

DisplayMessage (-1, -1, "Saving data to \"%s\"...", outfile);
LogMessage (": Saving data to \"%s\"...\n", outfile);
/* open the file */
if ((file = fopen (outfile, "wb")) == NULL)
   fatal_error ("Unable to open file \"%s\"", outfile);
WriteBytes (file, "PWAD", 4);   /* Pwad file */
file_write_i32 (file, 11);   /* 11 entries */
file_write_i32 (file, 0);    /* fix this up later */
counter = 12;
dir = Level->next;

/* Output the THINGS data */
ObjectsNeeded (OBJ_THINGS, 0);
for (n = 0; n < NumThings; n++)
   {
   file_write_i16 (file, Things[n].xpos );
   file_write_i16 (file, Things[n].ypos );
   file_write_i16 (file, Things[n].angle);
   file_write_i16 (file, Things[n].type );
   file_write_i16 (file, Things[n].when );
   counter += 10;
   }
dir = dir->next;

/* Update MapMinX, MapMinY, MapMaxX, MapMaxY */
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

/* Output the LINEDEFS */
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
   counter += 14;
   }
dir = dir->next;

/* Output the SIDEDEFS */
ObjectsNeeded (OBJ_SIDEDEFS, 0);
for (n = 0; n < NumSideDefs; n++)
   {
   file_write_i16 (file, SideDefs[n].xoff);
   file_write_i16 (file, SideDefs[n].yoff);
   WriteBytes (file, &(SideDefs[n].tex1), 8L);
   WriteBytes (file, &(SideDefs[n].tex2), 8L);
   WriteBytes (file, &(SideDefs[n].tex3), 8L);
   file_write_i16 (file, SideDefs[n].sector);
   counter += 30;
   }
dir = dir->next;

if (MadeMapChanges)
   {
   /* Output the VERTEXES */
   ObjectsNeeded (OBJ_VERTICES, 0);
   for (n = 0; n < NumVertices; n++)
      {
      file_write_i16 (file, Vertices[n].x);
      file_write_i16 (file, Vertices[n].y);
      counter += 4;
      }
   }
else
   {
   /* Copy the VERTEXES */
   ObjectsNeeded (0);
   size = dir->dir.size;
   counter += size;
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, size);
   }
dir = dir->next;

/* SEGS, SSECTORS and NODES */
for (n = 0; n < 3; n++)
   {
   if (! MadeMapChanges)
      {
      size = dir->dir.size;
      counter += size;
      wad_seek (dir->wadfile, dir->dir.start);
      CopyBytes (file, dir->wadfile->fd, size);
      }
   dir = dir->next;
   }

/* Output the SECTORS */
ObjectsNeeded (OBJ_SECTORS, 0);
for (n = 0; n < NumSectors; n++)
   {
   file_write_i16 (file, Sectors[n].floorh);
   file_write_i16 (file, Sectors[n].ceilh );
   WriteBytes (file, &(Sectors[n].floort), 8);
   WriteBytes (file, &(Sectors[n].ceilt),  8);
   file_write_i16 (file, Sectors[n].light  );
   file_write_i16 (file, Sectors[n].special);
   file_write_i16 (file, Sectors[n].tag    );
   counter += 26;
   }
dir = dir->next;

/* REJECT */
if (! MadeMapChanges)
   {
   /* Copy the REJECT data */
   ObjectsNeeded (0);
   rejectsize = dir->dir.size;
   size = rejectsize;
   counter += size;
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, size);
   }
dir = dir->next;

/* Copy the BLOCKMAP data */
if (! MadeMapChanges)
   {
   ObjectsNeeded (0);
   blocksize = dir->dir.size;
   size = blocksize;
   counter += size;
   wad_seek (dir->wadfile, dir->dir.start);
   CopyBytes (file, dir->wadfile->fd, size);
   }
dir = dir->next;


/* output the actual directory */
dirstart = counter;
counter = 12;
size = 0;
dir = Level;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, &(dir->dir.name), 8);
dir = dir->next;

size = (i32) NumThings * 10;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "THINGS\0\0", 8);
counter += size;
dir = dir->next;

size = (i32) NumLineDefs * 14;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "LINEDEFS", 8);
counter += size;
dir = dir->next;

size = (i32) NumSideDefs * 30;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "SIDEDEFS", 8);
counter += size;
dir = dir->next;

if (MadeMapChanges)
   size = (i32) NumVertices * 4;
else
   size = dir->dir.size;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "VERTEXES", 8);
counter += size;
dir = dir->next;

if (MadeMapChanges)
  size = 0;
else
  size = dir->dir.size;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "SEGS\0\0\0\0", 8);
counter += size;
dir = dir->next;

if (MadeMapChanges)
  size = 0;
else
  size = dir->dir.size;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "SSECTORS", 8);
counter += size;
dir = dir->next;

if (MadeMapChanges)
  size = 0;
else
  size = dir->dir.size;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "NODES\0\0\0", 8);
counter += size;
dir = dir->next;

size = (i32) NumSectors * 26;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "SECTORS\0", 8);
counter += size;
dir = dir->next;

if (MadeMapChanges)
  size = 0;
else
  size = rejectsize;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "REJECT\0\0", 8);
counter += size;
dir = dir->next;

if (MadeMapChanges)
  size = 0;
else
  size = blocksize;
file_write_i32 (file, counter);
file_write_i32 (file, size);
WriteBytes (file, "BLOCKMAP", 8);
counter += size;
dir = dir->next;

/* Fix up the directory start information */
if (fseek (file, 8, SEEK_SET))
   fatal_error ("error writing to file");
file_write_i32 (file, dirstart);

/* Close the file */
fclose (file);

/* The file is now up to date */
if (MadeMapChanges)
   remind_to_build_nodes = 1;
MadeChanges = 0;
MadeMapChanges = 0;
ObjectsNeeded (0);

/* Update pointers in Master Directory */
OpenPatchWad (outfile);

/* This should free the old "*.bak" file */
CloseUnusedWadFiles ();
}



/*
   function used by qsort to sort the texture names
*/
int SortTextures (const void *a, const void *b)
{
return strcmp (*((const char *const *)a), *((const char *const *)b));
}


/*
   read in the wall texture names
*/
void ReadWTextureNames ()
{
MDirPtr dir;
i32 *offsets;
int n;
i32 val;

verbmsg ("Reading texture names\n");
dir = FindMasterDir (MasterDir, "TEXTURE1");
wad_seek (dir->wadfile, dir->dir.start);
wad_read_i32 (dir->wadfile, &val);
NumWTexture = (int) val + 1;
/* read in the offsets for texture1 names */
offsets = (i32 *) GetMemory ((long) NumWTexture * 4);
wad_read_i32 (dir->wadfile, offsets + 1, NumWTexture - 1);
/* read in the actual names */
WTexture = (char **) GetMemory ((long) NumWTexture * sizeof (char *));
WTexture[0] = (char *) GetMemory (9);
strcpy (WTexture[0], "-");
for (n = 1; n < NumWTexture; n++)
   {
   WTexture[n] = (char *) GetMemory (9);
   wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
   wad_read_bytes (dir->wadfile, WTexture[n], 8);
   WTexture[n][8] = '\0';
   }
FreeMemory (offsets);
if (Registered)
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
	 WTexture[NumWTexture + n] = (char *) GetMemory (9);
	 wad_seek (dir->wadfile, dir->dir.start + offsets[n]);
	 wad_read_bytes (dir->wadfile, WTexture[NumWTexture + n], 8);
	 WTexture[NumWTexture + n][8] = '\0';
	 }
      NumWTexture += val;
      FreeMemory (offsets);
      }
   }
/* sort the names */
qsort (WTexture, NumWTexture, sizeof (char *), SortTextures);
}



/*
   forget the wall texture names
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
   read in the floor/ceiling texture names
*/

void ReadFTextureNames ()
{
MDirPtr dir;
int n, m;

verbmsg ("Reading flat names\n");
/* count the names */
dir = FindMasterDir (MasterDir, "F1_START");
dir = dir->next;
for (n = 0; dir && strcmp (dir->dir.name, "F1_END"); n++)
   dir = dir->next;
NumFTexture = n;
/* get the actual names from master dir. */
dir = FindMasterDir (MasterDir, "F1_START");
dir = dir->next;
FTexture = (char **) GetMemory (NumFTexture * sizeof (char *));
for (n = 0; n < NumFTexture; n++)
   {
   FTexture[n] = (char *) GetMemory (9);
   strncpy (FTexture[n], dir->dir.name, 8);
   FTexture[n][8] = '\0';
   dir = dir->next;
   }

if (Registered)
   {
   /* count the names */
   dir = FindMasterDir (MasterDir, "F2_START");
   dir = dir->next;
   for (n = 0; dir && strcmp (dir->dir.name, "F2_END"); n++)
      dir = dir->next;
   /* get the actual names from master dir. */
   dir = FindMasterDir (MasterDir, "F2_START");
   dir = dir->next;
   FTexture = (char **) ResizeMemory (FTexture,
      (NumFTexture + n) * sizeof (char *));
   for (m = NumFTexture; m < NumFTexture + n; m++)
      {
      FTexture[m] = (char *) GetMemory (9);
      strncpy (FTexture[m], dir->dir.name, 8);
      FTexture[m][8] = '\0';
      dir = dir->next;
      }
   NumFTexture += n;
   }

/* AYM 19971130 added reading of F3_START if present (Doom II) */
   {
   /* count the names */
   dir = FindMasterDir (MasterDir, "F3_START");
   if (dir)
      {
      dir = dir->next;
      for (n = 0; dir && strcmp (dir->dir.name, "F3_END"); n++)
	 dir = dir->next;
      /* get the actual names from master dir. */
      dir = FindMasterDir (MasterDir, "F3_START");
      dir = dir->next;
      FTexture = (char **) ResizeMemory (FTexture,
	 (NumFTexture + n) * sizeof (char *));
      for (m = NumFTexture; m < NumFTexture + n; m++)
	 {
	 FTexture[m] = (char *) GetMemory (9);
	 strncpy (FTexture[m], dir->dir.name, 8);
	 FTexture[m][8] = '\0';
	 dir = dir->next;
	 }
      NumFTexture += n;
      }
   }

/* AYM 19970815: reads new floor/ceiling textures between all
   (FF_START, F_END) pairs */
for (dir = MasterDir; (dir = FindMasterDir (dir, "FF_START")) != NULL;)
   {
   MDirPtr dir0;
   /* count the names */
   dir = dir->next;
   dir0 = dir;
   for (n = 0; dir && strcmp (dir->dir.name, "F_END"); n++)
      dir = dir->next;
   /* get the actual names from master dir. */
   FTexture = (char **) ResizeMemory (FTexture,
      (NumFTexture + n) * sizeof (char *));
   for (m = NumFTexture; m < NumFTexture + n; m++)
      {
      FTexture[m] = (char *) GetMemory (9);
      strncpy (FTexture[m], dir0->dir.name, 8);
      FTexture[m][8] = '\0';
      dir0 = dir0->next;
      }
   NumFTexture += n;
   }

/* sort the names */
qsort (FTexture, NumFTexture, sizeof (char *), SortTextures);

/* AYM 19970817: eliminate all but the last duplicates of a texture */
for (n = 0; n < NumFTexture; n++)
   {
   if (n+1 < NumFTexture && ! strcmp (FTexture[n], FTexture[n+1]))
      {
      FreeMemory (FTexture[n]);
      for (m = n; m+1 < NumFTexture; m++)
	 FTexture[m] = FTexture[m+1];
      FTexture[m] = NULL;  /* useless but cleaner */
      NumFTexture--;       /* I don't bother resizing FTexture */
      }
   }
}



/*
   forget the floor/ceiling texture names
*/

void ForgetFTextureNames ()
{
int n;

/* forget all names */
for (n = 0; n < NumFTexture; n++)
   FreeMemory (FTexture[n]);

/* forget the array */
NumFTexture = 0;
FreeMemory (FTexture);
}



/* end of file */
