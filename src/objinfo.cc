/*
 *	objinfo.cc
 *	AYM 1998-09-20
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2000 André Majorel.

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
#include "disppic.h"
#include "flats.h"	// DisplayFloorTexture()
#include "game.h"	// THINGDEF_SPECTRAL
#include "gfx.h"
#include "levels.h"
#include "objinfo.h"
#include "sprites.h"
#include "things.h"


static const int sprite_width  = 90;
static const int sprite_height = 90;


objinfo_c::objinfo_c ()
{
box_disp    = false;
obj_no      = OBJ_NO_NONE;
obj_no_disp = OBJ_NO_NONE;
prev_sector = OBJ_NO_NONE;
out_y1      = 0;
}


void objinfo_c::draw ()
{
char texname[WAD_TEX_NAME + 1];
int  tag, n;
int  sd1 = OBJ_NO_NONE;
int  sd2 = OBJ_NO_NONE;
int  s1 = OBJ_NO_NONE;
int  s2 = OBJ_NO_NONE;
int  x0, y0;		// Outer top left corner
int  ix0, iy0;		// Inner top left corner
int  width;
int  height;

// Am I already drawn ?
if (! is_obj (obj_no) || obj_no == obj_no_disp && obj_type == obj_type_disp)
   return;

// Does the box need to be redrawn ?
if (obj_type != obj_type_disp)
   box_disp = false;

// The caller should have called set_y1() before !
if (! out_y1)
   return;

ObjectsNeeded (obj_type, 0);
switch (obj_type)
   {
   case OBJ_THINGS:
      {
      const int columns = 28;
      width  = 2 * BOX_BORDER + 3 * WIDE_HSPACING + columns * FONTW
	                                                        + sprite_width;
      height = 2 * BOX_BORDER + 2 * WIDE_VSPACING
         + max ((int) (6.5 * FONTH), sprite_height);
      x0 = 0;
      y0 = out_y1 - height + 1;
      ix0 = x0 + BOX_BORDER + WIDE_HSPACING;
      iy0 = y0 + BOX_BORDER + WIDE_VSPACING;
      int ix1 = x0 + width - 1 - BOX_BORDER - WIDE_HSPACING;
      int iy1 = y0 + height - 1 - BOX_BORDER - WIDE_VSPACING;
      if (box_disp)
	 {
	 push_colour (WINBG);
         DrawScreenBox (ix0, iy0, ix0 + columns * FONTW - 1, iy1);
	 pop_colour ();
	 }
      else
         DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
      if (obj_no < 0)
         {
         const char *message = "(no thing selected)";
	 set_colour (WINFG_DIM);
	 DrawScreenText (x0 + (width - FONTW * strlen (message)) / 2,
            y0 + (height - FONTH) / 2, message);
         break;
	 }
      set_colour (YELLOW);
      DrawScreenText (ix0, iy0, "Thing #%d", obj_no);
      set_colour (WINFG);
      DrawScreenText (-1, iy0 + (int) (1.5 * FONTH), 0);
      DrawScreenText (-1, -1, "Coords: (%d, %d)",
         Things[obj_no].xpos, Things[obj_no].ypos);
      DrawScreenText (-1, -1, "Type:   %d",
         Things[obj_no].type);
      DrawScreenText (-1, -1, "Desc:   %.20s",
	 get_thing_name (Things[obj_no].type));
      DrawScreenText (-1, -1, "Angle:  %s",
	 GetAngleName (Things[obj_no].angle));
      DrawScreenText (-1, -1, "Flags:  %s",
	 GetWhenName (Things[obj_no].when));

      // Show the corresponding sprite
      {
      int sx1 = ix1 + 1 - HOLLOW_BORDER;
      int sy1 = iy1 + 1 - HOLLOW_BORDER;
      int sx0 = sx1 + 1 - sprite_width;
      int sy0 = sy1 + 1 - sprite_height;
      draw_box_border (sx0 - HOLLOW_BORDER, sy0 - HOLLOW_BORDER, 
		       sprite_width + 2 * HOLLOW_BORDER,
		       sprite_height + 2 * HOLLOW_BORDER,
                       HOLLOW_BORDER, 0);
      const char *sprite_root = get_thing_sprite (Things[obj_no].type);
      char flags = get_thing_flags (Things[obj_no].type);
      if (sprite_root != NULL)
         {
         const char *sprite_name = sprite_by_root (sprite_root);
         if (sprite_name != NULL)
	    {
	    hookfunc_comm_t c;
	    c.x0 = sx0;
	    c.y0 = sy0;
	    c.x1 = sx1;
	    c.y1 = sy1;
	    c.xofs = INT_MIN;
	    c.yofs = INT_MIN;
	    c.name = sprite_name;
	    c.flags = (flags & THINGDEF_SPECTRAL) ? HOOK_SPECTRAL : 0;
	    display_pic (&c);
	    }
	 else
	    {
	    push_colour (WINBG);
	    DrawScreenBox (sx0, sy0, sx1, sy1);
	    pop_colour ();
            set_colour (WINFG);
	    DrawScreenString (
	       sx0 + (sprite_width - strlen (sprite_root) * FONTW) / 2,
	       sy0 + sprite_height / 2 + 1 - 3 * FONTH / 2, sprite_root);
	    DrawScreenText (
	       sx0 + (sprite_width - 3 * FONTW) / 2,
	       sy0 + sprite_height / 2 + 1 - FONTH / 2, "not");
	    DrawScreenText (
	       sx0 + (sprite_width - 5 * FONTW) / 2,
	       sy0 + sprite_height / 2 + 1 + FONTH / 2, "found");
	    }
         }
      else
         {
	 push_colour (WINBG);
	 DrawScreenBox (sx0, sy0, sx1, sy1);
	 pop_colour ();
         set_colour (WINFG_DIM);
         DrawScreenText (
            sx0 + (sprite_width - 2 * FONTW) / 2,
	    sy0 + sprite_height / 2 + 1 - FONTH, "no");
         DrawScreenText (
            sx0 + (sprite_width - 6 * FONTW) / 2,
	    sy0 + sprite_height / 2 + 1, "sprite");
         }
      }
      }
      break;
   case OBJ_LINEDEFS:
      width  = 2 * BOX_BORDER + 2 * WIDE_HSPACING + 26 * FONTW;
      height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + (int) (8.5 * FONTH);
      x0 = 0;
      y0 = out_y1 - height + 1;
      ix0 = x0 + BOX_BORDER + WIDE_HSPACING;
      iy0 = y0 + BOX_BORDER + WIDE_VSPACING;
      // Ignore box_disp -- always redraw the whole box
      DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
      if (obj_no >= 0)
	 {
	 set_colour (YELLOW);
	 DrawScreenText (ix0, iy0, "Linedef #%d", obj_no);
	 set_colour (WINFG);
	 DrawScreenText (-1, iy0 + (int) (1.5 * FONTH),
                         "Flags:%3d %s",
                         LineDefs[obj_no].flags,
                         GetLineDefFlagsName (LineDefs[obj_no].flags));
	 DrawScreenText (-1, -1, "Type: %3d %s",
                         LineDefs[obj_no].type,
                         GetLineDefTypeName (LineDefs[obj_no].type));
	 sd1 = LineDefs[obj_no].sidedef1;
	 sd2 = LineDefs[obj_no].sidedef2;
	 tag = LineDefs[obj_no].tag;
	 s1 = LineDefs[obj_no].start;
	 s2 = LineDefs[obj_no].end;
	 ObjectsNeeded (OBJ_SIDEDEFS, OBJ_SECTORS, 0);
	 if (tag > 0)
	    {
	    for (n = 0; n < NumSectors; n++)
	       if (Sectors[n].tag == tag)
		  break;
	    }
	 else
	    n = NumSectors;
	 if (n < NumSectors)
	    DrawScreenText (-1, -1, "Tag:      %d (#%d)", tag, n);
	 else
	    DrawScreenText (-1, -1, "Tag:      %d (none)", tag);
	 DrawScreenText (-1, -1, "Vertices: (#%d, #%d)", s1, s2);
	 ObjectsNeeded (OBJ_VERTICES, 0);
	 n = ComputeDist (Vertices[s2].x - Vertices[s1].x,
                          Vertices[s2].y - Vertices[s1].y);
	 DrawScreenText (-1, -1, "Length:   %d", n);
	 DrawScreenText (-1, -1, "1st sd:   #%d", sd1);
	 DrawScreenText (-1, -1, "2nd sd:   #%d", sd2);
	 if (sd1 >= 0)
	    s1 = SideDefs[sd1].sector;
	 else
	    s1 = -1;
	 if (sd2 >= 0)
	    s2 = SideDefs[sd2].sector;
	 else
	    s2 = -1;
	 }
      else
         {
         const char *message = "(no linedef selected)";
	 set_colour (WINFG_DIM);
	 DrawScreenText (x0 + (width - FONTW * strlen (message)) / 2,
            y0 + (height - FONTH) / 2, message);
	 }

      x0 += width;
      width  = 2 * BOX_BORDER + 2 * WIDE_HSPACING + 24 * FONTW;
      ix0 = x0 + BOX_BORDER + WIDE_HSPACING;
      y0 = out_y1 - height + 1;
      // Ignore box_disp -- always redraw the whole box
      DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
      if (obj_no >= 0 && sd1 >= 0)
	 {
	 set_colour (YELLOW);
	 DrawScreenText (ix0, iy0, "First sidedef (#%d)", sd1);

         // 1S linedef without a normal texture ?
         if (sd2 < 0
            && SideDefs[sd1].tex3[0] == '-' && SideDefs[sd1].tex3[1] == '\0')
            set_colour (LIGHTRED);
         else
	    set_colour (WINFG);

	 DrawScreenText (-1, iy0 + (int) (1.5 * FONTH),
	    "Normal texture: %.*s", WAD_TEX_NAME, SideDefs[sd1].tex3);
	 if (s1 >= 0 && s2 >= 0 && Sectors[s1].ceilh > Sectors[s2].ceilh)
	    {
	    if (SideDefs[sd1].tex1[0] == '-' && SideDefs[sd1].tex1[1] == '\0')
	       set_colour (LIGHTRED);
            else
               set_colour (WINFG);
	    }
	 else
	    set_colour (WINFG_DIM);
	 DrawScreenText (-1, -1, "Upper texture:  %.*s",
	     WAD_TEX_NAME, SideDefs[sd1].tex1);
	 if (s1 >= 0 && s2 >= 0 && Sectors[s1].floorh < Sectors[s2].floorh)
	    {
	    if (SideDefs[sd1].tex2[0] == '-' && SideDefs[sd1].tex2[1] == '\0')
	       set_colour (LIGHTRED);
            else
               set_colour (WINFG);
	    }
	 else
	    set_colour (WINFG_DIM);
	 DrawScreenText (-1, -1, "Lower texture:  %.*s",
	     WAD_TEX_NAME, SideDefs[sd1].tex2);
	 set_colour (WINFG);
	 DrawScreenText (-1, -1, "Tex. X offset:  %d", SideDefs[sd1].xoff);
	 DrawScreenText (-1, -1, "Tex. Y offset:  %d", SideDefs[sd1].yoff);
	 DrawScreenText (-1, -1, "Sector:         #%d", s1);
	 }
      else
	 {
         const char *message = "(no first sidedef)";
	 set_colour (WINFG_DIM);
	 DrawScreenText (x0 + (width - FONTW * strlen (message)) / 2,
            y0 + (height - FONTH) / 2, message);
	 }
      x0 += width;
      ix0 = x0 + BOX_BORDER + WIDE_HSPACING;
      y0 = out_y1 - height + 1;
      // Ignore box_disp -- always redraw the whole box
      DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
      if (obj_no >= 0 && sd2 >= 0)
	 {
	 set_colour (YELLOW);
	 DrawScreenText (ix0, iy0, "Second sidedef (#%d)", sd2);
	 set_colour (WINFG);
	 texname[WAD_TEX_NAME] = '\0';
	 strncpy (texname, SideDefs[sd2].tex3, WAD_TEX_NAME);
	 DrawScreenText (-1, iy0 + (int) (1.5 * FONTH),
	    "Normal texture: %s", texname);
	 strncpy (texname, SideDefs[sd2].tex1, WAD_TEX_NAME);
	 if (s1 >= 0 && s2 >= 0 && Sectors[s2].ceilh > Sectors[s1].ceilh)
	    {
	    if (texname[0] == '-' && texname[1] == '\0')
	       set_colour (LIGHTRED);
	    }
	 else
	    set_colour (WINFG_DIM);
	 DrawScreenText (-1, -1, "Upper texture:  %s", texname);
	 set_colour (WINFG);
	 strncpy (texname, SideDefs[sd2].tex2, WAD_TEX_NAME);
	 if (s1 >= 0 && s2 >= 0 && Sectors[s2].floorh < Sectors[s1].floorh)
	    {
	    if (texname[0] == '-' && texname[1] == '\0')
	       set_colour (LIGHTRED);
	    }
	 else
	    set_colour (WINFG_DIM);
	 DrawScreenText (-1, -1, "Lower texture:  %s", texname);
	 set_colour (WINFG);
	 DrawScreenText (-1, -1, "Tex. X offset:  %d", SideDefs[sd2].xoff);
	 DrawScreenText (-1, -1, "Tex. Y offset:  %d", SideDefs[sd2].yoff);
	 DrawScreenText (-1, -1, "Sector:         #%d", s2);
	 }
      else
	 {
         const char *message = "(no second sidedef)";
	 set_colour (WINFG_DIM);
	 DrawScreenText (x0 + (width - FONTW * strlen (message)) / 2,
            y0 + (height - FONTH) / 2, message);
	 }
      break;
   case OBJ_VERTICES:
      width  = 2 * BOX_BORDER + 2 * WIDE_HSPACING + 29 * FONTW;
      height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + (int) (2.5 * FONTH);
      x0 = 0;
      y0 = out_y1 - height + 1;
      ix0 = x0 + BOX_BORDER + WIDE_HSPACING;
      iy0 = y0 + BOX_BORDER + WIDE_VSPACING;
      // Ignore box_disp -- always redraw the whole box
      DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
      if (obj_no < 0)
	 {
         const char *message = "(no vertex selected)";
	 set_colour (WINFG_DIM);
	 DrawScreenText (x0 + (width - FONTW * strlen (message)) / 2,
            y0 + (height - FONTH) / 2, message);
	 break;
	 }
      set_colour (YELLOW);
      DrawScreenText (ix0, iy0, "Vertex #%d", obj_no);
      set_colour (WINFG);
      DrawScreenText (-1, iy0 + (int) (1.5 * FONTH),
         "Coordinates: (%d, %d)", Vertices[obj_no].x, Vertices[obj_no].y);
      break;
   case OBJ_SECTORS:
      {
      int x1, y1;
      int ix1, iy1;
      const int columns = 32;
      width  = 2 * BOX_BORDER
             + 2 * WIDE_HSPACING
             + columns * FONTW
             + WIDE_HSPACING + DOOM_FLAT_WIDTH;
      height = 2 * BOX_BORDER
             + 2 * WIDE_VSPACING
             + max ((int) (9.5 * FONTH),
                    WIDE_HSPACING + 4 * HOLLOW_BORDER + 2 * DOOM_FLAT_HEIGHT);
      x0 = 0;
      y0 = out_y1 - height + 1;
      x1 = x0 + width - 1;
      y1 = y0 + height - 1;
      ix0 = x0 + BOX_BORDER + WIDE_HSPACING;
      iy0 = y0 + BOX_BORDER + WIDE_VSPACING;
      ix1 = x1 - BOX_BORDER - WIDE_HSPACING;
      iy1 = y1 - BOX_BORDER - WIDE_VSPACING;
      if (box_disp)
	 {
	 push_colour (WINBG);
	 DrawScreenBox (ix0, iy0, ix0 + columns * FONTW - 1, iy1);
	 pop_colour ();
	 }
      else
         DrawScreenBox3D (x0, y0, x1, y1);
      if (obj_no < 0)
	 {
         const char *const message = "(no sector selected)";
	 set_colour (WINFG_DIM);
	 DrawScreenText (x0 + (width - FONTW * strlen (message)) / 2,
            y0 + (height - FONTH) / 2, message);
	 break;
	 }
      set_colour (YELLOW);
      DrawScreenText (ix0, iy0, "Sector #%d", obj_no);
      set_colour (WINFG);
      if (prev_sector >= 0 && prev_sector != obj_no)
         {
	 DrawScreenText (-1, iy0 + (int) (1.5 * FONTH),
	    "Floor height:    %d (%+d)",
	    Sectors[obj_no].floorh, Sectors[obj_no].floorh - prev_floorh);
	 DrawScreenText (-1, -1,
	    "Ceiling height:  %d (%+d)",
	    Sectors[obj_no].ceilh,  Sectors[obj_no].ceilh - prev_ceilh);
	 }
      else
	 {
	 DrawScreenText (-1, iy0 + (int) (1.5 * FONTH),
	    "Floor height:    %d",   Sectors[obj_no].floorh);
	 DrawScreenText (-1, -1,
	    "Ceiling height:  %d",   Sectors[obj_no].ceilh);
         }
      DrawScreenText (-1, -1, "Headroom:        %d",   Sectors[obj_no].ceilh
                                                     - Sectors[obj_no].floorh);
      DrawScreenText (-1, -1, "Floor texture:   %.*s",
	  WAD_FLAT_NAME, Sectors[obj_no].floort);
      DrawScreenText (-1, -1, "Ceiling texture: %.*s",
	  WAD_FLAT_NAME, Sectors[obj_no].ceilt);
      DrawScreenText (-1, -1, "Light level:     %d",   Sectors[obj_no].light);
      DrawScreenText (-1, -1, "Type: %3d        %s",   Sectors[obj_no].special,
                                  GetSectorTypeName (Sectors[obj_no].special));
      tag = Sectors[obj_no].tag;
      ObjectsNeeded (OBJ_LINEDEFS, 0);
      if (tag == 0)
	 n = NumLineDefs;
      else
	 for (n = 0; n < NumLineDefs; n++)
	    if (LineDefs[n].tag == tag)
	       break;
      if (n < NumLineDefs)
	 DrawScreenText (-1, -1, "Tag:             %d (#%d)", tag, n);
      else if (tag == 99 || tag == 999)
	 DrawScreenText (-1, -1, "Tag:             %d (stairs?)", tag);
      else if (tag == 666)
	 DrawScreenText (-1, -1, "Tag:             %d (lower@end)", tag);
      else
	 DrawScreenText (-1, -1, "Tag:             %d (none)", tag);
      {
      hookfunc_comm_t block;

      // Display the floor texture in the bottom right corner
      block.x1 = ix1;
      block.x0 = block.x1 - (DOOM_FLAT_WIDTH + 2 * HOLLOW_BORDER - 1);
      block.y1 = iy1;
      block.y0 = block.y1 - (DOOM_FLAT_HEIGHT + 2 * HOLLOW_BORDER - 1);
      block.name = Sectors[obj_no].floort;
      display_flat_depressed (&block);

      // Display the ceiling texture above the floor texture
      block.y1 = block.y0 - (WIDE_VSPACING + 1);
      block.y0 = block.y1 - (DOOM_FLAT_HEIGHT + 2 * HOLLOW_BORDER - 1);
      block.name = Sectors[obj_no].ceilt;
      display_flat_depressed (&block);
      }
      break;
      }
   }
if (obj_type == OBJ_SECTORS)
   {
   if (obj_no != prev_sector)
      prev_sector = obj_no;
   if (obj_no >= 0)
      {
      prev_floorh = Sectors[obj_no].floorh;
      prev_ceilh  = Sectors[obj_no].ceilh;
      }
   }
box_disp = true;
obj_no_disp = obj_no;
obj_type_disp = obj_type;
}


