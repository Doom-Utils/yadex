/*
 *	output_null.h - Output_null class
 *	AYM 2005-03-01
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


#ifndef OUTPUT_NULL_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define OUTPUT_NULL_H

#include <stddef.h>

#include <string>


/*
 *	Output_null - write into the bit bucket
 */
class Output_null : public Output
{
  public :
    Output_null ();
    void          init     ();
    int           putc     (char c);
    int           puts     (const char *s);
    size_t        write    (const char *b, size_t recsz, size_t nrecs);
    const char   *pathname ();
    unsigned long line     ();
    unsigned long col      ();
    const char   *where    ();
    void          drop     (bool);
};


inline Output_null::Output_null ()
{
  init ();
}


inline void Output_null::init ()
{
  ;
}


inline int Output_null::putc (char c)
{
  (void) c;
  return 0;
}


inline int Output_null::puts (const char *s)
{
  (void) s;
  return 0;
}


inline size_t Output_null::write (const char *b, size_t recsz, size_t nrecs)
{
  (void) b;
  (void) recsz;
  (void) nrecs;
  return 0;
}


inline const char *Output_null::pathname ()
{
  return NULL;
}


inline unsigned long Output_null::line ()
{
  return 0;
}


inline unsigned long Output_null::col ()
{
  return 0;
}


inline const char *Output_null::where ()
{
  return NULL;
}


inline void Output_null::drop (bool yesno)
{
  (void) yesno;
}


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
