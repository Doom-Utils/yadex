/*
 *	selectn.cc
 *	Selection stuff
 *	AYM 1998-10-23
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
#include "bitvec.h"
#include "selectn.h"


/*
   test if an object is in the selection list
*/

Bool IsSelected (SelPtr list, int objnum)
{
SelPtr cur;

for (cur = list; cur; cur = cur->next)
   if (cur->objnum == objnum)
      return 1;
return 0;
}


/*
 *	DumpSelection
 *	List the contents of the selection
 */
void DumpSelection (SelPtr list)
{
int n;
SelPtr cur;

printf ("Selection:");
for (n = 0, cur = list; cur; cur = cur->next, n++)
   printf (" %d", cur->objnum);
printf ("  (%d)\n", n);
}


/*
 *	SelectObject
 *	add an object to the selection list
 */
void SelectObject (SelPtr *list, int objnum)
{
SelPtr cur;

if (! is_obj (objnum))
   fatal_error ("BUG: SelectObject called with %d", objnum);
cur = (SelPtr) GetMemory (sizeof (struct SelectionList));
cur->next = *list;
cur->objnum = objnum;
*list = cur;
}


/*
 *	select_unselect_obj
 *	If the object is not in the selection list, add it.
 *	If it already is, remove it.
 */
void select_unselect_obj (SelPtr *list, int objnum)
{
SelPtr cur;
SelPtr prev;

if (! is_obj (objnum))
   fatal_error ("s/u_obj called with %d", objnum);
for (prev = NULL, cur = *list; cur != NULL; prev = cur, cur = cur->next)
   // Already selected: unselect it.
   if (cur->objnum == objnum)
      {
      if (prev != NULL)
         prev->next = cur->next;
      else
         *list = cur->next;
      FreeMemory (cur);
      if (prev != NULL)
         cur = prev->next;
      else
         cur = (SelPtr) NULL;
      return;
      }

// Not selected: select it.
cur = (SelPtr) GetMemory (sizeof (struct SelectionList));
cur->next = *list;
cur->objnum = objnum;
*list = cur;
}


/*
   remove an object from the selection list
*/

void UnSelectObject (SelPtr *list, int objnum)
{
SelPtr cur, prev;

if (! is_obj (objnum))
   fatal_error ("BUG: UnSelectObject called with %d", objnum);
prev = 0;
cur = *list;
while (cur)
   {
   if (cur->objnum == objnum)
      {
      if (prev)
	 prev->next = cur->next;
      else
	 *list = cur->next;
      FreeMemory (cur);
      if (prev)
	 cur = prev->next;
      else
	 cur = 0;
      }
   else
      {
      prev = cur;
      cur = cur->next;
      }
   }
}


/*
   forget the selection list
*/

void ForgetSelection (SelPtr *list)
{
SelPtr cur, prev;

cur = *list;
while (cur)
   {
   prev = cur;
   cur = cur->next;
   FreeMemory (prev);
   }
*list = 0;
}


/*
 *	list_to_bitvec
 *	Make a bit vector of <bitvec_size> elements from a list.
 *	It's up to the caller to delete the bit vector after use.
 */
bitvec_c *list_to_bitvec (SelPtr list, size_t bitvec_size)
{
SelPtr cur;
bitvec_c *bitvec;

bitvec = new bitvec_c (bitvec_size);
for (cur = list; cur; cur = cur->next)
   bitvec->set (cur->objnum);
return bitvec;
}

