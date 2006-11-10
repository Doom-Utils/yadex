/*
 *	names.cc
 *	BW & RQ sometime in 1993 or 1994.
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

#include <string>

#include "game.h"
#include "objid.h"


/*
   get the name of an object type
*/
const char *GetObjectTypeName (int objtype)
{
switch (objtype)
   {
   case OBJ_THINGS:   return "thing";
   case OBJ_LINEDEFS: return "linedef";
   case OBJ_SIDEDEFS: return "sidedef";
   case OBJ_VERTICES: return "vertex";
   case OBJ_SEGS:     return "segment";
   case OBJ_SSECTORS: return "ssector";
   case OBJ_NODES:    return "node";
   case OBJ_SECTORS:  return "sector";
   case OBJ_REJECT:   return "reject";
   case OBJ_BLOCKMAP: return "blockmap";
   }
return "< Bug! >";
}



/*
   what are we editing?
*/
const char *GetEditModeName (int objtype)
{
switch (objtype)
   {
   case OBJ_THINGS:   return "Things";
   case OBJ_LINEDEFS:
   case OBJ_SIDEDEFS: return "LD & SD";
   case OBJ_VERTICES: return "Vertices";
   case OBJ_SEGS:     return "Segments";
   case OBJ_SSECTORS: return "Seg-Sectors";
   case OBJ_NODES:    return "Nodes";
   case OBJ_SECTORS:  return "Sectors";
   }
return "< Bug! >";
}



/*
   get a short (16 char.) description of the type of a linedef
*/

static const char *bgl_trigger[8] =
{
  "W1", "WR", "S1", "SR", "G1", "GR", "D1", "DR"
};

static const char *bgl_speed[4] =
{
  "S", "N", "F", "T"
};

static const char *updown4[8] =
{
  "dN", "dn", "Dn", "DN", "uP", "up", "Up", "UP"
};

static const char *bgl_keys[16] =
{
  "any", "rcard", "bcard", "ycard", "rskul", "bskul", "yskul", "all6",
  "any", "red",  "blu",  "yel",  "red",  "blu",  "yel",  "all3"
};

static const char *bgl_door[16] =
{
  "ODC", "Open", "CDO", "Close"
};

static const char *bgl_door_delay[4] =
{
  "1", "4", "9", "30"			// 35/150/300/1050 tics
};

static const char *bgl_lift_target[4] =
{
  // LnF, NnF, LnC, HnF-LnF
  "LIF", "lEF", "LIC", "HEF/LIF"
};

static const char *bgl_lift_delay[4] =
{
  "1", "3", "5", "10"			// 35/105/165/350 tics
};

static const char *bgl_tex[2] =
{
  "", "X"
};

