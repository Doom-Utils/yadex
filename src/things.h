/*
 *	things.h
 *	Header for things.cc
 *	BW & RQ sometime in 1993 or 1994.
 */


#ifndef YH_THINGS  /* Prevent multiple inclusion */
#define YH_THINGS  /* Prevent multiple inclusion */


#include "wstructs.h"


/* starting areas */
#define THING_PLAYER1         1
#define THING_PLAYER2         2
#define THING_PLAYER3         3
#define THING_PLAYER4         4
#define THING_DEATHMATCH      11


extern
#ifndef YC_THINGS
const
#endif
int max_radius;


void        create_things_table ();
void        delete_things_table ();
acolour_t   get_thing_colour (w_thingtype_t type);
const char *get_thing_name   (w_thingtype_t type);
const char *get_thing_sprite (w_thingtype_t type);
int         get_thing_radius (w_thingtype_t type);
inline int  get_max_thing_radius () { return max_radius; }
const char *GetAngleName (int);
const char *GetWhenName (int);


#endif  /* Prevent multiple inclusion */

