/*
 *	cfgfile.cc
 *	AYM 1998-09-30
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
#include "cfgfile.h"
#include "gfx.h"
#include "help1.h"
#include "levels.h"


const char config_file_magic[] = "# Yadex configuration file version 2";


/*
 *	Description of the command line arguments and config file keywords
 */
typedef enum
   {
   // Boolean (toggle)
   // Receptacle is of type Bool
   // data_ptr is of type (Bool *)
   OPT_BOOLEAN,

   // "yes", "no", "ask"
   // Receptacle is of type confirm_t
   // data_ptr is of type confirm_t
   OPT_CONFIRM,

   // Integer number,
   // Receptacle is of type int
   // data_ptr is of type (int *)
   OPT_INTEGER,

   // String
   // Receptacle is of type (char[9])
   // data_ptr is of type (char *)
   OPT_STRINGBUF8,

   // String
   // Receptacle is of type (const char *)
   // data_ptr is of type (const char **)
   OPT_STRINGPTR,

   // String, but store in a list
   // Receptacle is of type ??
   // data_ptr is of type ??
   OPT_STRINGPTRACC,

   // List of strings
   // Receptacle is of type (const char *[])
   // data_ptr is of type (const char ***)
   OPT_STRINGPTRLIST,

   // End of the options description
   OPT_END
   } opt_type_t;
typedef struct
   {
   const char *long_name;	// Command line arg. or keyword
   const char *short_name;	// Abbreviated command line argument
   opt_type_t opt_type;		// Type of this option
   const char *flags;		// Flags for this option :
				// "1" = process only on pass 1 of
				//       parse_command_line_options()
				// "b" = BGI only (ignored ifndef Y_BGI)
				// "x" = X only (ignored ifndef Y_X11)
   const char *desc;		// Description of the option
   void *data_ptr;              // Pointer to the data
   } opt_desc_t;

/* The first option has neither long name nor short name.
   It is used for "lonely" arguments (i.e. file names). */
