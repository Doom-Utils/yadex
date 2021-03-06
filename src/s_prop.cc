/*
 *	s_prop.cc
 *	Sector properties
 *	Some of this was originally in editobj.c. It was moved here to
 *	improve overlay granularity (therefore memory consumption).
 *	AYM 1998-02-07
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Rapha�l Quinet and Brendon Wyber.

The rest of Yadex is Copyright � 1997-2003 Andr� Majorel and others.

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
#include "entry.h"
#include "flats.h"
#include "game.h"
#include "gfx.h"
#include "levels.h"
#include "menudata.h"
#include "objid.h"
#include "oldmenus.h"
#include "selectn.h"


class Menu_data_st : public Menu_data
{
  public :
    Menu_data_st (al_llist_t *list);
    virtual size_t nitems () const;
    virtual const char *operator[] (size_t n) const;

  private :
    mutable char buf[100];
    al_llist_t *list;
};


/*
 *	Menu_data_st::Menu_data_st - ctor
 */
Menu_data_st::Menu_data_st (al_llist_t *list) : list (list)
{
  al_lrewind (this->list);
}


/*
 *	Menu_data_st::nitems - return the number of items
 */
size_t Menu_data_st::nitems () const
{
  return al_lcount (list);
}


/*
 *	Menu_data_st::operator[] - return the nth item
 */
const char *Menu_data_st::operator[] (size_t n) const
{
  if (al_lseek (list, n, SEEK_SET) != 0)
  {
    sprintf (buf, "BUG: al_lseek(%p, %lu): %s",
      (void *) list, 
      (unsigned long) n,
      al_astrerror (al_aerrno));
    return buf;
  }
  const stdef_t *ptr = (const stdef_t *) al_lptr (list);
  if (ptr == NULL)
    sprintf (buf, "BUG: al_lptr(%p): %s",
      (void *) list,
      al_astrerror (al_aerrno));
  else
    sprintf (buf, "%2d - %.70s", ptr->number, ptr->longdesc);
  return buf;
}


/*
 *   TransferSectorProperties
 *
 *   -AJA- 2001-05-27
 */
void TransferSectorProperties (int src_sector, SelPtr sectors)
{
   SelPtr cur;

   for (cur=sectors; cur; cur=cur->next)
   {
      if (! is_obj(cur->objnum))
         continue;

      strncpy (Sectors[cur->objnum].floort, Sectors[src_sector].floort,
            WAD_FLAT_NAME);
      strncpy (Sectors[cur->objnum].ceilt, Sectors[src_sector].ceilt,
            WAD_FLAT_NAME);

      Sectors[cur->objnum].floorh  = Sectors[src_sector].floorh;
      Sectors[cur->objnum].ceilh   = Sectors[src_sector].ceilh;
      Sectors[cur->objnum].light   = Sectors[src_sector].light;
      Sectors[cur->objnum].special = Sectors[src_sector].special;
      Sectors[cur->objnum].tag     = Sectors[src_sector].tag;

      MadeChanges = 1;
   }
}

/*
 *	SectorProperties
 *	Sector properties "dialog"
 */
