/*
 *	yadex.h
 *	The header that all modules include.
 *	BW & RQ sometime in 1993 or 1994.
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1998 André Majorel.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307, USA.
*/


// Sanity checks
#if ! (defined(Y_BGI) ^ defined(Y_X11))
#error "You must #define either Y_BGI or Y_X11"
#endif

#if ! (defined(Y_DOS) ^ defined(Y_UNIX))
#error "You must #define either Y_DOS or Y_UNIX"
#endif


/*
 *	Standard headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#ifdef Y_DOS
#include <alloc.h>
#include <dos.h>
#include <bios.h>
#elif defined Y_UNIX
#include <unistd.h>
#define far
#define huge
#define stricmp    strcasecmp
#define strnicmp   strncasecmp
#define farmalloc  malloc
#define farrealloc realloc
#define farfree    free
#endif
// Not always in stdlib.h
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif


/*
 *	Additional libraries
 */
#ifdef Y_BGI 
#include <graphics.h>
#endif
#include <atclib.h>


/*
 *	Platform-independant types and formats.
 */
typedef unsigned char  u8;
typedef signed   char  i8;

typedef unsigned short u16;
#define F_U16_D "hu"
#define F_U16_H "hX"

typedef signed   short i16;
#define F_I16_D "hd"
#define F_I16_H "hX"

typedef unsigned long  u32;
#define F_U32_D "lu"
#define F_U32_H "lX"

typedef signed   long  i32;
#define F_I32_D "ld"
#define F_I32_H "lX"


/*
 *	Constants of the universe.
 */
#define HALFPI 1.5707963
#define ONEPI  3.1415926
#define TWOPI  6.2831852
#define ANSWER 42


/*
 *	Syntactic sugar
 */
//#ifndef Y_X11	// X11/Xlib.h already defines it !
typedef int Bool;	// Boolean data: true or false
//#endif


/*
 *	To avoid including certain headers.
 */
class rgb_c;


/*
 *	Doom definitions
 *	Things about the Doom engine and the wad file format.
 *	FIXME should move as much of this as possible to the ygd file...
 */
#define DOOM_PLAYER_HEIGHT          56
#define DOOM_FLAT_WIDTH             64
#define DOOM_FLAT_HEIGHT            64
#define DOOM_MIN_DEATHMATCH_STARTS  4
#define DOOM_COLOURS                256

/* The directory structure is the structure used by DOOM to order the
   data in it's wad files. */
typedef struct Directory huge *DirPtr;
struct Directory
   {
   i32  start;			// Offset to start of data
   i32  size;			// Byte size of data
   char name[8];		// Name of data block
   };

/* The wad file pointer structure is used for holding the information
   on the wad files in a linked list.
   The first wad file is the main wad file. The rest are patches. */
typedef struct WadFileInfo huge *WadPtr;
struct WadFileInfo
   {
   WadPtr next;			// Next file in linked list
   char *filename;		// Name of the wad file
   FILE *fd;			// C file stream information
   char type[4];		// Type of wad file ("IWAD" or "PWAD")
   i32  dirsize;		// Directory size of wad
   i32  dirstart;		// Offset to start of directory
   DirPtr directory;		// Array of directory information
   };

/* The master directory structure is used to build a complete directory
   of all the data blocks from all the various wad files. */
typedef struct MasterDirectory huge *MDirPtr;
struct MasterDirectory
   {
   MDirPtr next;		// Next in list
   WadPtr wadfile;		// File of origin
   struct Directory dir;	// Directory data
   };


/*
 *	The colour system.
 *	FIXME: only the logical side of the colour system
 *	should be declared here. The physical side should
 *	be moved to a less used header as very few modules
 *	need to know about it.
 */
/* acolour_t -- an application colour number.
   Several different application colours may refer to the same
   physical colour. */
typedef u8 acolour_t;
#define ACOLOUR_NONE 0xff  // The out-of-band value

