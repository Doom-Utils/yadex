/*
 *	oldmenus.h
 *	AYM 1998-12-04
 */


int DisplayMenu (int, int, const char *, ...);

int DisplayMenuList (
   int		x0,
   int		y0,
   const char	*title,
   al_llist_t	*list,
   const char	*(*getstr)(void *),
   int		*item_no);

int DisplayMenuArray (
   int		x0,
   int		y0,
   const char	*title,
   int		numitems,
   int		*ticked,
   char		**menustr,
   const i8	*shortcut_index,
   const int	*shortcut_key,
   int		*grayed_out);


