/*
 *	editloop.cc
 *	The main loop of the editor.
 *	BW & RQ sometime in 1993 or 1994.
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-1999 Andr� Majorel.

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
#include <assert.h>
#include "_edit.h"
#include "checks.h"
#include "dialog.h"
#include "drawmap.h"
#include "edisplay.h"
#include "editgrid.h"
#include "editloop.h"
#include "editobj.h"
#include "editzoom.h"
#include "entry.h"
#include "events.h"
#include "gfx.h"
#include "gfx2.h"	// show_character_set() show_pcolours()
#include "gotoobj.h"
#include "help2.h"
#include "l_flags.h"
#include "levels.h"
#include "menubar.h"
#include "menu.h"
#include "modpopup.h"
#include "prefer.h"
#include "selbox.h"
#include "selectn.h"
#include "spot.h"
#include "t_flags.h"
#include "t_spin.h"
#include "x_centre.h"

#ifdef Y_X11
#include <X11/Xlib.h>
#endif



extern Bool InfoShown;		/* should we display the info bar? */
#if defined Y_BGI && defined CIRRUS_PATCH
extern char HWCursor[];		/* Cirrus hardware cursor data */
#endif /* Y_BGI && CIRRUS_PATCH */
#ifdef DIALOG
#endif


static int menubar_out_y1;	/* FIXME */


/* prototypes of private functions */
static int SortLevels (const void *item1, const void *item2);

/*
   select a level
*/

const char *SelectLevel (int levelno)
{
MDirPtr dir;
static char name[9]; /* AYM it was [7] previously */
char **levels;
int n = 0;           /* number of levels in the dir. that match */

get_levels_that_match:
for (dir = MasterDir; dir; dir = dir->next)
   {
   if (levelname2levelno (dir->dir.name) > 0
    && (levelno==0 || levelname2levelno (dir->dir.name) % 100 == levelno))
      {
      if (n == 0)
	 levels = (char **) GetMemory (sizeof (char *));
      else
	 levels = (char **) ResizeMemory (levels, (n + 1) * sizeof (char *));
      levels[n] = dir->dir.name;
      n++;
      }
   }
if (n == 0 && levelno != 0)  /* In case no level matched levelno */
   {
   levelno = 0;               /* List ALL levels instead */
   goto get_levels_that_match;
   }
/* So that InputNameFromList don't fail if you mix EnMn's and MAPnn's */
qsort (levels, n, sizeof (char *), SortLevels);
al_scps (name, levels[0], sizeof name - 1);
if (n == 1)
   return name;
InputNameFromList (-1, -1, "Select a level to edit :", n, levels, name);
FreeMemory (levels);
return name;
}


/*
   compare 2 level names (for sorting)
*/

static int SortLevels (const void *item1, const void *item2)
{
/* FIXME should probably use stricmp() instead */
return strcmp (*((const char * const *) item1),
               *((const char * const *) item2));
}


/*
   get the name of the new wad file (returns NULL on Esc)
*/

char *GetWadFileName (const char *levelname)
{
#define BUFSZ 79
char *outfile = (char *) GetMemory (BUFSZ + 1);
WadPtr wad;

/* get the file name */
if (! fncmp (Level->wadfile->filename, MainWad))
   {
   al_scpslower (outfile, levelname, BUFSZ);
   al_saps (outfile, ".wad", BUFSZ);
   }
else
   strcpy (outfile, Level->wadfile->filename);
do
   InputFileName (-1, -1, "Name of the new wad file:", BUFSZ, outfile);
while (! fncmp (outfile, MainWad));
/* escape */
if (outfile[0] == '\0')
   {
   FreeMemory (outfile);
   return 0;
   }
/* if the wad file already exists, rename it to "*.bak" */
for (wad = WadFileList; wad; wad = wad->next)
   if (! fncmp (outfile, wad->filename))
   {
   al_fdrv_t drv;
   al_fpath_t path;
   al_fbase_t base;

   al_fana (wad->filename, drv, path, base, 0);
   sprintf (wad->filename, "%s%s%s.bak", drv, path, base);
   /* Need to close, then reopen: problems with SHARE.EXE */
   fclose (wad->fd);
   if (rename (outfile, wad->filename) < 0)
      {
      if (remove (wad->filename))
	 fatal_error ("could not delete file \"%s\"", wad->filename);
      if (rename (outfile, wad->filename))
	 fatal_error ("could not rename \"%s\" to \"%s\"", outfile,
            wad->filename);
      }
   wad->fd = fopen (wad->filename, "rb");
   if (! wad->fd)
      fatal_error ("could not reopen file \"%s\"", wad->filename);
   break;
   }
return outfile;
}


// A table of the modes in the editor.
typedef struct
   {
   i8 obj_type;		// Corresponding object type
   i8 item_no;		// # of item to tick in the "View" menu
   i8 menu_no;		// # of flavour of the "Misc op." menu
   } editmode_t;

const int NB_MODES = 4;

static const editmode_t modes[NB_MODES] =
   {
   { OBJ_THINGS,   0, MBM_MISC_T },
   { OBJ_LINEDEFS, 1, MBM_MISC_L },
   { OBJ_VERTICES, 2, MBM_MISC_V },
   { OBJ_SECTORS,  3, MBM_MISC_S },
   };


/*
 *	obj_type_to_mode_no
 *	Return the # of the mode that has this <obj_type>
 */
static int obj_type_to_mode_no (int obj_type)
{
int n;
for (n = 0; n < NB_MODES; n++)
   if (modes[n].obj_type == obj_type)
      break;
if (n == NB_MODES)
   fatal_error ("no mode for obj_type %d", obj_type);
return n;
}


/*
  the editor main loop
*/

