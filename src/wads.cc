/*
 *	wads.cc
 *	Wad file routines
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
#include "wads.h"


/* global variables */
WadPtr WadFileList = NULL;       /* linked list of wad files */
MDirPtr MasterDir = NULL;        /* the master directory */


/*
 *	wad_seek
 *	Go to position <offset> of wad file.
 *	If a seek error occurs, calls fatal_error().
 */
void wad_seek (WadPtr wadfile, long offset)
{
if (fseek (wadfile->fd, offset, 0))
   fatal_error ("%s: can't seek to %lXh", wadfile->filename, offset);
}



/*
 *	wad_read_bytes
 *	Read <count> bytes from wad file <wadfile>
 *	and store them into buffer <buf>.
 *	<count> is _not_ limited to size_t.
 *	If a read error occurs, calls fatal_error().
 */
void wad_read_bytes (WadPtr wadfile, void huge *buf, long count)
{
long ofs = ftell (wadfile->fd);
long bytes_read;

bytes_read = wad_read_bytes2 (wadfile, buf, count);
if (bytes_read != count)
   fatal_error ("%s(%lXh): tried to read %ld B, got only %ld",
      wadfile->filename, ofs, count, bytes_read);
}


/*
 *	wad_read_bytes2
 *	Read <count> bytes from wad file <wadfile>
 *	and store them into buffer <buf>.
 *	<count> is _not_ limited to size_t.
 *	Unlike wad_read_bytes2(), keeps quiet even if a read error occurs.
 *	Returns the number of bytes read.
 */
long wad_read_bytes2 (WadPtr wadfile, void *buf, long count)
{
long bytes_read_total;
size_t bytes_read;
size_t bytes_to_read;

bytes_read_total = 0;
bytes_to_read    = 0x8000;
while (count > 0)
   {
   if (count <= 0x8000)
      bytes_to_read = (size_t) count;
   bytes_read = fread (buf, 1, bytes_to_read, wadfile->fd);
   bytes_read_total += bytes_read;
   if (bytes_read != bytes_to_read)
      break;
   buf = (char huge *) buf + bytes_read;
   count -= bytes_read;
   }
return bytes_read_total;
}


/*
 *	wad_read_i16
 *	Read a little-endian 16-bit signed integer
 *	from wad file <wadfile>.
 *	If a read error occurs, calls fatal_error().
 */
void wad_read_i16 (WadPtr wadfile, i16 *buf)
{
*buf = getc (wadfile->fd) | (getc (wadfile->fd) << 8);

if (feof (wadfile->fd) || ferror (wadfile->fd))
   fatal_error ("%s(%lXh): read error or unexpected EOF",
      wadfile->filename, ftell (wadfile->fd));
}


/*
 *	wad_read_i32
 *	Read <count> little-endian 32-bit signed integers
 *	from wad file <wadfile>.
 *	If a read error occurs, calls fatal_error().
 */
void wad_read_i32 (WadPtr wadfile, i32 *buf, long count = 1)
{
while (count-- > 0)
   {
   *buf++ =    getc (wadfile->fd)
      | (      getc (wadfile->fd) << 8)
      | ((i32) getc (wadfile->fd) << 16)
      | ((i32) getc (wadfile->fd) << 24);
   }

if (feof (wadfile->fd) || ferror (wadfile->fd))
   fatal_error ("%s(%lXh): read error or unexpected EOF",
      wadfile->filename, ftell (wadfile->fd));
}


/*
 *	file_write_i16
 *	Write a little-endian 16-bit signed integer
 *	to file <fd>.
 *	Does no error checking.
 */
void file_write_i16 (FILE *fd, i16 buf)
{
putc (       buf & 0xff, fd);
putc ((buf >> 8) & 0xff, fd);
}


/*
 *	file_write_i32
 *	Write <count> little-endian 32-bit signed integers
 *	to file <fd>.
 *	Does no error checking.
 */
void file_write_i32 (FILE *fd, i32 buf, long count = 1)
{
// It would probably be more efficient
// to swap bytes in-core and write
// the whole i32 at once.
while (count-- > 0)
   {
   putc (        buf & 0xff, fd);
   putc ((buf >>  8) & 0xff, fd);
   putc ((buf >> 16) & 0xff, fd);
   putc ((buf >> 24) & 0xff, fd);
   }
}


/*
   find an entry in the master directory
*/

MDirPtr FindMasterDir (MDirPtr from, const char *name)
{
while (from)
   {
   if (!strncmp (from->dir.name, name, 8))
      break;
   from = from->next;
   }
return from;
}


/*
   output bytes to a binary file with error checking
*/

void WriteBytes (FILE *file, const void huge *buf, long size)
{
if (! Registered)
   return;
while (size > 0x8000)
   {
   if (fwrite (buf, 1, 0x8000, file) != 0x8000)
      fatal_error ("error writing to file");
   buf = (const char huge *) buf + 0x8000;
   size -= 0x8000;
   }
if (fwrite (buf, 1, size, file) != (size_t) size)
   fatal_error ("error writing to file");
}



/*
   copy bytes from a binary file to another with error checking
*/

void CopyBytes (FILE *dest, FILE *source, long size)
{
void huge *data;

if (! Registered)
   return;
data = GetFarMemory (0x8000 + 2);
while (size > 0x8000)
   {
   if (fread (data, 1, 0x8000, source) != 0x8000)
      fatal_error ("error reading from file");
   if (fwrite (data, 1, 0x8000, dest) != 0x8000)
      fatal_error ("error writing to file");
   size -= 0x8000;
   }
if (fread (data, 1, size, source) != (size_t) size)
   fatal_error ("error reading from file");
if (fwrite (data, 1, size, dest) != (size_t) size)
   fatal_error ("error writing to file");
FreeFarMemory (data);
}



/*
   check if a file exists and is readable
*/

Bool Exists (const char *filename)
{
FILE *test;

if ((test = fopen (filename, "rb")) == NULL)
   return 0;
fclose (test);
return 1;
}


/*
 *	entryname_cmp
 *	Compare two directory entries.
 *	This is basically an unrolled strnicmp (entry1, entry2, 8).
 */
int entryname_cmp (const char *entry1, const char *entry2)
{
if (entry1[0] != entry2[0]) return entry1[0] - entry2[0];
if (entry1[0] == '\0')      return 0;
if (entry1[1] != entry2[1]) return entry1[1] - entry2[1];
if (entry1[1] == '\0')      return 0;
if (entry1[2] != entry2[2]) return entry1[2] - entry2[2];
if (entry1[2] == '\0')      return 0;
if (entry1[3] != entry2[3]) return entry1[3] - entry2[3];
if (entry1[3] == '\0')      return 0;
if (entry1[4] != entry2[4]) return entry1[4] - entry2[4];
if (entry1[4] == '\0')      return 0;
if (entry1[5] != entry2[5]) return entry1[5] - entry2[5];
if (entry1[5] == '\0')      return 0;
if (entry1[6] != entry2[6]) return entry1[6] - entry2[6];
if (entry1[6] == '\0')      return 0;
if (entry1[7] != entry2[7]) return entry1[7] - entry2[7];
return 0;
}


/* end of file */