opt_desc_t options[] =		/* description of the command line options */
{
{ NULL,
  NULL,
  OPT_STRINGPTRACC,
  NULL,
  "Patch wad file",
  &PatchWads },

{ "autoscroll",
  NULL,
  OPT_BOOLEAN,
  NULL,
  "Enable autoscrolling",
  &autoscroll },

{ "autoscroll_amp",
  NULL,
  OPT_INTEGER,
  NULL,
  "Amp. of scrolling (% of screen size)",
  &autoscroll_amp },

{ "autoscroll_edge",
  NULL,
  OPT_INTEGER,
  NULL,
  "Max. dist. to edge (pixels)",
  &autoscroll_edge },

{ "bgi",
  NULL,
  OPT_STRINGPTR,
  "b",
  "(BGI only) BGI video driver",
  &BGIDriver },

{ "cirrus_cursor",
  "cc",
  OPT_BOOLEAN,
  "b",
  "(BGI only) Cirrus hardware cursor",
  &CirrusCursor },

{ "config_file",
  "f",
  OPT_STRINGPTR,
  "1",
  "Config file",
  &config_file },

{ "copy_linedef_reuse_sidedefs",
  NULL,
  OPT_BOOLEAN,
  NULL,
  "Use same sidedefs as original linedef",
  &copy_linedef_reuse_sidedefs },

{ "debug",
  "d",
  OPT_BOOLEAN,
  NULL,
  "Debug mode",
  &Debug },

{ "default_ceiling_height",
  NULL,
  OPT_INTEGER,
  NULL,
  "Default ceiling height",
  &default_ceiling_height },

{ "default_ceiling_texture",
  NULL,
  OPT_STRINGBUF8,
  NULL,
  "Default ceiling texture",
  default_ceiling_texture },

{ "default_floor_height",
  NULL,
  OPT_INTEGER,
  NULL,
  "Default floor height",
  &default_floor_height },

{ "default_floor_texture",
  NULL,
  OPT_STRINGBUF8,
  NULL,
  "Default floor texture",
  default_floor_texture },

{ "default_light_level",
  NULL,
  OPT_INTEGER,
  NULL,
  "Default light level",
  &default_light_level },

{ "default_lower_texture",
  NULL,
  OPT_STRINGBUF8,
  NULL,
  "Default lower texture",
  default_lower_texture },

{ "default_middle_texture",
  NULL,
  OPT_STRINGBUF8,
  NULL,
  "Default middle texture",
  default_middle_texture },

{ "default_thing",
  NULL,
  OPT_INTEGER,
  NULL,
  "Default thing number",
  &default_thing },

{ "default_upper_texture",
  NULL,
  OPT_STRINGBUF8,
  NULL,
  "Default upper texture",
  default_upper_texture },

{ "double_click_timeout",
  NULL,
  OPT_INTEGER,
  NULL,
  "Max delay in ms between clicks",
  &double_click_timeout },

{ "expert",
  "e",
  OPT_BOOLEAN,
  NULL,
  "Expert mode",
  &Expert },

{ "fake_cursor",
  "fc",
  OPT_BOOLEAN,
  NULL,
  "(X11 only) Fake cursor",
  &FakeCursor },

{ "file",
  NULL,
  OPT_STRINGPTRLIST,
  NULL,
  "Patch wad file",
  &PatchWads },

{ "font",
  "fn",
  OPT_STRINGPTR,
  NULL,
  "(X11 only) Font name",
  &font_name },

{ "game",
  "g",
  OPT_STRINGPTR,
  NULL,
  "Game",
  &Game },

{ "height",
  "h",
  OPT_INTEGER,
  "x",
  "(X11 only) Initial window height",
  &initial_window_height },

{ "help",
  "?",
  OPT_BOOLEAN,
  "1",
  "Show usage summary",
  &show_help },

{ "grid_max",
  NULL,
  OPT_INTEGER,
  NULL,
  "Max grid step (map units)",
  &GridMax },

{ "grid_min",
  NULL,
  OPT_INTEGER,
  NULL,
  "Min grid step (map units)",
  &GridMin },

{ "grid_pixels_min",
  NULL,
  OPT_INTEGER,
  NULL,
  "Min grid step (pixels)",
  &grid_pixels_min },

{ "idle_sleep_ms",
  NULL,
  OPT_INTEGER,
  NULL,
  "ms to sleep before XPending()",
  &idle_sleep_ms },

{ "info_bar",
  NULL,
  OPT_BOOLEAN,
  NULL,
  "Show the info bar",
  &InfoShown },

{ "insert_vertex_split_linedef",
  NULL,
  OPT_CONFIRM,
  NULL,
  "Split ld after ins. vertex",
  &insert_vertex_split_linedef },

{ "insert_vertex_merge_vertices",
  NULL,
  OPT_CONFIRM,
  NULL,
  "Merge vertices after ins. vertex",
  &insert_vertex_merge_vertices },

{ "iwad1",
  "i1",
  OPT_STRINGPTR,
  NULL,
  "The name of the Doom/Ultimate D. iwad",
  &Iwad1 },

{ "iwad2",
  "i2",
  OPT_STRINGPTR,
  NULL,
  "The name of the Doom II/Final D. iwad",
  &Iwad2 },

{ "iwad3",
  "i3",
  OPT_STRINGPTR,
  NULL,
  "The name of the Heretic iwad",
  &Iwad3 },

{ "iwad4",
  "i4",
  OPT_STRINGPTR,
  NULL,
  "The name of the Hexen iwad",
  &Iwad4 },

{ "iwad5",
  "i5",
  OPT_STRINGPTR,
  NULL,
  "The name of the Strife iwad",
  &Iwad5 },

{ "iwad6",
  "i6",
  OPT_STRINGPTR,
  NULL,
  "The name of the Doom alpha 0.2 iwad",
  &Iwad6 },

{ "iwad7",
  "i7",
  OPT_STRINGPTR,
  NULL,
  "The name of the Doom alpha 0.4 iwad",
  &Iwad7 },

{ "iwad8",
  "i8",
  OPT_STRINGPTR,
  NULL,
  "The name of the Doom alpha 0.5 iwad",
  &Iwad8 },

{ "iwad9",
  "i9",
  OPT_STRINGPTR,
  NULL,
  "The name of the Doom press rel. iwad",
  &Iwad9 },

#ifdef AYM_MOUSE_HACKS
{ "mouse_horizontal_sens",
  "mh",
  OPT_INTEGER,
  "b",
  "(BGI only) Mouse horizontal sensitivity",
  &MouseMickeysH },

{ "mouse_vertical_sens",
  "mv",
  OPT_INTEGER,
  "b",
  "(BGI only) Mouse vertical sensitivity",
  &MouseMickeysV },
#endif

{ "pwad",
  "pw",
  OPT_STRINGPTRACC,
  NULL,
  "Pwad file to load",
  &PatchWads },

{ "no_pixmap",
  "P",
  OPT_BOOLEAN,
  NULL,
  "(X11 only) Use no pixmap",
  &no_pixmap },

{ "quiet",
  "q",
  OPT_BOOLEAN,
  NULL,
  "Quiet mode",
  &Quiet },

{ "quieter",
  "qq",
  OPT_BOOLEAN,
  NULL,
  "Quieter mode",
  &Quieter },

{ "scroll_less",
  NULL,
  OPT_INTEGER,
  NULL,
  "Amp. of scrolling (% of screen size)",
  &scroll_less },

{ "scroll_more",
  NULL,
  OPT_INTEGER,
  NULL,
  "Amp. of scrolling (% of screen size)",
  &scroll_more },

{ "select0",
  "s0",
  OPT_BOOLEAN,
  NULL,
  "Automatic selection of 0th object",
  &Select0 },

{ "swap_buttons",
  "sb",
  OPT_BOOLEAN,
  NULL,
  "Swap mouse buttons",
  &SwapButtons },

{ "text_dot",
  "td",
  OPT_BOOLEAN,
  NULL,
  "DrawScreenText debug flag",
  &text_dot },

{ "thing_fudge",
  NULL,
  OPT_INTEGER,
  NULL,
  "Radius to highlight things",
  &thing_fudge },

{ "verbose",
  "v",
  OPT_BOOLEAN,
  NULL,
  "Verbose mode",
  &verbose },

{ "vertex_fudge",
  NULL,
  OPT_INTEGER,
  NULL,
  "Radius to highlight vertices",
  &vertex_fudge },

{ "video",
  "V",
  OPT_INTEGER,
  "b",
  "(BGI only) Video mode",
  &VideoMode },

{ "welcome_message",
  NULL, 
  OPT_BOOLEAN,
  NULL,
  "Print welcome message",
  &welcome_message },

{ "width",
  "w",
  OPT_INTEGER,
  "x",
  "(X11 only) Initial window width",
  &initial_window_width },

{ "zoom",
  "z",
  OPT_INTEGER,
  NULL,
  "Initial zoom factor",
  &InitialScale },

{ NULL,
  NULL,
  OPT_END,
  NULL,
  NULL,
  NULL }
};