#ifndef Y_BGI
/* The 16 VGA colours that DEU used to use.
   For the BGI version, those macros are defined in graphics.h
   FIXME: all references to these in the code should be removed. */
const acolour_t BLACK        = 0;
const acolour_t BLUE         = 1;
const acolour_t GREEN        = 2;
const acolour_t CYAN         = 3;
const acolour_t RED          = 4;
const acolour_t MAGENTA      = 5;
const acolour_t BROWN        = 6;
const acolour_t LIGHTGRAY    = 7;
const acolour_t DARKGRAY     = 8;
const acolour_t LIGHTBLUE    = 9;
const acolour_t LIGHTGREEN   = 10;
const acolour_t LIGHTCYAN    = 11;
const acolour_t LIGHTRED     = 12;
const acolour_t LIGHTMAGENTA = 13;
const acolour_t YELLOW       = 14;
const acolour_t WHITE        = 15;
#endif

// True logical colours
const acolour_t WINBG        = 16;
const acolour_t WINBG_LIGHT  = 17;
const acolour_t WINBG_DARK   = 18;
const acolour_t WINFG        = 19;
const acolour_t WINFG_DIM    = 20;
const acolour_t WINBG_HL     = 21;
const acolour_t WINFG_HL     = 22;
const acolour_t WINFG_DIM_HL = 23;
const acolour_t GRID1        = 24;
const acolour_t GRID2        = 25;
const acolour_t WINFGLABEL   = 26;

const acolour_t NCOLOURS     = 27;


/*
 *	More stuff
 */
/* AYM 19980112
This is the format of the block that the callback function (*hookfunc)()
and its caller InputNameFromListWithFunc() use to communicate. It
includes the old hookfunc parameters (x0, y0, x1, y1, name) plus a
couple of new parameters. The purpose of those new parameters is
fourfold :
  1. Give the caller an easy way to know the actual size of the
     image (for shift-F1).
  2. Hence let the caller display the size (remember "yes you
     can laugh at the way I did it..." ?).
  3. Provide the caller with various statistics such as number of
     patches in current texture etc. Good tourist information :)
  4. Give the caller a way to know whether the image has been
     drawn (i.e. "ready for shift-F1 ?")
Why did I create a structure instead of just adding parameters to the
hookfunc ? Just for the sake of streamlining the prototype of
InputNameFromListWithFunc() :)
In the following,
- "expected" means "set by caller, read by callee"
- "returned" means "set by callee, read by caller"
- "both"     means... both :-)
*/
typedef struct
   {
   int x0;           // [expected] Top left corner of where to draw image
   int y0;
   int x1;           // [expected] Bottom right corner
   int y1;
   int xofs;         // [expected] Top left corner of image in buffer
   int yofs;
   const char *name; // [expected] Name of image to display
   int flags;        // [both] Flags
   int width;        // [returned] Width of image
   int height;       // [returned] Height of image
   int npatches;     // [returned] Textures only : number of patches
   } hookfunc_comm_t;
#define HOOK_DRAWN       0x0001  // image is completely drawn
#define HOOK_SIZE_VALID  0x0002  // width and height are valid
#define HOOK_DISP_SIZE   0x0004  // caller should display "widthxheight"

// The actual definition is in selectn.h
typedef struct SelectionList *SelPtr;

// Confirmation options are stored internally this way :
typedef enum
   {
   YC_YES      = 'y',
   YC_NO       = 'n',
   YC_ASK      = 'a',
   YC_ASK_ONCE = 'o',
   } confirm_t;

// Bit bashing operations
const int YO_AND    = 'a';  // Argument = mask
const int YO_CLEAR  = 'c';  // Argument = bit#
const int YO_COPY   = 'd';  // Argument = source_bit# dest_bit#
const int YO_OR     = 'o';  // Argument = mask
const int YO_SET    = 's';  // Argument = bit#
const int YO_TOGGLE = 't';  // Argument = bit#
const int YO_XOR    = 'x';  // Argument = mask

