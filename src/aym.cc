/*
 *	aym.cc
 *	Misc. functions
 *	AYM 1997-??-??
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


/*
 *	levelname2levelno
 *	Used to know if directory entry is ExMy or MAPxy
 *	For "ExMy",  returns 10*x+y.
 *	For "MAPxy", returns 100+10*x+y.
 *	For other names, returns 0.
 */
int levelname2levelno (const char *name)
{
if (toupper (name[0]) == 'E'
 && isdigit (name[1])
 && name[1] != '0'
 && toupper (name[2]) == 'M'
 && isdigit (name[3])
 && name[3] != '0'
 && name[4] == '\0')
   return 10*(name[1]-'0') + name[3]-'0';
if (toupper (name[0]) == 'M'
 && toupper (name[1]) == 'A'
 && toupper (name[2]) == 'P'
 && isdigit (name[3])
 && isdigit (name[4])
 && name[5] == '\0')
   return 100 + 10*(name[3]-'0') + name[4]-'0';
return 0;
}


/*
 *	spec_path
 *	Extract the path of a spec
 */
const char *spec_path (const char *spec)
{
static char path[PATH_MAX+1];
size_t n;

*path = '\0';
strncat (path, spec, PATH_MAX);
for (n = strlen (path); n > 0 && ! al_fisps (path[n-1]); n--)
   ;
path[n] = '\0';
return path;
}


/*
 *	fncmp
 *	Compare two filenames
 *	For Unix, it's a simple strcmp.
 *	For DOS, it's case insensitive and "/" and "\" are equivalent.
 *	FIXME: should canonicalize both names and compare that.
 */
int fncmp (const char *name1, const char *name2)
{
#if defined Y_DOS
char c1, c2;
for (;;)
   {
   c1 = tolower ((unsigned char) *name1++);
   c2 = tolower ((unsigned char) *name2++);
   if (c1=='\\')
      c1 = '/';
   if (c2=='\\')
      c2 = '/';
   if (c1 != c2)
      return c1-c2;
   if (!c1)
      return 0;
   }
#elif defined Y_UNIX
return strcmp (name1, name2);
#endif
}


/*
 *	is_absolute
 *	Tell whether a file name is absolute or relative.
 *
 *	Note: for DOS, a filename of the form "d:foo" is
 *	considered absolute, even though it's technically
 *	relative to the	current working directory of "d:".
 *	My reasoning is that someone who wants to specify a
 *	name that's relative to one of the standard
 *	directories is not going to put a "d:" in front of it.
 */
int is_absolute (const char *filename)
{
#if defined Y_UNIX
return *filename == '/';
#elif defined Y_DOS
return *filename == '/'
   || *filename == '\\'
   || isalpha (*filename) && filename[1] == ':';
#endif
}


/*
 *	hextoi
 *	Convert [0-9a-fA-F] char to integer.
 *	Returns -1 if char is not a hexadecimal digit.
 */
int hextoi (char c)
{
if (isdigit (c))
   return c - '0';
if (isalpha (c))
   return toupper (c) - 'A' + 10;
return -1;
}


