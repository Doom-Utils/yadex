/*
 *	editobj.cc
 *	Object editing routines.
 *	BW & RQ sometime in 1993 or 1994.
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-1998 Andr� Majorel.

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
#include "_edit.h"
#include "dialog.h"
#include "editobj.h"
#include "entry.h"
#include "flats.h"
#include "game.h"
#include "gfx.h"
#include "levels.h"
#include "oldmenus.h"
#include "selectn.h"
#include "t_spin.h"
#include "x_rotate.h"


/*
   ask for an object number and check for maximum valid number
   (this is just like InputIntegerValue, but with a different prompt)
*/

int InputObjectNumber (int x0, int y0, int objtype, int curobj)
{
int val, key;
char prompt[80];

HideMousePointer ();
sprintf (prompt, "Enter a %s number between 0 and %d:", GetObjectTypeName (objtype), GetMaxObjectNum (objtype));
if (x0 < 0)
   x0 = (ScrMaxX - 25 - 8 * strlen (prompt)) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - 55) / 2;
DrawScreenBox3D (x0, y0, x0 + 25 + 8 * strlen (prompt), y0 + 55);
set_colour (WHITE);
DrawScreenText (x0 + 10, y0 + 8, prompt);
val = curobj;
while ((key
       = InputInteger (x0 + 10, y0 + 28, &val, 0, GetMaxObjectNum (objtype)))
       != YK_RETURN && key != YK_ESC)
   Beep ();
ShowMousePointer ();
return val;
}



/*
   ask for an object number and display a warning message
*/

int InputObjectXRef (int x0, int y0, int objtype, Bool allownone, int curobj)
{
int val, key;
char prompt[80];

HideMousePointer ();
sprintf (prompt, "Enter a %s number between 0 and %d%c",
 GetObjectTypeName (objtype),
 GetMaxObjectNum (objtype), allownone ? ',' : ':');
val = strlen (prompt);
if (val < 40)
   val = 40;
if (x0 < 0)
   x0 = (ScrMaxX - 25 - 8 * val) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - (allownone ? 85 : 75)) / 2;
DrawScreenBox3D (x0, y0, x0 + 25 + 8 * val, y0 + (allownone ? 85 : 75));
set_colour (WHITE);
DrawScreenText (x0 + 10, y0 + 8, prompt);
if (allownone)
   DrawScreenText (x0 + 10, y0 + 18, "or -1 for none:");
set_colour (LIGHTRED);
DrawScreenText (x0 + 10, y0 + (allownone ? 60 : 50),
 "Warning: modifying the cross-references");
DrawScreenText (x0 + 10, y0 + (allownone ? 70 : 60),
 "between some objects may crash the game.");
val = curobj;
while ((key = InputInteger (x0 + 10, y0 + (allownone ? 38 : 28), &val,
 allownone ? -1 : 0, GetMaxObjectNum (objtype))) != YK_RETURN && key != YK_ESC)
   Beep ();
ShowMousePointer ();
return val;
}



/*
   ask for two vertex numbers and check for maximum valid number
*/

Bool Input2VertexNumbers (int x0, int y0, const char *prompt1, int *v1, int *v2)
{
int  key;
int  maxlen, first;
char prompt2[80];
int text_x0;
int text_y0;
int entry1_x0;
int entry1_y0;
int entry2_x0;
int entry2_y0;
// FIXME should let InputInteger() tell us
const int entry_width  = 2 * HOLLOW_BORDER + 2 * NARROW_HSPACING + 7 * FONTW;
const int entry_height = 2 * HOLLOW_BORDER + 2 * NARROW_VSPACING + FONTH;

HideMousePointer ();
sprintf (prompt2, "Enter two numbers between 0 and %d:", NumVertices - 1);

if (strlen (prompt1) > strlen (prompt2))
   maxlen = strlen (prompt1);
else
   maxlen = strlen (prompt2);
if (x0 < 0)
   x0 = (ScrMaxX - 25 - 8 * maxlen) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - 75) / 2;
