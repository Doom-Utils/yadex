/*
 *	l_prop.c
 *	Linedefs properties
 *	Some of this was originally in editobj.c. It was moved here to
 *	improve overlay granularity (therefore memory consumption).
 *	AYM 1998-02-07
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
#include "entry.h"
#include "gfx.h"
#include "levels.h"
#include "oldmenus.h"
#include "game.h"
#include "selectn.h"


/*
 *	Prototypes of private functions
 */
static char *GetTaggedLineDefFlag (int linedefnum, int flagndx);
static int InputLinedefType (int x0, int y0, int *number);
static const char *PrintLdtgroup (void *ptr);
static const char *PrintLdtdef (void *ptr);


void LinedefProperties (int x0, int y0, SelPtr obj)
{
char  *menustr[8];
char   texname[9];
int    n, val;
SelPtr cur, sdlist;
int objtype = OBJ_LINEDEFS;
int    subwin_y0;
int    subsubwin_y0;

val = DisplayMenu (x0, y0, "Choose the object to edit:",
   "Edit the linedef",
   (LineDefs[obj->objnum].sidedef1 >= 0) ? "Edit the 1st sidedef"
                                                       : "Add a 1st sidedef",
   (LineDefs[obj->objnum].sidedef2 >= 0) ? "Edit the 2nd sidedef"
                                                       : "Add a 2nd sidedef",
   NULL);
subwin_y0 = y0 + BOX_BORDER + (2 + val) * FONTH;
switch (val)
   {
   case 1:
      for (n = 0; n < 8; n++)
	 menustr[n] = (char *) GetMemory (60);
      sprintf (menustr[7], "Edit linedef #%d", obj->objnum);
      sprintf (menustr[0], "Change Flags            (Current: %d)",
         LineDefs[obj->objnum].flags);
      sprintf (menustr[1], "Change type             (Current: %d)",
         LineDefs[obj->objnum].type);
      sprintf (menustr[2], "Change sector tag       (Current: %d)",
         LineDefs[obj->objnum].tag);
      sprintf (menustr[3], "Change starting vertex  (Current: #%d)",
         LineDefs[obj->objnum].start);
      sprintf (menustr[4], "Change ending vertex    (Current: #%d)",
         LineDefs[obj->objnum].end);
      sprintf (menustr[5], "Change 1st sidedef ref. (Current: #%d)",
         LineDefs[obj->objnum].sidedef1);
      sprintf (menustr[6], "Change 2nd sidedef ref. (Current: #%d)",
         LineDefs[obj->objnum].sidedef2);
      val = DisplayMenuArray (x0 + 42, subwin_y0, menustr[7], 7, NULL, menustr,
         NULL, NULL, NULL);
      for (n = 0; n < 8; n++)
	 FreeMemory (menustr[n]);
      subsubwin_y0 = subwin_y0 + BOX_BORDER + (2 + val) * FONTH;
      switch (val)
	 {
	 case 1:
	    val = DisplayMenu (x0 + 84, subsubwin_y0, "Toggle the flags:",
			       GetTaggedLineDefFlag (obj->objnum, 1),
			       GetTaggedLineDefFlag (obj->objnum, 2),
			       GetTaggedLineDefFlag (obj->objnum, 3),
			       GetTaggedLineDefFlag (obj->objnum, 4),
			       GetTaggedLineDefFlag (obj->objnum, 5),
			       GetTaggedLineDefFlag (obj->objnum, 6),
			       GetTaggedLineDefFlag (obj->objnum, 7),
			       GetTaggedLineDefFlag (obj->objnum, 8),
			       GetTaggedLineDefFlag (obj->objnum, 9),
			       "(Enter a decimal value)",
			       NULL);
	    if (val >= 1 && val <= 9)
	       {
	       for (cur = obj; cur; cur = cur->next)
		  LineDefs[cur->objnum].flags ^= 0x01 << (val - 1);
	       MadeChanges = 1;
	       }
	    else if (val == 10)
	       {
	       val = InputIntegerValue (x0 + 126, subsubwin_y0 + 12 * FONTH,
		  0, 511, LineDefs[obj->objnum].flags);
	       if (val != IIV_CANCEL)
		  {
		  for (cur = obj; cur; cur = cur->next)
		     LineDefs[cur->objnum].flags = val;
		  MadeChanges = 1;
		  }
	       }
	    break;
	 case 2:
	    if (! InputLinedefType (x0, subsubwin_y0, &val))
	       {
	       for (cur = obj; cur; cur = cur->next)
		  LineDefs[cur->objnum].type = val;
	       MadeChanges = 1;
	       }
	    break;
	 case 3:
	    val = InputIntegerValue (x0 + 84, subsubwin_y0, -32768, 32767, LineDefs[obj->objnum].tag);
	    if (val != IIV_CANCEL)  /* not Esc */
	       {
	       for (cur = obj; cur; cur = cur->next)
		  LineDefs[cur->objnum].tag = val;
	       MadeChanges = 1;
	       }
	    break;
	 case 4:
	    val = InputObjectXRef (x0 + 84, subsubwin_y0, OBJ_VERTICES, 0, LineDefs[obj->objnum].start);
	    if (val >= 0)
	       {
	       for (cur = obj; cur; cur = cur->next)
		  LineDefs[cur->objnum].start = val;
	       MadeChanges = 1;
	       MadeMapChanges = 1;
	       }
	    break;
	 case 5:
	    val = InputObjectXRef (x0 + 84, subsubwin_y0, OBJ_VERTICES, 0, LineDefs[obj->objnum].end);
	    if (val >= 0)
	       {
	       for (cur = obj; cur; cur = cur->next)
		  LineDefs[cur->objnum].end = val;
	       MadeChanges = 1;
	       MadeMapChanges = 1;
	       }
	    break;
	 case 6:
	    val = InputObjectXRef (x0 + 84, subsubwin_y0, OBJ_SIDEDEFS, 1, LineDefs[obj->objnum].sidedef1);
	    if (val >= -1)
	       {
	       for (cur = obj; cur; cur = cur->next)
		  LineDefs[cur->objnum].sidedef1 = val;
	       MadeChanges = 1;
	       MadeMapChanges = 1;
	       }
	    break;
	 case 7:
	    val = InputObjectXRef (x0 + 84, subsubwin_y0, OBJ_SIDEDEFS, 1, LineDefs[obj->objnum].sidedef2);
	    if (val >= -1)
	       {
	       for (cur = obj; cur; cur = cur->next)
		  LineDefs[cur->objnum].sidedef2 = val;
	       MadeChanges = 1;
	       MadeMapChanges = 1;
	       }
	    break;
	 }
      break;

   /* edit or add the first sidedef */
   case 2:
      ObjectsNeeded (OBJ_LINEDEFS, OBJ_SIDEDEFS, 0);
      if (LineDefs[obj->objnum].sidedef1 >= 0)
	 {
	 /* build a new selection list with the first SideDefs */
	 objtype = OBJ_SIDEDEFS;
	 sdlist = 0;
	 for (cur = obj; cur; cur = cur->next)
	    if (LineDefs[cur->objnum].sidedef1 >= 0)
	       SelectObject (&sdlist, LineDefs[cur->objnum].sidedef1);
	 }
      else
	 {
	 /* add a new first sidedef */
	 for (cur = obj; cur; cur = cur->next)
	    if (LineDefs[cur->objnum].sidedef1 == -1)
	       {
	       InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
	       LineDefs[cur->objnum].sidedef1 = NumSideDefs - 1;
	       }
	 break;
	 }
      /* FALL THROUGH */

   /* edit or add the second sidedef */
   case 3:
      if (objtype != OBJ_SIDEDEFS)
	 {
	 if (LineDefs[obj->objnum].sidedef2 >= 0)
	    {
	    /* build a new selection list with the second (or first) SideDefs */
	    objtype = OBJ_SIDEDEFS;
	    sdlist = 0;
	    for (cur = obj; cur; cur = cur->next)
	       if (LineDefs[cur->objnum].sidedef2 >= 0)
		  SelectObject (&sdlist, LineDefs[cur->objnum].sidedef2);
	       else if (LineDefs[cur->objnum].sidedef1 >= 0)
		  SelectObject (&sdlist, LineDefs[cur->objnum].sidedef1);
	    }
	 else
	    {
	    /* add a new second (or first) sidedef */
	    for (cur = obj; cur; cur = cur->next)
	       if (LineDefs[cur->objnum].sidedef1 == -1)
		  {
		  InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
		  ObjectsNeeded (OBJ_LINEDEFS, 0);
		  LineDefs[cur->objnum].sidedef1 = NumSideDefs - 1;
		  }
	       else if (LineDefs[cur->objnum].sidedef2 == -1)
		  {
		  n = LineDefs[cur->objnum].sidedef1;
		  InsertObject (OBJ_SIDEDEFS, -1, 0, 0);
		  strncpy (SideDefs[NumSideDefs - 1].tex3, "-", 8);
		  strncpy (SideDefs[n].tex3, "-", 8);
		  ObjectsNeeded (OBJ_LINEDEFS, 0);
		  LineDefs[cur->objnum].sidedef2 = NumSideDefs - 1;
		  LineDefs[cur->objnum].flags ^= 4;  // Set the 2S bit
		  LineDefs[cur->objnum].flags &= ~1;  // Clear the Im bit
		  }
	    break;
	    }
	 }
      ObjectsNeeded (OBJ_SIDEDEFS, 0);
      for (n = 0; n < 7; n++)
	 menustr[n] = (char *) GetMemory (60);
      sprintf (menustr[6], "Edit sidedef #%d", sdlist->objnum);
      texname[8] = '\0';
      strncpy (texname, SideDefs[sdlist->objnum].tex3, 8);
      sprintf (menustr[0], "Change middle texture   (Current: %s)", texname);
      strncpy (texname, SideDefs[sdlist->objnum].tex1, 8);
      sprintf (menustr[1], "Change upper texture    (Current: %s)", texname);
      strncpy (texname, SideDefs[sdlist->objnum].tex2, 8);
      sprintf (menustr[2], "Change lower texture    (Current: %s)", texname);
      sprintf (menustr[3], "Change texture X offset (Current: %d)",
         SideDefs[sdlist->objnum].xoff);
      sprintf (menustr[4], "Change texture Y offset (Current: %d)",
         SideDefs[sdlist->objnum].yoff);
      sprintf (menustr[5], "Change sector ref.      (Current: #%d)",
         SideDefs[sdlist->objnum].sector);
      val = DisplayMenuArray (x0 + 42, subwin_y0,
         menustr[6], 6, NULL, menustr, NULL, NULL, NULL);
      for (n = 0; n < 7; n++)
	 FreeMemory (menustr[n]);
      subsubwin_y0 = subwin_y0 + BOX_BORDER + (2 + val) * FONTH;
      switch (val)
	 {
	 case 1:
	    strncpy (texname, SideDefs[sdlist->objnum].tex3, 8);
	    ObjectsNeeded (0);
	    ChooseWallTexture (x0 + 84, subsubwin_y0 ,
	       "Choose a wall texture", NumWTexture, WTexture, texname);
	    ObjectsNeeded (OBJ_SIDEDEFS, 0);
	    if (strlen (texname) > 0)
	    {
	       for (cur = sdlist; cur; cur = cur->next)
		  if (cur->objnum >= 0)
		     strncpy (SideDefs[cur->objnum].tex3, texname, 8);
	       MadeChanges = 1;
	    }
	    break;
	 case 2:
	    strncpy (texname, SideDefs[sdlist->objnum].tex1, 8);
	    ObjectsNeeded (0);
	    ChooseWallTexture (x0 + 84, subsubwin_y0,
	       "Choose a wall texture", NumWTexture, WTexture, texname);
	    ObjectsNeeded (OBJ_SIDEDEFS, 0);
	    if (strlen (texname) > 0)
	    {
	       for (cur = sdlist; cur; cur = cur->next)
		  if (cur->objnum >= 0)
		     strncpy (SideDefs[cur->objnum].tex1, texname, 8);
	       MadeChanges = 1;
	    }
	    break;
	 case 3:
	    strncpy (texname, SideDefs[sdlist->objnum].tex2, 8);
	    ObjectsNeeded (0);
	    ChooseWallTexture (x0 + 84, subsubwin_y0,
	       "Choose a wall texture", NumWTexture, WTexture, texname);
	    ObjectsNeeded (OBJ_SIDEDEFS, 0);
	    if (strlen (texname) > 0)
	    {
	       for (cur = sdlist; cur; cur = cur->next)
		  if (cur->objnum >= 0)
		     strncpy (SideDefs[cur->objnum].tex2, texname, 8);
	       MadeChanges = 1;
	    }
	    break;
	 case 4:
	    val = InputIntegerValue (x0 + 84, subsubwin_y0, -32768, 32767, SideDefs[sdlist->objnum].xoff);
	    if (val != IIV_CANCEL)
	    {
	       for (cur = sdlist; cur; cur = cur->next)
		  if (cur->objnum >= 0)
		     SideDefs[cur->objnum].xoff = val;
	       MadeChanges = 1;
	    }
	    break;
	 case 5:
	    val = InputIntegerValue (x0 + 84, subsubwin_y0, -32768, 32767, SideDefs[sdlist->objnum].yoff);
	    if (val != IIV_CANCEL)
	    {
	       for (cur = sdlist; cur; cur = cur->next)
		  if (cur->objnum >= 0)
		     SideDefs[cur->objnum].yoff = val;
	       MadeChanges = 1;
	    }
	    break;
	 case 6:
	    val = InputObjectXRef (x0 + 84, subsubwin_y0, OBJ_SECTORS, 0, SideDefs[sdlist->objnum].sector);
	    if (val >= 0)
	    {
	       for (cur = sdlist; cur; cur = cur->next)
		  if (cur->objnum >= 0)
		     SideDefs[cur->objnum].sector = val;
	       MadeChanges = 1;
	    }
	    break;
	 }
      ForgetSelection (&sdlist);
      break;

   }
}


