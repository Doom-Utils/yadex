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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307, USA.
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
    virtual void               init     () = 0;
    virtual int                peekc    () = 0;
    virtual int                getc     (const char *callerid, int nest) = 0;
    virtual int                lastc    () = 0;
    virtual const char        *pathname () = 0;
    virtual const struct stat *stat     () = 0;
    virtual unsigned long      line     () = 0;
    virtual unsigned long      col      () = 0;
    virtual const char        *where    () = 0;
    virtual void               echo     (Output *) = 0;
    virtual Output *           echo     () = 0;

  private :
};


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