typedef u8 game_image_pixel_t;

/*
 *	Even more stuff ("the macros and constants")
 */

extern const char *const log_file;        // "yadex.log"
extern const char *const msg_unexpected;  // "unexpected error"
extern const char *const msg_nomem;       // "Not enough memory"

// Convert screen coordinates to map coordinates
#define MAPX(x)		(OrigX + (int) (((x) - ScrCenterX) / Scale))
#define MAPY(y)		(OrigY + (int) ((ScrCenterY - (y)) / Scale))

// Convert map coordinates to screen coordinates
#define SCREENX(x)	(ScrCenterX + (int) (((x) - OrigX) * Scale))
#define SCREENY(y)	(ScrCenterY + (int) ((OrigY - (y)) * Scale))

// Object types
#define OBJ_NONE	0
#define OBJ_THINGS	1
#define OBJ_LINEDEFS	2
#define OBJ_SIDEDEFS	3
#define OBJ_VERTICES	4
#define OBJ_SEGS	5
#define OBJ_SSECTORS	6
#define OBJ_NODES	7
#define OBJ_SECTORS	8
#define OBJ_REJECT	9
#define OBJ_BLOCKMAP	10

// Special object numbers */
#define OBJ_NO_NONE    -1
#define OBJ_NO_CANVAS  -2
#define is_obj(n)      ((n) >= 0)
#define is_linedef(n)  ((n) >= 0 && (n) < NumLineDefs)
#define is_sector(n)   ((n) >= 0 && (n) < NumSectors )
#define is_sidedef(n)  ((n) >= 0 && (n) < NumSideDefs)
#define is_thing(n)    ((n) >= 0 && (n) < NumThings  )
#define is_vertex(n)   ((n) >= 0 && (n) < NumVertices)

// Half the size of an object (Thing or Vertex) in map coords
#define OBJSIZE		7
// AYM 19980216: to avoid magnifying vertices too much when zooming in
#define VERTEXSIZE (Scale > 1 ? OBJSIZE/Scale+0.5 : OBJSIZE)

// AYM 19980213: InputIntegerValue() uses this to mean that Esc was pressed
#define IIV_CANCEL  INT_MIN


/*
 *	Not real variables -- just a way for functions
 *	that return pointers to report errors in a better
 *	fashion than by just returning NULL and setting
 *	a global variable.
 */
extern char error_non_unique[1];  // Found more than one
extern char error_none[1];        // Found none
extern char error_invalid[1];     // Invalid parameter


/*
 *	Interfile global variables
 */

// Defined in yadex.c
extern const char *install_dir; 	// Where yadex is installed
extern FILE *logfile;			// Filepointer to the error log
extern Bool  Registered;		// Registered or shareware iwad ?
extern int screen_lines;		// Lines that our TTY can display
extern int remind_to_build_nodes;	// Remind the user to build nodes

