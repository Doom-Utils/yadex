/*
 *	yadex.cc
 *	The main module.
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
#include <time.h>
#include "acolours.h"
#include "cfgfile.h"
#include "editlev.h"
#include "gfx.h"
#include "help1.h"
#include "mkpalette.h"
#include "rgb.h"
#include "sanity.h"
#include "sprites.h"


/*
 *	Constants (declared in yadex.h)
 */
const char *const log_file       = "yadex.log";
const char *const msg_unexpected = "unexpected error";
const char *const msg_nomem      = "Not enough memory";


/*
 *	Not real variables -- just unique pointer values
 *	used by functions that return pointers to 
 */
char error_non_unique[1];  // Found more than one
char error_none[1];        // Found none
char error_invalid[1];     // Invalid parameter


/*
 *	Global variables
 */
const char *install_dir = 0;		// Where Yadex is installed
FILE *      logfile     = NULL;		// Filepointer to the error log
Bool        Registered  = 0;		// Registered or shareware game?
int         screen_lines = 24;  	// Lines that our TTY can display
int         remind_to_build_nodes = 0;	// Remind user to build nodes

// Set from command line and/or config file
int       autoscroll                    = 1;
int       autoscroll_amp                = 10;
int       autoscroll_edge               = 30;
char *    config_file;
int       copy_linedef_reuse_sidedefs   = 0;
Bool      Debug				= 0;
int       default_ceiling_height	= 128;
char      default_ceiling_texture[9]	= "CEIL3_5";
int       default_floor_height		= 0;
char      default_floor_texture[9]	= "FLOOR4_8";
int       default_light_level		= 144;
char      default_lower_texture[9]	= "STARTAN3";
char      default_middle_texture[9]	= "STARTAN3";
char      default_upper_texture[9]	= "STARTAN3";
int       default_thing			= 3004;
int       double_click_timeout            = 200;
Bool      Expert			= 0;
char *    Game				= NULL;
int       grid_pixels_min               = 10;
int       GridMin			= 4;
int       GridMax			= 256;
Bool      InfoShown			= 1;
int       InitialScale			= 8;
confirm_t insert_vertex_split_linedef	= YC_ASK_ONCE;
confirm_t insert_vertex_merge_vertices	= YC_ASK_ONCE;
char *    Iwad1				= NULL;
char *    Iwad2				= NULL;
char *    Iwad3				= NULL;
char *    MainWad			= NULL;
#ifdef AYM_MOUSE_HACKS
int       MouseMickeysH			= 5;
int       MouseMickeysV			= 5;
#endif
char **   PatchWads			= NULL;
Bool      Quiet				= 0;
Bool      Quieter			= 0;
int       scroll_less			= 10;
int       scroll_more			= 90;
Bool      Select0			= 1;
int       show_help			= 0;
Bool      SwapButtons			= 0;
int       thing_fudge			= 400;
int       verbose			= 0;
int       vertex_fudge			= 400;
int       welcome_message		= 1;

// Global variables declared in game.h
al_llist_t *ldtdef     = NULL;
al_llist_t *ldtgroup   = NULL;
al_llist_t *stdef      = NULL;
al_llist_t *thingdef   = NULL;
al_llist_t *thinggroup = NULL;


/*
 *	Prototypes of private functions
 */
static int  parse_environment_vars ();
static void MainLoop ();
static void print_error_message (const char *fmt, va_list args);
static void add_base_colours ();


/*
 *	main
 *	Guess what.
 */
