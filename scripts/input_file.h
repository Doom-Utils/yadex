/*
 *	input_file.h - Input_file class
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


#ifndef INPUT_FILE_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define INPUT_FILE_H

#include <stddef.h>
#include <stdio.h>

#include <string>


class Input_file : public Input
{
  public :
    Input_file ();
    Input_file (const char *pathname, const struct stat *s, FILE *fp);
    void init ();
    void init (const char *pathname, const struct stat *s, FILE *fp);
    int peekc ();
    int getc (const char *callerid, int nest);
    int lastc ();
    const char *pathname ();
    const struct stat *stat ();
    unsigned long line ();
    unsigned long col ();
    const char *where ();
    void echo (Output *);
    Output *echo ();

  private :
    const char *pathname_;
    const struct stat *stat_;
    FILE *fp;

    unsigned long line_;
    unsigned long col_;
    bool bol;

    static const int NOCHAR = EOF - 1;
    int cnext;
    int c;

    std::string where_;

    Output *echo_;
};


inline Input_file::Input_file ()
{
  init ();
}


inline Input_file::Input_file (const char *pathname, const struct stat *s,
    FILE *fp)
{
  init (pathname, s, fp);
}


inline void Input_file::init ()
{
  pathname_ = NULL;
  stat_ = NULL;
  fp = NULL;
  line_ = 0;
  col_ = 0;
  echo_ = NULL;
  cnext = NOCHAR;
}


inline void Input_file::init (const char *pathname, const struct stat *s, FILE *fp)
{
  init ();
  pathname_ = pathname;
  stat_ = s;
  this->fp = fp;
}


/*
 *	Input_file::peekc - return the next character without "getting" it
 *
 *      Returns the character that the next call to getc() will return.
 */
inline int Input_file::peekc ()
{
  if (cnext == NOCHAR)
    cnext = std::getc (fp);
  return cnext;
}


/*
 *	Input_file::getc - read a character
 *
 *	Returns the character read or EOF.
 */
inline int Input_file::getc (const char *callerid, int nest)
{
  if (bol)
  {
    line_++;
    col_ = 0;
    bol = false;
  }

  c = peekc ();
  cnext = std::getc (fp);
  if (c != EOF)
    col_++;
  if (c == '\n')
    bol = true;

  if (debug & DEBUG_INPUT)
  {
    if (c == EOF)
      err ("M7610", "%s -> EOF %d %s", where (), nest, callerid);
    else if (isascii (c) && isprint (c))
      err ("M7620", "%s -> \"%c\" %d %s", where (), c, nest, callerid);
    else
      err ("M7630", "%s -> %02Xh %d %s",
	  where (), unsigned (c), nest, callerid);
  }

  if (echo_ != NULL && c != EOF)
    echo_->putc (c);

  return c;
}


/*
 *	Input_file::lastc - return the last character read
 *
 *	Returns whatever the call to getc() returned.
 */
inline int Input_file::lastc ()
{
  return c;
}


/*
 *	Input_file::where - position of the last character read
 */
inline const char *Input_file::where ()
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


inline const char *Input_file::pathname ()
{
  return pathname_;
}


inline const struct stat *Input_file::stat ()
{
  return stat_;
}


inline unsigned long Input_file::line ()
{
  return line_;
}


inline unsigned long Input_file::col ()
{
  return col_;
}


/*
 *	echo - control the echoing of characters read to an Output object
 *
 *	If the argument is a null pointer, characters read are not
 *	echoed (this is the default). Otherwise, characters read are
 *	echoed by calling output.putc().
 *
 *	This is used to parse an argument without expanding it.
 */
inline void Input_file::echo (Output *output)
{
  echo_ = output;
}


inline Output *Input_file::echo ()
{
  return echo_;
}


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