const char *GetLineDefTypeName (wad_ldtype_t type)
{
  if (CUR_LDTDEF != NULL && CUR_LDTDEF->number == type)
    return CUR_LDTDEF->shortdesc;
  for (al_lrewind (ldtdef); ! al_leol (ldtdef); al_lstep (ldtdef))
    if (CUR_LDTDEF->number == type)
      return CUR_LDTDEF->shortdesc;

  // Boom generalised linedef types
  if (type >= 0x2f80)
  {
    static std::string s;

    s.reserve (20);
    s = "";

    if (type < 0x3000)		// 0010 1111 1QMS STTT  2F80h-2FFFh: crushers
    {
      s += bgl_trigger[type & 0x0007];
      if (type & 0x0020)
	s += 'm';
      s += " Crush ";
      s += bgl_speed[(type >> 3) & 0x0003];
      if (type & 0x0040)
	s += " silent";
    }
    else if (type < 0x3400)	// 0011 00XU HHMS STTT  3000h-33FFh: stairs
    {
      static const char *units[4] =
      {
	"4", "8", "16", "24"
      };

      s += bgl_trigger[type & 0x0007];
      if (type & 0x0020)
	s += 'm';
      s += " Stair ";
      s += updown4[((type >> 3) & 0x0003) + 4 * ((type >> 8) & 0x0001)];
      s += ' ';
      s += units[(type >> 6) & 0x0003];
      s += ' ';
      s += bgl_tex[(type >> 9) & 0x0001];
    }
    else if (type < 0x3800)	// 0011 01GG DDMS STTT  3400h-37FFh: lifts
    {
      s += bgl_trigger[type & 0x0007];
      if (type & 0x0020)
	s += 'm';
      wad_ldtype_t target = (type >> 8) & 0x0003;
      if (target == 3)
      {
	s += "& Plat ";
      }
      else
      {
	s += " Lift ";			// FIXME
	s += bgl_lift_target[target];
	s += ' ';
      }
      s += bgl_lift_delay[(type >> 6) & 0x0003];
      s += ' ';
      s += bgl_speed[(type >> 3) & 0x0003];
    }
    else if (type < 0x3c00)	// 0011 10KK KKOS STTT  3800h-3BFFh: key doors
    {
      s += bgl_trigger[type & 0x0007];
      if (type & 0x0020)
        s += " Open ";
      else
        s += " ODC 4 ";
      s += bgl_speed[(type >> 3) & 0x0003];
      s += ' ';
      wad_ldtype_t same = (type >> 9) & 0x0001;
      wad_ldtype_t lock = (type >> 6) & 0x0007;
      s += bgl_keys[same * 8 + lock];
    }
    else if (type < 0x4000)	// 0011 11DD MAAS STTT  3C00h-3FFFh: doors
    {
      s += bgl_trigger[type & 0x0007];
      if (type & 0x0080)
	s += 'm';
      s += ' ';
#if 0
      wad_ldtype_t action = (type >> 5) & 0x0003;
      wad_ldtype_t delay  = (type >> 8) & 0x0003;
      s += bgl_door[4 * action + delay];
#else
      wad_ldtype_t action = (type >> 5) & 0x0003;
      s += bgl_door[action];
      if (action == 0 || action == 2)	// ODC & CDO
      {
	s += ' ';
	s += bgl_door_delay[(type >> 8) & 0x0003];
      }
#endif
      s += ' ';
      s += bgl_speed[(type >> 3) & 0x0003];
    }
    else			// 010! CCGG GUZS STTT  4000h-5FFFh: ceilings
    {				// 011! CCGG GUZS STTT  6000h-7FFFh: floors
      const unsigned trigger   =         type & 0x0007;
      const unsigned speed     = (type >>  3) & 0x0003;
      const unsigned model     = (type >>  5) & 0x0001;
      const unsigned direction = (type >>  6) & 0x0001;
      const unsigned target    = (type >>  7) & 0x0007;
      const unsigned change    = (type >> 10) & 0x0003;
      const unsigned crush     = (type >> 12) & 0x0001;
      const unsigned floor     = (type >> 13) & 0x0001;

      static const char *ftarget_str[8] =
      {
	// FIXME why do boomref.txt and the UDS disagree ? (LIF vs. LEF)
	"HEF", "LIF", "hEF", "LIC", "C", "SLT", "24", "32"
      };
      static const char *ctarget_str[8] =
      {
	"HEC", "LEC", "hEC", "HEF", "F", "SUT", "24", "32"
      };
      static const char *change_str[4] =
      {
	"", "XZ", "X", "XP"
      };

      s += bgl_trigger[trigger];
      if (change == 0 && model != 0)
	s += 'm';
      s += floor ? " F " : " C ";
      s += updown4[speed + 4 * direction];
      if (crush)
	s += '!';
      s += ' ';
      if (floor)
      {
	// Target NnF translates into hEF (if up) or lEF (if down)
	if (direction == 1 && target == 2)
	  s += "lEF";
	else
	  s += ftarget_str[target];
      }
      else
      {
	// Target NnC translates into hEC (if up) or lEC (if down)
	if (direction == 1 && target == 2)
	  s += "lEC";
	else
	  s += ctarget_str[target];
      }
      s += ' ';
      if (change != 0)
	s += (model == 0 ? 'T' : 'N');
      s += change_str[change];
    }

    return s.c_str ();
  }

  return "?? UNKNOWN";
}