void SectorProperties (int x0, int y0, SelPtr obj)
{
  char  *menustr[30];
  char   texname[WAD_FLAT_NAME + 1];
  int    n, val;
  SelPtr cur;
  int    subwin_y0;

  for (n = 0; n < 8; n++)
    menustr[n] = (char *) GetMemory (60);
  sprintf (menustr[7], "Edit sector #%d", obj->objnum);
  sprintf (menustr[0], "Change floor height     (Current: %d)",
	  Sectors[obj->objnum].floorh);
  sprintf (menustr[1], "Change ceiling height   (Current: %d)",
	  Sectors[obj->objnum].ceilh);
  sprintf (menustr[2], "Change floor texture    (Current: %.*s)",
	  (int) WAD_FLAT_NAME, Sectors[obj->objnum].floort);
  sprintf (menustr[3], "Change ceiling texture  (Current: %.*s)",
	  (int) WAD_FLAT_NAME, Sectors[obj->objnum].ceilt);
  sprintf (menustr[4], "Change light level      (Current: %d)",
	  Sectors[obj->objnum].light);
  sprintf (menustr[5], "Change type             (Current: %d)",
	  Sectors[obj->objnum].special);
  sprintf (menustr[6], "Change linedef tag      (Current: %d)",
	  Sectors[obj->objnum].tag);
  val = vDisplayMenu (x0, y0, menustr[7],
    menustr[0], YK_, 0,
    menustr[1], YK_, 0,
    menustr[2], YK_, 0,
    menustr[3], YK_, 0,
    menustr[4], YK_, 0,
    menustr[5], YK_, 0,
    menustr[6], YK_, 0,
    NULL);
  for (n = 0; n < 8; n++)
    FreeMemory (menustr[n]);
  subwin_y0 = y0 + BOX_BORDER + (2 + val) * FONTH;
  switch (val)
  {
    case 1:
      val = InputIntegerValue (x0 + 42, subwin_y0, -32768, 32767,
	Sectors[obj->objnum].floorh);
      if (val != IIV_CANCEL)
      {
	for (cur = obj; cur; cur = cur->next)
	  Sectors[cur->objnum].floorh = val;
	MadeChanges = 1;
      }
      break;

    case 2:
      val = InputIntegerValue (x0 + 42, subwin_y0, -32768, 32767,
	Sectors[obj->objnum].ceilh);
      if (val != IIV_CANCEL)
      {
	for (cur = obj; cur; cur = cur->next)
	  Sectors[cur->objnum].ceilh = val;
	MadeChanges = 1;
      }
      break;

    case 3:
      {
	*texname = '\0';
	strncat (texname, Sectors[obj->objnum].floort, WAD_FLAT_NAME);
	ObjectsNeeded (0);
	char **flat_names =
	  (char **) GetMemory (NumFTexture * sizeof *flat_names);
	for (size_t n = 0; n < NumFTexture; n++)
	  flat_names[n] = flat_list[n].name;
	ChooseFloorTexture (x0 + 42, subwin_y0, "Choose a floor texture",
	  NumFTexture, flat_names, texname);
	FreeMemory (flat_names);
	ObjectsNeeded (OBJ_SECTORS, 0);
	if (strlen (texname) > 0)
	{
	  for (cur = obj; cur; cur = cur->next)
	    strncpy (Sectors[cur->objnum].floort, texname, WAD_FLAT_NAME);
	  MadeChanges = 1;
	}
	break;
      }

    case 4:
      {
      *texname = '\0';
      strncat (texname, Sectors[obj->objnum].ceilt, WAD_FLAT_NAME);
      ObjectsNeeded (0);
      char **flat_names =
	(char **) GetMemory (NumFTexture * sizeof *flat_names);
      for (size_t n = 0; n < NumFTexture; n++)
	flat_names[n] = flat_list[n].name;
      ChooseFloorTexture (x0 + 42, subwin_y0, "Choose a ceiling texture",
	NumFTexture, flat_names, texname);
      FreeMemory (flat_names);
      ObjectsNeeded (OBJ_SECTORS, 0);
      if (strlen (texname) > 0)
      {
	for (cur = obj; cur; cur = cur->next)
	  strncpy (Sectors[cur->objnum].ceilt, texname, WAD_FLAT_NAME);
	MadeChanges = 1;
      }
      break;
      }

    case 5:
      val = InputIntegerValue (x0 + 42, subwin_y0, 0, 255,
	Sectors[obj->objnum].light);
      if (val != IIV_CANCEL)
      {
	for (cur = obj; cur; cur = cur->next)
	  Sectors[cur->objnum].light = val;
	MadeChanges = 1;
      }
      break;

    case 6:
      {
	val = 0;
	Menu_data_st menudata (stdef);
	if (DisplayMenuList (x0 + 42, subwin_y0, "Select type", menudata, &val)
	  < 0)
	  break;
	// KLUDGE last element of stdef means "enter value"
	if (val == al_lcount (stdef) - 1)
	{
	  val = InputIntegerValue (x0 + 84,
	    subwin_y0 + BOX_BORDER + (3 + val) * FONTH,
	    -32768, 32767, 0);
	  if (val == IIV_CANCEL)  // [Esc]
	    break;
	}
	else
	{
	  if (al_lseek (stdef, val, SEEK_SET))
	    fatal_error ("%s SP1 (%s)\n",
	      msg_unexpected, al_astrerror (al_aerrno));
	  val = CUR_STDEF->number;
	}
	for (cur = obj; cur; cur = cur->next)
	  Sectors[cur->objnum].special = val;
	MadeChanges = 1;
	break;
      }

    case 7:
      val = InputIntegerValue (x0 + 42, subwin_y0, -32768, 32767,
	Sectors[obj->objnum].tag);
      if (val != IIV_CANCEL)
      {
	for (cur = obj; cur; cur = cur->next)
	  Sectors[cur->objnum].tag = val;
	MadeChanges = 1;
      }
      break;
  }
}

/*
 * 	InputSectorType
 * 	Let the user select a sector type number and return it
 * 	Returns 0 if OK, <>0 if cancelled
 */
int InputSectorType (int x0, int y0, int *number)
{
  int	val;
  val = 0;
  *number = 0;
  Menu_data_st menudata (stdef);
	if (DisplayMenuList (x0 , y0, "Select type", menudata, &val)
	  < 0)
	  return 1;
  // KLUDGE last element of stdef means "enter value"
  if (val == al_lcount (stdef) - 1)
  	{
	  val = InputIntegerValue (x0 + 84,
	    y0 + BOX_BORDER + (3 + val) * FONTH,
	    -32768, 32767, 0);
	  if (val == IIV_CANCEL)  // [Esc]
	    return 1;
	}
	else
	{
	  if (al_lseek (stdef, val, SEEK_SET))
	    fatal_error ("%s SP1 (%s)\n",
	      msg_unexpected, al_astrerror (al_aerrno));
	  val = CUR_STDEF->number;
	}
  if (val < 0) return 1;  //unsuccessful
  *number = val;
  return 0;               //successful
}
