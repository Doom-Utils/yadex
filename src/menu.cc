/*
 *	menu.cc
 *	AYM 1998-12-01
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2000 André Majorel.

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


/*
These are the pop-up and pull-down menus.

Events that are not understood (E.G. [Left] and [Right])
are put back in the input buffer before the
function returns.
*/


#include "yadex.h"
#include "events.h"
#include "gfx.h"
#include "menu.h"


/* First subscript : 0 = normal, 1 = grayed out
   Second subscript : 0 = normal, 1 = highlighted */
const colour_pair_t menu_colour[2][2] =
  {
  { { WINBG,    WINFG        },    // Normal entry
    { WINBG_HL, WINFG_HL     } },  // Normal entry, highlighted
  { { WINBG,    WINFG_DIM    },    // Grayed out entry
    { WINBG_HL, WINFG_DIM_HL } }   // Grayed out entry, highlighted
  };


/*
 *	init
 *	Parse the parameters and initialize the menu.
 *	This is the "canonical" ctor for menu_c but it's
 *	impractical to use so it's private. You want to
 *	use one of the 3 other named ctors below instead.
 */
void menu_c::init (
   const char *title,			// Title of window (or NULL)
   int numitems,			// < 0 if in list mode
   int *ticked,				// Always NULL in list mode
   void *data,				// List or array of strings
   const char *(*getstr) (void *),	// Only in list mode
   const i8 *shortcut_index,		// Always NULL in list mode
   const int *shortcut_key,		// Always NULL in list mode
   int *grayed_out,			// Always NULL in list mode
   int delete_after_use)		// like the class member
{
memset (this, 0, sizeof *this);

if (numitems < 0)
   {
   use_list = 1;
   list = (al_llist_t *) data;
   this->numitems = al_lcount (list);
   if (getstr == NULL)	/* Sanity */
      fatal_error ("NULL getstr");
   }
else
   {
   use_list = 0;
   menustr = (char **) data;
   this->numitems = numitems;
   }

title_len = title ? strlen (title) : 0;

// Compute items_len, items_ks_len and prepare the cooked index shortcuts
items_len = 0;
items_ks_len = 0;
if (use_list)
   {
   int line;
   for (al_lrewind (list), line = 0;
        ! al_leol (list);
        al_lstep (list), line++)
      {
      const char *str = (*getstr) (al_lptr (list));
      size_t len = strlen (str);
      len += 4;  /* List menus always have "(1)" ... "(Z)" shortcuts */
      if (len > items_len)
         {
	 items_len = len;
	 items_ks_len = len;
         }
      cooked_index_shortcuts[line] = -1;
      if (shortcut_index != NULL
          && shortcut_index[line] >= 0
          && shortcut_index[line] < (int) strlen (str))
         cooked_index_shortcuts[line] =
            tolower (str[shortcut_index[line]]);
      }
   this->numitems = line;
   }
else
   {
   this->numitems = numitems;
   for (int line = 0; line < numitems; line++)
      {
      size_t len = (ticked != NULL ? 2 : 0)
                 + (shortcut_index == NULL ? 4 : 0)
                 + strlen (menustr[line]);
      size_t len_ks = len
                 + ((shortcut_key != NULL && shortcut_key[line] != -1)
                   ? strlen (key_to_string (shortcut_key[line])) + 2 : 0);
      if (len > items_len)
         items_len = len;
      if (len_ks > items_ks_len)
         items_ks_len = len_ks;
      cooked_index_shortcuts[line] = -1;
      if (shortcut_index != NULL
          && shortcut_index[line] >= 0
          && shortcut_index[line] < (int) strlen (menustr[line]))
         cooked_index_shortcuts[line]
            = tolower (menustr[line][shortcut_index[line]]);
      }
   }

this->ticked           = ticked;
this->getstr           = getstr;
this->shortcut_index   = shortcut_index;
this->shortcut_key     = shortcut_key;
this->grayed_out       = grayed_out;
this->delete_after_use = delete_after_use;
set_title (title);

popup         = 0;
force_numbers = 0;
visible       = 0;
visible_disp  = 0;
line_disp     = -1;
line          = 0;
user_ox0      = -1;
user_oy0      = -1;
ox0           = -1;
oy0           = -1;
need_geom     = 1;
}