// Defined in yadex.c and set from
// command line and/or config file
extern char *config_file;		// Name of the configuration file
extern int   copy_linedef_reuse_sidedefs;
extern Bool  Debug;			// Are we debugging?
extern int   default_ceiling_height;	// For new sectors
extern char  default_ceiling_texture[9];// For new sectors
extern int   default_floor_height;	// For new sectors
extern char  default_floor_texture[9];	// For new sectors
extern int   default_light_level;	// For new sectors
extern char  default_lower_texture[9];	// For new linedefs
extern char  default_middle_texture[9];	// For new linedefs
extern int   default_thing;		// For new THINGS
extern char  default_upper_texture[9];	// For new linedefs
extern int   double_click_timeout;// Max ms between clicks of double click.
extern Bool  Expert;		// Don't ask for confirmation for some ops.
extern char *Game;		// Name of game ("doom", "doom2" or "heretic")
extern int   grid_pixels_min;   // Minimum grid step in pixels when not locked
extern int   GridMin;	 	// Minimum grid step in map units
extern int   GridMax;		// Maximum grid step in map units
extern int   InitialScale;	// Initial zoom factor for map
extern confirm_t insert_vertex_merge_vertices;
extern confirm_t insert_vertex_split_linedef;
extern char *Iwad1;		// Name of the Doom iwad
extern char *Iwad2;		// Name of the Doom II iwad
extern char *Iwad3;		// Name of the Heretic iwad
extern char *MainWad;	        // Name of the main wad file
#ifdef AYM_MOUSE_HACKS
extern int   MouseMickeysH; 
extern int   MouseMickeysV; 
#endif
extern char **PatchWads;	// List of pwad files
extern Bool  Quiet;		// Don't beep when an object is selected
extern Bool  Quieter;		// Don't beep, even on error
extern int   scroll_normal;	// %s of screenful to scroll by
extern Bool  Select0;		// Autom. select obj. 0 when switching modes
extern int   show_help;		// Print usage message and exit.
extern Bool  SwapButtons;	// Swap right and middle mouse buttons
extern int   thing_fudge;	// How near to a thing you need to be to
				// highlight it, in percents of its radius.
extern int   verbose;		// Verbose mode
extern int   vertex_fudge;	// How near to a vertex you need to be to
				// highlight it, in percents of its radius.
extern int   welcome_message;	// Print the welcome message on startup.

// Defined in gfx.c
extern int   ScrMaxX;		// Maximum X screen coord
extern int   ScrMaxY;		// Maximum Y screen coord
extern float Scale;		// Scale to draw map 20 to 1

// Defined in wads.c
extern WadPtr  WadFileList;	// List of wad files
extern MDirPtr MasterDir;	// The master directory

// Defined in edit.c
extern Bool InfoShown;          // Is the bottom line displayed?

// Defined in mouse.c
#if defined(Y_BGI)
extern Bool UseMouse;		// Is there a mouse driver?
#elif defined(Y_X11)
#define UseMouse 1
#endif


/*
 *	Prototypes
 *	AYM 1998-10-16: DEU used to have _all_ prototypes here. Thus
 *	I had to recompile _all_ the modules every time I changed
 *	a single prototype. To avoid this, my theory is to put all
 *	the prototypes I can in individual headers. There is still
 *	room for improvement for that matter...
 */

/* aym.c */
#if !defined(PATH_MAX)
#define PATH_MAX 64
#endif
int levelname2levelno (const char *name);
const char *spec_path (const char *spec);
int fncmp (const char *name1, const char *name2);
int is_absolute (const char *filename);
int hextoi (char c);

/* checks.c (previously in editobj.c) */
void CheckLevel (int, int, int pulldown); /* SWAP! */
Bool CheckStartingPos (void); /* SWAP! */

/* colour1.c */
int getcolour (const char *s, rgb_c *rgb);

/* colour2.c */
int rgb2irgb (int r, int g, int b);

/* colour3.c */
void irgb2rgb (int c, rgb_c *rgb);

/* yadex.c */
int main (int, char *[]);
void Beep (void);
void PlaySound (int, int);
void fatal_error (const char *fmt, ...);
void report_error (const char *fmt, ...);
void LogMessage (const char *, ...);

/* editobj.c */
int InputObjectNumber (int, int, int, int);
int InputObjectXRef (int, int, int, Bool, int);
Bool Input2VertexNumbers (int, int, const char *, int *, int *);
void EditObjectsInfo (int, int, int, SelPtr);
void InsertStandardObject (int, int, int choice); /* SWAP! */
void MiscOperations (int, SelPtr *, int choice); /* SWAP! */

/* game.c */
void InitGameDefs (void);
void LoadGameDefs (const char *game);
void FreeGameDefs (void);

