/*
 *	things.cc
 *	Misc things routines.
 *	BW & RQ sometime in 1993 or 1994.
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


#define YC_THINGS


#include "yadex.h"
#include "game.h"
#include "things.h"


// This is the structure of a table of THINGS attributes.
// In this table, THINGS are sorted by increasing number.
// It is searched in a dichotomic fashion by get_thing_*().
// This table is only here for speed.
typedef struct
  {
  w_thingtype_t type;
  short         radius;
  acolour_t     colour;
  const char    *desc;
  const char	*sprite;
  } thing_attributes_t;

static thing_attributes_t *things_table;
static size_t nthings;
static size_t table_idx;
int max_radius;


static int things_table_cmp (const void *a, const void *b);


/*
 *	create_things_table
 *	Build things_table, a table of THINGS attributes
 *	that's used by get_thing_*() to speed things up.
 *	Call delete_things_table to delete that table.
 */
void create_things_table ()
{
size_t n;

max_radius = 0;
nthings = al_lcount (thingdef);
things_table = (thing_attributes_t *) malloc (nthings * sizeof *things_table);
if (! things_table)
   fatal_error ("Not enough memory");
for (al_lrewind (thingdef), n = 0; n < nthings; al_lstep (thingdef), n++)
   {
   things_table[n].type   = CUR_THINGDEF->number;
   things_table[n].radius = CUR_THINGDEF->radius;
   max_radius = max (max_radius, CUR_THINGDEF->radius);

   // Fetch the app colour no. for the thinggroup
   for (al_lrewind (thinggroup); ! al_leol (thinggroup); al_lstep (thinggroup))
      {
      if (CUR_THINGGROUP  /* don't segfault if zero thinggroup ! */
       && CUR_THINGGROUP->thinggroup == CUR_THINGDEF->thinggroup)
	 {
         things_table[n].colour = CUR_THINGGROUP->acn;
         break;
         }
      }

   things_table[n].desc   = CUR_THINGDEF->desc;
   things_table[n].sprite = CUR_THINGDEF->sprite;
   }

// Sort the table by increasing THING type
qsort (things_table, nthings, sizeof *things_table, things_table_cmp);

#if 0
printf ("Type  Colour Radius Desc\n");
for (n = 0; n < nthings; n++)
   printf ("%5d %-6d %3d    %s\n",
      things_table[n].type,
      things_table[n].colour,
      things_table[n].radius,
      things_table[n].desc);
#endif
}


/*
 *	delete_things_table
 *	Free what create_things_table() allocated.
 */
void delete_things_table (void)
{
if (things_table)
   {
   free (things_table);
   nthings = 0;
   }
}


/*
 *	things_table_cmp
 *	Used by create_things_table() to sort the table
 *	by increasing THING type.
 */
static int things_table_cmp (const void *a, const void *b)
{
return ((const thing_attributes_t *) a)->type
     - ((const thing_attributes_t *) b)->type;
}


/*
 *	LOOKUP_THING
 *	Does a binary search in things_table for THING of type <type>.
 *	If fails, execution is transfered to the next statement.
 *	If succeeds, does a "goto got_it" and <n> contains the index.
 *	To further speed things up, the last index found is kept
 *	between invocations in a static variable. Thus, if several
 *	attributes of the same thing type are queried in a row,
 *	the table search is done only once.
 */
#define LOOKUP_THING \
size_t nmin, nmax;\
if (table_idx >= 0 && table_idx < nthings\
   && things_table[table_idx].type == type)\
   goto got_it;\
nmin = 0;\
nmax = nthings - 1;\
for (;;)\
   {\
   table_idx = (nmin + nmax) / 2;\
   if (type > things_table[table_idx].type)\
      {\
      if (nmin >= nmax)\
         break;\
      nmin = table_idx + 1;\
      }\
   else if (type < things_table[table_idx].type)\
      {\
      if (nmin >= nmax)\
         break;\
      nmax = table_idx - 1;\
      }\
   else\
      goto got_it;\
   }


/*
 *	get_thing_colour
 *	Return the colour of THING of given type.
 */
acolour_t get_thing_colour (w_thingtype_t type)
{
LOOKUP_THING
return LIGHTCYAN;  // Not found.
got_it :
return things_table[table_idx].colour;
}


/*
 *	get_thing_name
 *	Return the description of THING of given type.
 */
const char *get_thing_name (w_thingtype_t type)
{
LOOKUP_THING
{
static char buf[20];
sprintf (buf, "<UNKNOWN %4d>", type);  // Not found.
return buf;
}
got_it :
return things_table[table_idx].desc;
}


/*
 *	get_thing_sprite
 *	Return the root of the sprite name for THING of given type.
 */
const char *get_thing_sprite (w_thingtype_t type)
{
LOOKUP_THING
{
return NULL;  // Not found
}
got_it :
return things_table[table_idx].sprite;
}


/*
 *	get_thing_radius
 *	Return the radius of THING of given type.
 */
int get_thing_radius (w_thingtype_t type)
{
LOOKUP_THING
return 16;  // Not found.
got_it :
return things_table[table_idx].radius;
}


/*
 *	get_max_thing_radius
 *	Return the radius of the largest THING that exists.
 *	This is a speedup function, used by GetCurObject()
 *	to avoid calculating the distance for the THINGS
 *	that are obviously too far away.
 */

/* It's now inlined in things.h
int get_max_thing_radius ()
{
return max_radius;
}
*/


/*
 *	GetAngleName
 *	Get the name of the angle
 */
const char *GetAngleName (int angle)
{
static char buf[30];

switch (angle)
   {
   case 0:
      return "East";
   case 45:
      return "North-east";
   case 90:
      return "North";
   case 135:
      return "North-west";
   case 180:
      return "West";
   case 225:
      return "South-west";
   case 270:
      return "South";
   case 315:
      return "South-east";
   }
sprintf (buf, "<ILLEGAL ANGLE %d>", angle);
return buf;
}


/*
 *	GetWhenName
 *	get string of when something will appear
 */
const char *GetWhenName (int when)
{
static char buf[17];
// "N" is a Boom extension ("not in deathmatch")
// "C" is a Boom extension ("not in cooperative")
const char *flag_chars = "?????????CNMD431";
int n;

for (n = 0; n < 16; n++)
   {
   if (when & (0x8000u >> n))
      buf[n] = flag_chars[n];
   else
      buf[n] = '-';
   }
buf[n] = '\0';
return buf;

#if 0
static char buf[30];
char *ptr = buf;
*ptr = '\0';
if (when & 0x01)
   {
   strcpy (ptr, "D12");
   ptr += 3;
   }
if (when & 0x02)
   {
   if (ptr != buf)
      *ptr++ = ' ';
   strcpy (ptr, "D3");
   ptr += 2;
   }
if (when & 0x04)
   {
   if (ptr != buf)
      *ptr++ = ' ';
   strcpy (ptr, "D45");
   ptr += 3;
   }
if (when & 0x08)
   {
   if (ptr != buf)
      *ptr++ = ' ';
   strcpy (ptr, "Deaf");
   ptr += 4;
   }
if (when & 0x10)
   {
   if (ptr != buf)
      *ptr++ = ' ';
   strcpy (ptr, "Multi");
   ptr += 5;
   }
return buf;
#endif
}



