/*
 *	dialog.cc
 *	Dialog boxes.
 *	AYM 1998-11-30
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
#include <X11/Xlib.h>
#include "dialog.h"
#include "gfx.h"


/*
   ask for confirmation (prompt2 may be NULL)
   Returns zero for "no", non-zero for "yes".
*/
Bool Confirm (int x0, int y0, const char *prompt1, const char *prompt2)
{
const char *const prompt3 = "Press Y to confirm or any other key to cancel";
int key;
size_t maxlen;
double n_lines_of_text;
int width;
int height;
int text_x0;
int text_x1;
int x1;
int text_y0;
int text_y1;
int y1;

HideMousePointer ();
maxlen = strlen (prompt3);
if (strlen (prompt1) > maxlen)
   maxlen = strlen (prompt1);
if (prompt2 != NULL && strlen (prompt2) > maxlen)
   maxlen = strlen (prompt2);
n_lines_of_text = (prompt2 == NULL ? 2.5 : 3.5);
width = 2 * BOX_BORDER + 2 * WIDE_HSPACING + maxlen * FONTW;
height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + (int) (n_lines_of_text * FONTH);
if (x0 < 0)
   x0 = (ScrMaxX - width) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - height) / 2;
text_x0 = x0 + BOX_BORDER + WIDE_HSPACING;
text_x1 = text_x0 + maxlen * FONTW - 1;
x1      = text_x1 + WIDE_HSPACING + BOX_BORDER;
text_y0 = y0 + BOX_BORDER + WIDE_VSPACING;
text_y1 = text_y0 + (int) (n_lines_of_text * FONTH) - 1;
y1      = text_y1 + WIDE_HSPACING + BOX_BORDER;
DrawScreenBox3D (x0, y0, x1, y1);
set_colour (WHITE);
DrawScreenText (text_x0, text_y0, prompt1);
if (prompt2 != NULL)
   DrawScreenText (text_x0, text_y0 + FONTH, prompt2);
set_colour (YELLOW);
DrawScreenText (text_x0, text_y1 - FONTH - 1, prompt3);
key = get_key ();
is.key = 0;  // Shouldn't have to do that but EditorLoop() is broken
ShowMousePointer ();
return (key == 'Y' || key == 'y');
}


/*
 *	Confirm2
 *	Ask for confirmation, in a smarter fashion
 *	Returns zero for "no", non-zero for "yes".
 */
int Confirm2 (int x0, int y0, confirm_t *confirm_flag,
   const char *prompt1, const char *prompt2)
{
int r;

if (*confirm_flag == YC_YES)
   return 1;
if (*confirm_flag == YC_NO)
   return 0;
r = Confirm (x0, y0, prompt1, prompt2);
if (*confirm_flag == YC_ASK_ONCE)
   *confirm_flag = r ? YC_YES : YC_NO;  // We won't ask again
return r;
}


/*
   display a notification and wait for a key (prompt2 may be NULL)
*/
void Notify (int x0, int y0, const char *prompt1, const char *prompt2)
{
const char *const prompt3 = "Press any key to continue...";
size_t maxlen;
double n_lines_of_text;
int width;
int height;
int text_x0;
int text_x1;
int x1;
int text_y0;
int text_y1;
int y1;

HideMousePointer ();
maxlen = strlen (prompt3);
if (strlen (prompt1) > maxlen)
   maxlen = strlen (prompt1);
if (prompt2 != NULL && strlen (prompt2) > maxlen)
   maxlen = strlen (prompt2);
n_lines_of_text = (prompt2 == NULL ? 2.5 : 3.5);
width = 2 * BOX_BORDER + 2 * WIDE_HSPACING + maxlen * FONTW;
height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + (int) (n_lines_of_text * FONTH);
if (x0 < 0)
   x0 = (ScrMaxX - width) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - height) / 2;
text_x0 = x0 + BOX_BORDER + WIDE_HSPACING;
text_x1 = text_x0 + maxlen * FONTW - 1;
x1      = text_x1 + WIDE_HSPACING + BOX_BORDER;
text_y0 = y0 + BOX_BORDER + WIDE_VSPACING;
text_y1 = text_y0 + (int) (n_lines_of_text * FONTH) - 1;
y1      = text_y1 + WIDE_HSPACING + BOX_BORDER;
DrawScreenBox3D (x0, y0, x1, y1);
set_colour (WHITE);
DrawScreenText (text_x0, text_y0, prompt1);
if (prompt2 != NULL)
   DrawScreenText (text_x0, text_y0 + FONTH, prompt2);
set_colour (YELLOW);
DrawScreenText (text_x0, text_y1 - FONTH - 1, prompt3);
get_key_or_click ();
ShowMousePointer ();
}


/*
 *	debmes - Display a message in a box only if in debug mode
 *
 *	Simple wrapper around Notify(). Don't try to make it display
 *	more than 200 characters or you'll crash the program.
 *	BUG: if result of formatting contains "%"'s, it will be
 *	formatted again...
 */
void debmes (const char *fmt, ...)
{
char buf[200];
va_list arglist;

if (Debug != 1)
  return;
va_start (arglist, fmt);

#ifdef Y_SNPRINTF
vsnprintf (buf, sizeof buf, fmt, arglist);
#else
vsprintf (buf, fmt, arglist);
#endif

#ifdef Y_BGI
setviewport (0, 0, ScrMaxX, ScrMaxY, 1);
#endif /* FIXME! */
Notify (-1, -1, buf, NULL);
}


/*
   clear the screen and display a message
*/
void DisplayMessage (int x0, int y0, const char *msg, ...)
{
char prompt[120];
va_list args;

va_start (args, msg);
#ifdef Y_SNPRINTF
vsnprintf (prompt, sizeof prompt, msg, args);
#else
vsprintf (prompt, msg, args);
#endif
va_end (args);

int width = 2 * BOX_BORDER + 2 * WIDE_HSPACING + FONTW * strlen (prompt);
int height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + FONTH;
if (x0 < 0)
   x0 = (ScrMaxX - width) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - height) / 2;
HideMousePointer ();
DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
push_colour (WINFG);
DrawScreenText (x0 + BOX_BORDER + WIDE_HSPACING,
   y0 + BOX_BORDER + WIDE_VSPACING, prompt);
pop_colour ();
ShowMousePointer ();
XFlush (dpy);
}


/*
   let's make the user angry...
*/
void NotImplemented (void)
{
Notify (-1, -1, "This function is not implemented... Yet!", NULL);
}


