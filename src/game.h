/*
 *	game.h
 *	Header for game.cc
 *	AYM 19980129
 */


#include "rgb.h"


/*
 *	Data structures for game definition data
 */

/* ldt <number> <ldtgroup> <shortdesc> <longdesc> */
typedef struct
   {
   int number;
   char ldtgroup;
   const char *shortdesc;
   const char *longdesc;
   } ldtdef_t;

/* ldtgroup <ldtgroup> <description> */
typedef struct
   {
   char ldtgroup;
   const char *desc;
   } ldtgroup_t;

/* st <number> <shortdesc> <longdesc> */
typedef struct
   {
   int number;
   const char *shortdesc;
   const char *longdesc;
   } stdef_t;

/* thing <number> <thinggroup> <radius> <description> */
typedef struct
   {
   int number;		// Thing number
   char thinggroup;	// Thing group
   int radius;		// Radius of thing
   const char *desc;	// Short description of thing
   const char *sprite;  // Root of name of sprite for thing
   } thingdef_t;
/* (1)  This is only here for speed, to avoid having to lookup
        thinggroup for each thing when drawing things */

/* thinggroup <thinggroup> <colour> <description> */
typedef struct
   {
   char thinggroup;	// Thing group
   rgb_c rgb;		// RGB colour
   acolour_t acn;	// Application colour#
   const char *desc;	// Description of thing group
   } thinggroup_t;


/*
 *	Global variables that contain game definition data
 *	Those variables are defined in deu.c
 */

extern al_llist_t *ldtdef;
extern al_llist_t *ldtgroup;
extern al_llist_t *stdef;
extern al_llist_t *thingdef;
extern al_llist_t *thinggroup;


/* shorthands to make program more readable */
#define CUR_LDTDEF     ((ldtdef_t     *)al_lptr (ldtdef    ))
#define CUR_LDTGROUP   ((ldtgroup_t   *)al_lptr (ldtgroup  ))
#define CUR_STDEF      ((stdef_t      *)al_lptr (stdef     ))
#define CUR_THINGDEF   ((thingdef_t   *)al_lptr (thingdef  ))
#define CUR_THINGGROUP ((thinggroup_t *)al_lptr (thinggroup))

#define LDT_FREE    '\0'  /* KLUDGE: bogus ldt group   (see game.c) */
#define ST_FREE     '\0'  /* KLUDGE: bogus sector type (see game.c) */
#define THING_FREE  '\0'  /* KLUDGE: bogus thing group (see game.c) */