static const char *standard_directories[] =
  {
  ".",
  "~",			/* $HOME */
  "/",			/* $Y_DIR */
  "/usr/local/etc",
  "/etc",
  0
  };


static void append_item_to_list (const char ***list, const char *item);
int parse_one_config_file (const char *filename);
static confirm_t confirm_e2i (const char *external);
static const char *confirm_i2e (confirm_t internal);


/*
 *	parse_config_file
 *
 *	If <filename> is NULL, the standard directories are searched
 *	for a file named "yadex.cfg".
 *
 *	If <filename> is not NULL and is not an absolute name (i.e.
 *	does not start with a "/"), the standard directories are
 *	searched for a file named <filename>. <filename> may contain
 *	slashes, E.G. if you use "-config foo/yadex.cfg", the function
 *	will search for "./foo/yadex.cfg", "~/foo/yadex.cfg",
 *	"/opt/etc/foo/yadex.cfg", and so on.
 *
 *	If <filename> is not NULL but is an absolute name, only that
 *	file is searched.
 */
int parse_config_file (const char *filename)
{
const char **dirname;
int is_absolute;
char name[257];
int failure = 1;

is_absolute = filename && *filename == '/';

if (is_absolute)
   failure = parse_one_config_file (filename);
else
   {
   for (dirname = standard_directories; *dirname; dirname++)
      {
      if (! strcmp (*dirname, "~"))
         if (getenv ("HOME"))
	    al_scps (name, getenv ("HOME"), sizeof name - 1);
         else
            continue;
      else if (! strcmp (*dirname, "/"))
         if (install_dir)
            al_scps (name, install_dir, sizeof name - 1);
         else
            continue;
      else
	 al_scps (name, *dirname, sizeof name - 1);
      al_sapc (name, '/', sizeof name - 1);
      al_saps (name, filename ? filename : "yadex.cfg", sizeof name - 1);
      failure = parse_one_config_file (name);
      if (! failure)
         break;
      }
   }

if (failure && is_absolute)
   printf ("Can't open config file \"%s\" (%s)\n.", filename, strerror (errno));
else if (failure)
   printf ("No config file found.\n");
else
   printf ("Using config file \"%s\".\n", is_absolute ? filename : name);
return failure;
}


