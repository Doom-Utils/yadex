/*
 *	entry.cc
 *	Entry "widgets" (ahem).
 *	AYM 1998-11-30
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
#include "entry.h"
#include "gfx.h"


/*
   display the integer input box
*/
int InputInteger (int x0, int y0, int *valp, int minv, int maxv)
{
int  key, val;
Bool neg, ok, firstkey;
int entry_out_x0;
int entry_out_y0;
int entry_out_x1;
int entry_out_y1;
int entry_text_x0;
int entry_text_y0;
int entry_text_x1;
int entry_text_y1;

entry_out_x0  = x0;
entry_text_x0 = entry_out_x0 + HOLLOW_BORDER + NARROW_HSPACING;
entry_text_x1 = entry_text_x0 + 7 * FONTW - 1;
entry_out_x1  = entry_text_x1 + HOLLOW_BORDER + NARROW_HSPACING;
entry_out_y0  = y0;
entry_text_y0 = entry_out_y0 + HOLLOW_BORDER + NARROW_VSPACING;
entry_text_y1 = entry_text_y0 + FONTH - 1;
entry_out_y1  = entry_text_y1 + HOLLOW_BORDER + NARROW_VSPACING;
DrawScreenBoxHollow (entry_out_x0, entry_out_y0, entry_out_x1, entry_out_y1, BLACK);
neg = (*valp < 0);
val = neg ? -(*valp) : *valp;
firstkey = 1;
for (;;)
   {
   ok = (neg ? -val : val) >= minv && (neg ? -val : val) <= maxv;
   set_colour (BLACK);
   DrawScreenBox (entry_text_x0, entry_text_y0, entry_text_x1, entry_text_y1);
   if (ok)
      set_colour (WHITE);
   else
      set_colour (LIGHTGRAY);
   if (neg)
      DrawScreenText (entry_text_x0, entry_text_y0, "-%d", val);
   else
      DrawScreenText (entry_text_x0, entry_text_y0, "%d",  val);
   key = get_key ();
   if (firstkey && is_ordinary (key) && key != ' ')
      {
      val = 0;
      neg = 0;
      }
   firstkey = 0;
   if (val < 3275 && key >= '0' && key <= '9')
      val = val * 10 + dectoi (key);
   else if (val > 0 && key == YK_BACKSPACE)
      val = val / 10;
   else if (neg && key == YK_BACKSPACE)
      neg = 0;
   else if (key == '-')
      neg = !neg;
   else if (ok && key == YK_RETURN)
      break; /* return "val" */
   else if (key == YK_LEFT || key == YK_RIGHT
	 || key == YK_UP   || key == YK_DOWN
	 || key == YK_TAB  || key == YK_BACKTAB)
      break; /* return "val", even if not valid */
   else if (key == YK_ESC)
      {
      neg = 0;
      val = IIV_CANCEL; /* return a value out of range */
      break;
      }
   else
      Beep ();
   }
if (neg)
   *valp = -val;
else
   *valp = val;
is.key = 0;  // Shouldn't have to do that but EditorLoop() is broken
return key;
}


/*
   ask for an integer value and check for minimum and maximum
*/
int InputIntegerValue (int x0, int y0, int minv, int maxv, int defv)
{
int  val, key;
char prompt[80];

HideMousePointer ();
y_snprintf (prompt, sizeof prompt, "Enter a decimal number between %d and %d:",
   minv, maxv);
if (x0 < 0)
   x0 = (ScrMaxX - 25 - FONTW * strlen (prompt)) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - 55) / 2;
DrawScreenBox3D (x0, y0, x0 + 25 + FONTW * strlen (prompt), y0 + 55);
set_colour (WHITE);
DrawScreenText (x0 + 10, y0 + 8, prompt);
val = defv;
while ((key = InputInteger (x0 + 10, y0 + 28, &val, minv, maxv)) != YK_RETURN
 && key != YK_ESC)
   Beep ();
