/*
 *	pic2img.h
 *	AYM 1998-??-??
 */


/* LoadPicture() normally attempts to load the whole picture lump
   into memory at once. However, this variable sets an upper limit
   to the size of the buffer that it allocates. If the picture is
   larger than this, it will be read in several passes. */
#define MAX_PICTURE_BUFFER_SIZE 100000


int LoadPicture (
   game_image_pixel_t *buffer,	/* Buffer to load picture into */
   int buf_width,		/* Dimensions of the buffer */
   int buf_height,
   const char *picname,		/* Picture lump name */
   int pic_x_offset,		/* Coordinates of top left corner of picture */
   int pic_y_offset,		/* relative to top left corner of buffer. */
   int *pic_width,		/* To return the size of the picture */
   int *pic_height);		/* (can be NULL) */

