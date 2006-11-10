/*
 *	output_file.h - Output_file class
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


#ifndef OUTPUT_FILE_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define OUTPUT_FILE_H

#include <stddef.h>
#include <stdio.h>

#include <string>


/*
 *	Output_file - write into an stdio stream
 */
class Output_file : public Output
{
  public :
    Output_file (const char *pathname, FILE *fp);
    void          init     ();
    int           putc     (char c);
    int           puts     (const char *s);
    size_t        write    (const char *b, size_t recsz, size_t nrecs);
    const char   *pathname ();
    unsigned long line     ();
    unsigned long col      ();
    const char   *where    ();
    void          drop     (bool);

  private :
    const char *pathname_;
    FILE *fp;

    unsigned long line_;
    unsigned long col_;
    bool bol;

    std::string where_;

    bool drop_;
};


inline Output_file::Output_file (const char *pathname, FILE *fp)
{
  init ();
  pathname_ = pathname;
  this->fp = fp;
}


inline void Output_file::init ()
{
  pathname_ = NULL;
  fp = NULL;
  line_ = 0;
  col_ = 0;
  drop_ = false;
}


inline int Output_file::putc (char c)
{
  if (bol)
  {
    line_++;
    col_ = 0;
    bol = false;
  }

  int r = std::putc (c, fp);
  if (r != EOF)
    col_++;
  if (c == '\n')
    bol = true;

  return r == c;
}


inline int Output_file::puts (const char *s)
{
  while (*s != '\0')
    putc (*s++);
  return 0;				// FIXME
}


inline size_t Output_file::write (const char *b, size_t recsz, size_t nrecs)
{
  return fwrite (b, recsz, nrecs, fp);
}


/*
 *	Output_file::where - position of the last character written
 */
inline const char *Output_file::where ()
{
  char buf[22];				// Large enough for 64 bits

  where_ = pathname_;
  where_ += '(';
  sprintf (buf, "%lu", line_);
  where_ += buf;
  where_ += ',';
  sprintf (buf, "%lu", col_);
  where_ += buf;
  where_ += ')';
  return where_.c_str ();
}


inline unsigned long Output_file::line ()
{
  return line_;
}


inline unsigned long Output_file::col ()
{
  return col_;
}


inline const char *Output_file::pathname ()
{
  return pathname_;
}


inline void Output_file::drop (bool yesno)
{
  drop_ = yesno;
}


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