ShowMousePointer ();
return val;
}


/*
   ask for a filename
*/
void InputFileName (int x0, int y0, const char *prompt, size_t maxlen,
   char *filename)
{
int   key;
size_t l;
size_t boxlen;
Bool  firstkey;
int width;
int title_y0;
int entry_out_x0;
int entry_out_y0;
int entry_out_x1;
int entry_out_y1;
int entry_text_x0;
int entry_text_y0;
int entry_text_x1;
int entry_text_y1;

HideMousePointer ();
for (l = strlen (filename) + 1; l <= maxlen; l++)
   filename [l] = '\0';
/* compute the width of the input box */
if (maxlen > 20)
   boxlen = 20;
else
   boxlen = maxlen;
/* compute the width of the dialog box */
if (strlen (prompt) > boxlen)
   l = strlen (prompt);
else
   l = boxlen;

width = 2 * HOLLOW_BORDER + 2 * NARROW_HSPACING + boxlen * FONTW;
if ((int) (strlen (prompt) * FONTW) > width)
   width = strlen (prompt) * FONTW;
width += 2 * BOX_BORDER + 2 * WIDE_HSPACING;

if (x0 < 0)
   x0 = (ScrMaxX - width) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - 2 * BOX_BORDER
                 - 2 * WIDE_VSPACING
                 - (int) (2.5 * FONTH)
                 - 2 * HOLLOW_BORDER
                 - 2 * NARROW_VSPACING) / 2;
/* draw the dialog box */
entry_out_x0  = x0 + BOX_BORDER + WIDE_HSPACING;
entry_text_x0 = entry_out_x0  + HOLLOW_BORDER + NARROW_HSPACING;
entry_text_x1 = entry_text_x0 + boxlen * FONTW - 1;
entry_out_x1  = entry_text_x1 + NARROW_HSPACING + HOLLOW_BORDER;
title_y0      = y0 + BOX_BORDER + WIDE_VSPACING;
entry_out_y0  = title_y0 + (int) (1.5 * FONTH);
entry_text_y0 = entry_out_y0  + HOLLOW_BORDER + NARROW_VSPACING;
entry_text_y1 = entry_text_y0 + FONTH - 1;
entry_out_y1  = entry_text_y1 + NARROW_VSPACING + HOLLOW_BORDER;

DrawScreenBox3D (x0, y0, x0 + width - 1, entry_out_y1 + WIDE_VSPACING);
DrawScreenBoxHollow (entry_out_x0, entry_out_y0, entry_out_x1, entry_out_y1, BLACK);
set_colour (YELLOW);
DrawScreenText (entry_out_x0, title_y0, prompt);
firstkey = 1;
for (;;)
   {
   l = strlen (filename);
   set_colour (BLACK);
   DrawScreenBox (entry_text_x0, entry_text_y0, entry_text_x1, entry_text_y1);
   set_colour (WHITE);
   if (l > boxlen)
      {
      DrawScreenText (entry_text_x0, entry_text_y0, "<%s",
	  filename + (l - boxlen + 1));
      }
   else
      DrawScreenText (entry_text_x0, entry_text_y0, filename);
   key = get_key ();
   if (firstkey && is_ordinary (key))
      {
      for (l = 0; l <= maxlen; l++)
	 filename[l] = '\0';
      l = 0;
      }
   firstkey = 0;
   if (l < maxlen && is_ordinary (key))
      {
      filename[l] = key;
      filename[l + 1] = '\0';
      }
   else if (l > 0 && key == YK_BACKSPACE)
      filename[l-1] = '\0';
   else if (key == YK_RETURN)
      break;  /* return "filename" */
   else if (key == YK_ESC)
      {
      filename[0] = '\0'; /* return an empty string */
      break;
      }
   else
      Beep ();
   }
ShowMousePointer ();
is.key = 0;  // Shouldn't have to do that but EditorLoop() is broken
}


