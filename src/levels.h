/*
 *	levels.h
 *	BW & RQ sometime in 1993 or 1994
 */


#ifndef YH_LEVELS
#define YH_LEVELS

/* the includes */
#include "wstructs.h"
#include "things.h"

/* the external variables from levels.c */
extern MDirPtr Level;		/* master dictionary entry for the level */

extern int   NumThings;		/* number of things */
extern TPtr  Things;		/* things data */
extern int   NumLineDefs;	/* number of linedefs */
extern LDPtr LineDefs;		/* linedefs data */
extern int   NumSideDefs;	/* number of sidedefs */
extern SDPtr SideDefs;		/* sidedefs data */
extern int   NumVertices;	/* number of vertices */
extern VPtr  Vertices;		/* vertices data */
extern int   NumSegs;		/* number of segments */
#if 0				/* Was needed by the node builder { */
extern SEPtr Segs;		/* list of segments */
extern SEPtr LastSeg;		/* last segment in the list */
extern int   NumSSectors;	/* number of subsectors */
extern SSPtr SSectors;		/* list of subsectors */
extern SSPtr LastSSector;	/* last subsector in the list */
#endif                          /* } */
extern int   NumSectors;	/* number of sectors */
extern SPtr  Sectors;		/* sectors data */
extern int   NumWTexture;	/* number of wall textures */
extern char  **WTexture;	/* array of wall texture names */
extern int   NumFTexture;	/* number of floor/ceiling textures */
extern char  **FTexture;	/* array of texture names */

extern int   MapMaxX;		/* maximum X value of map */
extern int   MapMaxY;		/* maximum Y value of map */
extern int   MapMinX;		/* minimum X value of map */
extern int   MapMinY;		/* minimum Y value of map */

extern Bool  MadeChanges;	/* made changes? */
extern Bool  MadeMapChanges;	/* made changes that need rebuilding? */


#endif	// Prevent multiple inclusion
