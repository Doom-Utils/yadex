/*
 *	list.cc
 *	Pick an item from a list.
 *	AYM 1998-08-22
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


#include "yadex.h"
#include "gfx.h"


/* FIXME move this to yadex.h */
void SavePic (int x0, int y0, int width, int height, const char *name);


/*
   ask for a name in a given list and call a function (for displaying objects,
   etc.)

   Arguments:
      x0, y0  : where to draw the box.
      prompt  : text to be displayed.
      listsize: number of elements in the list.
      list    : list of names (picture names, level names, etc.).
      listdisp: number of lines that should be displayed.
      name    : what we are editing...
      width   : \ width and height of an optional window where a picture
      height  : / can be displayed (used to display textures, sprites, etc.).
      hookfunc: function that should be called to display a picture.
		(x1, y1, x2, y2 = coordinates of the window in which the
		 picture must be drawn, name = name of the picture).
AYM 1998-02-12 : if hookfunc is <> NULL, a message "Press shift-F1 to save
  image to file" is displayed and shift-F1 does just that.
AYM 1998-08-22 : listdisp is now ignored and overwritten.
*/

void InputNameFromListWithFunc (
   int x0,
   int y0,
   const char *prompt,
   size_t listsize,
   const char *const *list,
   int listdisp,
   char *name,
   int width,
   int height,
   void (*hookfunc)(hookfunc_comm_t *c),
   char flags_to_pass_to_callback)
{
const char *msg1 = "Press Shift-F1 to";
const char *msg2 = "save image to file";
int    key;
size_t n;
int    l;
int    l0;
int    x1, y1, x2, y2;
size_t maxlen;
int    xlist;
int    picture_size_drawn = 0;
Bool   ok, firstkey;
int    entry_out_x0;	/* Edge of name entry widget including border */
int    entry_out_y0;
int    entry_out_x1;
int    entry_out_y1;
int    entry_text_x0;	/* Edge of text area of name entry widget */
int    entry_text_y0;
int    entry_text_x1;
int    entry_text_y1;

/* compute maxlen */
maxlen = 1;
for (n = 0; n < listsize; n++)
   if (strlen (list[n]) > maxlen)
      maxlen = strlen (list[n]);
for (n = strlen (name) + 1; n <= maxlen; n++)
   name[n] = '\0';
/* compute the minimum width of the dialog box */
l0 = 12;
if (hookfunc != NULL)
   {
   if ((int) (strlen (msg1) + 2) > l0)  // (int) to prevent GCC warning
      l0 = strlen (msg1) + 2;
   if ((int) (strlen (msg2) + 2) > l0)  // (int) to prevent GCC warning
      l0 = strlen (msg2) + 2;
   }
xlist = 10 + l0 * FONTW;
l = l0 + maxlen;
if ((int) (strlen (prompt)) > l)  // (int) to prevent GCC warning
   l = strlen (prompt);
l = 10 + FONTW * l;
x1 = l + 8;
y1 = 10 + 1;
if (width > 0)
   l += 16 + width;
if (height > 65)
   n = height + 20;
else
   n = 85;
if (x0 < 0)
   x0 = (ScrMaxX - l) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - n) / 2;
x1 += x0;
y1 += y0;
if (x1 + width - 1 < ScrMaxX)
   x2 = x1 + width - 1;
else
   x2 = ScrMaxX;
if (y1 + height - 1 < ScrMaxY)
   y2 = y1 + height - 1;
else
   y2 = ScrMaxY;

entry_out_x0  = x0 + 10;
entry_text_x0 = entry_out_x0 + HOLLOW_BORDER + NARROW_HSPACING;
entry_text_x1 = entry_text_x0 + 10 * FONTW - 1;
entry_out_x1  = entry_text_x1 + HOLLOW_BORDER + NARROW_HSPACING;
entry_out_y0  = y0 + 28;
entry_text_y0 = entry_out_y0 + HOLLOW_BORDER + NARROW_VSPACING;
entry_text_y1 = entry_text_y0 + FONTH - 1;
entry_out_y1  = entry_text_y1 + HOLLOW_BORDER + NARROW_VSPACING;

