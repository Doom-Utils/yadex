/*
 *	bitvec.h
 *	A rudimentary bit vector class.
 *	AYM 1998-11-22
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


#include <limits.h>


class bitvec_c
   {
   public :
      inline bitvec_c (size_t n_elements)
         { 
	 a = (char *) GetMemory (n_elements / CHAR_BIT + 1);
 	 memset (a, 0, n_elements / CHAR_BIT + 1);
         this->n_elements = n_elements;
         return this;
         }

      inline ~bitvec_c ()
         {
         FreeMemory (a);
         }

      inline size_t nelements ()  // Return the number of elements
         {
         return n_elements;
         }

      inline int get (size_t n)  // Get bit <n>
         {
         return a[n/CHAR_BIT] & (1 << (n % CHAR_BIT));
         }

      inline void set (size_t n)  // Set bit <n> to 1
         {
         a[n/CHAR_BIT] |= 1 << (n % CHAR_BIT);
         }

      inline void unset (size_t n)  // Set bit <n> to 0
         {
         a[n/CHAR_BIT] &= ~(1 << (n % CHAR_BIT));
         }

   private :
      size_t n_elements;
      char *a;
   };

