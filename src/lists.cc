/*
 *	list.cc
 *	Pick an item from a list.
 *	AYM 1998-08-22
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
   int listsize,
   const char *const *list,
   int listdisp,
   char *name,
   int width,
   int height,
   void (*hookfunc)(hookfunc_comm_t *c))
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
   if (strlen (msg1) + 2 > l0)
      l0 = strlen (msg1) + 2;
   if (strlen (msg2) + 2 > l0)
      l0 = strlen (msg2) + 2;
   }
xlist = 10 + l0 * FONTW;
l = l0 + maxlen;
if (strlen (prompt) > l)
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
   /* test if "name" is in the list */
   for (n = 0; n < listsize; n++)
      if (strcmp (name, list[n]) <= 0)
	 break;
   ok = n < listsize ? !strcmp (name, list[n]) : 0;
   if (n > listsize - 1)
      n = listsize - 1;
   /* display the "listdisp" next items in the list */
   set_colour (WINBG);
   DrawScreenBox (x0 + xlist,                  entry_out_y0,
                  x0 + xlist + FONTW * maxlen, entry_out_y0 + FONTH * listdisp);
   set_colour (WINFG);
   for (l = 0; l < listdisp && n + l < listsize; l++)
      DrawScreenText (x0 + xlist, entry_out_y0 + l * FONTH, list[n+l]);
   l = strlen (name);
   set_colour (BLACK);
   DrawScreenBox (entry_text_x0, entry_text_y0, entry_text_x1, entry_text_y1);
   if (ok)
      set_colour (WHITE);
   else
      set_colour (WINFG_DIM);
   DrawScreenText (entry_text_x0, entry_text_y0, name);
   /* call the function to display the picture, if any */
   if (hookfunc)
      {
      /* clear the window */
      set_colour (BLACK);
      DrawScreenBox (x1, y1, x2, y2);
      /* display the picture "name" */
      c.x0   = x1;
      c.y0   = y1;
      c.x1   = x2;
      c.y1   = y2;
      c.name = name;
      if (picture_size_drawn)
	{
	set_colour (WINBG);
	DrawScreenBox (x0 + 10, y0 + 50,
           x0 + 10 + 10 * FONTW - 1, y0 + 50 + FONTH - 1);
	picture_size_drawn = 0;
	}
      hookfunc (&c);
      if ((c.flags & HOOK_SIZE_VALID) && (c.flags & HOOK_DISP_SIZE))
	{
	set_colour (WINFG);
	DrawScreenText (x0 + 10, y0 + 50, "%dx%d", c.width, c.height);
	picture_size_drawn = 1;
	}
      }
   /* process user input */
   key = get_key ();
   if (firstkey && is_ordinary (key) && key != ' ')
      {
      for (l = 0; l <= maxlen; l++)
	 name[l] = '\0';
      l = 0;
      }
   firstkey = 0;
   if (l < maxlen && key >= 'a' && key <= 'z')
      {
      name[l] = key + 'A' - 'a';
      name[l + 1] = '\0';
      }
   else if (l < maxlen && is_ordinary (key) && key != ' ')
      {
      name[l] = key;
      name[l + 1] = '\0';
      }
   else if (l > 0 && key == YK_BACKSPACE)
      name[l - 1] = '\0';
   else if (n < listsize - 1 && key == YK_DOWN)
      strcpy (name, list[n + 1]);
   else if (n > 0 && key == YK_UP)
      strcpy (name, list[n - 1]);
   else if (n < listsize - listdisp && key == YK_PD)
      strcpy (name, list[min (n + listdisp, listsize - 1)]); // AYM
   else if (n > 0 && key == YK_PU)
      {
      if (n > listdisp)
	 strcpy (name, list[n - listdisp]);
      else
	 strcpy (name, list[0]);
      }
   else if (key == YK_END)
      strcpy (name, list[listsize - 1]);
   else if (key == YK_HOME)
      strcpy (name, list[0]);
   else if (key == YK_F1 + YK_SHIFT   /* Shift-F1 : dump image to file */
    && hookfunc != NULL
    && (c.flags & HOOK_DRAWN))
      {
      SavePic (c.x0, c.y0, c.width, c.height, c.name);  /* FIXME */
      }
   else if (key == YK_TAB)
      strcpy (name, list[n]);
   else if (ok && key == YK_RETURN)
      break; /* return "name" */
   else if (key == YK_ESC)
      {
      name[0] = '\0'; /* return an empty string */
      break;
      }
   else
      Beep ();
   }
}



/*
   ask for a name in a given list
*/

void InputNameFromList (
   int x0,
   int y0,
   const char *prompt,
   int listsize,
   const char *const *list,
   char *name)
{
HideMousePointer ();
InputNameFromListWithFunc (x0, y0, prompt, listsize, list, 5, name, 0, 0, NULL);
ShowMousePointer ();
}