listdisp = (n - (entry_out_y0 - y0) - BOX_BORDER - WIDE_VSPACING) / FONTH;

/* draw the dialog box */
DrawScreenBox3D (x0, y0, x0 + l, y0 + n);
DrawScreenBoxHollow (entry_out_x0, entry_out_y0, entry_out_x1, entry_out_y1, BLACK);
set_colour (YELLOW);
DrawScreenText (x0 + 10, y0 + 8, prompt);
set_colour (WINFG);
if (hookfunc != NULL)
   {
   DrawScreenText (x0 + 10, y0 + n - BOX_BORDER - WIDE_VSPACING - 2 * FONTH,
      msg1);
   DrawScreenText (x0 + 10, y0 + n - BOX_BORDER - WIDE_VSPACING - FONTH, msg2);
   }
if (width > 0)
   DrawScreenBoxHollow (x1 - 1, y1 - 1, x2 + 1, y2 + 1, BLACK);
firstkey = 1;
for (;;)
   {
   hookfunc_comm_t c;
   // Test if "name" is in the list
   for (n = 0; n < listsize; n++)
      if (y_stricmp (name, list[n]) <= 0)
	 break;
   ok = n < listsize ? ! y_stricmp (name, list[n]) : 0;
   if (n >= listsize)
      n = listsize - 1;

   // Display the <listdisp> next items in the list
   {
   int l;				// Current line
   int y = entry_out_y0;		// Y-coord of current line
   int xmin = x0 + xlist;
   int xmax = xmin + FONTW * maxlen - 1;
   for (l = 0; l < listdisp && n + l < listsize; l++)
      {
      set_colour (WINBG);
      DrawScreenBox (xmin, y, xmax, y + FONTH - 1);
      set_colour (WINFG);
      DrawScreenText (xmin, y, list[n+l]);
      y += FONTH;
      }
   if (l < listdisp)  // Less than <listdisp> names to display
      {
      set_colour (WINBG);
      DrawScreenBox (xmin, y, xmax, entry_out_y0 + listdisp * FONTH - 1);
      }
   }

   // Display the entry box and the current text
   set_colour (BLACK);
   DrawScreenBox (entry_text_x0, entry_text_y0, entry_text_x1, entry_text_y1);
   if (ok)  // FIXME this colour scheme should be changed.
      set_colour (WHITE);
   else
      set_colour (WINFG);
   DrawScreenText (entry_text_x0, entry_text_y0, name);

   // Call the function to display the picture, if any
   if (hookfunc)
      {
      /* clear the window */
      set_colour (BLACK);
      DrawScreenBox (x1, y1, x2, y2);
      /* display the picture "name" */
      c.x0    = x1;
      c.y0    = y1;
      c.x1    = x2;
      c.y1    = y2;
      c.name  = name;
      c.flags = flags_to_pass_to_callback;
      if (picture_size_drawn)
	 {
	 set_colour (WINBG);
	 DrawScreenBox (x0 + 10, y0 + 50,
	    x0 + 10 + 10 * FONTW - 1, y0 + 50 + FONTH - 1);
	 picture_size_drawn = 0;
	 }
      if (ok)
	 hookfunc (&c);
      if ((c.flags & HOOK_SIZE_VALID) && (c.flags & HOOK_DISP_SIZE))
	 {
	 set_colour (WINFG);
	 DrawScreenText (x0 + 10, y0 + 50, "%dx%d", c.width, c.height);
	 picture_size_drawn = 1;
	 }
      }

   // Process user input
   key = get_key ();
   if (firstkey && is_ordinary (key) && key != ' ')
      {
      for (size_t i = 0; i <= maxlen; i++)
	 name[i] = '\0';
      }
   firstkey = 0;
   size_t len = strlen (name);
   if (len < maxlen && key >= 'a' && key <= 'z')
      {
      name[len] = key + 'A' - 'a';
      name[len + 1] = '\0';
      }
   else if (len < maxlen && is_ordinary (key) && key != ' ')
      {
      name[len] = key;
      name[len + 1] = '\0';
      }
   else if (len > 0 && key == YK_BACKSPACE)		// BS
      name[len - 1] = '\0';
   else if (key == 21 || key == 23)			// ^U, ^W
      *name = '\0';
   else if (key == YK_DOWN)				// [Down]
      {
      /* Look for the next item in the list that has a
	 different name. Why not just use the next item ?
	 Because sometimes the list has duplicates (for example
	 when editing a Doom II pwad in Doom mode) and then the
	 viewer gets "stuck" on the first duplicate. */
      size_t m = n + 1;
      while (m < listsize && ! y_stricmp (list[n], list[m]))
	 m++;
      if (m < listsize)
         strcpy (name, list[m]);
      else
	 Beep ();
      }
   else if (key == YK_UP)				// [Up]
      {
      // Same trick as for [Down]
      int m = n - 1;
      while (m >= 0 && ! y_stricmp (list[n], list[m]))
	 m--;
      if (m >= 0)
         strcpy (name, list[m]);
      else
         Beep ();
      }
   else if (key == YK_PD || key == 6 || key == 22)	// [Pgdn], ^F, ^V
      { 
      if (n < listsize - listdisp)
         strcpy (name, list[y_min (n + listdisp, listsize - 1)]);
      else
	 Beep ();
      }
   else if ((key == YK_PU || key == 2) && n > 0)	// [Pgup], ^B
      {
      if ((int) n > listdisp)
	 strcpy (name, list[n - listdisp]);
      else
	 strcpy (name, list[0]);
      }
   else if (key == 14)					// ^N
      {
      if (n + 1 >= listsize)
	 {
	 Beep ();
         goto done_with_event;
	 }
      while (n + 1 < listsize)
	 {
	 n++;
	 if (y_strnicmp (list[n - 1], list[n], 4))
	    break;
	 }
      strcpy (name, list[n]);
      }
   else if (key == 16)					// ^P
      {
      if (n < 1)
	 {
	 Beep ();
	 goto done_with_event;
	 }
      // Put in <n> the index of the first entry of the current
      // group or, if already at the beginning of the current
      // group, the first entry of the previous group.
      if (n > 0)
	 {
	 if (y_strnicmp (list[n], list[n - 1], 4))
	    n--;
	 while (n > 0 && ! y_strnicmp (list[n], list[n - 1], 4))
	    n--;
	 }
      strcpy (name, list[n]);
      }
   else if (key == (YK_CTRL | YK_PD) || key == YK_END)	// [Ctrl][Pgdn], [End]
      {
      if (n + 1 >= listsize)
	 {
	 Beep ();
	 goto done_with_event;
	 }
      strcpy (name, list[listsize - 1]);
      }
   else if (key == (YK_CTRL | YK_PU) || key == YK_HOME)	// [Ctrl][Pgup], [Home]
      {
      if (n < 1)
	 {
	 Beep ();
	 goto done_with_event;
	 }
      strcpy (name, list[0]);
      }
   else if (key == YK_F1 + YK_SHIFT	// [Shift][F1] : dump image to file
    && hookfunc != NULL
    && (c.flags & HOOK_DRAWN))
      {
      SavePic (c.x0, c.y0, c.width, c.height, c.name);  /* FIXME */
      }
   else if (key == YK_TAB)				// [Tab]
      strcpy (name, list[n]);
   else if (ok && key == YK_RETURN)			// [Return]
      break; /* return "name" */
   else if (key == YK_ESC)				// [Esc]
      {
      name[0] = '\0'; /* return an empty string */
      break;
      }
   else
      Beep ();
done_with_event:
   ;
   }
}



/*
   ask for a name in a given list
*/

void InputNameFromList (
   int x0,
   int y0,
   const char *prompt,
   size_t listsize,
   const char *const *list,
   char *name)
{
HideMousePointer ();
InputNameFromListWithFunc (x0, y0, prompt, listsize, list, 5, name, 0, 0, NULL);
ShowMousePointer ();
}