text_x0 = x0 + BOX_BORDER + WIDE_HSPACING;
text_y0 = y0 + BOX_BORDER + WIDE_VSPACING;
entry1_x0 = text_x0 + 13 * FONTW;
entry1_y0 = text_y0 + 3 * FONTH - HOLLOW_BORDER - NARROW_VSPACING;
entry2_x0 = entry1_x0;
entry2_y0 = text_y0 + 5 * FONTH - HOLLOW_BORDER - NARROW_VSPACING;

DrawScreenBox3D (x0, y0,
   x0 + 2 * BOX_BORDER + 2 * WIDE_HSPACING
      + max (entry_width + 13 * FONTW, maxlen * FONTW) - 1,
   y0 + 2 * BOX_BORDER + 2 * WIDE_VSPACING + 6 * FONTH - 1);
set_colour (WHITE);
DrawScreenText (text_x0, text_y0, prompt1);
set_colour (WINFG);
DrawScreenText (text_x0, text_y0 + FONTH, prompt2);
DrawScreenText (text_x0, text_y0 + 3 * FONTH, "Start vertex");
DrawScreenText (text_x0, text_y0 + 5 * FONTH, "End vertex");

first = 1;
key = 0;
for (;;)
   {
   DrawScreenBoxHollow (entry1_x0, entry1_y0,
      entry1_x0 + entry_width - 1, entry1_y0 + entry_height - 1, BLACK);
   set_colour (first ? WHITE : DARKGRAY);
   DrawScreenText (entry1_x0 + HOLLOW_BORDER + NARROW_HSPACING,
      entry1_y0 + HOLLOW_BORDER + NARROW_VSPACING, "%d", *v1);

   DrawScreenBoxHollow (entry2_x0, entry2_y0,
      entry2_x0 + entry_width - 1, entry2_y0 + entry_height - 1, BLACK);
   set_colour (! first ? WHITE : DARKGRAY);
   DrawScreenText (entry2_x0 + HOLLOW_BORDER + NARROW_HSPACING,
      entry2_y0 + HOLLOW_BORDER + NARROW_VSPACING, "%d", *v2);

   if (first)
      key = InputInteger (entry1_x0, entry1_y0, v1, 0, NumVertices - 1);
   else
      key = InputInteger (entry2_x0, entry2_y0, v2, 0, NumVertices - 1);
   if (key==YK_LEFT || key==YK_RIGHT || key==YK_TAB || key==YK_BACKTAB)
      first = !first;
   else if (key == YK_ESC)
      break;
   else if (key == YK_RETURN)
      {
      if (first)
	 first = 0;
      else if (*v1 < 0 || *v1 >= NumVertices
            || *v2 < 0 || *v2 >= NumVertices)
	 Beep ();
      else
	 break;
      }
   else
      Beep ();
   }
ShowMousePointer ();
return (key == YK_RETURN);
}



/*
   edit an object or a group of objects
*/

void EditObjectsInfo (int x0, int y0, int objtype, SelPtr obj) /* SWAP! */
{
char  *menustr[3];
int    n, val;
SelPtr cur;
int    subwin_y0;

ObjectsNeeded (objtype, 0);
if (! obj)
   return;
switch (objtype)
   {
   case OBJ_THINGS:
      ThingProperties (x0, y0, obj);
      break;

   case OBJ_VERTICES:
      for (n = 0; n < 3; n++)
	 menustr[n] = (char *) GetMemory (60);
      sprintf (menustr[2], "Edit Vertex #%d", obj->objnum);
      sprintf (menustr[0], "Change X position (Current: %d)",
         Vertices[obj->objnum].x);
      sprintf (menustr[1], "Change Y position (Current: %d)",
         Vertices[obj->objnum].y);
      val = DisplayMenuArray (0, y0,
         menustr[2], 2, NULL, menustr, NULL, NULL, NULL);
      for (n = 0; n < 3; n++)
	 FreeMemory (menustr[n]);
      subwin_y0 = y0 + BOX_BORDER + (2 + val) * FONTH;
      switch (val)
	 {
	 case 1:
	    val = InputIntegerValue (x0 + 42, subwin_y0,
               min (MapMinX, -10000),
               max (MapMaxX, 10000),
               Vertices[obj->objnum].x);
	    if (val != IIV_CANCEL)
	       {
	       n = val - Vertices[obj->objnum].x;
	       for (cur = obj; cur; cur = cur->next)
		  Vertices[cur->objnum].x += n;
	       MadeChanges = 1;
	       MadeMapChanges = 1;
	       }
	    break;

	 case 2:
	    val = InputIntegerValue (x0 + 42, subwin_y0,
               min (MapMinY, -10000),
               max (MapMaxY, 10000),
               Vertices[obj->objnum].y);
	    if (val != IIV_CANCEL)
	       {
	       n = val - Vertices[obj->objnum].y;
	       for (cur = obj; cur; cur = cur->next)
		  Vertices[cur->objnum].y += n;
	       MadeChanges = 1;
	       MadeMapChanges = 1;
	       }
	    break;
	 }
      break;

   case OBJ_LINEDEFS:
      LinedefProperties (x0, y0, obj);
      break;

   case OBJ_SECTORS:
      SectorProperties (x0, y0, obj);
      break;
   }
}