/*
 *	parse_one_config_file
 *	Try to parse one particular config file.
 *	Returns 0 on success, <>0 on failure.
 */
int parse_one_config_file (const char *filename)
{
FILE *cfgfile;
char  line[1024];
char *value;
char *option;
char *p;
const opt_desc_t *o;

cfgfile = fopen (filename, "r");
if (cfgfile == NULL)
   return 1;

/* The first line of the configuration file must
   contain exactly config_file_magic. */
if (fgets (line, sizeof line, cfgfile) == NULL
   || memcmp (line, config_file_magic, sizeof config_file_magic - 1)
   || line[sizeof config_file_magic - 1] != '\n'
   || line[sizeof config_file_magic] != '\0')
   {
   report_error ("%s is not a valid Yadex configuration file", filename);
   report_error ("Perhaps a leftover from a previous version of Yadex ?");
   return 1;
   }

while (fgets (line, sizeof line, cfgfile) != NULL)
   {
   if (line[0] == '#' || strlen (line) < 2)
      continue;
   if (line[strlen (line) - 1] == '\n')
      line[strlen (line) - 1] = '\0';
   /* skip blanks before the option name */
   option = line;
   while (isspace (*option))
      option++;
   /* skip the option name */
   value = option;
   while (value[0] && value[0] != '=' && !isspace (value[0]))
      value++;
   if (!value[0])
      {
      report_error ("invalid line in %s (ends prematurely)", filename);
      return 1;
      }
   if (value[0] == '=')
      {
      /* mark the end of the option name */
      value[0] = '\0';
      }
   else
      {
      /* mark the end of the option name */
      value[0] = '\0';
      value++;
      /* skip blanks after the option name */
      while (isspace (value[0]))
	 value++;
      if (value[0] != '=')
         {
	 report_error ("invalid line in %s (no '=')", filename);
         return 1;
         }
      }
   value++;
   /* skip blanks after the equal sign */
   while (isspace (value[0]))
      value++;
   for (o = options + 1; ; o++)
      {
      if (o->opt_type == OPT_END)
         {
         report_error ("Invalid option in %s: \"%s\"", filename, option);
         return 1;
         }
      if (!y_stricmp (option, o->long_name))
	 {
	 if (o->flags != NULL && strchr (o->flags, '1'))
	    break;
#if ! defined Y_BGI
	 if (o->flags != NULL && strchr (o->flags, 'b'))
	    break;
#endif
#if ! defined Y_X11
	 if (o->flags != NULL && strchr (o->flags, 'x'))
	    break;
#endif
	 switch (o->opt_type)
	    {
	    case OPT_BOOLEAN:
	       if (!y_stricmp (value, "yes") || !y_stricmp (value, "true")
	        || !y_stricmp (value, "on") || !y_stricmp (value, "1"))
		  {
		  if (o->data_ptr)
		     *((Bool *) (o->data_ptr)) = 1;
		  }
	       else if (!y_stricmp (value, "no") || !y_stricmp (value, "false")
	             || !y_stricmp (value, "off") || !y_stricmp (value, "0"))
		  {
		  if (o->data_ptr)
		     *((Bool *) (o->data_ptr)) = 0;
		  }
	       else
                  {
		  report_error ("invalid value for option %s: \"%s\"",
		    option, value);
                  return 1;
                  }
	       break;
            case OPT_CONFIRM:
               if (o->data_ptr)
                  *((confirm_t *) o->data_ptr) = confirm_e2i (value);
               break;
	    case OPT_INTEGER:
	       if (o->data_ptr)
	          *((int *) (o->data_ptr)) = atoi (value);
	       break;
            case OPT_STRINGBUF8:
               if (o->data_ptr)
                  al_scps ((char *) o->data_ptr, value, 8);
               break;
	    case OPT_STRINGPTR:
	       p = (char *) GetMemory (strlen (value) + 1);
	       strcpy (p, value);
	       if (o->data_ptr)
	          *((char **) (o->data_ptr)) = p;
	       break;
	    case OPT_STRINGPTRACC:
	       p = (char *) GetMemory (strlen (value) + 1);
	       strcpy (p, value);
	       if (o->data_ptr)
	          append_item_to_list ((const char ***) o->data_ptr, p);
	       break;
	    case OPT_STRINGPTRLIST:
	       while (value[0])
		  {
		  option = value;
		  while (option[0] && !isspace (option[0]))
		     option++;
		  option[0] = '\0';
		  option++;
		  while (isspace (option[0]))
		     option++;
		  p = (char *) GetMemory (strlen (value) + 1);
		  strcpy (p, value);
	          if (o->data_ptr)
		     append_item_to_list ((const char ***) o->data_ptr, p);
		  value = option;
		  }
	       break;
	    default:
               {
               report_error ("unknown option type (BUG!)");
               return 1;
               }
	    }
	 break;
	 }
      }
   }
fclose (cfgfile);
return 0;
}


