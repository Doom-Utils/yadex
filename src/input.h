/*
 *	input.h
 *	User input (mouse and keyboard)
 *	AYM 1998-06-16
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


typedef struct
  {
  /* Window events */
  int  width;		/* New window width */
  int  height;		/* New window height */
  /* Mouse events */
  char in_window;	/* <>0 iff mouse pointer is in window */
  char butl;		/* <>0 iff left mouse button is depressed */
  char butm;		/* <>0 iff middle mouse button is depressed */
  char butr;		/* <>0 iff right mouse button is depressed */
  int  x;		/* Mouse pointer position */
  int  y;
  /* Keyboard */
  char shift;		/* <>0 iff either [Shift] is pressed */
  char ctrl;		/* <>0 iff either [Ctrl] is pressed */
  char alt;		/* <>0 iff either [Alt]/[Meta] is pressed */
  char scroll_lock;	/* Always 0 for the moment */
  /* General */
  int  key;             /* Code of key just pressed (ASCII + YK_*) */
  unsigned long time;	/* Date of event in ms (1) */
  } input_status_t;

/* Notes:
(1) Defined only for the following events: key, button, motion
    enter and leave.
*/

/* Event and key codes */
enum
  {
  YK_BACKSPACE	= '\b',
  YK_TAB	= '\t',
  YK_RETURN	= '\r',
  YK_ESC	= 0x1b,
  YK_DEL	= 0x7f,
  YK_		= 256,
  YK_BACKTAB,
  YK_DOWN,
  YK_END,
  YK_F1,
  YK_F2,
  YK_F3,
  YK_F4,
  YK_F5,
  YK_F6,
  YK_F7,
  YK_F8,
  YK_F9,
  YK_F10,
  YK_HOME,
  YK_INS,
  YK_LEFT,
  YK_PU,
  YK_PD,
  YK_RIGHT,
  YK_UP,
  YK__LAST,  // Marks the end of key events
  // Those are not key numbers but window events
  YE_RESIZE,
  YE_EXPOSE,
  // Those are not key numbers but mouse events
  YE_BUTL_PRESS,
  YE_BUTL_RELEASE,
  YE_BUTM_PRESS,
  YE_BUTM_RELEASE,
  YE_BUTR_PRESS,
  YE_BUTR_RELEASE,
  YE_WHEEL_UP,		// Negative movement, normally bound to button 4
  YE_WHEEL_DOWN,	// Positive movement, normally bound to button 5
  YE_ENTER,
  YE_LEAVE,
  YE_MOTION,
  // Those are not key numbers but application
  //  events (i.e. generated internally)
  YE_ZOOM_CHANGED,
  // Those are ORed with the other key numbers :
  YK_SHIFT	= 0x2000,
  YK_CTRL	= 0X4000, 
  YK_ALT	= 0x8000
  };

/* Defined in input.c -- see the comment there */
extern input_status_t is;

/* Whether c is an "ordinary" character, that is a printable (non-
   control) character. We cannot use isprint because its argument
   must be <= 255 and in considers A0H-FFH non-printable. */
#define is_ordinary(c) ((c) < 256 && ((c) & 0x60) && (c) != 0x7f)

/* Apply this to is.key to find out whether it contains a key press event. */
#define event_is_key(n) (((n) & (YK_SHIFT-1)) > 0 && ((n) & (YK_SHIFT-1)) < YK__LAST)

void init_input_status ();
void get_input_status ();
int  has_input_event ();
int  have_key ();
int  get_key ();
void get_key_or_click ();
const char *key_to_string (int k);


