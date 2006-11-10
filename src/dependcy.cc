/*
 *	dependcy.cc
 *	Dependency class
 *	AYM 2000-04-09
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


#include "yadex.h"  /* Just to force a recompile when you ./configure again */
#include "dependcy.h"
#include "serialnum.h"


Dependency::Dependency (Serial_num *sn)
{
  serial_num  = sn;
  token_valid = false;
}


bool Dependency::outdated ()
{
  if (! token_valid)
    return true;
  return serial_num->outdated (token);
}


void Dependency::update ()
{
  serial_num->update (token);
  token_valid = true;
}

