/*
 *	editlev.cc
 *	AYM 1998-09-06
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
#include <time.h>
#ifdef Y_X11
#include <X11/Xlib.h>
#endif
#include "editlev.h"
#include "editloop.h"
#include "events.h"
#include "gfx.h"
#include "levels.h"


static void WriteYadexLog (const char *file, const char *level,
 time_t *t0, time_t *t1);


/*
 *	find_level
 *	Look in the master directory for levels that match
 *	the name in <name_given>.
 *
 *	<name_given> can have one of the following formats :
 *	
 *	  [Ee]n[Mm]m      EnMm
 *	  [Mm][Aa][Pp]nm  MAPnm
 *	  n               MAP0n
 *	  nm              Either EnMn or MAPnm
 *
 *	Return:
 *	- If <name_given> is either [Ee]n[Mm]m or [Mm][Aa][Pp]nm,
 *	  - if the level was found, its canonical (uppercased)
 *	    name in a freshly malloc'd buffer,
 *	  - else, NULL.
 *	- If <name_given> is either n or nm,
 *	  - if either EnMn or MAPnm was found, the canonical name
 *	    of the level found, in a freshly malloc'd buffer,
 *	  - if none was found, <error_none>,
 *	  - if the <name_given> is invalid, <error_invalid>,
 *	  - if several were found, <error_non_unique>.
 */
char *find_level (const char *name_given)
{
// If <name_given> is "[Ee]n[Mm]m" or "[Mm][Aa][Pp]nm", look for that
if (levelname2levelno (name_given))
   {
   char *canonical_name = strdup (name_given);
   for (char *p = canonical_name; *p; p++)
      *p = toupper (*p);  // But shouldn't FindMasterDir() be case-insensitive ?
   if (FindMasterDir (MasterDir, canonical_name))
      return canonical_name;
   else
      {
      free (canonical_name);
      return NULL;
      }
   }

// So it must be a short name ("1", "23").
int n = atoi (name_given);
if (n < 1 || n > 99)
   return error_invalid;
char *name1 = (char *) malloc (6);
char *name2 = (char *) malloc (6);
sprintf (name1, "E%dM%d", n / 10, n % 10);
sprintf (name2, "MAP%02d", n);
int match1 = FindMasterDir (MasterDir, name1) != NULL;
int match2 = FindMasterDir (MasterDir, name2) != NULL;
if (match1 && ! match2)		// Found only ExMy
   {
   free (name2);
   return name1;
   }
else if (match2 && ! match1)	// Found only MAPxy
   {
   free (name1);
   return name2;
   }
else if (match1 && match2)	// Found both
   {
   free (name1);
   free (name2);
   return error_non_unique;
   }
else				// Found none
   {
   free (name1);
   free (name2);
   return error_none;
   }
}


/*
   the driving program
*/

void EditLevel (const char *levelname, Bool newlevel)
{
ReadWTextureNames ();
ReadFTextureNames ();
InitGfx ();
/* Call init_input_status() as shortly as possible after the creation
   of the window to minimize the risk of calling get_input_status(),
   get_key(), have_key(), etc. with <is> still uninitialized. */
init_input_status ();
init_event ();
CheckMouseDriver ();
if (!levelname2levelno (levelname)
 || ! FindMasterDir (MasterDir, levelname))
   levelname = SelectLevel (atoi (levelname)); /* returns "" on Esc */
if (levelname2levelno (levelname))
   {
   time_t t0, t1;
   ClearScreen ();
   // FIXME I don't see the point in doing this for a new level
   ReadLevelData (levelname);
   if (newlevel)
      {
      ForgetLevelData ();
      MapMinX = -2000;
      MapMinY = -2000;
      MapMaxX = 2000;
      MapMaxY = 2000;
      }
   LogMessage (": Editing %s...\n", levelname);
   {
#define BUFSZ 100
   char buf[BUFSZ + 1];
   al_scps (buf, "Yadex - ",               BUFSZ);
   al_saps (buf, Level->wadfile->filename, BUFSZ);
   al_saps (buf, " - ",                    BUFSZ);
   al_saps (buf, Level->dir.name,          BUFSZ);
   XStoreName (dpy, win, buf);
#undef BUFSZ
   }
   time (&t0);
   EditorLoop (levelname);
   time (&t1);
   LogMessage (": Finished editing %s...\n", levelname);
   WriteYadexLog (Level->wadfile->filename, levelname, &t0, &t1);
   TermGfx ();
   if (! Registered)
      printf ("Please register the game"
         " if you want to be able to save your changes.\n");
   ForgetLevelData ();
   /* forget the level pointer */
   Level = 0;
   }
else
   TermGfx ();
ForgetWTextureNames ();
ForgetFTextureNames ();
}


/*
 *	WriteYadexLog - Keep track of time spent editing that wad file
 *	FIXME should be in a separate module
 */
static void WriteYadexLog (const char *file, const char *level, time_t *t0, time_t *t1)
{
al_fspec_t logname;
al_fdrv_t  drive;
al_fpath_t path;
al_fbase_t base;

al_fana (file, drive, path, base, 0);
sprintf (logname, "%s%s%s.yl", drive, path, base);

/* if log file does not already exist, do _not_ create it */
if (al_fnature (logname) == 1)
   {
   FILE *logfd;
   logfd = fopen (logname, "a");
   if (logfd)
      {
      struct tm *tm = localtime (t0);
      fprintf (logfd, "%04d%02d%02d\tedit\t%s\t%ld\n",
       tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, level, (long)(*t1-*t0)/60);
      fclose (logfd);
      }
   }
}