/*
 *	set_title
 *	Set the title of the menu (it's ignored unless the
 *	menu is set in popup mode).
 *	Bug: changing the title does not take effect until
 *	the next display from scratch.
 */
void menu_c::set_title (const char *title)
{
this->title = title;
size_t title_len = title ? strlen (title) : 0;

// If the length of the title has changed,
// force geom() to be called again.
if (title_len != this->title_len)
   need_geom = 1;

this->title_len = title_len;
}


/*
 *	Ctor from arguments in variable number.
 *
 *	Another ctor for menu_c, suited to the creation of pull
 *	down menus.
 *
 *	Expects the title of the menu followed by repeats of :
 *	  const char   *menustr
 *	  int          shortcut_index
 *	  int          shortcut_key
 *	  menu_flags_t flags
 *	  [int         ticked]  <-- Only if (flags & MEN_TICK)
 *	Pass a NULL pointer after the last repeat.
 */
menu_c::menu_c (const char *title, ...)
{
va_list	args;
int	num;
int	*ticked         = new int [36];
char	**menustr       = new char* [36];
i8	*shortcut_index = new i8  [36];
int	*shortcut_key   = new int [36];
int	*grayed_out     = new int [36];
int	has_tick = 0;

/* put the va_args in the menustr table and the two strings */
num = 0;
va_start (args, title);
while ((num < 36) && ((menustr[num] = va_arg (args, char *)) != NULL))
   {
   menu_flags_t flags;
   shortcut_index[num] = va_arg (args, int);
   shortcut_key  [num] = va_arg (args, int);
   flags = (menu_flags_t) va_arg (args, int);
   if (flags & MEN_TICK)
      {
      has_tick = 1;
      ticked[num] = va_arg (args, int);
      }
   else
      ticked[num] = 0;
   grayed_out[num] = !! (flags & MEN_GRAY);
   num++;
   }
va_end (args);

/* display the menu */
init (
   title,
   num,
   has_tick ? ticked : 0,	// 0 = NULL
   menustr,
   0,				// getstr() function pointer
   shortcut_index,
   shortcut_key,
   grayed_out,
   1);				// delete_after_use
}


/*
 *	Ctor from arrays
 *
 *	Another ctor for menu_c, suited to the creation of the
 *	menu from an array.
 */
menu_c::menu_c (
   const char   *title,
   int          numitems,
   int		*ticked,
   char         **menustr,
   const i8     *shortcut_index,
   const int    *shortcut_key,
   int		*grayed_out)
{
init (
   title,		// title
   numitems,		// numitems
   ticked,		// ticked
   menustr,		// data
   NULL,		// getstr
   shortcut_index,	// shortcut_index
   shortcut_key,	// shortcut_key
   grayed_out,		// grayed_out
   0);			// don't delete after use
}


/*
 *	Ctor from a list
 *
 *	Another ctor for menu_c, suited to the creation of the
 *	menu from a list.
 */
menu_c::menu_c (
   const char   *title,
   al_llist_t   *list,
   const char   *(*getstr) (void *))
{
init (
   title,	// title
   -1,		// numitems
   0,		// ticked
   list,	// data
   getstr,	// getstr
   0,		// shortcut_index
   0,		// shortcut_key
   0,		// grayed_out
   0);		// don't delete after use
}


/*
 *	~menu_c
 */
menu_c::~menu_c ()
{
if (delete_after_use)
   {
   if (menustr)
      delete[] menustr;
   if (menustr)
      delete[] ticked;
   if (menustr)
      delete[] shortcut_index;
   if (menustr)
      delete[] shortcut_key;
   if (menustr)
      delete[] grayed_out;
   }
}


/*
 *	set_coords
 *	Position or reposition the menu window.
 *	(<x0>,<y0>) is the top left corner.
 *	If <x0> is < 0, the window is horizontally centred.
 *	If <y0> is < 0, the window is vertically centred.
 */