/* geom.c */
unsigned ComputeAngle (int, int);
unsigned ComputeDist (int, int);

/* gfx.c */
void set_colour (acolour_t);
void SetLineThickness (int thick);
void SetDrawingMode (int xor);
void DrawMapCircle (int, int, int);
void DrawMapLine (int mapx1, int mapy1, int mapx2, int mapy2);
void DrawMapVector (int, int, int, int);
void DrawMapArrow (int, int, unsigned);
void DrawScreenLine (int, int, int, int);
void DrawScreenBox (int, int, int, int);
void DrawScreenBox3D (int, int, int, int);
void DrawScreenBox3DShallow (int, int, int, int);
void DrawScreenBoxHollow (int x0, int y0, int x1, int y1, acolour_t colour);
void draw_box_border (int x, int y, int width, int height,
   int thickness, int raised);
void DrawScreenText (int, int, const char *, ...);
void DrawPointer (Bool);

/* input.c */
#include "input.h"

/* l_align.c (previously in objects.c) */
void AlignTexturesY (SelPtr *); /* SWAP! */
void AlignTexturesX (SelPtr *); /* SWAP! */

/* l_misc.c (previously in objects.c) */
void FlipLineDefs (SelPtr, Bool); /* SWAP! */
void SplitLineDefs (SelPtr); /* SWAP! */
void MakeRectangularNook (SelPtr obj, int width, int depth, int convex); /* SWAP! */
void SetLinedefLength (SelPtr obj, int length, int move_2nd_vertex);

/* l_prop.c (previously in editobj.c) */
void LinedefProperties (int x0, int y0, SelPtr obj);

/* l_unlink.c */
void unlink_sidedef (SelPtr linedefs, int side1, int side2);

/* levels.c */
void ReadLevelData (const char *); /* SWAP! */
void ForgetLevelData (void); /* SWAP! */
void SaveLevelData (const char *); /* SWAP! */
void ReadWTextureNames (void);
void ForgetFTextureNames (void);
void ReadFTextureNames (void);
void ForgetWTextureNames (void);

/* list.c */
void InputNameFromListWithFunc (int, int, const char *, int,
   const char *const *, int, char *, int, int,
   void (*hookfunc)(hookfunc_comm_t *));
void InputNameFromList (int, int, const char *, int, const char *const *,
   char *);

/* memory.c */
void *GetMemory (unsigned long size);
void *ResizeMemory (void *, unsigned long size);
void FreeMemory (void *);
void huge *GetFarMemory (unsigned long size);
void huge *ResizeFarMemory (void huge *old, unsigned long size);
void FreeFarMemory (void huge *);

/* mouse.c (this module is entirely DOS-specific) */
#if defined(Y_BGI)
void CheckMouseDriver (void);
void ShowMousePointer (void);
void HideMousePointer (void);
void GetMouseCoords (int *, int *, int *);
void SetMouseCoords (int, int);
void SetMouseLimits (int, int, int, int);
void ResetMouseLimits (void);
void MouseCallBackFunction (void);
#elif defined(Y_X11)
#define CheckMouseDriver nop
#define ShowMousePointer nop
#define HideMousePointer nop
#define GetMouseCoords   nop
#define SetMouseCoords   nop
#define SetMouseLimits   nop
#define ResetMouseLimits nop
#endif

/* names.c */
const char *GetObjectTypeName (int);
const char *GetEditModeName (int);
const char *GetLineDefTypeName (int);
const char *GetLineDefTypeLongName (int);
const char *GetLineDefFlagsName (int);
const char *GetLineDefFlagsLongName (int);
const char *GetSectorTypeName (int);
const char *GetSectorTypeLongName (int);

/* nop.c */
void nop (...);

