/*
 *	gfx.h
 *	AYM 1998-08-01
 */


#ifndef YH_GFX  /* Prevent multiple inclusion */
#define YH_GFX  /* Prevent multiple inclusion */


/* Width and height of font cell. Those figures are not meant to
   represent the actual size of the character but rather the step
   between two characters. For example, for the original 8x8 BGI
   font, FONTW was 8 and FONTH was 10. DrawScreenText() is supposed
   to draw characters properly _centered_ within that space, taking
   into account the optional underscoring. */
extern int FONTW;
extern int FONTH;
extern int font_xofs;
extern int font_yofs;


/* This number is the Y offset to use when underscoring a character.
   For example, if you want to draw an underscored "A" at (x,y),
   you should do :
     DrawScreenText (x, y,         "A");
     DrawScreenText (x, y + FONTU, "_"); */
#define FONTU  1

/* Narrow spacing around text. That's the distance in pixels
   between the characters and the inner edge of the box. */
#define NARROW_HSPACING  4
#define NARROW_VSPACING  2

/* Wide spacing around text. That's the distance in pixels
   between the characters and the inner edge of the box. */
#define WIDE_HSPACING  FONTW
#define WIDE_VSPACING  (FONTH / 2)

/* Boxes */
#define BOX_BORDER 2	// Offset between outer and inner edges of a 3D box
#define NARROW_BORDER 1	// Same thing for a shallow 3D box
#define HOLLOW_BORDER 1	// Same thing for a hollow box

/* Parameters set by command line args and configuration file */
extern const char *BGIDriver;	// BGI: default extended BGI driver
extern Bool  CirrusCursor;	// use HW cursor on Cirrus Logic VGA cards
extern Bool  FakeCursor;	// use a "fake" mouse cursor
extern const char *font_name;	// X: the name of the font to load
				// (if NULL, use the default)
extern int   initial_window_width;// X: the name says it all
extern int   initial_window_height;// X: the name says it all
extern int   no_pixmap;		// X: use no pixmap -- direct window output
extern int   VideoMode;		// BGI: default video mode for VESA cards

/* Global variables */
extern int   GfxMode;		// current graphics mode, or 0 for text
extern int   OrigX;		// the X origin
extern int   OrigY;		// the Y origin
extern int   ScrCenterX;	// X coord of screen center
extern int   ScrCenterY;	// Y coord of screen center
#ifdef Y_X11
typedef unsigned long xpv_t;	// The type of a pixel value in X's opinion
#ifdef X_PROTOCOL
extern Display *dpy;
extern int      scn;
extern Colormap cmap;		// The X colormap
extern Window   win;
extern Drawable drw;
extern GC       gc;
extern Visual  *win_vis;	// The visual for win
extern int      win_depth;	// The depth of win in bits
extern int      win_bpp;	// The depth of win in bytes
#endif  // ifdef X_PROTOCOL
#endif  // ifdef Y_X11
extern int	text_dot;	// DrawScreenText() debug flag

/* gfx.c */
/* Only the functions that are not used by many modules are here.
   The others are in yadex.h */
void InitGfx (void);
void SwitchToVGA256 (void);
void SwitchToVGA16 (void);
void TermGfx (void);
void SetWindowSize (int width, int height);
void ClearScreen (void);
void update_display ();
void DrawScreenMeter (int, int, int, int, float);
int TranslateToDoomColor (int);
#if defined Y_BGI && defined CIRRUS_PATCH
void SetHWCursorPos (unsigned, unsigned);
void SetHWCursorCol (long, long);
void SetHWCursorMap (char *);
#endif /* Y_BGI && CIRRUS_PATCH */
#ifdef PCOLOUR_NONE
void set_pcolour (pcolour_t colour);
#endif
void push_colour (acolour_t colour);
void pop_colour (void);
void draw_map_point (int mapx, int mapy);


#endif  /* Prevent mutliple inclusion */