void menu_c::set_coords (int x0, int y0)
{
if (x0 != ox0 || y0 != oy0)
   need_geom = 1;  // Force geom() to be called

user_ox0 = x0;
user_oy0 = y0;
}


/*
 *	set_ticked
 *	Tick or untick menu item <item_no>.
 *	If the menu has no "ticked/unticked" property,
 *	does nothing.
 */
void menu_c::set_ticked (int item_no, int ticked)
{
if (item_no < 0 || item_no >= numitems)
   fatal_error ("item_no out of range");
if (this->ticked)
   this->ticked[item_no] = ticked;
}


/*
 *	set_grayed_out
 *	Gray-out or ungray-out menu item <item_no>.
 *	If the menu has no "grayed-out" property,
 *	does nothing.
 */
void menu_c::set_grayed_out (int item_no, int grayed_out)
{
if (item_no < 0 || item_no >= numitems)
   fatal_error ("item_no out of range");
if (this->grayed_out)
   this->grayed_out[item_no] = grayed_out;
}


/*
 *	geom
 *	Recalculate the screen coordinates etc.
 */
void menu_c::geom ()
{
size_t width_chars = 0;
if (title && popup)
   width_chars = y_max (width_chars, title_len);
if (force_numbers)
   width_chars = y_max (width_chars, items_len + 4);
else
   width_chars = y_max (width_chars, items_ks_len);
int title_height = title && popup ? (int) (1.5 * FONTH) : 0;

width  = 2 * BOX_BORDER + 2 * WIDE_HSPACING + width_chars * FONTW;
height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + numitems * FONTH + title_height;

if (user_ox0 < 0)
   ox0 = (ScrMaxX - width) / 2;
else
   ox0 = user_ox0;
ix0 = ox0 + BOX_BORDER;
ix1 = ix0 + 2 * WIDE_HSPACING + width_chars * FONTW - 1;
ox1 = ix1 + BOX_BORDER;
if (ox1 > ScrMaxX)
   {
   int overlap = ox1 - ScrMaxX;
   ox0 -= overlap;
   ix0 -= overlap;
   ix1 -= overlap;
   ox1 -= overlap;
   }

if (user_oy0 < 0)
   oy0 = (ScrMaxY - height) / 2;
else
   oy0 = user_oy0;
iy0 = oy0 + BOX_BORDER;
ty0 = iy0 + FONTH / 2;				// Title of menu
ly0 = ty0 + title_height;			// First item of menu
iy1 = ly0 + numitems * FONTH + FONTH / 2 - 1;
oy1 = iy1 + BOX_BORDER;

need_geom = 0;
}


/*
 *	process_event
 *	Process event in *<is>.
 *	Return one of the following :
 *	- MEN_CANCEL: user pressed [Esc] or clicked outside
 *	  the menu. The caller should delete the menu.
 *	- MEN_INVALID: we didn't understand the event so we put it
 *	  back in the input buffer.
 *	- MEN_OTHER: we understood the event and processed it.
 *	- the number of the item that was validated.
 */