int main (int argc, char *argv[])
{
int r;

// Set <screen_lines>
if (getenv ("LINES") != NULL)
   screen_lines = atoi (getenv ("LINES"));
else
   screen_lines = 0;
if (screen_lines == 0)
   screen_lines = 24;

// InitSwap must be called before any call to GetMemory(), etc.
InitSwap ();

// First a quick pass through the command line
// arguments to detect -?, -f and -help.
r = parse_command_line_options (argc - 1, argv + 1, 1);
if (r)
   goto syntax_error;

if (show_help)
   {
   print_usage (stdout);
   exit (1);
   }

printf ("%s\n"
   "This program is derived from DEU 5.21"
   " by Raphaël Quinet and Brendon Wyber.\n", what ());

// Where am I installed ? (the config file might be there)
#if defined Y_DOS
install_dir = spec_path (argv[0]);
#elif defined Y_UNIX
install_dir = getenv ("Y_DIR");
#endif

// The config file provides some values.
r = parse_config_file (config_file);
if (! r)
   {
   // Environment variables can override them.
   r = parse_environment_vars ();
   if (! r)
      {
      // And the command line argument can override both.
      r = parse_command_line_options (argc - 1, argv + 1, 2);
      }
   }
if (r)
   {
   syntax_error :
   fprintf (stderr, "Try \"yadex -help\" or \"man yadex\".\n");
   exit (1);
   }

if (Iwad1 == NULL && Iwad2 == NULL)
   {
   report_error ("You didn't say where the iwad is.");
   fprintf (stderr,
     "To say where doom.wad  is, use \"-i1 <file>\""
     " or put \"iwad1=<file>\" in yadex.cfg.\n"
     "To say where doom2.wad is, use \"-i2 <file>\""
     " or put \"iwad2=<file>\" in yadex.cfg.\n");
   exit (1);
   }
else if (Game != NULL && ! strcmp (Game, "doom")
      || Game == NULL && Iwad2 == NULL)
   {
   if (Iwad1 == NULL)
      {
      report_error ("You have to tell me where doom.wad is.");
      fprintf (stderr,
         "Use \"-i1 <file>\" or put \"iwad1=<file>\" in yadex.cfg.\n");
      exit (1);
      }
   MainWad = Iwad1;
   }
else if (Game != NULL && ! strcmp (Game, "doom2")
      || Game == NULL && Iwad1 == NULL)
   {
   if (Iwad2 == NULL)
      {
      report_error ("You have to tell me where doom2.wad is.");
      fprintf (stderr,
         "Use \"-i2 <file>\" or put \"iwad2=<file>\" in yadex.cfg.\n");
      exit (1);
      }
   MainWad = Iwad2;
   }
else if (! strcmp (Game, "heretic"))
   {
   if (Iwad3 == NULL)
      {
      report_error ("You have to tell me where heretic.wad is.");
      fprintf (stderr,
         "Use \"-i3 <file>\" or put \"iwad3=<file>\" in yadex.cfg.\n");
      exit (1);
      }
   MainWad = Iwad3;
   }
else
   {
   report_error ("You didn't say for which game you want to edit.");
   fprintf (stderr, "Use \"-g doom|doom2|heretic\" on the command line\n"
      "or put \"game=doom|doom2|heretic\" in yadex.cfg.\n");
   exit (1);
   }
if (Debug)
   {
   logfile = fopen (log_file, "a");
   if (logfile == NULL)
      printf ("Warning: Could not open log file \"%s\"", log_file);
   LogMessage (": Welcome to Yadex!\n");
   }
if (Quieter)
   Quiet = 1;

// Sanity checks (useful when porting).
check_types ();

// Misc. things done only once.
add_base_colours ();

// Load game definitions (*.ygd).
InitGameDefs ();
LoadGameDefs (Game);

// Load the iwad and the pwads.
OpenMainWad (MainWad);
if (PatchWads)
   {
   const char * const *pwad_name;
   for (pwad_name = PatchWads; *pwad_name; pwad_name++)
      OpenPatchWad (*pwad_name);
   }
/* sanity check */
CloseUnusedWadFiles ();

if (welcome_message)
   print_welcome (stdout);

/* all systems go! */
MainLoop ();

/* that's all, folks! */
CloseWadFiles ();
FreeGameDefs ();
LogMessage (": The end!\n\n\n");
if (logfile != NULL)
   fclose (logfile);
if (remind_to_build_nodes)
   printf (
      "-> You have made changes to one or more wads. Don't forget to pass\n"
      "   them through a nodes builder such as BSP before running them.\n");
return 0;
}


/*
 *	parse_environment_vars
 *	Check certain environment variables.
 *	Returns 0 on success, <>0 on error.
 */
static int parse_environment_vars ()
{
char *value;

value = getenv ("Y_GAME");
if (value != NULL)
   Game = value;
return 0;
}


/*
   play a fascinating tune
*/

void Beep ()
{
#if defined Y_DOS
if (! Quieter)
   {
   sound (640);
   delay (100);
   nosound ();
   }
#elif defined Y_UNIX
return;
#endif
}



/*
   play a sound
*/

