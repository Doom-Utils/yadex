/*
 *	objinfo.h
 *	AYM 1998-09-20
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


#include "edwidget.h"


class objinfo_c : public edwidget_c
   {
   public :
      objinfo_c ();
      inline void set (int obj_type, int obj_no)
	 {
	 this->obj_no   = obj_no;
	 this->obj_type = obj_type;
	 }

      inline void set_y1 (int y1)
	 { out_y1 = y1; }

      /* Methods declared in edwidget_c */
      inline void unset ()
	 { obj_no = OBJ_NO_NONE; }

      void draw ();

      inline void undraw ()
         { }  // Sorry, I don't know how to undraw myself

      inline int can_undraw ()
         { return 0; }  // I don't have the ability to undraw myself

      inline int need_to_clear ()
	 { return is_obj (obj_no_disp) && ! is_obj (obj_no); }

      inline void clear ()
	 { obj_no_disp = OBJ_NO_NONE; }

   private :
      int obj_no;	 /* The no. and type of the object we should display */
      int obj_type;	 /* info about. */
      int obj_no_disp;	 /* The no. and type of the object for which info */
      int obj_type_disp; /* is really displayed. */
      int prev_sector;   /* No. of the last sector for which info was disp'd. */
      int prev_floorh;   /* Its floor height. */
      int prev_ceilh;    /* Its ceiling height. */
      int out_y1;        /* The bottom outer edge of the info window. */
   };

