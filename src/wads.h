/*
 *	wads.h
 *	AYM 1998-11-25
 */


i16 wad_read_i16 (WadPtr wadfile);
void wad_read_i16 (WadPtr wadfile, i16 *buf);
void wad_read_i32 (WadPtr wadfile, i32 *buf, long count = 1);
void file_write_i16 (FILE *fd, i16 buf);
void file_write_i32 (FILE *fd, i32 buf, long count = 1);
void file_write_name (FILE *fd, const char *name);


#if 0
/*
 *	flat_name_cmp
 *	Compare two flat names like strcmp() compares two strings.
 */
inline int flat_name_cmp (const char *name1, const char *name2)
{
}


/*
 *	tex_name_cmp
 *	Compare two texture names like strcmp() compares two strings.
 *	T
 */
inline int tex_name_cmp (const char *name1, const char *name2)
{
}


/*
 *	sprite_name_cmp
 *	Compare two sprite names like strcmp() compares two strings.
 */
inline int sprite_name_cmp (const char *name1, const char *name2)
{
}
#endif

