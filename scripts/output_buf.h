/*
 *	output_buf.h - Output_buf class
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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef OUTPUT_BUF_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define OUTPUT_BUF_H

#include <stddef.h>

#include <string>


/*
 *	Output_buf - write into a dynamically allocated buffer
 */
class Output_buf : public Output
{
  public :
    Output_buf ();
    void          init     ();
    int           putc     (char c);
    int           puts     (const char *s);
    size_t        write    (const char *b, size_t recsz, size_t nrecs);
    const char   *pathname ();
    unsigned long line     ();
    unsigned long col      ();
    const char   *where    ();
    void          drop     (bool);
    std::string   buf;    // Yep. Would you like a goto with that ?

  private :
    bool drop_;
};


inline Output_buf::Output_buf ()
{
  init ();
}


inline void Output_buf::init ()
{
  buf.clear ();
  drop_ = false;
}


inline int Output_buf::putc (char c)
{
  buf += c;
  return 0;
}


inline int Output_buf::puts (const char *s)
{
  buf += s;
  return 0;
}


inline size_t Output_buf::write (const char *b, size_t recsz, size_t nrecs)
{
  while (nrecs-- > 0)
  {
    buf.append (b, recsz);
    b += recsz;
  }
  return 0;
}


inline const char *Output_buf::pathname ()
{
  return NULL;
}


inline unsigned long Output_buf::line ()
{
  return 0;
}


inline unsigned long Output_buf::col ()
{
  return 0;
}


inline const char *Output_buf::where ()
{
  return NULL;
}


inline void Output_buf::drop (bool yesno)
{
  drop_ = yesno;
}


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
