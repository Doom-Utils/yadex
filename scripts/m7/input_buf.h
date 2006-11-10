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
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
    void		init ();
    int			peekc ();
    int			getc (const char *callerid, int nest);
    int			lastc ();
    const char	       *pathname ();
    const struct stat  *stat ();
    unsigned long	lastline ();
    unsigned long	lastcol  ();
    const char	       *lastwhere ();
    unsigned long	nextline ();
    unsigned long	nextcol  ();
    const char	       *nextwhere ();
    void		echo (Output *);
    Output	       *echo ();

  private :
    const std::string &buf;
    std::string::const_iterator i;

    unsigned long lastline_;
    unsigned long lastcol_;

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
  lastline_ = 1;
  lastcol_ = 0;
  echo_ = NULL;
  c = UCHAR_MAX + 1;
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
  if (c == '\n')
  {
    lastline_++;
    lastcol_ = 0;
  }

  if (i == buf.end ())
    c = EOF;
  else
  {
    c = *i;
    ++i;
  }
  if (c != EOF)
    lastcol_++;

  if (debug & DEBUG_INPUT)
  {
    if (c == EOF)
      err ("M7530", "%s -> EOF %d %s", lastwhere (), nest, callerid);
    else if (isascii (c) && isprint (c))
      err ("M7540", "%s -> \"%c\" %d %s", lastwhere (), c, nest, callerid);
    else
      err ("M7550", "%s -> %02Xh %d %s",
	  lastwhere (), unsigned (c), nest, callerid);
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
 *	Input_buf::nextwhere - position of the last character read
 */
inline const char *Input_buf::nextwhere ()
{
  char buf[22];				// Large enough for 64 bits

  where_ = "buf(";
  sprintf (buf, "%lu", nextline ());
  where_ += buf;
  where_ += ',';
  sprintf (buf, "%lu", nextcol ());
  where_ += buf;
  where_ += ')';
  return where_.c_str ();
}


/*
 *	Input_buf::lastwhere - position of the last character read
 */
inline const char *Input_buf::lastwhere ()
{
  char buf[22];				// Large enough for 64 bits

  where_ = "buf(";
  sprintf (buf, "%lu", lastline_);
  where_ += buf;
  where_ += ',';
  sprintf (buf, "%lu", lastcol_);
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


inline unsigned long Input_buf::nextline ()
{
  if (c == '\n')
    return lastline_ + 1;
  else
    return lastline_;
}


inline unsigned long Input_buf::nextcol ()
{
  if (c == '\n')
    return 1;
  else
    return lastcol_;
}


inline unsigned long Input_buf::lastline ()
{
  return lastline_;
}


inline unsigned long Input_buf::lastcol ()
{
  return lastcol_;
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
