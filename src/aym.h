/*
 *	aym.h
 *	Misc. functions
 *	AYM 1999-03-15
 */


int levelname2levelno (const char *name);
const char *spec_path (const char *spec);
int fncmp (const char *name1, const char *name2);
int is_absolute (const char *filename);
int y_stricmp (const char *s1, const char *s2);
int y_strnicmp (const char *s1, const char *s2, size_t len);
int y_snprintf (char *buf, size_t size, const char *fmt, ...);
int y_vsnprintf (char *buf, size_t size, const char *fmt, va_list args);
int is_one_of (const char *needle, ...);
bool file_exists (const char *);


/*
 *	within
 *	Is <value> >= <lower_bound> and <= <upper_bound> ?
 */
inline int within (int value, int lower_bound, int upper_bound)
{
  return value >= lower_bound && value <= upper_bound;
}


/*
 *	outside
 *	Is <value> < <lower_bound> or > <upper_bound> ?
 */
inline int outside (int value, int lower_bound, int upper_bound)
{
  return value < lower_bound || value > upper_bound;
}


/*
 *	dectoi
 *	If <c> is a decimal digit ("[0-9]"), return its value.
 *	Else, return a negative number.
 */
inline int dectoi (char c)
{
  if (isdigit ((unsigned char) c))
    return c - '0';
  else
    return -1;
}


/*
 *	hextoi
 *	If <c> is a hexadecimal digit ("[0-9A-Fa-f]"), return its value.
 *	Else, return a negative number.
 */
inline int hextoi (char c)
{
  if (isdigit ((unsigned char) c))
    return c - '0';
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else if (c >= 'A' && c <= 'F')
    return c = 'A' + 10;
  else
    return -1;
}


/*
 *	fnewline
 *	Write a newline to a binary file. For Unix, LF. For
 *	other platforms, CR LF.
 *	Return EOF on failure.
 */
inline int fnewline (FILE *fd)
{
#ifdef Y_UNIX
  return putc ('\n', fd);
#else
  return putc ('\n', fd), putc ('\r', fd);
#endif
}