void PlaySound (int freq, int msec)
{
#if defined Y_DOS
if (! Quiet)
   {
   sound (freq);
   delay (msec);
   nosound ();
   }
#elif defined Y_UNIX
freq = msec;	// To prevent a warning about unused variables
return;
#endif
}



/*
 *	fatal_error
 *	Print an error message and terminate the program with code 2.
 */
void fatal_error (const char *fmt, ...)
{
va_list args;

va_start (args, fmt);
print_error_message (fmt, args);
// Clean up things and free swap space
ForgetLevelData ();
ForgetWTextureNames ();
ForgetFTextureNames ();
CloseWadFiles ();
exit (2);
}


/*
 *	report_error
 *	Print an error message but does not terminate the program.
 */
void report_error (const char *fmt, ...)
{
va_list args;

va_start (args, fmt);
print_error_message (fmt, args);
}


/*
 *	print_error_message
 *	Print an error message to stderr.
 */
static void print_error_message (const char *fmt, va_list args)
{
//Beep (); Beep ();

// With BGI, we have to switch back to text mode
// before printing the error message so do it now...
#ifdef Y_BGI
if (GfxMode)
   {
   sleep (1);
   TermGfx ();
   }
#endif

fflush (stdout);
fprintf (stderr, "Yadex: Error: ");
vfprintf (stderr, fmt, args);
fputc ('\n', stderr);
if (Debug && logfile != NULL)
   {
   fprintf (logfile, "Yadex: Error: ");
   vfprintf (logfile, fmt, args);
   fputc ('\n', logfile);
   }

// ... on the other hand, with X, we don't have to
// call TermGfx() before printing so we do it last so
// that a segfault occuring in TermGfx() does not
// prevent us from seeing the stderr message.
#ifdef Y_X11
if (GfxMode)
   TermGfx ();  // Don't need to sleep (1) either.
#endif
}


/*
   write a message in the log file
*/

void LogMessage (const char *logstr, ...)
{
va_list  args;
time_t   tval;
char    *tstr;

if (Debug && logfile != NULL)
   {
   va_start (args, logstr);
   /* if the message begins with ":", output the current date & time first */
   if (logstr[0] == ':')
      {
      time (&tval);
      tstr = ctime (&tval);
      tstr[strlen (tstr) - 1] = '\0';
      fprintf (logfile, "%s", tstr);
      }
   vfprintf (logfile, logstr, args);
   va_end (args);
   fflush (logfile);  /* AYM 19971031 */
   }
}



/*
   the main program menu loop
*/

