/*
 *	m7.h - common definitions for m7
 *	AYM 2005-03-05
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


#ifndef M7_H  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define M7_H

typedef enum
{
  DEBUG_INPUT  = 0x0001,
  DEBUG_SYNTAX = 0x0002,
  DEBUG_DEFINE = 0x0004,
  DEBUG_SUBCTX = 0x0008
} debug_t;

extern int debug;

void err (const char *code, const char *fmt, ...);

#else
#  warn XXX "M7_H" redefined
#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
