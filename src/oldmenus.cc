/*
 *	oldmenus.cc
 *	Old-fashioned menu functions, somewhat similar to the
 *	ones DEU 5.21 had in menus.c. Since they contain an event
 *	loop of their own, they can't handle expose and configure
 *	notify events properly. When I have the time, I'll
 *	replace this system by a modal widgets stack in
 *	edisplay.cc.
 *	AYM 1998-12-03
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1998 André Majorel.

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
#include <X11/Xlib.h>
#include "events.h"
#include "gfx.h"
#include "menu.h"
#include "oldmenus.h"


static int loop (menu_c *menu, int x, int y, int item_no);


/*
 *	DisplayMenu
 *	Display and execute a popup menu
 *	defined with variable arguments.
 */
int DisplayMenu (int x0, int y0, const char *menutitle, ...)
{
va_list args;
int     num;
char   *menustr[36];

/* put the va_args in the menustr table */
num = 0;
va_start (args, menutitle);
while ((num < 36) && ((menustr[num] = va_arg (args, char *)) != NULL))
   num++;
va_end (args);

/* display the menu */
return DisplayMenuArray (x0, y0, menutitle, num, NULL, menustr, NULL, NULL, NULL);
}


/*
 *	DisplayMenuArray
 *
 *	Display and execute a menu.
 *	Return the number of the selected item (1-based)
 *	or 0 if exited by [Esc].
 *	This function is deprecated.
 */
int DisplayMenuArray (
   int		x0,
   int		y0,
   const char	*menutitle,
   int		numitems,
   int		*ticked,
   char		**menustr,		/* Strings to display */
   const i8	*shortcut_index,
   const int	*shortcut_key,
   int		*grayed_out)
{
menu_c *menu = new menu_c (
   menutitle,
   numitems,
   ticked,
   menustr,
   shortcut_index,
   shortcut_key,
   grayed_out);
int r = loop (menu, x0, y0, 0);
delete menu;
return r + 1;
}


/*
 *	DisplayMenuList
 *	Display and execute a memu contained in a list. Each menu item
 *	string is obtained by calling (*getstr)() with a pointer on
 *	the list item.
 *	Return the number of the selected item (0-based)
 *	or -1 if exited by [Esc].
 *	This function is deprecated.
 */
int DisplayMenuList (
   int		x0,
   int		y0,
   const char	*menutitle,
   al_llist_t	*list,
   const char	*(*getstr)(void *),
   int		*item_no)
{
menu_c *menu = new menu_c (menutitle, list, getstr);
int r = loop (menu, x0, y0, item_no ? *item_no : 0);
if (item_no && r >= 0)
   *item_no = r;
delete menu;
return r;
}


/*
 *	loop
 *	Display and execute and menu.
 *	Return the number of the selected item (0-based)
 *	or -1 if exited by [Esc].
 *
 *	This function is nothing more than a quick and dirty
 *	hack, provided as a stopgap until the widget stack is
 *	implemented and the code uses it. 
 */
static int loop (menu_c *menu, int x, int y, int item_no)
{
menu->set_popup         (1);
menu->set_force_numbers (1);
menu->set_coords        (x, y);
menu->set_visible       (1);
menu->set_item_no       (item_no);
for (;;)
   {
   menu->draw ();
   if (has_event ())
      {
      is.key = get_event ();
      // If we had called get_input_status(), XNextEvent()
      // would have been called and we wouldn't have to do that.
      XFlush (dpy);
      }
   else
      get_input_status ();
   if (is.key == YE_EXPOSE)
      menu->clear ();  // Force menu to redraw itself from scratch
   else if (is.key == YE_BUTL_PRESS
      && (is.x < menu->get_x0 ()
         || is.x > menu->get_x1 ()
         || is.y < menu->get_y0 ()
         || is.y > menu->get_y1 ()))
      return -1;
   else
      {
      int r = menu->process_event (&is);
      if (r == MEN_CANCEL)
         return -1;
      else if (r >= 0)
         return r;
      }
   }
}




