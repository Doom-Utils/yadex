/*
 *	wads.h
 *	AYM 1998-11-25
 */


void wad_read_i16 (WadPtr wadfile, i16 *buf);
void wad_read_i32 (WadPtr wadfile, i32 *buf, long count = 1);
void file_write_i16 (FILE *fd, i16 buf);
void file_write_i32 (FILE *fd, i32 buf, long count = 1);