/*
 *	parse_command_line_options
 *	If <pass> is set to 1, ignores all options except those
 *	that have the "1" flag.
 *	Else, ignores all options that have the "1" flag.
 *	If an error occurs, report it with report_error()
 *	and returns non-zero. Else, returns 0.
 */
int parse_command_line_options (int argc, const char *const *argv, int pass)
{
const opt_desc_t *o;

while (argc > 0)
   {
   int ignore;

   // Which option is this ?
   if (**argv != '-' && **argv != '+')
      {
      o = options;
      argc++;
      argv--;
      }
   else
      for (o = options + 1; ; o++)
	 {
	 if (o->opt_type == OPT_END)
	    {
	    report_error ("invalid option: \"%s\"", argv[0]);
	    return 1;
	    }
	 if (o->short_name != NULL && ! y_stricmp (argv[0]+1, o->short_name)
	  || o->long_name  != NULL && ! y_stricmp (argv[0]+1, o->long_name))
            break;
         }

   // If this option has the "1" flag but pass is not 1
   // or it doesn't but pass is 1, ignore it.
   ignore = (o->flags != NULL && strchr (o->flags, '1')) != (pass == 1);

   switch (o->opt_type)
      {
      case OPT_BOOLEAN:
	 if (argv[0][0] == '-')
	    {
	    if (o->data_ptr && ! ignore)
	       *((Bool *) (o->data_ptr)) = 1;
	    }
	 else
	    {
	    if (o->data_ptr && ! ignore)
	       *((Bool *) (o->data_ptr)) = 0;
	    }
	 break;
      case OPT_CONFIRM:
	 if (argc <= 1)
	    {
	    report_error ("missing argument after \"%s\"", argv[0]);
	    return 1;
	    }
         argv++;
         argc--;
	 if (o->data_ptr && ! ignore)
	    *((confirm_t *) o->data_ptr) = confirm_e2i (argv[0]);
	 break;
      case OPT_INTEGER:
	 if (argc <= 1)
	    {
	    report_error ("missing argument after \"%s\"", argv[0]);
	    return 1;
	    }
	 argv++;
	 argc--;
	 if (o->data_ptr && ! ignore)
	    *((int *) (o->data_ptr)) = atoi (argv[0]);
	 break;
      case OPT_STRINGBUF8:
         if (argc <= 1)
            {
	    report_error ("missing argument after \"%s\"", argv[0]);
	    return 1;
            }
	 argv++;
	 argc--;
	 if (o->data_ptr && ! ignore)
	    al_scps ((char *) o->data_ptr, argv[0], 8);
	 break;
      case OPT_STRINGPTR:
	 if (argc <= 1)
	    {
	    report_error ("missing argument after \"%s\"", argv[0]);
	    return 1;
	    }
	 argv++;
	 argc--;
	 if (o->data_ptr && ! ignore)
	    *((const char **) (o->data_ptr)) = argv[0];
	 break;
      case OPT_STRINGPTRACC:
	 if (argc <= 1)
	    {
	    report_error ("missing argument after \"%s\"", argv[0]);
	    return 1;
	    }
	 argv++;
	 argc--;
	 if (o->data_ptr && ! ignore)
	    append_item_to_list ((const char ***) o->data_ptr, argv[0]);
	 break;
      case OPT_STRINGPTRLIST:
	 if (argc <= 1)
	    {
	    report_error ("missing argument after \"%s\"", argv[0]);
	    return 1;
	    }
	 while (argc > 1 && argv[1][0] != '-' && argv[1][0] != '+')
	    {
	    argv++;
	    argc--;
	    if (o->data_ptr && ! ignore)
	       append_item_to_list ((const char ***) o->data_ptr, argv[0]);
	    }
	 break;
      default:
	 {
	 report_error ("unknown option type (BUG!)");
	 return 1;
	 }
      }
   argv++;
   argc--;
   }
return 0;
}