/*
   get a short description of the flags of a linedef
*/

const char *GetLineDefFlagsName (int flags)
{
static char buf[20];
// "P" is a Boom extension ("pass through")
// "T" is for Strife ("translucent")
const char *flag_chars = "???T??PANBSLU2MI";
int n;

char *p = buf;
for (n = 0; n < 16; n++)
   {
   if (n != 0 && n % 4 == 0)
      *p++ = ' ';
   if (flags & (0x8000u >> n))
      *p++ = flag_chars[n];
   else
      *p++ = '-';
   }
*p = '\0';
return buf;

#if 0
static char temp[20];
if (flags & 0x0100)
   strcpy (temp, "A"); /* Already on the map (Ma) */
else
   strcpy (temp, "-");
if (flags & 0x80)
   strcat (temp, "V"); /* Invisible on the map (In) */
else
   strcat (temp, "-");
if (flags & 0x40)
   strcat (temp, "B"); /* Blocks sound (So) */
else
   strcat (temp, "-");
if (flags & 0x20)
   strcat (temp, "S"); /* Secret (normal on the map) (Se) */
else
   strcat (temp, "-");
if (flags & 0x10)
   strcat (temp, "L"); /* Lower texture offset changed (Lo) */
else
   strcat (temp, "-");
if (flags & 0x08)
   strcat (temp, "U"); /* Upper texture offset changed (Up) */
else
   strcat (temp, "-");
if (flags & 0x04)
   strcat (temp, "2"); /* Two-sided (2S) */
else
   strcat (temp, "-");
if (flags & 0x02)
   strcat (temp, "M"); /* Monsters can't cross this line (Mo) */
else
   strcat (temp, "-");
if (flags & 0x01)
   strcat (temp, "I"); /* Impassible (Im) */
else
   strcat (temp, "-");
if (strlen (temp) > 13)
{
   temp[13] = '|';
   temp[14] = '\0';
}
return temp;
#endif
}



/*
   get a long description of one linedef flag
*/

const char *GetLineDefFlagsLongName (int flags)
{
if (flags & 0x1000) return "Translucent [Strife]";
if (flags & 0x200)  return "Pass-through [Boom]";
if (flags & 0x100)  return "Always shown on the map";
if (flags & 0x80)   return "Never shown on the map";
if (flags & 0x40)   return "Blocks sound";
if (flags & 0x20)   return "Secret (shown as normal on the map)";
if (flags & 0x10)   return "Lower texture is \"unpegged\"";
if (flags & 0x08)   return "Upper texture is \"unpegged\"";
if (flags & 0x04)   return "Two-sided (may be transparent)";
if (flags & 0x02)   return "Monsters cannot cross this line";
if (flags & 0x01)   return "Impassible";
return "UNKNOWN";
}



/*
   get a short (14 char.) description of the type of a sector
*/

const char *GetSectorTypeName (int type)
{
/* KLUDGE: To avoid the last element which is bogus */
if (al_ltell (stdef) == al_lcount (stdef) - 1)
  al_lrewind (stdef);

if (CUR_STDEF != NULL && CUR_STDEF->number == type)
  return CUR_STDEF->shortdesc;
for (al_lrewind (stdef); ! al_leol (stdef); al_lstep (stdef))
  if (CUR_STDEF->number == type)
    return CUR_STDEF->shortdesc;
static char buf[30];
sprintf (buf, "UNKNOWN (%d)", type);
return buf;
}



/*
   get a long description of the type of a sector
*/

const char *GetSectorTypeLongName (int type)
{
/* KLUDGE: To avoid the last element which is bogus */
if (al_ltell (stdef) == al_lcount (stdef) - 1)
  al_lrewind (stdef);

if (CUR_STDEF != NULL && CUR_STDEF->number == type)
  return CUR_STDEF->longdesc;
for (al_lrewind (stdef); ! al_leol (stdef); al_lstep (stdef))
  if (CUR_STDEF->number == type)
    return CUR_STDEF->longdesc;
static char buf[30];
sprintf (buf, "UNKNOWN (%d)", type);
return buf;
}