static void MainLoop ()
{
char input[120];
char *com, *out;
FILE *file, *raw;
WadPtr wad;

for (;;)
   {
   /* get the input */
   printf ("\n[? for help]> ");
   fgets (input, sizeof input, stdin);
   /* Strip the trailing '\n' */
   if (strlen (input) > 0 && input[strlen (input) - 1] == '\n')
      input[strlen (input) - 1] = '\0';
   printf ("\n");
   /* strupr (input);  AYM 1998-06-13 */

   /* eat the white space and get the first command word */
   com = strtok (input, " ");

   /* user just hit return */
   if (com == NULL)
      printf ("[Please enter a command or ? for help.]\n");

   /* user inputting for help */
   else if (!strcmp (com, "?"))
      {
      printf ("?                                 --"
              " to display this text\n");
      printf ("b[uild] <WadFile>                 --"
              " to build a new iwad\n");
      printf ("c[reate] <levelname>              --"
              " to create and edit a new (empty) level\n");
      printf ("d[ump] <DirEntry> [outfile]       --"
              " to dump a directory entry in hex\n");
      printf ("e[dit] <levelname>                --"
              " to edit a game level saving results to\n");
      printf ("                                          a patch wad file\n");
      printf ("g[roup] <WadFile>                 --"
              " to group all patch wads in a file\n");
      printf ("i[nsert] <RawFile> <DirEntry>     --"
              " to insert a raw file in a patch wad file\n");
      printf ("l[ist] <WadFile> [outfile]        --"
              " to list the directory of a wadfile\n");
      printf ("m[aster] [outfile]                --"
              " to list the master directory\n");
      printf ("make_gimp_palette <outfile>       --"
              " to generate a gimp palette file from\n"
              "                                    "
              " entry 0 of lump PLAYPAL.\n");
      printf ("q[uit]                            --"
              " to quit\n");
      printf ("r[ead] <WadFile>                  --"
              " to read a new wad patch file\n");
      printf ("s[ave] <DirEntry> <WadFile>       --"
              " to save one object to a separate file\n");
      printf ("set                               --"
              " to list all options and their values\n");
      printf ("v[iew] [SpriteName]               --"
              " to display the sprites\n");
      printf ("w[ads]                            --"
              " to display the open wads\n");
      printf ("x[tract] <DirEntry> <RawFile>     --"
              " to save (extract) one object to a raw file\n");
      }

   /* user asked for list of open wad files */
   else if (!strcmp (com, "wads") || !strcmp (com, "w"))
      {
      printf ("%-20s  IWAD  (Main wad file)\n", WadFileList->filename);
      for (wad = WadFileList->next; wad; wad = wad->next)
	 {
	 printf ("%-20s  PWAD  (Patch wad file for %.8s)\n",
	  wad->filename, wad->directory[0].name);
	 }
      }

   /* user asked to quit */
   else if (!strcmp (com, "quit") || !strcmp (com, "q"))
      {
      if (! Registered)
	 printf ("Remember to register your copy of the game !\n");
#ifdef OLD
      printf ("Goodbye...\n");
#endif
      break;
      }

   /* user asked to edit a level */
   else if (!strcmp (com, "edit") || !strcmp (com, "e")
         || !strcmp (com, "create") || !strcmp (com, "c"))
      {
      const int newlevel = ! strcmp (com, "create") || ! strcmp (com, "c");

      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("Which level ?\n");
         continue;
	 }
      char *level_name = find_level (com);
      if (level_name == error_invalid)
         printf ("\"%s\" is not a valid level name.\n", com);
      else if (level_name == error_none)
         printf ("Neither E%dM%d nor MAP%02d exist.\n",
            atoi (com) / 10, atoi (com) % 10, atoi (com));
      else if (level_name == error_non_unique)
         printf ("Both E%dM%d and MAP%02d exist. Use an unambiguous name.\n",
            atoi (com) / 10, atoi (com) % 10, atoi (com));
      else if (level_name == NULL)
         {
         printf ("Level %s not found.", com);
         if (tolower (*com) == 'e' && ! strcmp (Game, "doom2")
            || tolower (*com) == 'm' && strcmp (Game, "doom2"))
            printf (" You are in %s mode.", Game);
         else if (tolower (*com) == 'e' && com[1] > '1' && ! Registered)
            printf (" You have the shareware iwad.");
         putchar ('\n');
         }
      else
         {
         EditLevel (level_name, newlevel);
         free (level_name);
         }
      }

   /* user asked to build a new main wad file */
   else if (!strcmp (com, "build") || !strcmp (com, "b"))
      {
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("[Wad file name argument missing.]\n");
	 continue;
	 }
      for (wad = WadFileList; wad; wad = wad->next)
	 if (!fncmp (com, wad->filename))
	    break;
      if (wad)
	 {
	 printf ("[File \"%s\" is opened and cannot be overwritten.]\n", com);
	 continue;
	 }
      BuildNewMainWad (com, 0);
      }

   /* user asked to build a compound patch wad file */
   else if (!strcmp (com, "group") || !strcmp (com, "g"))
      {
      if (WadFileList->next == NULL || WadFileList->next->next == NULL)
	 {
	 printf ("[You need at least two open wad files "
	   "if you want to group them.]\n");
	 continue;
	 }
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("[Wad file name argument missing.]\n");
	 continue;
	 }
      for (wad = WadFileList; wad; wad = wad->next)
	 if (!fncmp (com, wad->filename))
	    break;
      if (wad)
	 {
	 printf ("[File \"%s\" is opened and cannot be overwritten.]\n", com);
	 continue;
	 }
      BuildNewMainWad (com, 1);
      }

   /* user ask for a listing of a wad file */
   else if (!strcmp (com, "list") || !strcmp (com, "l"))
      {
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("[Wad file name argument missing.]\n");
	 continue;
	 }
      for (wad = WadFileList; wad; wad = wad->next)
	 if (!fncmp (com, wad->filename))
	    break;
      if (wad == NULL)
	 {
	 printf ("[Wad file \"%s\" is not open.]\n", com);
	 continue;
	 }
      out = strtok (NULL, " ");
      if (out)
	 {
	 printf ("Outputting directory of \"%s\" to \"%s\".\n",
	   wad->filename, out);
	 if ((file = fopen (out, "wt")) == NULL)
	    fatal_error ("error opening output file \"%s\"", com);
	 fprintf (file, "%s\n", what ());
	 ListFileDirectory (file, wad);
	 fprintf (file, "\nEnd of file.\n");
	 fclose (file);
	 }
      else
	 ListFileDirectory (stdout, wad);
      }

   /* user asked for the list of the master directory */
   else if (!strcmp (com, "master") || !strcmp (com, "m"))
      {
      out = strtok (NULL, " ");
      if (out)
	 {
	 printf ("Outputting master directory to \"%s\".\n", out);
	 if ((file = fopen (out, "wt")) == NULL)
	    fatal_error ("error opening output file \"%s\"", com);
	 fprintf (file, "%s\n", what ());
	 ListMasterDirectory (file);
	 fprintf (file, "\nEnd of file.\n");
	 fclose (file);
	 }
      else
	 ListMasterDirectory (stdout);
      }

   // make_gimp_palette
   else if (! strcmp (com, "make_gimp_palette"))
      {
      out = strtok (NULL, " ");
      if (out == NULL)
         {
         printf ("Output file name argument missing.\n");
         continue;
         }
      make_gimp_palette (0, out);
      }

   /* user asked to list all options and their values */
   else if (! strcmp (com, "set"))
      {
      dump_parameters (stdout);
      }

   /* user asked to read a new patch wad file */
   else if (!strcmp (com, "read") || !strcmp (com, "r"))
      {
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("Wad file name argument missing.\n");
	 continue;
	 }
      out = strtok (NULL, " ");
      if (out)
	*out = '\0';
      out = (char *) GetMemory (strlen (com) + 1);
      strcpy (out, com);
      OpenPatchWad (out);
      CloseUnusedWadFiles ();
      }

   /* user asked to dump the contents of a wad file */
   else if (!strcmp (com, "dump") || !strcmp (com, "d"))
      {
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("[Object name argument missing.]\n");
	 continue;
	 }
      out = strtok (NULL, " ");
      if (out)
	 {
	 printf ("Outputting directory entry data to \"%s\".\n", out);
	 if ((file = fopen (out, "wt")) == NULL)
	    fatal_error ("error opening output file \"%s\"", com);
	 fprintf (file, "%s\n", what ());
	 DumpDirectoryEntry (file, com);
	 fprintf (file, "\nEnd of file.\n");
	 fclose (file);
	 }
      else
	 DumpDirectoryEntry (stdout, com);
      }

   /* user asked to view the sprites */
   else if (!strcmp (com, "view") || !strcmp (com, "v"))
      {
      InitGfx ();
      com = strtok (NULL, " ");
      ChooseSprite (-1, -1, "Sprite viewer", com);
      TermGfx ();
      }

   /* user asked to save an object to a separate pwad file */
   else if (!strcmp (com, "save") || !strcmp (com, "s"))
      {
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("[Object name argument missing.]\n");
	 continue;
	 }
      if (strlen (com) > 8 || strchr (com, '.') != NULL)
	 {
	 printf ("[Invalid object name.]\n");
	 continue;
	 }
      out = strtok (NULL, " ");
      if (out == NULL)
	 {
	 printf ("[Wad file name argument missing.]\n");
	 continue;
	 }
      for (wad = WadFileList; wad; wad = wad->next)
	 if (!fncmp (out, wad->filename))
	    break;
      if (wad)
	 {
	 printf ("[This wad file is already in use. "
	    "You may not overwrite it.]\n");
	 continue;
	 }
      printf ("Saving directory entry data to \"%s\".\n", out);
      if ((file = fopen (out, "wb")) == NULL)
	 fatal_error ("error opening output file \"%s\"", out);
      SaveDirectoryEntry (file, com);
      fclose (file);
      }

   /* user asked to encapsulate a raw file in a pwad file */
   else if (!strcmp (com, "insert") || !strcmp (com, "i"))
      {
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("[Raw file name argument missing.]\n");
	 continue;
	 }
      out = strtok (NULL, " ");
      if (out == NULL)
	 {
	 printf ("[Object name argument missing.]\n");
	 continue;
	 }
      if (strlen (out) > 8 || strchr (out, '.') != NULL)
	 {
	 printf ("[Invalid object name.]\n");
	 continue;
	 }
      if ((raw = fopen (com, "rb")) == NULL)
	 fatal_error ("error opening input file \"%s\"", com);
      /* kluge */
      strcpy (input, out);
      strcat (input, ".wad");
      for (wad = WadFileList; wad; wad = wad->next)
	 if (!fncmp (input, wad->filename))
	    break;
      if (wad)
	 {
	 printf ("[This wad file is already in use (%s). "
	    "You may not overwrite it.]\n", input);
	 continue;
	 }
      printf ("Including new object %s in \"%s\".\n", out, input);
      if ((file = fopen (input, "wb")) == NULL)
	 fatal_error ("error opening output file \"%s\"", input);
      SaveEntryFromRawFile (file, raw, out);
      fclose (raw);
      fclose (file);
      }

   /* user asked to extract an object to a raw binary file */
   else if (!strcmp (com, "xtract")
	 || !strcmp (com, "extract")
	 || !strcmp (com, "x"))
      {
      com = strtok (NULL, " ");
      if (com == NULL)
	 {
	 printf ("[Object name argument missing.]\n");
	 continue;
	 }
      if (strlen (com) > 8 || strchr (com, '.') != NULL)
	 {
	 printf ("[Invalid object name.]\n");
	 continue;
	 }
      out = strtok (NULL, " ");
      if (out == NULL)
	 {
	 printf ("[Raw file name argument missing.]\n");
	 continue;
	 }
      for (wad = WadFileList; wad; wad = wad->next)
	 if (!fncmp (out, wad->filename))
	    break;
      if (wad)
	 {
	 printf ("[You may not overwrite an opened wad file with raw data.]\n");
	 continue;
	 }
      printf ("Saving directory entry data to \"%s\".\n", out);
      if ((file = fopen (out, "wb")) == NULL)
	 fatal_error ("error opening output file \"%s\"", out);
      SaveEntryToRawFile (file, com);
      fclose (file);
      }

   /* unknown command */
   else
      printf ("[Unknown command \"%s\"!]\n", com);
   }
}


