/*
 *	wstructs.h
 *	Wad files data structures.
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


#ifndef YH_WSTRUCTS	// To prevent multiple inclusion
#define YH_WSTRUCTS	// To prevent multiple inclusion


typedef i16 w_coord_t;


// THINGS
typedef i16 w_thingtype_t;
typedef i16 w_thingangle_t;
typedef i16 w_thingflags_t;
#if defined __GNUG__
__attribute__ ((__packed__))
#endif
struct Thing
   {
   w_coord_t      xpos;   /* x position */
   w_coord_t      ypos;   /* y position */
   w_thingangle_t angle;  /* facing angle */
   w_thingtype_t  type;   /* thing type */
   w_thingflags_t when;   /* appears when? */
   }
#if defined __GNUC__ && ! defined __GNUG__
__attribute__ ((__packed__))
#endif
;
typedef struct Thing huge *TPtr;


// LINEDEFS
#if defined __GNUG__
__attribute__ ((__packed__))
#endif
struct LineDef
   {
   i16 start;     /* from this vertex ... */
   i16 end;       /* ... to this vertex */
   i16 flags;     /* see NAMES.C for more info */
   i16 type;      /* see NAMES.C for more info */
   i16 tag;       /* crossing this linedef activates the sector with the same tag */
   i16 sidedef1;  /* sidedef */
   i16 sidedef2;  /* only if this line adjoins 2 sectors */
   }
#if defined __GNUC__ && ! defined __GNUG__
__attribute__ ((__packed__))
#endif
;
typedef struct LineDef huge *LDPtr;


// SIDEDEFS
#if defined __GNUG__
__attribute__ ((__packed__))
#endif
struct SideDef
   {
   w_coord_t xoff;  /* X offset for texture */
   w_coord_t yoff;  /* Y offset for texture */
   char tex1[8];    /* texture name for the part above */
   char tex2[8];    /* texture name for the part below */
   char tex3[8];    /* texture name for the regular part */
   i16 sector;      /* adjacent sector */
   }
#if defined __GNUC__ && ! defined __GNUG__
__attribute__ ((__packed__))
#endif
;
typedef struct SideDef huge *SDPtr;


// VERTICES
#if defined __GNUG__
__attribute__ ((__packed__))
#endif
struct Vertex
   {
   w_coord_t x;  /* X coordinate */
   w_coord_t y;  /* Y coordinate */
   }
#if defined __GNUC__ && ! defined __GNUG__
__attribute__ ((__packed__))
#endif
;
typedef struct Vertex huge *VPtr;


// SECTORS
#if defined __GNUG__
__attribute__ ((__packed__))
#endif
struct Sector
   {
   i16 floorh;    /* floor height */
   i16 ceilh;     /* ceiling height */
   char floort[8];/* floor texture */
   char ceilt[8]; /* ceiling texture */
   i16 light;     /* light level (0-255) */
   i16 special;   /* special behaviour (0 = normal, 9 = secret, ...) */
   i16 tag;       /* sector activated by a linedef with the same tag */
   }
#if defined __GNUC__ && ! defined __GNUG__
__attribute__ ((__packed__))
#endif
;
typedef struct Sector huge *SPtr;


#endif		// To prevent multiple inclusion