/*
 *	dump_parameters
 *	Print a list of the parameters with their current value.
 */
void dump_parameters (FILE *fd)
{
const opt_desc_t *o;
int desc_maxlen = 0;
int name_maxlen = 0;

for (o = options + 1; o->opt_type != OPT_END; o++)
   {
   int len = strlen (o->desc);
   desc_maxlen = al_amax (desc_maxlen, len);
   len = strlen (o->long_name);
   name_maxlen = al_amax (name_maxlen, len);
   }

for (o = options + 1; o->opt_type != OPT_END; o++)
   {
   fprintf (fd, "%-*s  %-*s  ",
      name_maxlen, o->long_name, desc_maxlen, o->desc);
   if (o->opt_type == OPT_BOOLEAN)
      fprintf (fd, "%s", *((int *)o->data_ptr) ? "enabled" : "disabled");
   else if (o->opt_type == OPT_CONFIRM)
      fputs (confirm_i2e (*((confirm_t *) o->data_ptr)), fd);
   else if (o->opt_type == OPT_STRINGBUF8)
      fprintf (fd, "\"%s\"", (char *) o->data_ptr);
   else if (o->opt_type == OPT_STRINGPTR)
      {
      if (o->data_ptr)
         fprintf (fd, "\"%s\"", *((char **) o->data_ptr));
      else
         fprintf (fd, "--none--");
      }
   else if (o->opt_type == OPT_INTEGER)
      fprintf (fd, "%d", *((int *)o->data_ptr));
   else if (o->opt_type == OPT_STRINGPTRACC
         || o->opt_type == OPT_STRINGPTRLIST)
      {
      if (o->data_ptr)
         {
         char **list;
         for (list = *((char ***) o->data_ptr); list && *list; list++)
            fprintf (fd, "\"%s\" ", *list);
         if (list == *((char ***) o->data_ptr))
            fprintf (fd, "--none--");
         }
      else
         fprintf (fd, "--none--");
      }
   fputc ('\n', fd);
   }
}


