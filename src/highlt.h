/*
 *	highlt.h
 *	AYM 1998-09-20
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


#include "edwidget.h"


class highlight_c : public edwidget_c
   {
   public :
      highlight_c (void);
      inline void set (int obj_type, int obj_no)
	 {
	 this->obj_type = obj_type;
	 this->obj_no   = obj_no;
	 }

      /* Methods declared in edwidget */
      inline void unset ()
	 { obj_no = OBJ_NO_NONE; }

      void draw          ();
      void undraw        ();

      inline int can_undraw ()
         { return 1; }  // I have the ability to undraw myself.

      inline int need_to_clear ()
	 { return 0; }  // I know how to undraw myself.

      inline void clear ()
	 { obj_no_disp = OBJ_NO_NONE; }

   private :
      int obj_no;	 /* The no. of the object we should highlight */
      int obj_type;	 /* The type of the object we should highlight */
      int obj_no_disp;	 /* The no. of the object that is really highlighted */
      int obj_type_disp; /* The type of the object that is really highlighted */
   };

