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
 *	For "ExMy" (case-insensitive),  returns 10*x+y.
 *	For "MAPxy" (case-insensitive), returns 100+10*x+y.
 *	For other names, returns 0.
 */
int levelname2levelno (const char *name)
{
const unsigned char *s = (const unsigned char *) name;
if (toupper (s[0]) == 'E'
 && isdigit (s[1])
 && s[1] != '0'
 && toupper (s[2]) == 'M'
 && isdigit (s[3])
 && s[3] != '0'
 && s[4] == '\0')
   return 10 * dectoi (s[1]) + dectoi (s[3]);
if (toupper (s[0]) == 'M'
 && toupper (s[1]) == 'A'
 && toupper (s[2]) == 'P'
 && isdigit (s[3])
 && isdigit (s[4])
 && s[5] == '\0')
   return 100 + 10 * dectoi (s[3]) + dectoi (s[4]);
return 0;
}


/*
 *	levelno2levelname
 *	Convert 
 */

/*
 *	spec_path
 *	Extract the path of a spec
 */
const char *spec_path (const char *spec)
{
static char path[Y_PATH + 1];
size_t n;

*path = '\0';
strncat (path, spec, sizeof path - 1);
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
 *	y_stricmp
 *	A case-insensitive strcmp()
 *	(same thing as DOS stricmp() or GNU strcasecmp())
 */
int y_stricmp (const char *s1, const char *s2)
{
for (;;)
   {
   if (tolower (*s1) != tolower (*s2))
      return (unsigned char) *s1 - (unsigned char) *s2;
   if (! *s1)
      if (! *s2)
         return 0;
      else
	 return -1;
   if (! *s2)
      return 1;
   s1++;
   s2++;
   }
}


/*
 *	y_strnicmp
 *	A case-insensitive strncmp()
 *	(same thing as DOS strnicmp() or GNU strncasecmp())
 */
int y_strnicmp (const char *s1, const char *s2, size_t len)
{
while (len-- > 0)
   {
   if (tolower (*s1) != tolower (*s2))
      return (unsigned char) *s1 - (unsigned char) *s2;
   if (! *s1)
      if (! *s2)
         return 0;
      else
	 return -1;
   if (! *s2)
      return 1;
   s1++;
   s2++;
   }
return 0;
}


/*
 *	y_snprintf
 *	If available, snprintf(). Else sprintf().
 */
int y_snprintf (char *buf, size_t size, const char *fmt, ...)
{
va_list args;
va_start (args, fmt);
#ifdef Y_SNPRINTF
return vsnprintf (buf, size, fmt, args);
#else
return vsprintf (buf, fmt, args);
#endif
}


/*
 *	y_vsnprintf
 *	If available, vsnprintf(). Else vsprintf().
 */
int y_vsnprintf (char *buf, size_t size, const char *fmt, va_list args)
{
#ifdef Y_SNPRINTF
return vsnprintf (buf, size, fmt, args);
#else
return vsprintf (buf, fmt, args);
#endif
}


/*
 *	is_one_of
 *	Return non-zero if <s> is equal (in the strcmp() sense)
 *	to one of the other strings (retrieved from the argument
 *	list as const char *). The last string must be followed
 *	by (const char *) 0.
 */
int is_one_of (const char *needle, ...)
{
  va_list args;
  va_start (args, needle);
  for (;;)
  {
    const char *haystack = va_arg (args, const char *);
    if (haystack == Y_NULL)
      break;
    if (! strcmp (needle, haystack))
      return 1;
  }
  return 0;
}



/*
 *	file_exists
 *	Check whether a file exists and is readable.
 *	Returns non-zero if it is, 0 if it isn't.
 */
bool file_exists (const char *filename)
{
FILE *test;

if ((test = fopen (filename, "rb")) == NULL)
   return 0;
fclose (test);
return 1;
}