/*
 *	dump_command_line_options
 *	Print a list of all command line options (usage message).
 */
void dump_command_line_options (FILE *fd)
{
const opt_desc_t *o;
int desc_maxlen = 0;
int name_maxlen = 0;

for (o = options + 1; o->opt_type != OPT_END; o++)
   {
   int len;
   if (o->short_name == NULL)
      continue;
   len = strlen (o->desc);
   desc_maxlen = al_amax (desc_maxlen, len);
   len = strlen (o->long_name);
   name_maxlen = al_amax (name_maxlen, len);
   }

for (o = options; o->opt_type != OPT_END; o++)
   {
   if (o->short_name == NULL)
      continue;
#if ! defined Y_BGI
   if (o->flags != NULL && strchr (o->flags, 'b'))
      continue;
#endif
#if ! defined Y_X11
   if (o->flags != NULL && strchr (o->flags, 'x'))
      continue;
#endif
   if (o->short_name != NULL)
      fprintf (fd, " -%-2s ", o->short_name);
   else
      fprintf (fd, "     ");
   if (o->long_name != NULL)
      fprintf (fd, "-%-*s ", name_maxlen, o->long_name);
   else
      fprintf (fd, "%*s", name_maxlen + 2, "");
   switch (o->opt_type)
      {
      case OPT_BOOLEAN:       fprintf (fd, "            "); break;
      case OPT_CONFIRM:       fprintf (fd, "yes|no|ask  "); break;
      case OPT_STRINGBUF8:
      case OPT_STRINGPTR:
      case OPT_STRINGPTRACC:  fprintf (fd, "<string>    "); break;
      case OPT_INTEGER:       fprintf (fd, "<integer>   "); break;
      case OPT_STRINGPTRLIST: fprintf (fd, "<string> ..."); break;
      }
   fprintf (fd, " %s\n", o->desc);
   }
}


/*
 *	confirm_e2i
 *	Convert the external representation of a confirmation
 *	flag ("yes", "no", "ask", "ask_once") to the internal
 *	representation (YC_YES, YC_NO, YC_ASK, YC_ASK_ONCE or
 *	'\0' if none).
 */
static confirm_t confirm_e2i (const char *external)
{
if (external != NULL)
   {
   if (! strcmp (external, "yes"))
      return YC_YES;
   if (! strcmp (external, "no"))
      return YC_NO;
   if (! strcmp (external, "ask"))
      return YC_ASK;
   if (! strcmp (external, "ask_once"))
      return YC_ASK_ONCE;
   }
return YC_ASK;
}


/*
 *	confirm_i2e
 *	Convert the internal representation of a confirmation
 *	flag (YC_YES, YC_NO, YC_ASK, YC_ASK_ONCE) to the external
 *	representation ("yes", "no", "ask", "ask_once" or "?").
 */
static const char *confirm_i2e (confirm_t internal)
{
if (internal == YC_YES)
   return "yes";
if (internal == YC_NO)
   return "no";
if (internal == YC_ASK)
   return "ask";
if (internal == YC_ASK_ONCE)
   return "ask_once";
return "?";
}


/*
 *	append_item_to_list
 *	Append a string to a null-terminated string list
 */
static void append_item_to_list (const char ***list, const char *item)
{
int i;

i = 0;
if (*list != NULL)
   {
   /* count the number of elements in the list (last = null) */
   while ((*list)[i] != NULL)
      i++;
   /* expand the list */
   *list = (const char **) ResizeMemory (*list, (i + 2) * sizeof **list);
   }
else
   {
   /* create a new list */
   *list = (const char **) GetMemory (2 * sizeof **list);
   }
/* append the new element */
(*list)[i] = item;
(*list)[i + 1] = NULL;
}