/*
   Yuck!  Dirty piece of code...
*/

Bool Input2Numbers (int x0, int y0, const char *name1, const char *name2,
   int v1max, int v2max, int *v1, int *v2)
{
int  key;
int  maxlen, first;
Bool ok;
char prompt[80];
// FIXME copied from InputInteger()...
int  entry_width = 2 * HOLLOW_BORDER + 2 * NARROW_HSPACING + 7 * FONTW;
int  entry_height = 2 * HOLLOW_BORDER + 2 * NARROW_VSPACING + FONTH;
int  entry1_out_x0;
int  entry1_out_y0;
int  entry1_out_x1;
int  entry1_out_y1;
int  entry1_text_x0;
int  entry1_text_y0;
int  entry2_out_x0;
int  entry2_out_y0;
int  entry2_out_x1;
int  entry2_out_y1;
int  entry2_text_x0;
int  entry2_text_y0;

HideMousePointer ();
#ifdef Y_SNPRINTF
snprintf (prompt, sizeof prompt, "Give the %s and %s for the object:",
   name1, name2);
#else
sprintf (prompt, "Give the %s and %s for the object:", name1, name2);
#endif
maxlen = strlen (prompt);
if (x0 < 0)
   x0 = (ScrMaxX - 25 - FONTW * maxlen) / 2;
if (y0 < 0)
   y0 = (ScrMaxY - 75) / 2;
DrawScreenBox3D (x0, y0, x0 + 25 + FONTW * maxlen, y0 + 75);

entry1_out_x0  = x0 + 10;	/* Totally bogus offset FIXME */
entry1_out_y0  = y0 + 38;	/* Totally bogus offset FIXME */
entry1_out_x1  = entry1_out_x0 + entry_width - 1;
entry1_out_y1  = entry1_out_y0 + entry_height - 1;
entry1_text_x0 = entry1_out_x0 + HOLLOW_BORDER + NARROW_HSPACING;
entry1_text_y0 = entry1_out_y0 + HOLLOW_BORDER + NARROW_VSPACING;

entry2_out_x0  = x0 + 180;	/* Totally bogus offset FIXME */
entry2_out_y0  = y0 + 38;	/* Totally bogus offset FIXME */
entry2_out_x1  = entry2_out_x0 + entry_width - 1;
entry2_out_y1  = entry2_out_y0 + entry_height - 1;
entry2_text_x0 = entry2_out_x0 + HOLLOW_BORDER + NARROW_HSPACING;
entry2_text_y0 = entry2_out_y0 + HOLLOW_BORDER + NARROW_VSPACING;

DrawScreenText (entry1_out_x0, y0 + 26, name1);
DrawScreenText (entry2_out_x0, y0 + 26, name2);
DrawScreenText (entry1_out_x0, y0 + 58, "(0-%d)", v1max);
DrawScreenText (entry2_out_x0, y0 + 58, "(0-%d)", v2max);

set_colour (WHITE);
DrawScreenText (x0 + 10, y0 + 8, prompt);
first = 1;
key = 0;
for (;;)
   {
   ok = 1;
   DrawScreenBoxHollow (entry1_out_x0, entry1_out_y0,
      entry1_out_x1, entry1_out_y1, BLACK);
   if (*v1 < 0 || *v1 > v1max)
      {
      set_colour (DARKGRAY);
      ok = 0;
      }
   else
      set_colour (LIGHTGRAY);
   DrawScreenText (entry1_text_x0, entry1_text_y0, "%d", *v1);
   DrawScreenBoxHollow (entry2_out_x0, entry2_out_y0,
      entry2_out_x1, entry2_out_y1, BLACK);
   if (*v2 < 0 || *v2 > v2max)
      {
      set_colour (DARKGRAY);
      ok = 0;
      }
   else
      set_colour (LIGHTGRAY);
   DrawScreenText (entry2_text_x0, entry2_text_y0, "%d", *v2);
   if (first)
      key = InputInteger (entry1_out_x0, entry1_out_y0, v1, 0, v1max);
   else
      key = InputInteger (entry2_out_x0, entry2_out_y0, v2, 0, v2max);
   if (key==YK_LEFT || key==YK_RIGHT || key==YK_TAB || key==YK_BACKTAB)
      first = !first;
   else if (key == YK_ESC)
      break;
   else if (key == YK_RETURN)
      {
      if (first)
	 first = 0;
      else if (ok)
	 break;
      else
	 Beep ();
      }
   else
      Beep ();
   }
ShowMousePointer ();
return (key == YK_RETURN);
}



