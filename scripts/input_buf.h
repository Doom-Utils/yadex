/*
 *	input_buf.h - Input_buf class
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


#ifndef INPUT_BUF_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define INPUT_BUF_H

#include <stddef.h>
#include <stdio.h>

#include <string>


class Input_buf : public Input
{
  public :
    Input_buf (const std::string &buf);
    void init ();
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
    const std::string &buf;
    std::string::const_iterator i;

    unsigned long line_;
    unsigned long col_;
    bool bol;

    int c;

    std::string where_;

    Output *echo_;
};


inline Input_buf::Input_buf (const std::string &buf) :
  buf (buf)
{
  init ();
}


inline void Input_buf::init ()
{
  i = buf.begin ();
  line_ = 0;
  col_ = 0;
  echo_ = NULL;
}


/*
 *	Input_buf::peekc - return the next character without "getting" it
 *
 *	Returns the character that the next call to getc() will return.
 */
inline int Input_buf::peekc ()
{
  if (i == buf.end ())
    return EOF;
  return *i;
}


/*
 *	Input_buf::getc - read a character
 *
 *	Returns the character read or EOF.
 */
inline int Input_buf::getc (const char *callerid, int nest)
{
  if (bol)
  {
    line_++;
    col_ = 0;
    bol = false;
  }

  if (i == buf.end ())
    c = EOF;
  else
  {
    c = *i;
    ++i;
  }
  if (c != EOF)
    col_++;
  if (c == '\n')
    bol = true;

  if (debug & DEBUG_INPUT)
  {
    if (c == EOF)
      err ("M7530", "%s -> EOF %d %s", where (), nest, callerid);
    else if (isascii (c) && isprint (c))
      err ("M7540", "%s -> \"%c\" %d %s", where (), c, nest, callerid);
    else
      err ("M7550", "%s -> %02Xh %d %s",
	  where (), unsigned (c), nest, callerid);
  }

  if (echo_ != NULL && c != EOF)
    echo_->putc (c);

  return c;
}


/*
 *	Input_buf::lastc - return the last character read
 *
 *	Returns whatever the call to getc() returned.
 */
inline int Input_buf::lastc ()
{
  return c;
}


/*
 *	Input_buf::where - position of the last character read
 */
inline const char *Input_buf::where ()
{
  char buf[22];				// Large enough for 64 bits

  where_ = "buf(";
  sprintf (buf, "%lu", line_);
  where_ += buf;
  where_ += ',';
  sprintf (buf, "%lu", col_);
  where_ += buf;
  where_ += ')';
  return where_.c_str ();
}


inline const char *Input_buf::pathname ()
{
  return NULL;
}


inline const struct stat *Input_buf::stat ()
{
  return NULL;
}


inline unsigned long Input_buf::line ()
{
  return line_;
}


inline unsigned long Input_buf::col ()
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
inline void Input_buf::echo (Output *output)
{
  echo_ = output;
}


inline Output *Input_buf::echo ()
{
  return echo_;
}


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
