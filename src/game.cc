/*
 *	game.cc
 *	Load .ygd file (Yadex Game Definitions)
 *	AYM 1998-01-04
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-1998 Andr� Majorel.

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
#include <errno.h>
#include "acolours.h"
#include "game.h"
#include "things.h"


static const char *standard_directories[] =
   {
   ".",
   "~",				/* "~" means "the user's home directory" */
   "/",				/* "/" means "where Yadex is installed" */
   "/usr/local/share/games",
   "/usr/share/games",
   0
   };

/*
 *	InitGameDefs
 *	Create empty lists for game definitions
 */
void InitGameDefs (void)
{
ldtdef      = al_lcreate (sizeof (ldtdef_t    ));
ldtgroup    = al_lcreate (sizeof (ldtgroup_t  ));
stdef       = al_lcreate (sizeof (stdef_t     ));
thingdef    = al_lcreate (sizeof (thingdef_t  ));
thinggroup  = al_lcreate (sizeof (thinggroup_t));
}


/*
 *	LoadGameDefs
 *	Looks for file <game>.ygd in various directories and reads it.
 *	Builds list ThingsDefs.
 *	A totally boring piece of code.
 */
void LoadGameDefs (const char *game)
{
FILE *ygdfile = 0;		/* YGD file descriptor */
#define YGD_BUF 200		/* max line length + 2 */
char readbuf[YGD_BUF];		/* buffer the line is read into */
#define MAX_TOKENS 10		/* tokens per line */
int lineno;			/* current line of file */
const char **dirname;
char filename[257];
char basename[13];

al_scps (basename, game,   sizeof basename - 1);
al_saps (basename, ".ygd", sizeof basename - 1);

for (dirname = standard_directories; *dirname; dirname++)
   {
   if (! strcmp (*dirname, "~"))
      if (getenv ("HOME"))
         al_scps (filename, getenv ("HOME"), sizeof filename - 1);
      else
         continue;
   else if (! strcmp (*dirname, "/"))
      if (install_dir)
         al_scps (filename, install_dir, sizeof filename - 1);
      else
         continue;
   else
      al_scps (filename, *dirname, sizeof filename - 1);
   al_sapc (filename, '/',      sizeof filename - 1);
   al_saps (filename, basename, sizeof filename - 1);
   ygdfile = fopen (filename, "r");
   if (ygdfile)
      break;
   }

if (! ygdfile)
   fatal_error ("Game definition file \"%s\" not found (%s)",
      basename, strerror (errno));
else
   printf ("Using game definition file \"%s\".\n", filename);

for (lineno = 1; fgets (readbuf, YGD_BUF, ygdfile); lineno++)
   {
   int         ntoks;
   char       *token[MAX_TOKENS];
   int         quoted;
   int         in_token;
   const char *iptr;
   char       *optr;
   char       *buf;
   const char *bad_arg_count = "%s(%d): directive \"%s\" takes %d parameters";

   if (lineno == 1)
      {
      if (strcmp (readbuf, "# Yadex game definition file version 1\n"))
         {
         report_error ("%s is not a Yadex game definition file", filename);
         fatal_error ("Perhaps a version mismatch ?");
         }
      continue;
      }

   /* duplicate the buffer */
   buf = (char *) malloc (strlen (readbuf) + 1);
   if (! buf)
      fatal_error ("not enough memory");
   
   /* break the line into whitespace-separated tokens.
      whitespace can be enclosed in double quotes. */
   for (in_token = 0, quoted = 0, iptr = readbuf, optr = buf, ntoks = 0;
    ; iptr++)
      {
      if (*iptr == '\n' || *iptr == '\0')
	 {
	 if (in_token)
	   *optr = '\0';
	 break;
	 }

      else if (*iptr == '"')
	 quoted ^= 1;

      // "#" at the beginning of a token
      else if (! in_token && ! quoted && *iptr == '#')
         break;

      // First character of token
      else if (! in_token && (quoted || ! isspace (*iptr)))
	 {
         if (ntoks >= (int) (sizeof token / sizeof *token))
            fatal_error ("%s(%d): more than %d tokens",
               filename, lineno, sizeof token / sizeof *token);
	 token[ntoks] = optr;
	 ntoks++;
	 in_token = 1;
	 *optr++ = *iptr;
	 }

      // First space between two tokens
      else if (in_token && ! quoted && isspace (*iptr))
	 {
	 *optr++ = '\0';
	 in_token = 0;
	 }

      // Character in the middle of a token
      else if (in_token)
	 *optr++ = *iptr;
      }
   if (quoted)
      fatal_error ("%s(%d): unmatched double quote", filename, lineno);

   /* process line */
   if (ntoks == 0)
      {
      free (buf);
      continue;
      }
   if (! strcmp (token[0], "ldt"))
      {
      ldtdef_t buf;

      if (ntoks != 5)
	 fatal_error (bad_arg_count, filename, lineno, token[0], 4);
      buf.number    = atoi (token[1]);
      buf.ldtgroup  = *token[2];
      buf.shortdesc = token[3];  /* FIXME: trunc to 16 char. */
      buf.longdesc  = token[4];  /* FIXME: trunc reasonably */
      if (al_lwrite (ldtdef, &buf))
	 fatal_error ("LGD1 (%s)", al_astrerror (al_aerrno));
      }
   else if (! strcmp (token[0], "ldtgroup"))
      {
      ldtgroup_t buf;

      if (ntoks != 3)
	 fatal_error (bad_arg_count, filename, lineno, token[0], 2);
      buf.ldtgroup = *token[1];
      buf.desc     = token[2];
      if (al_lwrite (ldtgroup, &buf))
	 fatal_error ("LGD2 (%s)", al_astrerror (al_aerrno));
      }
   else if (! strcmp (token[0], "st"))
      {
      stdef_t buf;

      if (ntoks != 4)
	 fatal_error (bad_arg_count, filename, lineno, token[0], 3);
      buf.number    = atoi (token[1]);
      buf.shortdesc = token[2];  /* FIXME: trunc to 14 char. */
      buf.longdesc  = token[3];  /* FIXME: trunc reasonably */
      if (al_lwrite (stdef, &buf))
	 fatal_error ("LGD3 (%s)", al_astrerror (al_aerrno));
      }
   else if (! strcmp (token[0], "thing"))
      {
      thingdef_t buf;

      if (ntoks < 5 || ntoks > 6)
	 fatal_error (
            "%s(d%): directive \"%s\" takes between 4 and 5 parameters",
            filename, lineno, token[0]);
      buf.number     = atoi (token[1]);
      buf.thinggroup = *token[2];
      buf.radius     = atoi (token[3]);
      buf.desc       = token[4];
      buf.sprite     = ntoks > 5 ? token[5] : 0;
      if (al_lwrite (thingdef, &buf))
	 fatal_error ("LGD4 (%s)", al_astrerror (al_aerrno));
      }
   else if (! strcmp (token[0], "thinggroup"))
      {
      thinggroup_t buf;

      if (ntoks != 4)
	 fatal_error (bad_arg_count, filename, lineno, token[0], 3);
      buf.thinggroup = *token[1];
      if (getcolour (token[2], &buf.rgb))
	 fatal_error ("%s(%d): bad colour spec \"%s\"",
            filename, lineno, token[2]);
      buf.acn = add_app_colour (buf.rgb);
      buf.desc = token[3];
      if (al_lwrite (thinggroup, &buf))
	 fatal_error ("LGD5 (%s)", al_astrerror (al_aerrno));
      }
   else
      fatal_error ("%s(%d): unknown directive \"%s\"", filename, lineno, token[0]);
   }

fclose (ygdfile);


/*
 *	Second pass
 */

/* Speed optimization : build the table of things attributes
   that get_thing_*() use. */
create_things_table ();

/* KLUDGE: Add bogus ldtgroup LDT_FREE. InputLinedefType()
   knows that it means "let the user enter a number". */
{
ldtgroup_t buf;

buf.ldtgroup = LDT_FREE;  /* that is '\0' */
buf.desc     = "Other (enter number)";
al_lseek (ldtgroup, 0, SEEK_END);
if (al_lwrite (ldtgroup, &buf))
  fatal_error ("LGD90 (%s)", al_astrerror (al_aerrno));
}

/* KLUDGE: Add bogus thinggroup THING_FREE.
   InputThingType() knows that it means "let the user enter a number". */
{
thinggroup_t buf;

buf.thinggroup = THING_FREE;  /* that is '\0' */
buf.desc       = "Other (enter number)";
al_lseek (thinggroup, 0, SEEK_END);
if (al_lwrite (thinggroup, &buf))
  fatal_error ("LGD91 (%s)", al_astrerror (al_aerrno));
}

/* KLUDGE: Add bogus sector type at the end of stdef.
   SectorProperties() knows that it means "let the user enter a number". */
{
stdef_t buf;

buf.number    = 0;     /* not significant */
buf.shortdesc = 0;     /* not significant */
buf.longdesc  = "Other (enter number)";
al_lseek (stdef, 0, SEEK_END);
if (al_lwrite (stdef, &buf))
  fatal_error ("LGD92 (%s)", al_astrerror (al_aerrno));
}

}


/*
 *	FreeGameDefs
 *	Free all memory allocated to game definitions
 */
void FreeGameDefs (void)
{
delete_things_table ();
al_ldiscard (ldtdef    );
al_ldiscard (ldtgroup  );
al_ldiscard (stdef     );
al_ldiscard (thingdef  );
al_ldiscard (thinggroup);
}

