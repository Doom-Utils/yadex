/*
 *	wads.cc
 *	Wad file routines
 *	BW & RQ sometime in 1993 or 1994.
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1999 André Majorel.

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
 *	wad_seek2
 *	Like wad_seek() but simply returns non-zero on error.
 */
int wad_seek2 (WadPtr wadfile, long offset)
{
if (fseek (wadfile->fd, offset, 0))
   return 1;
return 0;
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
 *	If a read error occurs, sets wadfile->error.
 */
i16 wad_read_i16 (WadPtr wadfile)
{
const size_t nbytes = 2;
u8 buf[nbytes];
if (fread (buf, 1, nbytes, wadfile->fd) != nbytes)
   {
   if (! wadfile->error)
      {
      //wadfile->err_ofs = ftell (wadfile->fd);
      wadfile->error = true;
      }
   return EOF;  // Whatever
   }
return buf[0] | buf[1] << 8;
}


/*
 *	wad_read_i16
 *	Same thing as above but the value read is not
 *	returned but stored in *buf.
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
void wad_read_i32 (WadPtr wadfile, i32 *buf, long count)
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
void file_write_i32 (FILE *fd, i32 buf, long count)
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
 *	file_write_name
 *	Write to file <fd> the directory entry name contained in
 *	<name>. The string written in the file is exactly the
 *	same as the string contained in <name> except that :
 *
 *	- only the first WAD_NAME characters of <name> are
 *	  used, or up to the first occurrence of a NUL,
 *	  
 *	- all letters are forced to upper case,
 *
 *	- if necessary, the string is padded to WAD_NAME
 *	  characters with NULs.
 *
 *	Does no error checking.
 */
void file_write_name (FILE *fd, const char *name)
{
const unsigned char *const p0 = (const unsigned char *) name;
const unsigned char *p = p0;  // "unsigned" for toupper()'s sake
for (; p - p0 < (ptrdiff_t) WAD_NAME && *p; p++)
   putc (toupper (*p), fd);
for (; p - p0 < (ptrdiff_t) WAD_NAME; p++)
   putc ('\0', fd);
}


/*
   find an entry in the master directory
*/

MDirPtr FindMasterDir (MDirPtr from, const char *name)
{
while (from)
   {
   if (! y_strnicmp (from->dir.name, name, WAD_NAME))
      break;
   from = from->next;
   }
return from;
}


/*
 *	Find an entry in the master directory
 */
MDirPtr FindMasterDir (MDirPtr from, const char *name1, const char *name2)
{
while (from)
   {
   if (! y_strnicmp (from->dir.name, name1, WAD_NAME)
	 || ! y_strnicmp (from->dir.name, name2, WAD_NAME))
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
   FIXME it's silly to allocate such a large buffer on memory
   constrained systems. The function should be able to fall back
   on a smaller buffer.
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



/* end of file */