/*
 *	add_base_colours
 *	Add the NCOLOURS base colours to the list of
 *	application colours.
 */
static void add_base_colours ()
{
for (size_t n = 0; n < NCOLOURS; n++)
   {
   rgb_c c;

   // The first 16 are the standard IRGB VGA colours.
   // FIXME they're to be removed and replaced by
   // "logical" colours.
   if (n < 16)
      irgb2rgb (n, &c);

   // Then there are the colours used to draw the
   // windows and the map. The colours used to draw
   // the things are parametrized in the .ygd ; they
   // are added by load_game().
   // FIXME they should not be hard-coded, of course !
   else if (n == WINBG)
      c.set (0x2a, 0x24, 0x18);  // 0x80, 0x70, 0x60
   else if (n == WINBG_LIGHT)
 //     c.set (0x50, 0x46, 0x40);  //0xc0 //0xa8 //0x80
      c.set (0x48, 0x42, 0x3c);  //0xc0 //0xa8 //0x80
   else if (n == WINBG_DARK)
      c.set (0x20, 0x1b, 0x16);  //0x40 //0x38 //0x30
   else if (n == WINFG)
      c.set (0xa0, 0xa0, 0xa0);
   else if (n == WINFG_DIM)
      c.set (0x48, 0x48, 0x48);
   else if (n == WINBG_HL)
      c.set (0x58, 0x50, 0x48);
   else if (n == WINFG_HL)
      c.set (0xc0, 0xc0, 0xc0);
   else if (n == WINFG_DIM_HL)
      c.set (0x70, 0x70, 0x70);
   else if (n == GRID1)
      c.set (0, 0, 0x60);
   else if (n == GRID2)
      c.set (0, 0, 0xa0);
   else if (n == GRID3)
      c.set (0, 0, 0xe0);
   else if (n == WINFGLABEL)  // Text in window, a bit dimmer
      c.set (0x88, 0x88, 0x88);
   else
      fatal_error ("Wrong acn %d", n);

   acolour_t acn = add_app_colour (c);
   if (acn != n)
      fatal_error ("add_base_colours: got %d for %d\n", acn, n);
   }
}