/*
*/

static char *GetTaggedLineDefFlag (int linedefnum, int flagndx)
{
   static char ldstr[9][50];

   if ((LineDefs[linedefnum].flags & (0x01 << (flagndx - 1))) != 0)
      strcpy (ldstr[flagndx - 1], "* ");
   else
      strcpy (ldstr[flagndx - 1], "  ");
   strcat (ldstr[flagndx - 1], GetLineDefFlagsLongName (0x01 << (flagndx - 1)));
   return ldstr[flagndx - 1];
}



/*
 *	InputLinedefType
 *	Let the user select a linedef type number and return it.
 *	Returns 0 if OK, <>0 if cancelled
 */
static int InputLinedefType (int x0, int y0, int *number)
{
int         r;
int         ldtgno = 0;
char        ldtg; 
al_llist_t *list = 0;

for (;;)
  {
  /*
   * First let user select a ldtgroup
   */
  if (DisplayMenuList (x0+84, y0, "Select group", ldtgroup,
   PrintLdtgroup, &ldtgno) < 0)
    {
    Beep ();
    return 1;
    }
  if (al_lseek (ldtgroup, ldtgno, SEEK_SET))
    fatal_error ("%s ILT1 (%s)", msg_unexpected, al_astrerror (al_aerrno));
  ldtg = CUR_LDTGROUP->ldtgroup;

  /*
   * KLUDGE: Special ldtgroup LDT_FREE means "enter number"
   * Don't look for this ldtgroup in the .ygd file : LoadGameDefs()
   * creates it manually.
   */
  if (ldtg == LDT_FREE)
    {
    /* FIXME should be unsigned ! should accept hex. */
    *number = InputIntegerValue (x0+126, y0 + (3 + ldtgno) * FONTH,
       -32768, 32767, 0);
    if (*number != IIV_CANCEL)
      break;
    goto again;
    }
    
  /*
   * Then build a list of pointers on all ldt that have this
   * ldtgroup and let user select one
   */
  list = al_lcreate (sizeof (void *));
  for (al_lrewind (ldtdef); ! al_leol (ldtdef); al_lstep (ldtdef))
    if (CUR_LDTDEF->ldtgroup == ldtg)
      {
      void *ptr = CUR_LDTDEF;
      al_lwrite (list, &ptr);
      }
  r = DisplayMenuList
   (x0+126, y0 + 2 * FONTH, "Select type", list, PrintLdtdef, NULL);
  if (r < 0)
    goto again;
  if (al_lseek (list, r, SEEK_SET))
    fatal_error ("%s ILT2 (%s)", msg_unexpected, al_astrerror (al_aerrno));
  *number = (*((ldtdef_t **) al_lptr (list)))->number;
  al_ldiscard (list);
  break;

  again :
  ;
  /* draw_map (OBJ_THINGS, 0, 0);  FIXME! */
  }

return 0;
}




/*
 *	PrintLdtgroup
 *	Used by DisplayMenuList when called by InputLinedefType
 */
static const char *PrintLdtgroup (void *ptr)
{
if (! ptr)
  fatal_error ("PLG1");
return ((ldtgroup_t *)ptr)->desc;
}


/*
 *	PrintLdtdef
 *	Used by DisplayMenuList when called by InputLinedefType
 */
static const char *PrintLdtdef (void *ptr)
{
static char buf[100];

if (! ptr)
  fatal_error ("PLD1");
sprintf (buf, "[%3d] %.70s",
 (*((ldtdef_t **)ptr))->number,
 (*((ldtdef_t **)ptr))->longdesc);
return buf;
}

