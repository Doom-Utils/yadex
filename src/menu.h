/*
 *	menu.h
 *	AYM 1998-08-15
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


#ifndef YH_MENUS  /* Prevent multiple inclusion */
#define YH_MENUS  /* Prevent multiple inclusion */


typedef struct 
  {
  acolour_t bg;
  acolour_t fg;
  } colour_pair_t;

/* First subscript : 0 = normal, 1 = grayed out
   Second subscript : 0 = normal, 1 = highlighted */
extern const colour_pair_t menu_colour[2][2];

typedef enum
  {
  MEN_GRAY = 0x01,  /* This menu item is grayed out */
  MEN_TICK = 0x02,  /* This menu item can be "ticked", boolean expr follows */
  } menu_flags_t;

// Values returned by process_event()
const int MEN_CANCEL  = -1;  // Exit by [Esc]. Caller should destroy the menu.
const int MEN_OTHER   = -2;  // Got other event and processed it.
const int MEN_INVALID = -3;  // Got invalid event. Caller should process it.

#include "edwidget.h"


class menu_c : public edwidget_c
{
	public :

/*
 *	Ctors
 */
inline menu_c () { memset (this, 0, sizeof *this); }

menu_c (const char *title, ...);

menu_c (
   const char   *title,
   al_llist_t   *list,
   const char   *(*getstr) (void *));

menu_c (
   const char	*title,
   int		numitems,
   int		*ticked,
   char         **menustr,
   const i8	*shortcut_index,
   const int	*shortcut_key,
   int		*grayed_out);

/*
 *	Dtors
 */
~menu_c ();

/*
 *	Configuration
 */
// Set the coordinates of the outer top left corner of
// the window. If <x> is < 0, the window will be centred
// horizontally. If <y> is < 0, the window will be centred
// vertically. By default, the window is put at the centre
// of the screen.
void set_coords (int x, int y);

// Set the title of the menu. The title is shown only if
// the menu is used in popup mode. By default there is
// no title.
void set_title (const char *title);

// Set the current item (the first item having number 0).
// By default the current item is set to 0 after creation.
inline void set_item_no (int item_no) { line = item_no; }

// Set (or clear) the popup flag.
// If the <popup> flag is set, the title of the menu is shown,
// and button releases are treated differently.
// By default, the popup flag is not set.
inline void set_popup (int popup)
   {
   if (!! popup != !! this->popup)
      need_geom = 1;  // Force geom() to be called
   this->popup = popup;
   }

// Set (or clear) the force_numbers flags.
// If the <force_numbers> flag is set,
// - shortcut keys and shortcut by index are neither shown
//   nor recognized,
// - the items are numbered [0-9A-Z] and can be selected by
//   pressing the corresponding key.
inline void set_force_numbers (int force_numbers)
   {
   if (!! force_numbers != !! this->force_numbers)
      need_geom = 1;  // Force geom() to be called.
   this->force_numbers = force_numbers;
   }

// Whether the widget should be visible.
inline void set_visible (int visible) { this->visible = visible; }

// Tick or untick a menu item.
void set_ticked (int item_no, int ticked);

// Gray-out or ungray-out a menu item.
void set_grayed_out (int item_no, int grayed_out);

/*
 *	Event processing
 */
int process_event (const input_status_t *is);
int last_shortcut_key ();


/*
 *	Widget functions
 */
void draw ();
inline void undraw () { }  // I can't undraw myself
inline int can_undraw () { return 0; }  // I can't undraw myself

inline int need_to_clear ()
   {
   return ! visible && visible_disp || need_geom;
   }

inline void clear () { visible_disp = 0; }
inline int req_width () { if (need_geom) geom (); return width; }
inline int req_height () { if (need_geom) geom ();  return height; }
inline int get_x0 () { return ox0_disp; }
inline int get_y0 () { return oy0_disp; }
inline int get_x1 () { return ox0_disp + width_disp - 1; }
inline int get_y1 () { return oy0_disp + height_disp - 1; }

	private :

void init (
   const char *title,			// Title of window (or NULL)
   int numitems,			// < 0 if in list mode
   int *ticked,				// Always NULL in list mode
   void *data,				// List or array of strings
   const char *(*getstr)(void *),	// Only in list mode
   const i8 *shortcut_index,		// Always NULL in list mode
   const int *shortcut_key,		// Always NULL in list mode
   int *grayed_out,			// Always NULL in list mode
   int delete_after_use);

void draw_one_line (int line, int highlighted);
void geom ();

// Menu data
const char *title;		// The title (or NULL if none)
size_t     title_len;		// Length of the title (or 0 if none)
int        use_list;		// Do the items come from a list ?
al_llist_t *list;		// The list they come from (or NULL)
const char *(*getstr) (void *);	// Only in list mode
int        numitems;		// The number of items
const char *const *menustr;	// The array they come from (or NULL)
size_t     items_ks_len;	// Length of the longest item counting key sc.
size_t     items_len;		// Length of the longest item not counting k.s.
int        *ticked;
const i8   *shortcut_index;
const int  *shortcut_key;
int        *grayed_out;
i8         delete_after_use;	// Should delete menustr, t, sc_i, sc_k and g_o
int        cooked_index_shortcuts[36];
i8         popup;		// Used as popup (not pull-down) menu.
				// If set, the title is shown and button
				// releases are treated differently.
i8         force_numbers;	// Force the use of standard shortcuts [0-9A-Z]
				// even if index/key shortcuts exist.

// Geometry as the user would like it to be
int    user_ox0;		// Top left corner. < 0 means centered.
int    user_oy0;		// Top left corner. < 0 means centered.

// Geometry as it should be
int    ix0, iy0, ix1, iy1;	// Corners of inner edge of window
int    ox0, oy0, ox1, oy1;	// Corners of outer edge of window
int    width;			// Overall width of window
int    height;			// Overall height of window
int    ty0;			// Y-pos of title
int    ly0;			// Y-pos of first item

// Geometry as it is displayed
int    ox0_disp;
int    oy0_disp;
int    width_disp;
int    height_disp;

// Status
i8     need_geom;		// Need to call geom() again
i8     visible;			// Should the menu be visible ?
i8     visible_disp;		// Is it really visible ?
i8     line;			// Which line should be highlighted ?
i8     line_disp;		// Which line is actually highlighted ?
int    _last_shortcut_key;	// Shortcut key for last selected item.
};


#endif  /* Prevent multiple inclusion */

