/*
 *	input.h - Input class
 *	AYM 2005-03-19
 */

/*
This file is copyright André Majorel 2005.

This program is free software; you can redistribute it and/or modify it under
the terms of version 2 of the GNU General Public License as published by the
Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#ifndef INPUT_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define INPUT_H

#include <stddef.h>
#include <stdio.h>

#include <string>


class Input
{
  public :
	     Input () { }
    virtual ~Input () { }
    virtual Output *           echo      () = 0;
    virtual const char        *pathname  () = 0;
    virtual const struct stat *stat      () = 0;
    virtual int                getc      (const char *callerid, int nest) = 0;
    virtual int                lastc     () = 0;
    virtual int                peekc     () = 0;
    virtual unsigned long      lastcol   () = 0;
    virtual unsigned long      lastline  () = 0;
    virtual const char        *lastwhere () = 0;
    virtual unsigned long      nextcol   () = 0;
    virtual unsigned long      nextline  () = 0;
    virtual const char        *nextwhere () = 0;
    virtual void               echo      (Output *) = 0;
    virtual void               init      () = 0;

  private :
};


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