/* objects.c */
void HighlightSelection (int, SelPtr); /* SWAP! */
int GetMaxObjectNum (int);
int GetCurObject (int objtype, int x, int y, int radius); /* SWAP! */
void SelectObjectsInBox (SelPtr *list, int, int, int, int, int); /* SWAP! */
void HighlightObject (int, int, int); /* SWAP! */
void DeleteObject (int, int); /* SWAP! */
void DeleteObjects (int, SelPtr *); /* SWAP! */
void InsertObject (int, int, int, int); /* SWAP! */
Bool IsLineDefInside (int, int, int, int, int); /* SWAP - needs Vertices & LineDefs */
int GetOppositeSector (int, Bool); /* SWAP! */
void CopyObjects (int, SelPtr); /* SWAP! */
Bool MoveObjectsToCoords (int, SelPtr, int, int, int); /* SWAP! */
void GetObjectCoords (int, int, int *, int *); /* SWAP! */
int FindFreeTag (void); /* SWAP! */

/* s_door.c (previously in objects.c) */
void MakeDoorFromSector (int); /* SWAP! */

/* s_lift.c (previously in objects.c) */
void MakeLiftFromSector (int); /* SWAP! */

/* s_merge.c (previously in objects.c */
void MergeSectors (SelPtr *); /* SWAP! */
void DeleteLineDefsJoinSectors (SelPtr *); /* SWAP! */

/* s_misc.c (previously in objects.c) */
void DistributeSectorFloors (SelPtr); /* SWAP! */
void DistributeSectorCeilings (SelPtr); /* SWAP! */
void RaiseOrLowerSectors (SelPtr obj);
void BrightenOrDarkenSectors (SelPtr obj);

/* s_prop.c (previously in editobj.c) */
void SectorProperties (int x0, int y0, SelPtr obj);

/* s_split.c (previously in objects.c) */
void SplitSector (int, int); /* SWAP! */
void SplitLineDefsAndSector (int, int); /* SWAP! */

/* swapmem.c */
void InitSwap (void);
void FreeSomeMemory (void);
void ObjectsNeeded (int, ...);

/* scrnshot.c */
void ScreenShot (void);

/* srcdate.c */
extern const char *const yadex_version;
extern const char *const yadex_source_date;

/* t_prop.c (previously in editobj.c) */
void ThingProperties (int x0, int y0, SelPtr obj);

/* textures.c */
void DisplayPic (hookfunc_comm_t *c);
void ChooseWallTexture (int, int, const char *, int, char **, char *);
void GetWallTextureSize (i16 *, i16 *, const char *);

/* v_merge.c */
void DeleteVerticesJoinLineDefs (SelPtr ); /* SWAP! */
void MergeVertices (SelPtr *); /* SWAP! */
Bool AutoMergeVertices (SelPtr *, int obj_type, char operation); /* SWAP! */

/* v_polyg.c */
void InsertPolygonVertices (int, int, int, int);

/* verbmsg.c */
void verbmsg (const char *fmt, ...);

/* wads.c */
void OpenMainWad (const char *);
void OpenPatchWad (const char *);
void CloseWadFiles (void);
void CloseUnusedWadFiles (void);
WadPtr BasicWadOpen (const char *);
void wad_read_bytes (WadPtr wadfile, void huge *buf, long count);
long wad_read_bytes2 (WadPtr wadfile, void huge *buf, long count);
void wad_seek (WadPtr, long);
MDirPtr FindMasterDir (MDirPtr, const char *);
void ListMasterDirectory (FILE *);
void ListFileDirectory (FILE *, WadPtr);
void BuildNewMainWad (const char *, Bool);
void WriteBytes (FILE *, const void huge *, long);
void CopyBytes (FILE *, FILE *, long);
int Exists (const char *);
void DumpDirectoryEntry (FILE *, const char *);
void SaveDirectoryEntry (FILE *, const char *);
void SaveEntryToRawFile (FILE *, const char *);
void SaveEntryFromRawFile (FILE *, FILE *, const char *);
int entryname_cmp (const char *entry1, const char *entry2);