/*
   insert a standard object at given position
*/

void InsertStandardObject (int xpos, int ypos, int choice) /* SWAP! */
{
int sector;
int n;
int a, b;

/* show where the object will be inserted */
HideMousePointer ();
DrawPointer (1);
ShowMousePointer ();
/* are we inside a Sector? */
sector = GetCurObject (OBJ_SECTORS, xpos, ypos, 1);

/* !!!! Should also check for overlapping objects (sectors) !!!! */
switch (choice)
   {
   case 1:
      a = 256;
      b = 128;
      if (Input2Numbers (-1, -1, "Width", "Height", 2000, 2000, &a, &b))
	 {
	 if (a < 8)
	    a = 8;
	 if (b < 8)
	    b = 8;
	 xpos = xpos - a / 2;
	 ypos = ypos - b / 2;
	 InsertObject (OBJ_VERTICES, -1, xpos, ypos);
	 InsertObject (OBJ_VERTICES, -1, xpos + a, ypos);
	 InsertObject (OBJ_VERTICES, -1, xpos + a, ypos + b);
	 InsertObject (OBJ_VERTICES, -1, xpos, ypos + b);
	 if (sector < 0)
	    InsertObject (OBJ_SECTORS, -1, 0, 0);
	 for (n = 0; n < 4; n++)
	    {
	    InsertObject (OBJ_LINEDEFS, -1, 0, 0);
	    LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
	    InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
	    if (sector >= 0)
	       SideDefs[NumSideDefs - 1].sector = sector;
	    }
	 ObjectsNeeded (OBJ_LINEDEFS, 0);
	 if (sector >= 0)
	    {
	    LineDefs[NumLineDefs - 4].start = NumVertices - 4;
	    LineDefs[NumLineDefs - 4].end = NumVertices - 3;
	    LineDefs[NumLineDefs - 3].start = NumVertices - 3;
	    LineDefs[NumLineDefs - 3].end = NumVertices - 2;
	    LineDefs[NumLineDefs - 2].start = NumVertices - 2;
	    LineDefs[NumLineDefs - 2].end = NumVertices - 1;
	    LineDefs[NumLineDefs - 1].start = NumVertices - 1;
	    LineDefs[NumLineDefs - 1].end = NumVertices - 4;
	    }
	 else
	    {
	    LineDefs[NumLineDefs - 4].start = NumVertices - 1;
	    LineDefs[NumLineDefs - 4].end = NumVertices - 2;
	    LineDefs[NumLineDefs - 3].start = NumVertices - 2;
	    LineDefs[NumLineDefs - 3].end = NumVertices - 3;
	    LineDefs[NumLineDefs - 2].start = NumVertices - 3;
	    LineDefs[NumLineDefs - 2].end = NumVertices - 4;
	    LineDefs[NumLineDefs - 1].start = NumVertices - 4;
	    LineDefs[NumLineDefs - 1].end = NumVertices - 1;
	    }
	 }
      break;
   case 2:
      a = 8;
      b = 128;
      if (Input2Numbers (-1, -1, "Number of sides", "Radius", 32, 2000, &a, &b))
	 {
	 if (a < 3)
	    a = 3;
	 if (b < 8)
	    b = 8;
	 InsertPolygonVertices (xpos, ypos, a, b);
	 if (sector < 0)
	    InsertObject (OBJ_SECTORS, -1, 0, 0);
	 for (n = 0; n < a; n++)
	    {
	    InsertObject (OBJ_LINEDEFS, -1, 0, 0);
	    LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
	    InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
	    if (sector >= 0)
	       SideDefs[NumSideDefs - 1].sector = sector;
	    }
	 ObjectsNeeded (OBJ_LINEDEFS, 0);
	 if (sector >= 0)
	    {
	    LineDefs[NumLineDefs - 1].start = NumVertices - 1;
	    LineDefs[NumLineDefs - 1].end = NumVertices - a;
	    for (n = 2; n <= a; n++)
	       {
	       LineDefs[NumLineDefs - n].start = NumVertices - n;
	       LineDefs[NumLineDefs - n].end = NumVertices - n + 1;
	       }
	    }
	 else
	    {
	    LineDefs[NumLineDefs - 1].start = NumVertices - a;
	    LineDefs[NumLineDefs - 1].end = NumVertices - 1;
	    for (n = 2; n <= a; n++)
	       {
	       LineDefs[NumLineDefs - n].start = NumVertices - n + 1;
	       LineDefs[NumLineDefs - n].end = NumVertices - n;
	       }
	    }
	 }
      break;
   case 3:
   /*
      a = 6;
      b = 16;
      if (Input2Numbers (-1, -1, "Number of steps", "Step height", 32, 48, &a, &b))
	 {
	 if (a < 2)
	    a = 2;
	 ObjectsNeeded (OBJ_SECTORS, 0);
	 n = Sectors[sector].ceilh;
	 h = Sectors[sector].floorh;
	 if (a * b < n - h)
	    {
	    Beep ();
	    Notify (-1, -1, "The stairs are too high for this Sector", 0);
	    return;
	    }
	 xpos = xpos - 32;
	 ypos = ypos - 32 * a;
	 for (n = 0; n < a; n++)
	    {
	    InsertObject (OBJ_VERTICES, -1, xpos, ypos);
	    InsertObject (OBJ_VERTICES, -1, xpos + 64, ypos);
	    InsertObject (OBJ_VERTICES, -1, xpos + 64, ypos + 64);
	    InsertObject (OBJ_VERTICES, -1, xpos, ypos + 64);
	    ypos += 64;
	    InsertObject (OBJ_SECTORS, sector, 0, 0);
	    h += b;
	    Sectors[NumSectors - 1].floorh = h;

	    InsertObject (OBJ_LINEDEFS, -1, 0, 0);
	    LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
	    LineDefs[NumLineDefs - 1].sidedef2 = NumSideDefs + 1;
	    InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
	    SideDefs[NumSideDefs - 1].sector = sector;
	    InsertObject (OBJ_SIDEDEFS, -1, 0, 0);

	    ObjectsNeeded (OBJ_LINEDEFS, 0);
	    LineDefs[NumLineDefs - 4].start = NumVertices - 4;
	    LineDefs[NumLineDefs - 4].end = NumVertices - 3;
	    LineDefs[NumLineDefs - 3].start = NumVertices - 3;
	    LineDefs[NumLineDefs - 3].end = NumVertices - 2;
	    LineDefs[NumLineDefs - 2].start = NumVertices - 2;
	    LineDefs[NumLineDefs - 2].end = NumVertices - 1;
	    LineDefs[NumLineDefs - 1].start = NumVertices - 1;
	    LineDefs[NumLineDefs - 1].end = NumVertices - 4;
	   }
	 }
      break;
   */
   case 4:
      NotImplemented ();
      break;
   }
}