int menu_c::process_event (const input_status_t *is)
{
const char *item_str;
int mouse_line;
char status;

/* What's the text of the current option ? */
if (use_list)
   {
   al_lseek (list, line, SEEK_SET);
   item_str = (*getstr) (al_lptr (list));
   }
else
   item_str = menustr[line];

if (is->x < ix0 || is->x > ix1 || is->y < ly0)
   mouse_line = -1;
else
   {
   mouse_line = (is->y - ly0) / FONTH;
   if (mouse_line >= numitems)
      mouse_line = -1;
   }

status = 'i';

// Clicking left button on an item: validate it.
if (is->key == YE_BUTL_PRESS && mouse_line >= 0)
   {
   line = mouse_line;  // Useless ?
   status = 'v';
   }

// Moving over the box sets current line.
else if (is->key == YE_MOTION && mouse_line >= 0)
   {
   line = mouse_line;
   status = 'o';
   }

// Releasing the button while standing on an item:
// has different effects depending on whether we're
// in pull-down or pop-up mode.
//
// In pull-down mode, the button was normally last
// pressed on the menu bar or on an item of this menu.
// So the current item is selected upon button release.
//
// In pop-up mode, if the button was pressed, it was
// most likely to exit a submenu (cf. the "thing type"
// menu) so we ignore the event.
else if (is->key == YE_BUTL_RELEASE && mouse_line >= 0 && ! popup)
   status = 'v';

// [Enter], [Return]: accept selection
else if (is->key == YK_RETURN)
   status = 'v';

// [Esc]: cancel
else if (is->key == YK_ESC)
   status = 'c';

// [Up]: select previous line
else if (is->key == YK_UP)
   {
   if (line > 0)
     line--;
   else
     line = numitems - 1;
   status = 'o';
   }

// [Down]: select next line
else if (is->key == YK_DOWN)
   {
   if (line < numitems - 1)
     line++;
   else
     line = 0;
   status = 'o';
   }

// [Home]: select first line
else if (is->key == YK_HOME)
   {
   line = 0;
   status = 'o';
   }

// [End]: select last line
else if (is->key == YK_END)
   {
   line = numitems - 1;
   status = 'o';
   }

// [Pgup]: select line - 5
else if (is->key == YK_PU)
   {
   if (line >= 5)
      line -= 5;
   else
      line = 0;
   status = 'o';
   }

// [Pgdn]: select line + 5
else if (is->key == YK_PD)
   {
   if (line < numitems - 5)
      line += 5;
   else
      line = numitems - 1;
   status = 'o';
   }

// [a]-[z], [A]-[Z], [0]-[9]: select corresponding item
else if ((shortcut_index == NULL || force_numbers)
	 && is->key >= '1' && is->key <= '9' && is->key - '1' < numitems)
   {
   line = is->key - '1';
   status = 'o';
   send_event (YK_RETURN);
   }
else if ((shortcut_index == NULL || force_numbers)
	 && is->key >= 'A' && is->key <= 'Z' && is->key - 'A' + 9 < numitems)
   {
   line = is->key - 'A' + 9;
   status = 'o';
   send_event (YK_RETURN);
   }
else if ((shortcut_index == NULL || force_numbers)
	 && is->key >= 'a' && is->key <= 'z' && is->key - 'a' + 9 < numitems)
   {
   line = is->key - 'a' + 9;
   status = 'o';
   send_event (YK_RETURN);
   }

// A shortcut ?
else
   {
   // First, check the list of index shortcuts
   // (only if is->key is a regular key)
   if (shortcut_index != NULL
      && ! force_numbers
      && is->key == (unsigned char) is->key)
      {
      for (int n = 0; n < numitems; n++)
	 if (cooked_index_shortcuts[n] != -1
	     && cooked_index_shortcuts[n] == tolower (is->key))
	    {
	    line = n;
	    status = 'o';
            send_event (YK_RETURN);
	    break;
	    }
      }

   // If no index shortcut matched, check the list of
   // shortcut keys. It's important to do the index
   // shortcuts before so that you can override a shortcut
   // key (normally global) with an index shortcut (normally
   // local).
   if (status == 'i' && shortcut_key != NULL && ! force_numbers)
      {
      for (int n = 0; n < numitems; n++)
	 if (is->key == shortcut_key[n])
	    {
	    line = n;
	    status = 'o';
            send_event (YK_RETURN);
	    break;
	    }
      }
   }

// See last_shortcut_key()
if (status == 'v')
   _last_shortcut_key = shortcut_key ? shortcut_key[line] : 0;

// Return the item# if validated,
// MEN_CANCEL if cancelled,
// MEN_OTHER or MEN_INVALID if neither.
if (status == 'v')
   return line;
else if (status == 'c')
   return MEN_CANCEL;
else if (status == 'o')
   return MEN_OTHER;
else if (status == 'i')
   return MEN_INVALID;
else
   {
   fatal_error ("m::pe: bad status %02X", status);  // Can't happen
   return 0;  // To please the compiler
   }
}


/*
 *	last_shortcut_key
 *	Return the code of the shortcut key for the last
 *	selected item. This function shouldn't exist :
 *	it's just there because it helps editloop.cc. When
 *	real key bindings are implemented in editloop.cc,
 *	get_shortcut_key() should disappear.
 */
