/*
 *	dialog.h
 *	Dialog boxes
 *	AYM 1998-11-30
 */


#ifndef YH_DIALOG  /* DO NOT INSERT ANYTHING BEFORE THIS LINE */
#define YH_DIALOG

// Confirmation options are stored internally this way :
typedef enum
{
  YC_YES      = 'y',
  YC_NO       = 'n',
  YC_ASK      = 'a',
  YC_ASK_ONCE = 'o',
  YC_YNAN     = 'q'
} confirm_t;

bool confirm (int x0, int y0, confirm_t &mode,
   const char *prompt1, const char *prompt2);
bool confirm_simple (int x0, int y0, const char *prompt1, const char *prompt2);
void Notify (int, int, const char *, const char *);
void debmes (const char *fmt, ...);
void DisplayMessage (int, int, const char *, ...);
void NotImplemented (void);

#endif  /* DO NOT ADD ANYTHING AFTER THIS LINE */
