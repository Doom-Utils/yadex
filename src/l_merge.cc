/*
 *	l_merge - merge superimposed linedefs
 *	AYM 2003-12-31
 */

/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2005 André Majorel and others.

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
#include <vector>
#include <algorithm>
#include "dialog.h"
#include "levels.h"
#include "l_merge.h"


#if 1

bool merge_superimposed_linedefs ()
{
  return false;
}

#else

struct Linedef_ref
{
  wad_vn_t  vertexl;
  wad_vn_t  vertexh;
  wad_ldn_t ldno;
  char      in;				// b0 = in group A, b1 = in group B

  // For sorting by lowest-numbered vertex
  bool operator< (const Linedef_ref &rhs)
  {
    if (this->vertexl != rhs.vertexl) return this->vertexl - rhs.vertexl;
    if (this->vertexh != rhs.vertexh) return this->vertexh - rhs.vertexh;

    // Make sure the lowest-numbered comes first
    if (this->ldno != rhs.ldno) return this->ldno - rhs.ldno;

    return 0;  // Can't happen
  }
};


struct Mergeme
{
  wad_ldn_t ldno;			// Linedef to be merged...
  wad_ldn_t ldno_master;		// with linedef...

  bool operator< (const Mergeme &rhs)
  {
    return this->ldno - rhs.ldno;
  }
};


/*
 *	merge_superimposed_linedefs - merge any superimposed linedefs
 *
 *	The argument is a list of vertices that have moved. The function
 *	will only look for superimpositions between a linedef that uses
 *	at least one of the vertices in the list and a linedef that uses
 *	one of the vertices NOT in the list. The idea is that, if you
 *	have deliberately created superimposed linedefs in a level, you
 *	don't want Yadex to go through all of them again and again every
 *	time you drag a vertex.
 *
 *	If the list is empty, the function will check every linedef
 *	against every other linedef. This is for when you do want to
 *	look for superimposed linedefs everywhere, for example during
 *	the consistency checks.
 *
 *	Return true if the window must be redrawn, false otherwise.
 */
bool merge_superimposed_linedefs (const std::vector<bool> &ina,
    const std::vector<bool> &inb)
{
  bool redraw = false;

  /* Make an array of all the linedefs sorted by lowest-numbered vertex.
     If two linedefs use the same vertices, they'll be next to each
     other in the array. Thus finding all superimposed linedefs involves
     a simple O(n) scan of the list.
     
     If both vertices are the same, the lowest-numbered linedef comes
     first. This is necessary to enforce the lowest-numbered-linedef
     -takes-precedence policy. Plus the code that goes through Mergeme
     relies on it. */
  std::vector<Linedef_ref> linedefs (NumLineDefs);
  const size_t nlinedefs = min (ina.size (), inb.size ());

  for (int ld = 0; ld < nlinedefs; ld++)
  {
    linedefs[ld].vertexl = y_min (LineDefs[ld].start, LineDefs[ld].end);
    linedefs[ld].vertexh = y_max (LineDefs[ld].start, LineDefs[ld].end);
    linedefs[ld].ldno    = ld;
    linedefs[ld].in      = ina[ld] | (inb[ld] << 1);
  }
  sort (linedefs.begin (), linedefs.end ());

  /* Scan the array for superimposed linedefs. For each linedef that's
     superimposed with the previous linedef, ask if it should be merged
     with it. If so, add it to the list (doing it on the spot is
     impossible because deleting multiple objects must be done in
     descending object number order (hence the bug in versions <= 1.7.0)). */
  std::vector<Mergeme>     mergeme;
  confirm_t                confirm_flag = YC_YNAN;
  const Linedef_ref *const ldmax        = linedefs + NumLineDefs;
  const Linedef_ref       *ldref        = linedefs;

  for (const Linedef_ref *ld = ldref + 1; ld < ldmax; ld++)
  {
    if (ld->vertexl != ldref->vertexl || ld->vertexh != ldref->vertexh)
    {
      ldref++;
      continue;
    }

    int ld1 = ldref->linedefno;
    int ld2 =    ld->linedefno;

    char prompt[81];
    y_snprintf (prompt, sizeof prompt,
	"Linedef %d is superimposed to linedef %d",
	ld2, ld1);
    if (Expert
      || (redraw = true,
	  confirm (-1, -1, confirm_flag, prompt, "Merge it with linedef %d ?")))
    {
      mergeme.push_back (ld2, ld1);
      if (confirm_flag == YC_NO)
	break;
    }
  }

  /* Merge all the linedefs to be merged. Process the highest-numbered
     linedefs first. */
  sort (mergeme.begin (), mergeme.end ());
  for (std::vector<Mergeme>::iterator i = mergme.end ();
      i != mergeme.begin (); --i)
  {
    printf ("merging linedef %d with linedef %d", i->ldno, i->ldno_master);
    if (i->ldno <= i->ldno_master)	// Be paranoid
    {
      nf_bug ("merge_superimposed_linedefs: bad sequence %d %d",
	  i->ldno, i->ldno_master);
      continue;
    }
    //DeleteObject (Objid (OBJ_LINEDEFS, i->linedefno));
  }

#if 0
  for (ld = 0; ld+1 < NumLineDefs; ld++)
     {
     if (linedefs[ld+1].vertexl != linedefs[ld].vertexl
      || linedefs[ld+1].vertexh != linedefs[ld].vertexh)
       continue;
     int ld1 = linedefs[ld].linedefno;
     int ld2 = linedefs[ld+1].linedefno;
     char prompt[81];
     y_snprintf (prompt, sizeof prompt, "Linedefs %d and %d are superimposed",
	ld1, ld2);
     redraw = true;
     if (Expert || confirm (-1, -1, confirm_flag,
	prompt, "(and perhaps others too). Merge them ?"))
	{
	LDPtr ldo = LineDefs + ld1;
	LDPtr ldn = LineDefs + ld2;
	/* Test if the linedefs have the same orientation */
	if (ldn->start == ldo->end)
	   flipped = true;
	else
	   flipped = false;
	/* Merge linedef ldo (ld) into linedef ldn (ld+1) */
	/* FIXME When is this done ? Most of the time when adding a
	   door/corridor/window between two rooms, right ? So we should
	   handle this in a smarter way by copying middle texture into
	   lower and upper texture if the sectors don't have the same
	   heights and maybe setting the LTU and UTU flags.
	   This also applies when a linedef becomes two-sided as a
	   result of creating a new sector. */
	if (ldn->sidedef1 < 0)
	   {
	   if (flipped)
	      {
	      ldn->sidedef1 = ldo->sidedef2;
	      ldo->sidedef2 = OBJ_NO_NONE;
	      }
	   else
	      {
	      ldn->sidedef1 = ldo->sidedef1;
	      ldo->sidedef1 = OBJ_NO_NONE;
	      }
	   }
	if (ldn->sidedef2 < 0)
	   {
	   if (flipped)
	      {
	      ldn->sidedef2 = ldo->sidedef1;
	      ldo->sidedef1 = OBJ_NO_NONE;
	      }
	   else
	      {
	      ldn->sidedef2 = ldo->sidedef2;
	      ldo->sidedef2 = OBJ_NO_NONE;
	      }
	   }
	if (ldn->sidedef1 >= 0 && ldn->sidedef2 >= 0 && (ldn->flags & 0x04) == 0)
	   ldn->flags = 0x04;
	DeleteObject (Objid (OBJ_LINEDEFS, ld1));
	}
     }

  FreeMemory (linedefs);
  }
#endif

  return redraw;
}


#endif