/*
   menu of miscellaneous operations
*/

void MiscOperations (int objtype, SelPtr *list, int val) /* SWAP! */
{
char   msg[80];
int    angle, scale;

if (val > 1 && ! *list)
   {
   Beep ();
   sprintf (msg, "You must select at least one %s", GetObjectTypeName (objtype));
   Notify (-1, -1, msg, 0);
   return;
   }
switch (val)
   {
   case 1:
      sprintf (msg, "First free tag number: %d", FindFreeTag ());
      Notify (-1, -1, msg, 0);
      break;
   case 2:
      if ((objtype == OBJ_THINGS
         || objtype == OBJ_VERTICES) && ! (*list)->next)
	 {
	 Beep ();
	 sprintf (msg, "You must select more than one %s",
            GetObjectTypeName (objtype));
	 Notify (-1, -1, msg, 0);
	 return;
	 }
      angle = 0;
      scale = 100;
      if (Input2Numbers (-1, -1, "rotation angle (�)", "scale (%)",
         360, 1000, &angle, &scale))
	 RotateAndScaleObjects (objtype, *list, (double) angle * 0.0174533,
            (double) scale * 0.01);
      break;
   case 3:
      if (objtype == OBJ_VERTICES)
	 {
	 DeleteVerticesJoinLineDefs (*list);
	 ForgetSelection (list);
	 }
      else if (objtype == OBJ_LINEDEFS)
	 {
	 SplitLineDefs (*list);
	 }
      else if (objtype == OBJ_SECTORS)
	 {
	 if ((*list)->next)
	    {
	    Beep ();
	    Notify (-1, -1, "You must select exactly one sector", 0);
	    }
	 else
	    {
	    MakeDoorFromSector ((*list)->objnum);
	    }
	 }
      else if (objtype == OBJ_THINGS)
         spin_things (*list, -45);
      break;
   case 4:
      if (objtype == OBJ_VERTICES)
	 {
	 MergeVertices (list);
	 }
      else if (objtype == OBJ_LINEDEFS)
	 {
	 if (! (*list)->next || (*list)->next->next)
	    {
	    Beep ();
	    Notify (-1, -1, "You must select exactly two linedefs", 0);
	    }
	 else
	    {
	    SplitLineDefsAndSector ((*list)->objnum, (*list)->next->objnum);
	    ForgetSelection (list);
	    }
	 }
      else if (objtype == OBJ_SECTORS)
	 {
	 if ((*list)->next)
	    {
	    Beep ();
	    Notify (-1, -1, "You must select exactly one Sector", 0);
	    }
	 else
	    {
	    MakeLiftFromSector ((*list)->objnum);
	    }
	 }
      else if (objtype == OBJ_THINGS)
         spin_things (*list, 45);
      break;
   case 5:
      if (objtype == OBJ_VERTICES)
	 {
	 if (! (*list)->next || (*list)->next->next)
	    {
	    Beep ();
	    Notify (-1, -1, "You must select exactly two Vertices", 0);
	    }
	 else
	    {
	    SplitSector ((*list)->objnum, (*list)->next->objnum);
	    ForgetSelection (list);
	    }
	 }
      else if (objtype == OBJ_LINEDEFS)
	 {
	 DeleteLineDefsJoinSectors (list);
	 }
      else if (objtype == OBJ_SECTORS)
	 {
	 if (! (*list)->next || ! (*list)->next->next)
	    {
	    Beep ();
	    Notify (-1, -1, "You must select three or more sectors", 0);
	    }
	 else
	    {
	    DistributeSectorFloors (*list);
	    }
	 }
      break;
   case 6:
      if (objtype == OBJ_LINEDEFS)
	 {
	 FlipLineDefs (*list, 1);
	 }
      else if (objtype == OBJ_SECTORS)
	 {
	 if (! (*list)->next || ! (*list)->next->next)
	    {
	    Beep ();
	    Notify (-1, -1, "You must select three or more sectors", 0);
	    }
	 else
	    {
	    DistributeSectorCeilings (*list);
	    }
	 }
      break;
   case 7:
      if (objtype == OBJ_LINEDEFS)
	 {
	 if (Expert
            || Confirm (-1, -1,
               "Warning: the sector references are also swapped",
               "You may get strange results if you don't know what you are doing..."))
	    FlipLineDefs (*list, 0);
	 }
      else if (objtype == OBJ_SECTORS)
	RaiseOrLowerSectors (*list);
      break;
   case 8:
      if (objtype == OBJ_SECTORS)
	 {
	 BrightenOrDarkenSectors (*list);
	 break;
	 }
      if (objtype == OBJ_LINEDEFS)
	 {
	 SelPtr sdlist, cur;

	 /* select all sidedefs */
	 ObjectsNeeded (OBJ_LINEDEFS);
	 sdlist = 0;
	 for (cur = *list; cur; cur = cur->next)
	    {
	    if (LineDefs[cur->objnum].sidedef1 >= 0)
	       SelectObject (&sdlist, LineDefs[cur->objnum].sidedef1);
	    if (LineDefs[cur->objnum].sidedef2 >= 0)
	       SelectObject (&sdlist, LineDefs[cur->objnum].sidedef2);
	    }
	 /* align the textures along the Y axis (height) */
	 AlignTexturesY (&sdlist);
	 }
      /* AYM 19971219 : is the fall through intentional ? */
   case 9:
      if (objtype == OBJ_SECTORS)
	 {
	 NotImplemented ();  // FIXME
	 break;
	 }
      if (objtype == OBJ_LINEDEFS)
	 {
	 SelPtr sdlist, cur;

	 /* select all sidedefs */
	 ObjectsNeeded (OBJ_LINEDEFS,0);
	 sdlist = 0;
	 for (cur = *list; cur; cur = cur->next)
	    {
	    if (LineDefs[cur->objnum].sidedef1 >= 0)
	       SelectObject (&sdlist, LineDefs[cur->objnum].sidedef1);
	    if (LineDefs[cur->objnum].sidedef2 >= 0)
	       SelectObject (&sdlist, LineDefs[cur->objnum].sidedef2);
	    }
	 /* align the textures along the X axis (width) */
	 AlignTexturesX (&sdlist);
	 }
      break;

   case 10:
      // Linedef -> Make linedef single-sided
      if (objtype == OBJ_LINEDEFS)
	 {
	 SelPtr cur;
	 ObjectsNeeded (OBJ_LINEDEFS, 0);
	 for (cur = *list; cur; cur = cur->next)
	    {
	    LineDefs[cur->objnum].sidedef2 = -1;  /* remove ref. to 2nd SD */
	    LineDefs[cur->objnum].flags &= ~0x04; /* clear "2S" bit */
	    LineDefs[cur->objnum].flags |= 0x01;  /* set "Im" bit */
	    /* Don't delete the 2nd sidedef, it could be used
               by another linedef. And if it isn't, the next
               cross-references check will delete it anyway. */
	    }
	 }
      break;

   case 11:
      // Linedef -> Make rectangular nook
      if (objtype == OBJ_LINEDEFS)
	 MakeRectangularNook (*list, 32, 16, 0);
      break;

   case 12:
      // Linedef -> Make rectangular boss
      if (objtype == OBJ_LINEDEFS)
	 MakeRectangularNook (*list, 32, 16, 1);
      break;

   case 13:
      // Linedef -> Set length (1st vertex)
      if (objtype == OBJ_LINEDEFS)
	 {
	 static int length = 24;
	 length = InputIntegerValue (-1, -1, -10000, 10000, length);
	 if (length != IIV_CANCEL)
	   SetLinedefLength (*list, length, 0);
	 }
      break;

   case 14:
      // Linedef -> Set length (2nd vertex)
      if (objtype == OBJ_LINEDEFS)
	 {
	 static int length = 24;
	 length = InputIntegerValue (-1, -1, -10000, 10000, length);
	 if (length != IIV_CANCEL)
	   SetLinedefLength (*list, length, 1);
	 }
      break;

   case 15:
      // Linedef -> Unlink 1st sidedef
      if (objtype == OBJ_LINEDEFS)
         unlink_sidedef (*list, 1, 0);
      break;

   case 16:
      // Linedef -> Unlink 2nd sidedef
      if (objtype == OBJ_LINEDEFS)
         unlink_sidedef (*list, 0, 1);
      break;

   }
}


