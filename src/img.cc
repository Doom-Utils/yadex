/*
 *	img.cc - Game image object (255 colours + transparency)
 *	AYM 2000-06-13
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public domain in
1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-2000 André Majorel.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include "yadex.h"
#include "gcolour2.h"  /* colour0 */
#include "img.h"


// Holds the private data members
class Img_priv
{
  public:
    Img_priv () { buf = 0; width = 0; height = 0; }
    ~Img_priv () { if (buf != 0) delete[] buf; }
    img_pixel_t *buf;
    img_dim_t    width;
    img_dim_t    height;
};


/*
 *	Img::Img - default constructor
 *
 *	The new image is a null image.
 */
Img::Img ()
{
  p = new Img_priv;
}


/*
 *	Img::Img - constructor with dimensions
 *
 *	The new image is set to the specified dimensions.
 */
Img::Img (img_dim_t width, img_dim_t height)
{
  p = new Img_priv;
  resize (width, height);
}


/*
 *	Img::~Img - dtor
 */
Img::~Img ()
{
  delete p;
}


/*
 *	Img::is_null - return true iff this is a null image
 */
bool Img::is_null () const
{
  return p->buf == 0;
}


/*
 *	Img::width - return the current width
 *
 *	If the image is null, return 0.
 */
img_dim_t Img::width () const
{
  return p->width;
}


/*
 *	Img::height - return the current height
 *
 *	If the image is null, return 0.
 */
img_dim_t Img::height () const
{
  return p->height;
}


/*
 *	Img::buf - return a const pointer on the buffer
 *
 *	If the image is null, return a null pointer.
 */
const img_pixel_t *Img::buf () const
{
  return p->buf;
}


/*
 *	Img::wbuf - return a writable pointer on the buffer
 *
 *	If the image is null, return a null pointer.
 */
img_pixel_t *Img::wbuf ()
{
  return p->buf;
}


/*
 *	Img::clear - clear the image
 */
void Img::clear ()
{
  if (p->buf != 0)
    memset (p->buf, IMG_TRANSP, p->width * p->height);
}


/*
 *	Img::resize - resize the image
 *
 *	If either dimension is zero, the image becomes a null
 *	image.
 */
void Img::resize (img_dim_t width, img_dim_t height)
{
  // FIXME should check that the width and height are valid.
  if (width == p->width && height == p->height
    || width * height == p->width * p->height)
    return;

  // Unallocate old buffer
  if (p->buf != 0)
  {
    delete[] p->buf;
    p->buf = 0;
  }

  // Is it a null image ?
  if (width == 0 || height == 0)
  {
    p->width  = 0;
    p->height = 0;
    return;
  }

  // Allocate new buffer
  p->width  = width;
  p->height = height;
  p->buf = new img_pixel_t[width * height + 10];  // Some slack
  clear ();
}