int menu_c::last_shortcut_key ()
{
return _last_shortcut_key;
}


/*
 *	draw
 *	If necessary, redraw everything from scratch.
 *	Else, if <line> has changed, refresh the highlighted line.
 */
void menu_c::draw ()
{
int from_scratch = 0;

if (need_geom)
   geom ();

// Do we need to redraw everything from scratch ?
if (visible && ! visible_disp
   || ox0 != ox0_disp
   || oy0 != oy0_disp
   || width != width_disp
   || height != height_disp)
   from_scratch = 1;

// Display the static part of the menu
if (from_scratch)
   {
   int l;  // Line number

   HideMousePointer ();
   DrawScreenBox3D (ox0, oy0, ox1, oy1);
   set_colour (YELLOW);
   if (popup && title != NULL)
      DrawScreenString (ix0 + WIDE_HSPACING, ty0, title);
   ShowMousePointer ();

   for (use_list ?   al_lrewind (list) : 0,            l = 0;
	use_list ? ! al_leol    (list) : l < numitems;
	use_list ?   al_lstep   (list) : 0,            l++)
      {
      set_colour (WINFG);
      draw_one_line (l, 0);
      }
   visible_disp = 1;
   ox0_disp     = ox0;
   oy0_disp     = oy0;
   width_disp   = width;
   height_disp  = height;
   }

// Display the "highlight" bar
if (from_scratch || line != line_disp)
   {
   if (line_disp >= 0 && line_disp < numitems)
      {
      if (use_list)
	 {
	 if (al_lseek (list, line_disp, SEEK_SET))
	    fatal_error ("%s line_disp=%d (%s)",
	       msg_unexpected, line_disp, al_astrerror (al_aerrno));
	 }
      draw_one_line (line_disp, 0);
      }
   if (line >= 0 && line < numitems)
      {
      if (use_list)
	 {
	 if (al_lseek (list, line, SEEK_SET))
	    fatal_error ("%s line=%d (%s)",
	       msg_unexpected, line, al_astrerror (al_aerrno));
	 if (line == 0)
	    al_lrewind (list);
	 }
      draw_one_line (line, 1);
      }
   line_disp = line;
   }
}


/*
 *	draw_one_line
 *	Display just one line of a menu.
 *
 *  	<line> is the number of the option to draw (0 = first option).
 *	<highlighted> tells whether the option should be drawn highlighted.
 *	
 *	If <shortcut_index> is NULL, options are numbered
 *	(the string "(#)" is prepended to each line).
 */
void menu_c::draw_one_line (int line, int highlighted)
{
int y = ly0 + FONTH * line;
int x = ix0 + FONTW;
const char *text = use_list ? (*getstr) (al_lptr (list)) : menustr[line];
i8  shortcut_index = this->shortcut_index ? this->shortcut_index[line] : -1;
int shortcut_key   = this->shortcut_key   ? this->shortcut_key  [line] : -1;
int grayed_out     = this->grayed_out     ? this->grayed_out    [line] : 0;

highlighted = !! highlighted;  // Force to [0..1]
grayed_out  = !! grayed_out;   // Force to [0..1]

HideMousePointer ();
set_colour (menu_colour[grayed_out][highlighted].bg);
DrawScreenBox (ix0, y, ix1, y + FONTH - 1);
set_colour (menu_colour[grayed_out][highlighted].fg);
if (ticked)
   {
   DrawScreenString (x, y, ticked[line] ? "*" : " ");
   x += 2 * FONTW;
   }
if (force_numbers || ! this->shortcut_index)
   {
   DrawScreenText (x, y, "(%c)", al_adigits[line + 1]);
   DrawScreenString (x + FONTW, y + FONTU, "_");
   x += 4 * FONTW;
   }
DrawScreenString (x, y, text);
if (! force_numbers && shortcut_index >= 0)
   DrawScreenString (x + shortcut_index * FONTW, y + FONTU, "_");
if (! force_numbers && shortcut_key != -1)
   {
   const char *string = key_to_string (shortcut_key);
   DrawScreenString (ix1 + 1 - FONTW - strlen (string) * FONTW, y, string);
   }
ShowMousePointer ();
}


