/*
 *	gamedef.h - Gamedef class
 *	AYM 2000-08-30
 */

/*
This file is Copyright © 2000-2005 André Majorel.

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


#ifndef YH_GAMEDEF  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define YH_GAMEDEF


class Gamedef_priv;


/*
 *	Linedefs
 */
typedef struct
{
  char        ldtgroup;
  const char *shortdesc;
  const char *longdesc;
} linedef_type_t;

typedef char linedef_type_group_id_t;

typedef struct
{
  const char *desc;
} linedef_type_group_t;


/*
 *	Sectors
 */
typedef struct
{
  const char *shortdesc;
  const char *longdesc;
} sector_type_t;


/*
 *	Things
 */
typedef char thing_type_group_id_t;

typedef struct
{
  char        flags;		// Flags
  int         radius;		// Radius of thing
  const char *desc;		// Short description of thing
  const char *sprite;		// Root of name of sprite for thing
} thing_type_t;

const char THINGDEF_SPECTRAL = 0x01;

typedef struct
{
  char        thinggroup;	// Thing group
  rgb_c       rgb;		// RGB colour
  acolour_t   acn;		// Application colour#
  const char *desc;		// Description of thing group
} thing_type_group_t;


/*
 *	General
 */
typedef enum { YGLF__, YGLF_ALPHA, YGLF_DOOM, YGLF_HEXEN } yglf_t;
typedef enum { YGLN__, YGLN_E1M10, YGLN_E1M1, YGLN_MAP01 } ygln_t;
typedef enum { YGPF_NORMAL, YGPF_ALPHA, YGPF_PR } ygpf_t;
typedef enum { YGTF_NORMAL, YGTF_NAMELESS, YGTF_STRIFE11 } ygtf_t;
typedef enum { YGTL_NORMAL, YGTL_TEXTURES, YGTL_NONE } ygtl_t;
typedef enum				// flat_format
{
  YGFF_NORMAL,				// Doom, Strife: 64x64
  YGFF_HERETIC,				// Heretic: 64x64 and 64x65
  YGFF_HEXEN,				// Hexen: 64x64 and 64x128
} ygff_t;


// Shorthands to make the code more readable

#define LDT_FREE    '\0'  /* KLUDGE: bogus ldt group   (see game.c) */
#define ST_FREE     '\0'  /* KLUDGE: bogus sector type (see game.c) */
#define THING_FREE  '\0'  /* KLUDGE: bogus thing group (see game.c) */


/*
 *	Gamedef - contain all the definitions relative to a game
 */
class Gamedef
{
  public :
    Gamedef ();
    ~Gamedef ();
    const linedef_type_t& linedef_type (wad_ldtype_t type) const;
    const sector_type_t&  sector_type  (wad_stype_t  type) const;
    const thing_type_t&   thing_type   (wad_ttype_t  type) const;
    void linedef_type     (wad_ldtype_t type, linedef_type_t& data);
    void sector_type      (wad_stype_t type,  sector_type_t&  data);
    void thing_type       (wad_ttype_t type,  thing_type_t&   data);
    void del_linedef_type (wad_ldtype_t type);
    void del_sector_type  (wad_stype_t  type);
    void del_thing_type   (wad_ttype_t  type);
    int flat_format       (ygff_t flat_format);
    int level_format      (yglf_t level_format);
    int level_name        (yglf_t level_name);
    int picture_format    (ygpf_t picture_format);
    int texture_format    (ygtf_t texture_format);
    int texture_lumps     (ygtl_t texture_lumps);
    ygff_t flat_format    () const;
    yglf_t level_format   () const;
    ygln_t level_name     () const;
    ygpf_t picture_format () const;
    ygtf_t texture_format () const;
    ygtl_t texture_lumps  () const;

  private :
    Gamedef            (const Gamedef&);	// Too lazy to implement it
    Gamedef& operator= (const Gamedef&);	// Too lazy to implement it
    Gamedef_priv *priv;
};


#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
