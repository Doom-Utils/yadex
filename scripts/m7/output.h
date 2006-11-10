/*
 *	output.h - Output class
 *	AYM 2005-02-27
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


#ifndef OUTPUT_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define OUTPUT_H

#include <stddef.h>
#include <stdio.h>

#include <string>


class Output
{
  public :
             Output () { }
    virtual ~Output () { }
    virtual void          init     () = 0;
    virtual int           putc     (char c) = 0;
    virtual int           puts     (const char *s) = 0;
    virtual size_t        write (const char *b, size_t recsz, size_t nrecs) = 0;
    virtual const char   *pathname () = 0;
    virtual unsigned long line     () = 0;
    virtual unsigned long col      () = 0;
    virtual const char   *where    () = 0;
    virtual void          drop     (bool) = 0;
};

#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
