/*
 *	dialog.cc
 *	Dialog boxes.
 *	AYM 1998-11-30
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2005 André Majorel and others.

This program is free software; you can redistribute it and/or modify it under
the terms of version 2 of the GNU Library General Public License as published
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#include "yadex.h"
#include <X11/Xlib.h>
#include "dialog.h"
#include "gfx.h"



/*
 *	confirm - ask for confirmation
 *
 *	Ask for confirmation. prompt2 may be NULL.
 *
 *	mode may be one of:
 *
 *	- YC_ASK:	Prompt is "[y]es, [n]o". The function returns
 *			true if [y] was pressed, false if [n] was
 *			pressed. [return] is an alias for [y]. [esc] is
 *			an alias for [n].
 *
 *	- YC_ASK_ONCE:	Same as YC_ASK except that if [y] ([n]) is
 *			pressed, mode is set to YC_YES (YC_NO) so that
 *			later calls return without prompting the user.
 *
 *	- YC_YNAN:	Prompt is "[y]es, [n]no, [a]ll, [N]one". The
 *			function returns true if [y] or [a] was pressed,
 *			false is [n] or [N] was pressed. In addition, if
 *			[a] ([N]) is pressed, mode is set to YC_YES
 *			(YC_NO) so that later calls return without
 *			prompting the user.
 *
 *			[return] is an alias for [a]. [esc] is an alias
 *			for [N].
 *
 *	- YC_YES:	Return true without even drawing the window.
 *
 *	- YC_NO:	Return false without even drawing the window.
 */
bool confirm (int x0, int y0, confirm_t &mode,
   const char *prompt1, const char *prompt2)
{
  const char *prompt3 = "Bug: bad mode";
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
  bool rc;

  if (mode == YC_YES)
    return true;
  if (mode == YC_NO)
    return false;

  if (mode == YC_ASK || mode == YC_ASK_ONCE)
    prompt3 = "[y]es, [n]o";
  else if (mode == YC_YNAN)
    prompt3 = "[y]es, [n]o, [a]ll, [N]one";
  else
    nf_bug ("confirm: bad mode %d", int (mode));

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
  HideMousePointer ();
  for (bool first_time = true; ; first_time = false)
  {
    if (first_time || is.key == YE_EXPOSE)
    {
      DrawScreenBox3D (x0, y0, x1, y1);
      set_colour (WHITE);
      DrawScreenText (text_x0, text_y0, prompt1);
      if (prompt2 != NULL)
	DrawScreenText (text_x0, text_y0 + FONTH, prompt2);
      set_colour (WINTITLE);
      DrawScreenText (text_x0, text_y1 - FONTH - 1, prompt3);
    }
    get_input_status ();

    // [esc] and [return] are aliases
    if (is.key == YK_RETURN)
    {
      if (mode == YC_ASK || mode == YC_ASK_ONCE)
	is.key = 'y';
      else if (mode == YC_YNAN)
	is.key = 'a';
      else
	nf_bug ("confirm: bad mode %d", int (mode));
    }
    if (is.key == YK_ESC)
    {
      if (mode == YC_ASK || mode == YC_ASK_ONCE)
	is.key = 'n';
      else if (mode == YC_YNAN)
	is.key = 'q';
      else
	nf_bug ("confirm: bad mode %d", int (mode));
    }

    if (is.key == 'y')
    {
      if (mode == YC_ASK_ONCE)
	mode = YC_YES;
      rc = true;
      break;
    }
    if (is.key == 'n')
    {
      if (mode == YC_ASK_ONCE)
	mode = YC_NO;
      rc = false;
      break;
    }
    if (is.key == 'a' && mode == YC_YNAN)
    {
      mode = YC_YES;
      rc = true;
      break;
    }
    if (is.key == 'N' && mode == YC_YNAN)
    {
      mode = YC_NO;
      rc = false;
      break;
    }
  }

  is.key = 0;  // Shouldn't have to do that but EditorLoop() is broken
  ShowMousePointer ();
  return rc;
}


/*
 *	confirm_simple - ask for confirmation
 *
 *	Ask for confirmation (prompt2 may be NULL). This is equivalent
 *	to confirm() with mode set to YC_ASK.
 *
 *	Return zero for "no", non-zero for "yes".
 */
bool confirm_simple (int x0, int y0, const char *prompt1, const char *prompt2)
{
  confirm_t mode = YC_ASK;

  return confirm (x0, y0, mode, prompt1, prompt2);
}


/*
 *	Notify - notification dialog box
 *
 *	Display a notification and wait for a key (prompt2 may
 *	be NULL)
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
  set_colour (WINTITLE);
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
  y_vsnprintf (buf, sizeof buf, fmt, arglist);
#ifdef Y_BGI
  setviewport (0, 0, ScrMaxX, ScrMaxY, 1);
#endif  /* FIXME! */
  Notify (-1, -1, buf, NULL);
}


/*
 *	DisplayMessage - clear the screen and display a message
 */
void DisplayMessage (int x0, int y0, const char *msg, ...)
{
  char prompt[120];
  va_list args;

  va_start (args, msg);
  y_vsnprintf (prompt, sizeof prompt, msg, args);
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
 *	NotImplemented - make the user angry...
 */
void NotImplemented (void)
{
  Notify (-1, -1, "This function is not implemented... Yet!", NULL);
}