void EditorLoop (const char *levelname) /* SWAP! */
{
edit_t e;
/* FIXME : all these variables should be moved to edit_t : */
int    RedrawMap;
Bool   DragObject = 0;
int    oldbuttons;

Bool   StretchSelBox = 0;

int object;  /* The object under the pointer */

memset (&e, 0, sizeof e);	/* Catch-all */
e.move_speed         = 20;
e.extra_zoom         = 0;
// If you change this, don't forget to change
// the initialisation of the menu bar below.
e.obj_type           = OBJ_THINGS;
e.grid_step          = 128;
e.grid_step_min      = GridMin;
e.grid_step_max      = GridMax;
e.grid_step_locked   = 0;
e.grid_shown         = 1;
e.grid_snap          = 1;
e.infobar_shown      = InfoShown;
e.show_object_numbers= 0;
e.rulers_shown       = 0;
e.click_obj_no       = OBJ_NO_NONE;
e.click_obj_type     = -1;
e.click_ctrl         = 0;
e.highlight_obj_no   = OBJ_NO_NONE;
e.highlight_obj_type = -1;
e.Selected           = 0;
e.selbox             = new selbox_c;
e.edisplay           = new edisplay_c (&e);
e.menubar            = new menubar_c;
e.spot               = new spot_c;
e.modpopup           = new modpopup_c;
e.modal              = '\0';

MadeChanges = 0;
MadeMapChanges = 0;
if (InitialScale < 1)
   InitialScale = 1;
else if (InitialScale > 20)
   InitialScale = 20;
edit_set_zoom (&e, 1.0 / InitialScale);
CenterMapAroundCoords ((MapMinX + MapMaxX) / 2, (MapMinY + MapMaxY) / 2);
if (UseMouse)
   {
   ResetMouseLimits ();
   SetMouseCoords (is.x, is.y);
   ShowMousePointer ();
#if defined Y_BGI && defined CIRRUS_PATCH
   if (CirrusCursor)
      {
      SetHWCursorMap (HWCursor);
      SetHWCursorPos (is.x, is.y);
      }
#endif /* Y_BGI && CIRRUS_PATCH */
   oldbuttons = 0;
   }
else
  FakeCursor = 1;

/* Create the menu bar */
{
e.menubar->compute_menubar_coords (0, 0, ScrMaxX, ScrMaxY);

e.mb_menu[MBM_FILE] = new menu_c (NULL,
   "Save",       0, YK_F2, 0,
   "Save as...", 5, YK_F3, 0,
   "Print",      0, -1,    MEN_GRAY,
   "Quit",       0, 'q',   0,
   NULL);

e.mb_menu[MBM_EDIT] = new menu_c (NULL,
   "Copy object(s)",   0, 'o',    0,
   "Add object",       0, YK_INS, 0,
   "Delete object(s)", 0, YK_DEL, 0,
   "Preferences",      0, YK_F5,  0,
   NULL);

// If you change the order of modes here, don't forget
// to modify the <modes> array.
e.mb_menu[MBM_VIEW] = new menu_c (NULL,
   "Things",              0, 't',        MEN_TICK, e.obj_type==OBJ_THINGS,
   "Linedefs & sidedefs", 0, 'l',        MEN_TICK, e.obj_type==OBJ_LINEDEFS,
   "Vertices",            0, 'v',        MEN_TICK, e.obj_type==OBJ_VERTICES,
   "Sectors",             0, 's',        MEN_TICK, e.obj_type==OBJ_SECTORS,
   "Next mode",           0, YK_TAB,     0,
   "Prev mode",           0, YK_BACKTAB, 0,
   "Zoom in",             5, '+',        0,
   "Zoom out",            5, '-',        0,
   "Extra zoom",          6, ' ',        MEN_TICK, !! e.extra_zoom,
   "3D preview",          0, '3',        MEN_GRAY,
   NULL);

e.mb_menu[MBM_SEARCH] = new menu_c (NULL,
   "Find/change",       0, YK_F4, MEN_GRAY,
   "Repeat last find",  0, -1,    MEN_GRAY,
   "Next object",       0, 'n',   0,
   "Prev object",       0, 'p',   0,
   "Jump to object...", 0, 'j',   0,
   NULL);

e.mb_menu[MBM_MISC_L] = new menu_c ("Misc. operations",
   "Find first free tag number",            16, -1, 0,
   "Rotate and scale linedefs",              0, -1, 0,
   "Split linedef (add new vertex)",        23, -1, 0,
   "Split linedefs and sector",              0, -1, 0,
   "Delete linedefs and join sectors",       0, -1, 0,
   "Flip linedef",                           0, -1, 0,
   "Swap sidedefs",                          1, -1, 0,
   "Align textures (Y offset)",             16, -1, 0,
   "Align textures (X offset)",             16, -1, 0,
   "Remove 2nd sidedef (make single-sided)", 7, -1, 0,
   "Make rectangular nook (32x16)",         17, -1, 0,
   "Make rectangular boss (32x16)",         17, -1, 0,
   "Set length (move 1st vertex)...",        4, -1, 0,
   "Set length (move 2nd vertex)...",       -1, -1, 0,
   "Unlink 1st sidedef",                     0, -1, 0,
   "Unlink 2nd sidedef",                    -1, -1, 0,
   NULL);

e.mb_menu[MBM_MISC_S] = new menu_c ("Misc. operations",
   "Find first free tag number",        16, -1, 0,
   "Rotate and scale sectors",           0, -1, 0,
   "Make door from sector",              5, -1, 0,
   "Make lift from sector",              5, -1, 0,
   "Distribute sector floor heights",   18, -1, 0,
   "Distribute sector ceiling heights", 18, -1, 0,
   "Raise or lower sectors...",          9, -1, 0,
   "Brighten or darken sectors...",      0, -1, 0,
   //"Unlink area",                        0, -1, 0,
   NULL);

e.mb_menu[MBM_MISC_T] = new menu_c ("Misc. operations",
   "Find first free tag number",        16,  -1, 0,
   "Rotate and scale things",            0,  -1, 0,
   "Spin things 45� clockwise",          0, 'x', 0,
   "Spin things 45� counter-clockwise", 16, 'w', 0,
   NULL);

e.mb_menu[MBM_MISC_V] = new menu_c ("Misc. operations",
   "Find first free tag number",      16, -1, 0,
   "Rotate and scale vertices",        0, -1, 0,
   "Delete vertex and join linedefs",  0, -1, 0,
   "Merge several vertices into one",  0, -1, 0,
   "Add a linedef and split sector",  18, -1, 0,
   NULL);

e.mb_menu[MBM_OBJECTS] = new menu_c ("Insert a pre-defined object",
   "Rectangle",         0, -1, 0,
   "Polygon (N sides)", 0, -1, 0,
   NULL);

e.mb_menu[MBM_CHECK] = new menu_c ("Check level consistency",
   "Number of objects",                 0,  -1, 0,
   "Check if all sectors are closed",   13, -1, 0,
   "Check all cross-references",        10, -1, 0,
   "Check for missing textures",        10, -1, 0,
   "Check texture names",               6,  -1, 0,
   NULL);

e.mb_menu[MBM_HELP] = new menu_c (NULL,
   "Keyboard & mouse...", 0, YK_F1, 0,
   "Info bar",            0, 'i',   MEN_TICK, e.infobar_shown,
   "About Yadex...",      0, YK_ALT + 'a', 0,
   NULL);

e.mb_ino[MBI_FILE] =
   e.menubar->add_item ("File",    0, 0, e.mb_menu[MBM_FILE]   );
e.mb_ino[MBI_EDIT] =
   e.menubar->add_item ("Edit",    0, 0, e.mb_menu[MBM_EDIT]   );
e.mb_ino[MBI_VIEW] =
   e.menubar->add_item ("View",    0, 0, e.mb_menu[MBM_VIEW]  );
e.mb_ino[MBI_SEARCH] =
   e.menubar->add_item ("Search",  0, 0, e.mb_menu[MBM_SEARCH] );
e.mb_ino[MBI_MISC] =
   e.menubar->add_item ("Misc",    0, 0, e.mb_menu[MBM_MISC_T] );
e.mb_ino[MBI_OBJECTS] =
   e.menubar->add_item ("Objects", 0, 0, e.mb_menu[MBM_OBJECTS]);
e.mb_ino[MBI_CHECK] =
   e.menubar->add_item ("Check",   0, 0, e.mb_menu[MBM_CHECK]  );
e.mb_ino[MBI_HELP] =
   e.menubar->add_item ("Help",    0, 1, e.mb_menu[MBM_HELP]   );

menubar_out_y1 = 2 * BOX_BORDER + 2 * NARROW_VSPACING + FONTH - 1;  // FIXME
}

// FIXME this should come from the .ygd
// instead of being hard-coded.
menu_c *menu_linedef_flags = new menu_c (NULL,
   "Impassable",		0, -1, 0,
   "Monsters cannot cross",	0, -1, 0,
   "Double-sided",		0, -1, 0,
   "Upper texture unpegged",	0, -1, 0,
   "Lower texture unpegged",	0, -1, 0,
   "Secret (shown as normal)",	0, -1, 0,
   "Blocks sound",		0, -1, 0,
   "Invisible",			2, -1, 0,
   "Always shown",		0, -1, 0,
   "Pass through",		0, -1, 0,  // Boom extension
   NULL);

menu_c *menu_thing_flags = new menu_c (NULL,
   "Easy",			0, -1, 0,
   "Medium",			4, -1, 0,
   "Hard",			0, -1, 0,
   "Deaf",			0, -1, 0,
   "Multiplayer",		0, -1, 0,
   "Not in DM   (Boom)",	0, -1, 0,  // Boom extension
   "Not in coop (Boom)", 	7, -1, 0,  // Boom extension
   NULL);

/* AYM 1998-06-22
   This is the big mean loop. I organized it in three main steps :

   1. Update the display according to the current state of affairs.
   2. Wait for the next event (key press, mouse click/motion, etc.).
   3. Process this event and change states as appropriate.

   This piece of code remembers a lot of state in various variables and
   flags. Hope you can work your way through it. If you don't, don't
   hesitate to ask me. You never know, I might help you to be even more
   confused. ;-) */

for (RedrawMap = 1; ; RedrawMap = 0)
   {
   int motion;

   /*
    *  Step 1 -- Do all the displaying work
    */

   // Hold refresh until all events are processed
   // if (! has_event () && ! has_input_event ())
   //   {
      if (is_obj (e.highlight_obj_no))				// FIXME
	 e.edisplay->highlight_object (e.highlight_obj_no);	// Should
      else							// be in
	 e.edisplay->forget_highlight ();			// edisplay_c !

   if (is.in_window)
      e.spot->set (edit_mapx_snapped (&e, e.pointer_x),
                   edit_mapy_snapped (&e, e.pointer_y));
   else
      e.spot->unset ();

      e.edisplay->refresh ();
      /* The display is now up to date */
   //   }


   /*
    *  Step 2 -- Get the next event
    */

   if (has_key_press_event ())
      is.key = get_event ();
   else
      get_input_status ();
   e.pointer_in_window = is.in_window;
   if (is.in_window)
      {
      /* AYM 1998-07-04
	 If the map coordinates of the pointer have changed,
	 generate a pointer motion event. I don't like to do
	 that but it makes things much simpler elsewhere. */
      if (MAPX (is.x) != e.pointer_x || MAPY (is.y) != e.pointer_y)
	 motion = 1;
      else
	 motion = 0;

      e.pointer_x = MAPX (is.x);
      e.pointer_y = MAPY (is.y);
      object = GetCurObject (e.obj_type, e.pointer_x, e.pointer_y, 20);
      }

   /*
    *  Step 3 -- Process the event
    *  This section is (should be) a long list of elif's
    */

   /*
    *	Step 3.1A -- If a pull-down menu is "on",
    *	try to make it process the event.
    */

   if (e.menubar->pulled_down () >= 0)
      {
      int menu_no = e.menubar->pulled_down ();
      menu_c *menu = e.menubar->get_menu (menu_no);
      int r = menu->process_event (&is);

      if (r == MEN_CANCEL)
	 {
	 e.menubar->pull_down (-1);
	 e.menubar->highlight (-1);
         goto done;
	 }

      // The event was understood and processed normally
      // by the menu event handler so we're done.
      else if (r == MEN_OTHER)
	 goto done;

      // The event was not understood by the menu event
      // handler so let's see what the normal event handler
      // can do with it.
      else if (r == MEN_INVALID)
	 ;

      else
	 {
	 e.menubar->pull_down (-1);
	 e.menubar->highlight (-1);
	 if (menu_no == e.mb_ino[MBI_MISC])
	    {
	    if (e.Selected)
	       MiscOperations (e.obj_type, &e.Selected, r + 1);
	    else
	       {
	       if (is_obj (e.highlight_obj_no))
		  SelectObject (&e.Selected, e.highlight_obj_no);
	       MiscOperations (e.obj_type, &e.Selected, r + 1);
	       if (is_obj (e.highlight_obj_no))
		  UnSelectObject (&e.Selected, e.highlight_obj_no);
	       }
	    e.highlight_obj_no = OBJ_NO_NONE;
	    DragObject = 0;
	    StretchSelBox = 0;
	    RedrawMap = 1;
	    }
	 else if (menu_no == e.mb_ino[MBI_OBJECTS])
            {
	    /* code duplicated from 'F9' - I hate to do that */
	    int savednum = NumLineDefs;
	    InsertStandardObject (e.pointer_x, e.pointer_y, r + 1);
	    if (NumLineDefs > savednum)
	       {
	       ForgetSelection (&e.Selected);
	       e.obj_type = OBJ_LINEDEFS;
	       for (int i = savednum; i < NumLineDefs; i++)
		  SelectObject (&e.Selected, i);
	       e.highlight_obj_no = OBJ_NO_NONE;
	       DragObject = 0;
	       StretchSelBox = 0;
	       }
	    RedrawMap = 1;
            }
	 else if (menu_no == e.mb_ino[MBI_CHECK])
	    {
	    if (r == 0)
	       Statistics ();
	    else if (r == 1)
	       CheckSectors ();
	    else if (r == 2)
	       CheckCrossReferences ();
	    else if (r == 3)
	       CheckTextures ();
	    else if (r == 4)
	       CheckTextureNames ();
	    }
	 else
	    send_event (menu->last_shortcut_key ());
         goto done;
	 }
      }

   /*
    *	Step 3.1B -- If the "Misc operations" popup is on,
    *	try to make it process the event.
    */

   else if (e.modpopup->get () == e.mb_menu[MBM_MISC_L]
      || e.modpopup->get () == e.mb_menu[MBM_MISC_S]
      || e.modpopup->get () == e.mb_menu[MBM_MISC_T]
      || e.modpopup->get () == e.mb_menu[MBM_MISC_V])
      {
      int r = (e.modpopup->get ())->process_event (&is);

      // [Esc] or click outside the popup menu. Close it.
      if (r == MEN_CANCEL
         || r == MEN_INVALID && is.key == YE_BUTL_PRESS)
	 {
         e.modpopup->unset ();
         goto done2;
	 }

      // The event was understood and processed normally
      // by the menu event handler so we're done.
      else if (r == MEN_OTHER)
	 goto done2;

      // The event was not understood by the menu event
      // handler so let's see what the normal event handler
      // can do with it.
      else if (r == MEN_INVALID)
	 ;

      else
	 {
         e.modpopup->unset ();
	 if (e.Selected)
	    MiscOperations (e.obj_type, &e.Selected, r + 1);
	 else
	    {
	    if (is_obj (e.highlight_obj_no))
	       SelectObject (&e.Selected, e.highlight_obj_no);
	    MiscOperations (e.obj_type, &e.Selected, r + 1);
	    if (is_obj (e.highlight_obj_no))
	       UnSelectObject (&e.Selected, e.highlight_obj_no);
	    }
	 e.highlight_obj_no = OBJ_NO_NONE;
	 DragObject = 0;
	 StretchSelBox = 0;
	 goto done2;
	 }
      }

   /*
    *	Step 3.1C -- If the "Insert standard object" popup is on,
    *	try to make it process the event.
    */

   else if (e.modpopup->get () == e.mb_menu[MBM_OBJECTS])
      {
      int r = (e.modpopup->get ())->process_event (&is);

      // [Esc] or click outside the popup menu. Close it.
      if (r == MEN_CANCEL
         || r == MEN_INVALID && is.key == YE_BUTL_PRESS)
	 {
         e.modpopup->unset ();
         goto done2;
	 }

      // The event was understood and processed normally
      // by the menu event handler so we're done.
      else if (r == MEN_OTHER)
	 goto done2;

      // The event was not understood by the menu event
      // handler so let's see what the normal event handler
      // can do with it.
      else if (r == MEN_INVALID)
	 ;

      else
	 {
         e.modpopup->unset ();
	 int savednum = NumLineDefs;
	 InsertStandardObject (e.pointer_x, e.pointer_y, r + 1);
	 if (NumLineDefs > savednum)
	    {
	    ForgetSelection (&e.Selected);
	    e.obj_type = OBJ_LINEDEFS;
	    for (int i = savednum; i < NumLineDefs; i++)
	       SelectObject (&e.Selected, i);
	    e.highlight_obj_no = OBJ_NO_NONE;
	    DragObject = 0;
	    StretchSelBox = 0;
	    }
         goto done2;
	 }
      }

   /*
    *	Step 3.1D -- if the "Set/toggle/clear" linedef flag popup
    *	is on, try to make it process the event.
    */
   else if (e.modpopup->get () == menu_linedef_flags)
      {
      int r = (e.modpopup->get ())->process_event (&is);

      // [Esc] or click outside the popup menu. Close it.
      if (r == MEN_CANCEL
         || r == MEN_INVALID && is.key == YE_BUTL_PRESS)
	 {
         e.modpopup->unset ();
         goto done2;
	 }

      // The event was understood and processed normally
      // by the menu event handler so we're done.
      else if (r == MEN_OTHER)
	 goto done2;

      // The event was not understood by the menu event
      // handler so let's see what the normal event handler
      // can do with it.
      else if (r == MEN_INVALID)
	 ;

      else
	 {
         int op;
         e.modpopup->unset ();
         if (e.modal == 's')
            op = YO_SET;
         else if (e.modal == 't')
            op = YO_TOGGLE;
         else if (e.modal == 'c')
            op = YO_CLEAR;
         else
            fatal_error ("modal=%02X", e.modal);
         if (! e.Selected)
            {
            SelectObject (&e.Selected, e.highlight_obj_no);
            frob_linedefs_flags (e.Selected, op, r);
            UnSelectObject (&e.Selected, e.highlight_obj_no);
            }
         else
            {
            frob_linedefs_flags (e.Selected, op, r);
            }
         goto done2;
	 }
      }

   /*
    *	Step 3.1E -- if the "Set/toggle/clear" thing flag popup
    *	is on, try to make it process the event.
    */
   else if (e.modpopup->get () == menu_thing_flags)
      {
      int r = (e.modpopup->get ())->process_event (&is);

      // [Esc] or click outside the popup menu. Close it.
      if (r == MEN_CANCEL
         || r == MEN_INVALID && is.key == YE_BUTL_PRESS)
	 {
         e.modpopup->unset ();
         goto done2;
	 }

      // The event was understood and processed normally
      // by the menu event handler so we're done.
      else if (r == MEN_OTHER)
	 goto done2;

      // The event was not understood by the menu event
      // handler so let's see what the normal event handler
      // can do with it.
      else if (r == MEN_INVALID)
	 ;

      else
         {
         int op;
         e.modpopup->unset ();
         if (e.modal == 's')
            op = YO_SET;
         else if (e.modal == 't')
            op = YO_TOGGLE;
         else if (e.modal == 'c')
            op = YO_CLEAR;
         else
            fatal_error ("modal=%02X", e.modal);
         if (! e.Selected)
            {
            SelectObject (&e.Selected, e.highlight_obj_no);
            frob_things_flags (e.Selected, op, r);
            UnSelectObject (&e.Selected, e.highlight_obj_no);
            }
         else
            {
            frob_things_flags (e.Selected, op, r);
            }
         goto done2;
	 }
      }

   /*
    *	Step 3.2 -- "Normal" event handling
    */

   /*
    *	Step 3.2.1 -- Non keyboard events
    */

   if (is.key == YE_EXPOSE)
      {
      RedrawMap = 1;
      goto done2;
      }

   else if (is.key == YE_RESIZE)
      {
      SetWindowSize (is.width, is.height);
      e.menubar->compute_menubar_coords (0, 0, ScrMaxX, ScrMaxY);
      RedrawMap = 1;
      goto done2;
      }

   // To prevent normal handling when a popup menu is on.
   if (e.modpopup->get () != 0)
      {
      goto done2;
      }

   /*
    *	Step 3.2.2 -- Mouse events
    */


   // Clicking on an item of the menu bar
   // pulls down the corresponding menu.
   else if (is.key == YE_BUTL_PRESS
      && e.menubar->is_on_menubar_item (is.x, is.y) >= 0)
      {
      int itemno;

      e.click_obj_no = OBJ_NO_NONE;
      itemno = e.menubar->is_on_menubar_item (is.x, is.y);
      if (itemno >= 0)
         e.menubar->pull_down (itemno);
      else
         Beep ();
      goto done;
      }

   /* Clicking on an empty space starts a new selection box.
      Unless [Ctrl] is pressed, it also clears the current selection. */
   else if (is.key == YE_BUTL_PRESS && ! is_obj (object))
      {
      e.menubar->highlight (-1);  // Close any open menu
      e.click_obj_no = OBJ_NO_CANVAS;
      e.click_ctrl   = is.ctrl;
      if (! is.ctrl)
         {
         ForgetSelection (&e.Selected);
         RedrawMap = 1;
         }
      e.selbox->set_1st_corner (e.pointer_x, e.pointer_y);
      e.selbox->set_2nd_corner (e.pointer_x, e.pointer_y);
      }

   /* Clicking on an unselected object unselects
      everything but that object. Additionally,
      we write the number of the object in case
      the user is about to drag it. */
   else if (is.key == YE_BUTL_PRESS && ! is.ctrl
      && ! IsSelected (e.Selected, object))
      {
      e.menubar->highlight (-1);  // Close any open menu
      e.click_obj_no   = object;
      e.click_obj_type = e.obj_type;
      e.click_ctrl     = 0;
      e.click_time     = is.time;
      ForgetSelection (&e.Selected);
      SelectObject (&e.Selected, object);
      /* I don't like having to do that */
      if (e.obj_type == OBJ_THINGS && is_obj (object))
	 MoveObjectsToCoords (e.obj_type, 0,
            Things[object].xpos, Things[object].ypos, 0);
      else if (e.obj_type == OBJ_VERTICES && is_obj (object))
	 MoveObjectsToCoords (e.obj_type, 0,
            Vertices[object].x, Vertices[object].y, 0);
      else
	 MoveObjectsToCoords (e.obj_type, 0,
            e.pointer_x, e.pointer_y, e.grid_snap ? e.grid_step : 0);
      RedrawMap = 1;
      }

   /* Second click of a double click on an object */
   else if (is.key == YE_BUTL_PRESS && ! is.ctrl
      && IsSelected (e.Selected, object)
      && object == e.click_obj_no
      && e.obj_type == e.click_obj_type
      && is.time - e.click_time <= double_click_timeout)
      {
      // Very important! If you don't do that, the release of the
      // click that closed the properties menu will drag the object.
      e.click_obj_no = OBJ_NO_NONE;
      send_event (YK_RETURN);
      goto done;
      }

   /* Clicking on a selected object does nothing ;
      the user might want to drag the selection. */
   else if (is.key == YE_BUTL_PRESS && ! is.ctrl
      && IsSelected (e.Selected, object))
      {
      e.menubar->highlight (-1);  // Close any open menu
      e.click_obj_no   = object;
      e.click_obj_type = e.obj_type;
      e.click_ctrl     = 0;
      e.click_time     = is.time;
      /* I don't like having to do that */
      if (e.obj_type == OBJ_THINGS && is_obj (object))
	 MoveObjectsToCoords (e.obj_type, 0,
            Things[object].xpos, Things[object].ypos, 0);
      else if (e.obj_type == OBJ_VERTICES && is_obj (object))
	 MoveObjectsToCoords (e.obj_type, 0,
            Vertices[object].x, Vertices[object].y, 0);
      else
	 MoveObjectsToCoords (e.obj_type, 0,
            e.pointer_x, e.pointer_y, e.grid_snap ? e.grid_step : 0);
      }

   /* Clicking on selected object with [Ctrl] pressed unselects it.
      Clicking on unselected object with [Ctrl] pressed selects it. */
   else if (is.key == YE_BUTL_PRESS && is.ctrl && is_obj (object))
      {
      e.menubar->highlight (-1);  // Close any open menu
      e.click_obj_no   = object;
      e.click_obj_type = e.obj_type;
      e.click_ctrl     = 1;
      if (IsSelected (e.Selected, object))
	UnSelectObject (&e.Selected, object);
      else
	SelectObject (&e.Selected, object);
      RedrawMap = 1;
      }

   /* Clicking anywhere else closes the pull-down menus. */
   else if (is.key == YE_BUTL_PRESS)
      e.menubar->highlight (-1);  // Close any open menu

   /* Releasing the button while there was a selection box
      causes all the objects within the box to be selected. */
   else if (is.key == YE_BUTL_RELEASE && e.click_obj_no == OBJ_NO_CANVAS)
      {
      int x1, y1, x2, y2;
      e.selbox->get_corners (&x1, &y1, &x2, &y2);
      SelectObjectsInBox (&e.Selected, e.obj_type, x1, y1, x2, y2);
      e.selbox->unset_corners ();
      RedrawMap = 1;
      }

   /* Releasing the button while dragging : drop the selection. */
   else if (is.key == YE_BUTL_RELEASE && is_obj (e.click_obj_no))
      {
      if (AutoMergeVertices (&e.Selected, e.obj_type, 'm'))
         RedrawMap = 1;
      }

   // Moving the pointer with the left button pressed
   // after clicking on an item of the menu bar : pull
   // down menus as the pointer passes over them.
   else if (is.key == YE_MOTION
      && is.butl
      && e.click_obj_no == OBJ_NO_NONE)
      {
      int itemno = e.menubar->is_on_menubar_item (is.x, is.y);
      if (itemno >= 0)
         e.menubar->pull_down (itemno);
      goto done;
      }

   /* Moving the pointer with the left button pressed
      and a selection box exists : move the second
      corner of the selection box. */
   else if ((is.key == YE_MOTION || motion)
      && is.butl && e.click_obj_no == OBJ_NO_CANVAS)
      {
      e.selbox->set_2nd_corner (e.pointer_x, e.pointer_y);
      }

   /* Moving the pointer with the left button pressed
      but no selection box exists and [Ctrl] was not
      pressed when the button was pressed :
      drag the selection. */
   else if ((is.key == YE_MOTION || motion)
      && is.butl
      && is_obj (e.click_obj_no)
      && ! e.click_ctrl)
      {
      if (! e.Selected)
	 {
	 SelectObject (&e.Selected, e.click_obj_no);
	 if (MoveObjectsToCoords (e.obj_type, e.Selected,
            e.pointer_x, e.pointer_y, e.grid_snap ? e.grid_step : 0))
	    RedrawMap = 1;
	 ForgetSelection (&e.Selected);
	 }
      else
	 if (MoveObjectsToCoords (e.obj_type, e.Selected,
            e.pointer_x, e.pointer_y, e.grid_snap ? e.grid_step : 0))
	    RedrawMap = 1;
      }

   /* AYM : added is.in_window */
   if (is.in_window && ! is.butl && ! is.shift)
      {
      /* Check if there is something near the pointer */
      e.highlight_obj_no   = object;
      e.highlight_obj_type = e.obj_type;
      }

   /*
    *	Step 3.2.3 -- Keyboard events
    */

   if (event_is_key (is.key)
      || is.key == YE_WHEEL_UP
      || is.key == YE_WHEEL_DOWN)
      {
      if (is.key == YK_LEFT && e.menubar->highlighted () >= 0)
         {
         int new_item = e.menubar->highlighted () - 1;
         if (new_item < 0)
            new_item = e.mb_ino[MBI_HELP];
         e.menubar->pull_down (new_item);
         RedrawMap = 1;
         }

      else if (is.key == YK_RIGHT && e.menubar->highlighted () >= 0)
         {
         int new_item = e.menubar->highlighted () + 1;
         if (new_item > e.mb_ino[MBI_HELP])
            new_item = 0;
         e.menubar->pull_down (new_item);
         RedrawMap = 1;
         }

      else if (is.key == YK_ALT + 'f')
         e.menubar->pull_down (e.mb_ino[MBI_FILE]);

      else if (is.key == YK_ALT + 'e')
         e.menubar->pull_down (e.mb_ino[MBI_EDIT]);

      else if (is.key == YK_ALT + 's')
	 e.menubar->pull_down (e.mb_ino[MBI_SEARCH]);

      else if (is.key == YK_ALT + 'v')
	 e.menubar->pull_down (e.mb_ino[MBI_VIEW]);

      else if (is.key == YK_ALT + 'm')
	 e.menubar->pull_down (e.mb_ino[MBI_MISC]);

      else if (is.key == YK_ALT + 'o')
	 e.menubar->pull_down (e.mb_ino[MBI_OBJECTS]);

      else if (is.key == YK_ALT + 'c')
	 e.menubar->pull_down (e.mb_ino[MBI_CHECK]);

      else if (is.key == YK_ALT + 'h')
	 e.menubar->pull_down (e.mb_ino[MBI_HELP]);

      /* user wants to exit */
      else if (is.key == 'q')
         {
	 ForgetSelection (&e.Selected);
	 if (CheckStartingPos ())
	    {
	    if (Registered && MadeChanges)
	       {
	       char *outfile;

	       outfile = GetWadFileName (levelname);
	       if (outfile)
	          {
		  SaveLevelData (outfile);
		  break;
	          }
	       }
	    else
	       break;
	    }
	 RedrawMap = 1;
         }
      else if (is.key == '\f')  /* Ctrl-L : force redraw */
	{
	RedrawMap = 1;
	}
      else if (is.key == YK_ESC) /* 'Esc' */
         {
	 if (DragObject)
	    DragObject = 0;
	 else if (StretchSelBox)
	    StretchSelBox = 0;
	 else
	    {
	    ForgetSelection (&e.Selected);
	    if (!MadeChanges
	     || Confirm (-1, -1, "You have unsaved changes."
				"  Do you really want to quit?", 0))
	       break;
	    RedrawMap = 1;
	    }
         }

      /* user is lost */
      else if (is.key == YK_F1) /* 'F1' */
         {
	 DisplayHelp ();
	 RedrawMap = 1;
         }

      /* pop up the About... window */
      else if (is.key == YK_ALT + 'a')  /* Alt-A */
         {
         about_yadex ();
         RedrawMap = 1;
         }

      /* user wants to save a screen shot into yadex.gif */
      else if (is.key == YK_F1 + YK_SHIFT)  /* shift-F1 */
	 {
	 ScreenShot ();
	 }

      /* user wants to save the level data */
      else if (is.key == YK_F2 && Registered) /* 'F2' */
         {
	 char *outfile;

	 if (CheckStartingPos ())
	    {
	    outfile = GetWadFileName (levelname);
	    if (outfile)
	       SaveLevelData (outfile);
	    }
	 RedrawMap = 1;
         }

      /* user wants to save and change the episode and mission numbers */
      else if (is.key == YK_F3 && Registered) /* 'F3' */
         {
	 char *outfile;
	 int   m;
	 MDirPtr newLevel, oldl, newl;

	 if (CheckStartingPos ())
	    {
	    outfile = GetWadFileName (levelname);
	    if (outfile)
	       {
	       const char *newlevelname;
	       newlevelname = SelectLevel (0);
	       if (newlevelname && strcmp (newlevelname, levelname))
	          {
		  /* horrible but it works... */
		  newLevel = FindMasterDir (MasterDir, newlevelname);
		  oldl = Level;
		  newl = newLevel;
		  for (m = 0; m < 11; m++)
		     {
		     newl->wadfile = oldl->wadfile;
		     if (m > 0)
			newl->dir = oldl->dir;
		     /*
		     if (!fncmp (outfile, oldl->wadfile->filename))
		        {
			oldl->wadfile = WadFileList;
			oldl->dir = lost...
		        }
		     */
		     oldl = oldl->next;
		     newl = newl->next;
	   	     }
		  Level = newLevel;
	          }
	       SaveLevelData (outfile);
	       }
	    }
	 RedrawMap = 1;
         }

      // [F5]: pop up the "Preferences" menu
      else if (is.key == YK_F5
         && e.menubar->highlighted () < 0)
         {
	 Preferences (-1, -1);
	 RedrawMap = 1;
         }

      // [a]: pop up the "Set flag" menu
      else if (is.key == 'a'
         && e.menubar->highlighted () < 0
         && (e.Selected || is_obj (e.highlight_obj_no)))
         {
         e.modal = 's';  // Set
         if (e.obj_type == OBJ_LINEDEFS)
            {
            menu_linedef_flags->set_title ("Set linedef flag");
            e.modpopup->set (menu_linedef_flags, 0);
            }
         else if (e.obj_type == OBJ_THINGS)
            {
            menu_thing_flags->set_title ("Set thing flag");
            e.modpopup->set (menu_thing_flags, 0);
            }
         }

      // [b]: pop up the "Toggle flag" menu
      else if (is.key == 'b'
         && e.menubar->highlighted () < 0
         && (e.Selected || is_obj (e.highlight_obj_no)))
         {
         e.modal = 't';  // Toggle
         if (e.obj_type == OBJ_LINEDEFS)
            {
            menu_linedef_flags->set_title ("Toggle linedef flag");
            e.modpopup->set (menu_linedef_flags, 0);
            }
         else if (e.obj_type == OBJ_THINGS)
            {
            menu_thing_flags->set_title ("Toggle thing flag");
            e.modpopup->set (menu_thing_flags, 0);
            }
         }

      // [c]: pop up the "Clear flag" menu
      else if (is.key == 'c'
         && e.menubar->highlighted () < 0
         && (e.Selected || is_obj (e.highlight_obj_no)))
         {
         e.modal = 'c';  // Clear;
         if (e.obj_type == OBJ_LINEDEFS)
            {
            menu_linedef_flags->set_title ("Clear linedef flag");
            e.modpopup->set (menu_linedef_flags, 0);
            }
         else if (e.obj_type == OBJ_THINGS)
            {
            menu_thing_flags->set_title ("Clear thing flag");
            e.modpopup->set (menu_thing_flags, 0);
            }
         }

      // [F8]: pop up the "Misc. operations" menu
      else if (is.key == YK_F8
         && e.menubar->highlighted () < 0)
         {
         e.modpopup->set (e.menubar->get_menu (MBI_MISC), 1);
         }

      // [F9]: pop up the "Insert a standard object" menu
      else if (is.key == YK_F9
         && e.menubar->highlighted () < 0)
         {
         e.modpopup->set (e.menubar->get_menu (MBI_OBJECTS), 1);
         }

      // [F10]: pop up the "Checks" menu
      else if (is.key == YK_F10
         && e.menubar->highlighted () < 0)
         {
	 CheckLevel (-1, -1);
	 RedrawMap = 1;
         }

      // [i]: show/hide the info bar
      else if (is.key == 'i')
         {
	 e.infobar_shown = !e.infobar_shown;
         // In the "Help" menu tick/untick the "Info bar" item.
         e.mb_menu[MBM_HELP]->set_ticked (1, e.infobar_shown);
	 RedrawMap = 1;
         }

      // [+], [=], wheel: zooming in
      else if (is.key == '+' || is.key == '=' || is.key == YE_WHEEL_UP)
         {
         edit_zoom_in (&e);
	 RedrawMap = 1;
         }

      // [-], [_], wheel: zooming out
      else if (is.key == '-' || is.key == '_' || is.key == YE_WHEEL_DOWN)
         {
         edit_zoom_out (&e);
	 RedrawMap = 1;
         }

      // [0], [1], ... [9]: set the zoom factor
      else if (is.key >= '0' && is.key <= '9')
         {
         edit_set_zoom (&e, is.key == '0' ? 0.1 : 1.0 / (is.key - '0'));
	 RedrawMap = 1;
         }

      // [']: center window on center of map
      else if (is.key == '\'')
         {
         CenterMapAroundCoords ((MapMinX + MapMaxX) / 2,
            (MapMinY + MapMaxY) / 2);
         RedrawMap = 1;
         }

      // [`]: center window on center of map
      // and set zoom to view the entire map
      else if (is.key == '`')
         {
         double xzoom;
         if (MapMaxX - MapMinX)
            xzoom = .95 * ScrMaxX / (MapMaxX - MapMinX);
         else
            xzoom = 1;
         double yzoom;
         if (MapMaxY - MapMinY)
            yzoom = .9 * ScrMaxY / (MapMaxY - MapMinY);
         else
            yzoom = 1;
         edit_set_zoom (&e, min (xzoom, yzoom));
         CenterMapAroundCoords ((MapMinX + MapMaxX) / 2,
            (MapMinY + MapMaxY) / 2);
         RedrawMap = 1;
         }

#if 0
      /* user wants to move */
      else if (is.key == YK_UP && (is.y - e.move_speed) >= 0)
         {
	 if (UseMouse)
	    SetMouseCoords (is.x, is.y - e.move_speed);
	 else
	    is.y -= e.move_speed;
         }
      else if (is.key == YK_DOWN && (is.y + e.move_speed) <= ScrMaxY)
         {
	 if (UseMouse)
	    SetMouseCoords (is.x, is.y + e.move_speed);
	 else
	    is.y += e.move_speed;
         }
      else if (is.key == YK_LEFT && (is.x - e.move_speed) >= 0)
         {
	 if (UseMouse)
	    { 
	    SetMouseCoords (is.x - e.move_speed, is.y);
	    LogMessage ("dec'd px=%d by %d\n", is.x, e.move_speed); /* AYM */
	    }
	 else
	    is.x -= e.move_speed;
         }
      else if (is.key == YK_RIGHT)
	 {
	 if ((is.x + e.move_speed) <= ScrMaxX)
	    {
	    if (UseMouse)
	       {
	       SetMouseCoords (is.x + e.move_speed, is.y);
	       LogMessage ("inc'd px=%d by %d\n", is.x, e.move_speed); /* AYM */
	       }
	    else
	       {
	       is.x += e.move_speed;
	       LogMessage ("inc'd px by %d\n",  e.move_speed); /* AYM */
	       }
	    }
	 else
	    LogMessage ("px=%d ms=%d smx=%d\n", is.x, e.move_speed, ScrMaxX);
	 }
#endif

      // [Left], [Right], [Up], [Down]:
      // scroll <scroll_less> percents of a screenful.
      else if (is.key == YK_LEFT && MAPX (ScrCenterX) > -20000)
         {
	 OrigX -= (int) ((double) ScrMaxX * scroll_less / 100 / Scale);
	 RedrawMap = 1;
	 }
      else if (is.key == YK_RIGHT && MAPX (ScrCenterX) < 20000)
         {
	 OrigX += (int) ((double) ScrMaxX * scroll_less / 100 / Scale);
	 RedrawMap = 1;
	 }
      else if (is.key == YK_UP && MAPY (ScrCenterY) > -20000)
         {
	 OrigY += (int) ((double) ScrMaxY * scroll_less / 100 / Scale);
	 RedrawMap = 1;
	 }
      else if (is.key == YK_DOWN && MAPY (ScrCenterY) < 20000)
         {
	 OrigY -= (int) ((double) ScrMaxY * scroll_less / 100 / Scale);
	 RedrawMap = 1;
	 }

      // [Page-up], [Page-down], [Home], [End]:
      // scroll <scroll_more> percents of a screenful.
      else if (is.key == YK_PU && MAPY (ScrCenterY) < /*MapMaxY*/ 20000)
	 {
	 OrigY += (int) ((double) ScrMaxY * scroll_more / 100 / Scale);
	 RedrawMap = 1;
	 }
      else if (is.key == YK_PD && MAPY (ScrCenterY) > /*MapMinY*/ -20000)
	 {
	 OrigY -= (int) ((double) ScrMaxY * scroll_more / 100 / Scale);
	 RedrawMap = 1;
	 }
      else if (is.key == YK_HOME && MAPX (ScrCenterX) > /*MapMinX*/ -20000)
	 {
	 OrigX -= (int) ((double) ScrMaxX * scroll_more / 100 / Scale);
	 RedrawMap = 1;
	 }
      else if (is.key == YK_END && MAPX (ScrCenterX) < /*MapMaxX*/ 20000)
	 {
	 OrigX += (int) ((double) ScrMaxX * scroll_more / 100 / Scale);
	 RedrawMap = 1;
	 }

#if 0
      /* user wants to change the movement speed */
      else if (is.key == ' ')
	 e.move_speed = e.move_speed == 1 ? 20 : 1;
#endif
      else if (is.key == ' ')
         {
         e.extra_zoom = ! e.extra_zoom;
         edit_set_zoom (&e, Scale * (e.extra_zoom ? 4 : 0.25));
         // In the "View" menu, tick/untick the "Extra zoom" item
         e.mb_menu[MBM_VIEW]->set_ticked (8, e.extra_zoom);
         RedrawMap = 1;
         }

      /* user wants to change the edit mode */
      else if (is.key == YK_TAB || is.key == YK_BACKTAB
       || is.key == 't' || is.key == 'v' || is.key == 'l' || is.key == 's')
	 {
         int old_mode;
         int new_mode;
	 int    PrevMode = e.obj_type;
	 SelPtr NewSel;

         // What's the number of the current mode ?
         old_mode = obj_type_to_mode_no (e.obj_type);

         // What's the number of the new mode ?
	 if (is.key == YK_TAB)				// [Tab]
            new_mode = (old_mode + 1) % NB_MODES;
	 else if (is.key == YK_BACKTAB)			// [Shift]-[Tab]
            new_mode = old_mode == 0 ? NB_MODES - 1 : old_mode - 1;
	 else
	    {
	    if (is.key == 't')
	       new_mode = obj_type_to_mode_no (OBJ_THINGS);
	    else if (is.key == 'v')
	       new_mode = obj_type_to_mode_no (OBJ_VERTICES);
	    else if (is.key == 'l')
	       new_mode = obj_type_to_mode_no (OBJ_LINEDEFS);
	    else if (is.key == 's')
	       new_mode = obj_type_to_mode_no (OBJ_SECTORS);
            else
               fatal_error ("changing mode with %04X", is.key);
	    /* unselect all */
	    ForgetSelection (&e.Selected);
	    }

         // Set the object type according to the new mode.
         e.obj_type = modes[new_mode].obj_type;

         // In the "View" menu, untick the old mode
         // and tick the new mode.
         e.mb_menu[MBM_VIEW]->set_ticked (modes[old_mode].item_no, 0);
         e.mb_menu[MBM_VIEW]->set_ticked (modes[new_mode].item_no, 1);

         // Change the flavour of the "Misc" menu.
         e.menubar->set_menu (e.mb_ino[MBI_MISC],
            e.mb_menu[modes[new_mode].menu_no]);

	 /* special cases for the selection list... */
	 if (e.Selected)
	    {
	    /* select all linedefs bound to the selected sectors */
	    if (PrevMode == OBJ_SECTORS && e.obj_type == OBJ_LINEDEFS)
	       {
	       int l, sd;

	       ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
	       NewSel = 0;
	       for (l = 0; l < NumLineDefs; l++)
	          {
		  sd = LineDefs[l].sidedef1;
		  if (sd >= 0 && IsSelected (e.Selected, SideDefs[sd].sector))
		     SelectObject (&NewSel, l);
		  else
		     {
		     sd = LineDefs[l].sidedef2;
		     if (sd >= 0 && IsSelected (e.Selected, SideDefs[sd].sector))
			SelectObject (&NewSel, l);
		     }
	          }
	       ForgetSelection (&e.Selected);
	       e.Selected = NewSel;
	    }
	    /* select all Vertices bound to the selected linedefs */
	    else if (PrevMode == OBJ_LINEDEFS && e.obj_type == OBJ_VERTICES)
	       {
	       ObjectsNeeded (OBJ_LINEDEFS, 0);
	       NewSel = 0;
	       while (e.Selected)
	          {
		  if (!IsSelected (NewSel, LineDefs[e.Selected->objnum].start))
		     SelectObject (&NewSel, LineDefs[e.Selected->objnum].start);
		  if (!IsSelected (NewSel, LineDefs[e.Selected->objnum].end))
		     SelectObject (&NewSel, LineDefs[e.Selected->objnum].end);
		  UnSelectObject (&e.Selected, e.Selected->objnum);
	          }
	       e.Selected = NewSel;
	       }
	    /* select all sectors that have their linedefs selected */
	    else if (PrevMode == OBJ_LINEDEFS && e.obj_type == OBJ_SECTORS)
	       {
	       int l, sd;

	       ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
	       NewSel = 0;
	       /* select all sectors... */
	       for (l = 0; l < NumSectors; l++)
		  SelectObject (&NewSel, l);
	       /* ... then unselect those that should not be in the list */
	       for (l = 0; l < NumLineDefs; l++)
		  if (!IsSelected (e.Selected, l))
		     {
		     sd = LineDefs[l].sidedef1;
		     if (sd >= 0)
			UnSelectObject (&NewSel, SideDefs[sd].sector);
		     sd = LineDefs[l].sidedef2;
		     if (sd >= 0)
		       UnSelectObject (&NewSel, SideDefs[sd].sector);
		     }
	       ForgetSelection (&e.Selected);
	       e.Selected = NewSel;
	       }
	    /* select all linedefs that have both ends selected */
	    else if (PrevMode == OBJ_VERTICES && e.obj_type == OBJ_LINEDEFS)
	       {
	       int l;

	       ObjectsNeeded (OBJ_LINEDEFS, 0);
	       NewSel = 0;
	       for (l = 0; l < NumLineDefs; l++)
		  if (IsSelected (e.Selected, LineDefs[l].start)
		   && IsSelected (e.Selected, LineDefs[l].end))
		     SelectObject (&NewSel, l);
	       ForgetSelection (&e.Selected);
	       e.Selected = NewSel;
	       }
	    /* unselect all */
	    else
	       ForgetSelection (&e.Selected);
	    }
	 if (GetMaxObjectNum (e.obj_type) >= 0 && Select0)
	    e.highlight_obj_no = 0;
	 else
	    e.highlight_obj_no = OBJ_NO_NONE;
         e.highlight_obj_type = e.obj_type;

	 DragObject = 0;
	 StretchSelBox = 0;
	 RedrawMap = 1;
	 }

      /* User wants to increase the grid step */
      else if (is.key == 'G')
	 {
	 if (e.grid_step < e.grid_step_max)
	    e.grid_step *= 2;
	 else
	    e.grid_step = e.grid_step_min;
	 RedrawMap = 1;
	 }

      /* User wants to decrease the grid step */
      else if (is.key == 'g')
	 {
	 if (e.grid_step > e.grid_step_min)
	    e.grid_step /= 2;
	 else
	    e.grid_step = e.grid_step_max;
	 RedrawMap = 1;
	 }

      /* user wants to display or hide the grid */
      else if (is.key == 'h')
	 {
	 e.grid_shown = ! e.grid_shown;
	 RedrawMap = 1;
	 }

      /* Reset the grid to grid_step_max */
      else if (is.key == 'H')
	 {
	 e.grid_step = e.grid_step_max;
	 RedrawMap = 1;
	 }

      /* Toggle the snap_to_grid flag */
      else if (is.key == 'y')
         {
         e.grid_snap = ! e.grid_snap;
         }

      /* Toggle the lock_grip_step flag */
      else if (is.key == 'z')
         {
         e.grid_step_locked = ! e.grid_step_locked;
         }
 
      /* user wants to toggle the rulers */
      else if (is.key == 'r')
	 e.rulers_shown = !e.rulers_shown;

      /* user wants to select the next or previous object */
      else if (is.key == 'n' || is.key == '>')
	 {
	 if (e.highlight_obj_no < GetMaxObjectNum (e.obj_type))
            e.highlight_obj_no++;
	 else if (GetMaxObjectNum (e.obj_type) >= 0)
	    e.highlight_obj_no = 0;
	 }

      else if (is.key == 'p' || is.key == '<')
	 {
	 if (is_obj (e.highlight_obj_no) && e.highlight_obj_no > 0)
            e.highlight_obj_no--;
	 else
	    e.highlight_obj_no = GetMaxObjectNum (e.obj_type);
	 }

      else if (is.key == 'j' || is.key == '#')
	 {
	 e.highlight_obj_no = InputObjectNumber (-1, -1, e.obj_type,
                                                 e.highlight_obj_no);
	 if (is_obj (e.highlight_obj_no))
	    GoToObject (e.obj_type, e.highlight_obj_no);
	 RedrawMap = 1;
	 }

      /* user wants to clear all marks and redraw the map */
      else if (is.key == 'c')
	 {
	 ForgetSelection (&e.Selected);
	 RedrawMap = 1;
	 DragObject = 0;
	 StretchSelBox = 0;
	 }

      /* user wants to copy a group of objects */
      else if (is.key == 'o'
         && (e.Selected || is_obj (e.highlight_obj_no)))
	 {
         int x, y;

	 /* copy the object(s) */
	 if (! e.Selected)
	    SelectObject (&e.Selected, e.highlight_obj_no);
	 CopyObjects (e.obj_type, e.Selected);
	 /* enter drag mode */
	 //DragObject = 1;
         /* AYM 19980619 : got to look into this!! */
	 //e.highlight_obj_no = e.Selected->objnum;

         // Find the "hotspot" in the object(s)
         if (is_obj (e.highlight_obj_no) && ! e.Selected)
            GetObjectCoords (e.highlight_obj_type, e.highlight_obj_no, &x, &y);
         else
            centre_of_objects (e.obj_type, e.Selected, &x, &y);

         // Drag the object(s) so that the "hotspot" is under the pointer
         MoveObjectsToCoords (e.obj_type, 0, x, y, 0);
         MoveObjectsToCoords (e.obj_type, e.Selected,
            e.pointer_x, e.pointer_y, 0);
	 RedrawMap = 1;
	 StretchSelBox = 0;
	 }

      // [Return]: edit the properties of the current object.
      else if (is.key == YK_RETURN
         && (e.Selected || is_obj (e.highlight_obj_no)))
	 {
	 if (e.Selected)
	    EditObjectsInfo (0, menubar_out_y1 + 1, e.obj_type, e.Selected);
	 else
	    {
	    SelectObject (&e.Selected, e.highlight_obj_no);
	    EditObjectsInfo (0, menubar_out_y1 + 1, e.obj_type, e.Selected);
	    UnSelectObject (&e.Selected, e.highlight_obj_no);
	    }
	 RedrawMap = 1;
	 DragObject = 0;
	 StretchSelBox = 0;
	 }

      // [w]: spin things 1/8 turn counter-clockwise
      else if (is.key == 'w' && e.obj_type == OBJ_THINGS
         && (e.Selected || is_obj (e.highlight_obj_no)))
	 {
	 if (! e.Selected)
	    {
	    SelectObject (&e.Selected, e.highlight_obj_no);
	    spin_things (e.Selected, 45);
	    UnSelectObject (&e.Selected, e.highlight_obj_no);
	    }
	 else
	    {
	    spin_things (e.Selected, 45);
	    }
	 RedrawMap = 1;  /* FIXME: should redraw only the things */
	 DragObject = 0;
	 StretchSelBox = 0;
	 }

      // [w]: split linedefs and sectors
      else if (is.key == 'w' && e.obj_type == OBJ_LINEDEFS
         && e.Selected && e.Selected->next && ! e.Selected->next->next)
         {
         SplitLineDefsAndSector (e.Selected->next->objnum, e.Selected->objnum);
         ForgetSelection (&e.Selected);
         RedrawMap = 1;
         DragObject = 0;
         StretchSelBox = 0;
         }

      // [x]: spin things 1/8 turn clockwise
      else if (is.key == 'x' && e.obj_type == OBJ_THINGS
         && (e.Selected || is_obj (e.highlight_obj_no)))
	 {
	 if (! e.Selected)
	    {
	    SelectObject (&e.Selected, e.highlight_obj_no);
	    spin_things (e.Selected, -45);
	    UnSelectObject (&e.Selected, e.highlight_obj_no);
	    }
	 else
	    {
	    spin_things (e.Selected, -45);
	    }
	 RedrawMap = 1;  /* FIXME: should redraw only the things */
	 DragObject = 0;
	 StretchSelBox = 0;
	 }

      // [x]: split linedefs
      else if (is.key == 'x' && e.obj_type == OBJ_LINEDEFS
         && (e.Selected || is_obj (e.highlight_obj_no)))
         {
         if (! e.Selected)
            {
            SelectObject (&e.Selected, e.highlight_obj_no);
            SplitLineDefs (e.Selected);
            UnSelectObject (&e.Selected, e.highlight_obj_no);
            }
         else
            SplitLineDefs (e.Selected);
         RedrawMap = 1;
         DragObject = 0;
         StretchSelBox = 0;
         }

      /* user wants to delete the current object */
      else if (is.key == YK_DEL
         && (e.Selected || e.highlight_obj_no >= 0)) /* 'Del' */
	 {
	 if (e.obj_type == OBJ_THINGS
	  || Expert
	  || Confirm (-1, -1,
		(e.Selected ? "Do you really want to delete these objects?"
			  : "Do you really want to delete this object?"),
		(e.Selected ? "This will also delete the objects bound to them."
			  : "This will also delete the objects bound to it.")))
	    {
	    if (e.Selected)
	       DeleteObjects (e.obj_type, &e.Selected);
	    else
	       DeleteObject (e.obj_type, e.highlight_obj_no);
	    }
         // AYM 1998-09-20 I thought I'd add this
         // (though it doesn't fix the problem : if the object has been
         // deleted, HighlightObject is still called with a bad object#).
         e.highlight_obj_no = OBJ_NO_NONE;
	 DragObject = 0;
	 StretchSelBox = 0;
	 RedrawMap = 1;
	 }

      // [Ins]: insert a new object
      else if (is.key == YK_INS) /* 'Ins' */
	 {
	 SelPtr cur;
         int prev_obj_type = e.obj_type;

	 /* first special case: if several vertices are
	    selected, add new linedefs */
	 if (e.obj_type == OBJ_VERTICES
	    && e.Selected && e.Selected->next)
	    {
	    int firstv;
            int obj_no;

	    ObjectsNeeded (OBJ_LINEDEFS, 0);
	    if (e.Selected->next->next)
	       firstv = e.Selected->objnum;
	    else
	       firstv = -1;
	    e.obj_type = OBJ_LINEDEFS;
	    /* create linedefs between the vertices */
	    for (cur = e.Selected; cur->next; cur = cur->next)
	       {
	       /* check if there is already a linedef between the two vertices */
	       for (obj_no = 0; obj_no < NumLineDefs; obj_no++)
		  if ((LineDefs[obj_no].start == cur->next->objnum
		    && LineDefs[obj_no].end   == cur->objnum)
		   || (LineDefs[obj_no].end   == cur->next->objnum
		    && LineDefs[obj_no].start == cur->objnum))
		     break;
	       if (obj_no < NumLineDefs)
		  cur->objnum = obj_no;
	       else
		  {
		  InsertObject (OBJ_LINEDEFS, -1, 0, 0);
		  e.highlight_obj_no = NumLineDefs - 1;
		  LineDefs[e.highlight_obj_no].start = cur->next->objnum;
		  LineDefs[e.highlight_obj_no].end = cur->objnum;
		  cur->objnum = e.highlight_obj_no;
		  }
	       }
	    /* close the polygon if there are more than 2 vertices */
	    if (firstv >= 0 && is.shift)
	       {
	       for (e.highlight_obj_no = 0;
                    e.highlight_obj_no < NumLineDefs;
                    e.highlight_obj_no++)
		  if ((LineDefs[e.highlight_obj_no].start == firstv
		    && LineDefs[e.highlight_obj_no].end   == cur->objnum)
		   || (LineDefs[e.highlight_obj_no].end   == firstv
		    && LineDefs[e.highlight_obj_no].start == cur->objnum))
		     break;
	       if (e.highlight_obj_no < NumLineDefs)
		  cur->objnum = obj_no;
	       else
		  {
		  InsertObject (OBJ_LINEDEFS, -1, 0, 0);
		  e.highlight_obj_no = NumLineDefs - 1;
		  LineDefs[e.highlight_obj_no].start = firstv;
		  LineDefs[e.highlight_obj_no].end   = cur->objnum;
		  cur->objnum = e.highlight_obj_no;
		  }
	       }
	    else
	       UnSelectObject (&e.Selected, cur->objnum);
	    }
	 /* second special case: if several linedefs are selected,
	    add new sidedefs and one sector */
	 else if (e.obj_type == OBJ_LINEDEFS && e.Selected)
	    {
	    ObjectsNeeded (OBJ_LINEDEFS, 0);
	    for (cur = e.Selected; cur; cur = cur->next)
	       if (LineDefs[cur->objnum].sidedef1 >= 0
		&& LineDefs[cur->objnum].sidedef2 >= 0)
		  {
		  char msg[80];

		  Beep ();
		  sprintf (msg, "Linedef #%d already has two sidedefs", cur->objnum);
		  Notify (-1, -1, "Error: cannot add the new sector", msg);
		  break;
		  }
	    if (! cur)
	       {
	       e.obj_type = OBJ_SECTORS;
	       InsertObject (OBJ_SECTORS, -1, 0, 0);
	       e.highlight_obj_no = NumSectors - 1;
	       for (cur = e.Selected; cur; cur = cur->next)
		  {
		  InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
		  SideDefs[NumSideDefs - 1].sector = e.highlight_obj_no;
		  ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
		  if (LineDefs[cur->objnum].sidedef1 >= 0)
		     {
		     int s;

		     s = SideDefs[LineDefs[cur->objnum].sidedef1].sector;
		     if (s >= 0)
			{
			Sectors[e.highlight_obj_no].floorh = Sectors[s].floorh;
			Sectors[e.highlight_obj_no].ceilh = Sectors[s].ceilh;
			strncpy (Sectors[e.highlight_obj_no].floort,
			   Sectors[s].floort, 8);
			strncpy (Sectors[e.highlight_obj_no].ceilt,
			   Sectors[s].ceilt, 8);
			Sectors[e.highlight_obj_no].light = Sectors[s].light;
			}
		     LineDefs[cur->objnum].sidedef2 = NumSideDefs - 1;
		     LineDefs[cur->objnum].flags = 4;
		     strncpy (SideDefs[NumSideDefs - 1].tex3, "-", 8);
		     strncpy (SideDefs[LineDefs[cur->objnum].sidedef1].tex3,
		        "-", 8);
		     }
		  else
		     LineDefs[cur->objnum].sidedef1 = NumSideDefs - 1;
		  }
	       ForgetSelection (&e.Selected);
	       SelectObject (&e.Selected, e.highlight_obj_no);
	       }
	    }
	 /* normal case: add a new object of the current type */
	 else
	    {
	    ForgetSelection (&e.Selected);
	    InsertObject (e.obj_type, e.highlight_obj_no,
	       edit_mapx_snapped (&e, e.pointer_x),
	       edit_mapy_snapped (&e, e.pointer_y));
	    e.highlight_obj_no = GetMaxObjectNum (e.obj_type);
	    if (e.obj_type == OBJ_LINEDEFS)
	       {
	       int v1 = LineDefs[e.highlight_obj_no].start;
	       int v2 = LineDefs[e.highlight_obj_no].end;
	       if (! Input2VertexNumbers (-1, -1,
		 "Choose the two vertices for the new linedef", &v1, &v2))
		  {
		  DeleteObject (e.obj_type, e.highlight_obj_no);
		  e.highlight_obj_no = OBJ_NO_NONE;
		  }
	       else
		  {
		  LineDefs[e.highlight_obj_no].start = v1;
		  LineDefs[e.highlight_obj_no].end   = v2;
		  }
	       }
	    else if (e.obj_type == OBJ_VERTICES)
	       {
	       SelectObject (&e.Selected, e.highlight_obj_no);
	       if (AutoMergeVertices (&e.Selected, e.obj_type, 'i'))
		  RedrawMap = 1;
	       ForgetSelection (&e.Selected);
	       }
	    }

         // Mode-changing code, duplicated from above.
         // As RQ would say: "I hate to do that". So do I.
         // The best solution would be to have a mode
         // changing function that only changes e.obj_type
         // and emits a "mode change" message. Another part
         // of the code, responsible for the menus, would
         // intercept that message and update them. I don't
         // have the time (as of 1998-12-14) to do it now.
         if (e.obj_type != prev_obj_type)
	    {
            int old_mode;
	    int new_mode;

	    // What's the number of the current mode ?
	    old_mode = obj_type_to_mode_no (prev_obj_type);

	    // What's the number of the new mode ?
            new_mode = obj_type_to_mode_no (e.obj_type);

	    // In the "View" menu, untick the old mode
	    // and tick the new mode.
	    e.mb_menu[MBM_VIEW]->set_ticked (modes[old_mode].item_no, 0);
	    e.mb_menu[MBM_VIEW]->set_ticked (modes[new_mode].item_no, 1);

	    // Change the flavour of the "Misc" menu.
	    e.menubar->set_menu (e.mb_ino[MBI_MISC],
	       e.mb_menu[modes[new_mode].menu_no]);
	    }

	 DragObject = 0;
	 StretchSelBox = 0;
	 RedrawMap = 1;
	 }

      /* Debug info (not documented) */
      else if (is.key == '!')
         {
         DumpSelection (e.Selected);
         }

      /* Show font (not documented) */
      else if (is.key == '@')
         {
         show_font ();
         }

      /* Show colours (not documented) */
      else if (is.key == '|')
         {
         show_colours ();
         }

      /* Show object numbers */
      else if (is.key == '&')
         {
         RedrawMap = 1;
         e.show_object_numbers = ! e.show_object_numbers;
         }

      /* user likes music */
      else if (is.key)
	 {
	 Beep ();
	 }
      }

   /*
    *	Step 4 -- Misc. cruft
    */

   done :

   // Auto-scrolling: scroll the map automatically
   // when the mouse pointer rests near the edge
   // of the window.
   // Scrolling is disabled when a pull-down menu
   // is visible because it would be annoying to see
   // the map scrolling while you're searching
   // through the menus.

   if (is.in_window
      && autoscroll
      && ! is.scroll_lock
      && e.menubar->pulled_down () < 0)
      {
      int distance;		// In pixels

#define actual_move(total,dist) \
   ((int) (((total * autoscroll_amp / 100)\
   * ((double) (autoscroll_edge - dist) / autoscroll_edge))\
   / Scale))

      distance = is.y;
      // The reason for the second member of the condition
      // is that we don't want to scroll when the user is
      // simply reaching for a menu...
      if (distance <= autoscroll_edge
         && e.menubar->is_under_menubar_item (is.x) < 0)
	 {
	 if (! UseMouse)
	    is.y += e.move_speed;
	 if (MAPY (ScrCenterY) < /*MapMaxY*/ 20000)
	    {
	    OrigY += actual_move (ScrMaxY, distance);
	    RedrawMap = 1;
	    }
	 }

      distance = ScrMaxY - is.y;
      if (distance <= autoscroll_edge)
	 {
	 if (! UseMouse)
	    is.y -= e.move_speed;
	 if (MAPY (ScrCenterY) > /*MapMinY*/ -20000)
	    {
	    OrigY -= actual_move (ScrMaxY, distance);
	    RedrawMap = 1;
	    }
	 }

      distance = is.x;
      if (distance <= autoscroll_edge)
	 {
	 if (! UseMouse)
	    is.x += e.move_speed;
	 if (MAPX (ScrCenterX) > /*MapMinX*/ -20000)
	    {
	    OrigX -= actual_move (ScrMaxX, distance);
	    RedrawMap = 1;
	    }
	 }

      // The reason for the second member of the condition
      // is that we don't want to scroll when the user is
      // simply reaching for the "Help" menu...
      // Note: the ordinate "3 * FONTH" is of course not
      // critical. It's just a rough approximation.
      distance = ScrMaxX - is.x;
      if (distance <= autoscroll_edge && is.y >= 3 * FONTH)
	 {
	 if (! UseMouse)
	    is.x -= e.move_speed;
	 if (MAPX (ScrCenterX) < /*MapMaxX*/ 20000)
	    {
	    OrigX += actual_move (ScrMaxX, distance);
	    RedrawMap = 1;
	    }
	 }
      }

   /*
    *	Step 5 -- Process events that were generated
    */

   done2:

   // Process events that were generated
   if (has_event (YE_ZOOM_CHANGED) && ! e.grid_step_locked)
      {
      get_event ();
      edit_grid_adapt (&e);
      RedrawMap = 1;
      }

   if (RedrawMap)
      e.edisplay->need_refresh ();
   }

delete e.edisplay;
delete e.selbox;
delete e.menubar;
for (size_t n = 0; n < MBM_HELP; n++)
   delete e.mb_menu[n];

delete menu_linedef_flags;
delete menu_thing_flags;
}


