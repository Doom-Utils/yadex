/*
 *	m7.cc - unobtrusive macro processor
 *	AYM 2005-02-26
 */

/*
This file is copyright André Majorel 2005.

This program is free software; you can redistribute it and/or modify it under
the terms of version 2 of the GNU General Public License as published by the
Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <map>
#include <string>
#include <vector>

#include "m7.h"
#include "output.h"
#include "output_buf.h"
#include "output_file.h"
#include "output_null.h"
#include "input.h"
#include "input_buf.h"
#include "input_file.h"


/*
 *	Types
 */
const char *nullmacro           = "";
const char *argmacro		= "\1";
const char *default_time_format = "%Y-%m-%d";

// Valid macro name character
static bool isident (int c)
{
  return c == '_' || isascii (c) && isalnum (c);
}

// Valid macro parameter name ($1, $2...)
static bool isparm (const char *s)
{
  if (*s == '\0')			// Can't happen
    return false;

  while (*s != '\0')
  {
    if (! (isascii (*s) && isdigit (*s)))
      return false;
    s++;
  }
  return true;
}

// Macro identifier
typedef std::string macname_t;


class Context;
typedef int (*dirfunc_t) (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);

// Definition of a macro argument
typedef enum
{
  MADF_ID       = 0x0001,		// Must be a valid macro name
  MADF_UINT     = 0x0002,		// Must be an unsigned integer
  MADF_GREEDY   = 0x0004,		// Extends till EOM
  MADF_NOEXPAND = 0x0008,		// Do not expand macro references
  MADF_MAND     = 0x0010		// Mandatory
} madflags_t;

inline const madflags_t operator| (const madflags_t &lhs, const madflags_t &rhs)
{
  madflags_t result = madflags_t (int (lhs) | int (rhs));
  return result;
}

struct Macargdef
{
  Macargdef () : flags (madflags_t (0)) { }
  Macargdef (madflags_t flags) : flags (flags) { }
  madflags_t flags;			// Bitwise OR of MADF_*
};

// Definition of a macro
typedef enum
{
  MDF_INTRINSIC = 0x0001,		// Built-in macro
  MDF_DNL       = 0x0002,		// Strip whitespace on the right
  MDF_RECURSE   = 0x0004		// Expansion is recursive
} mdflags_t;

inline const mdflags_t operator| (const mdflags_t &lhs, const mdflags_t &rhs)
{
  mdflags_t result = mdflags_t (int (lhs) | int (rhs));
  return result;
}

struct Macdef
{
  int		flags;			// Bitwise OR of MDF_*
  dirfunc_t	func;			// Intrinsic macro: callback
  std::string	value;			// User macro: value string
  unsigned short argmin;		// Need at least this many args
  short		argmax;			// Take up to that many args
  std::vector<Macargdef> args;

  Macdef ()
  {
  }

  Macdef (mdflags_t flags, const std::string &value, ...) :
    flags	(flags),
    func	(NULL),
    value	(value)
  {
    va_list argp;
    const Macargdef *mad;

    for (va_start (argp, value), argmin = 0, argmax = 0;
	(mad = va_arg (argp, const Macargdef *)) != NULL;)
    {
      args.push_back (*mad);
      argmax++;
      if (mad->flags & MADF_MAND)
	argmin++;
    }
  }

  Macdef (mdflags_t flags, dirfunc_t func, ...) :
    flags	(flags),
    func	(func),
    value	()
  {
    va_list argp;
    const Macargdef *mad;

    for (va_start (argp, func), argmin = 0, argmax = 0;
	(mad = va_arg (argp, const Macargdef *)) != NULL;)
    {
      args.push_back (*mad);
      argmax++;
      if (mad->flags & MADF_MAND)
	argmin++;
    }
  }

  Macdef (mdflags_t flags, const std::string &value, unsigned short argmin,
      short argmax, const std::vector<Macargdef> &argdefs) :
    flags	(flags),
    func	(NULL),
    value	(value),
    argmin	(argmin),
    argmax	(argmax),
    args	(argdefs)
  {
    args.resize (argmax);		// FIXME report errors
  }
};

// Symbol table
typedef std::map<macname_t, Macdef> definitions_t;
//static std::map<std::string, Macdef> macros;  // FIXME global

// Context
class Context
{
  public :
    Context () :
      parent (nullctx)
    {
    }
    Context (Context &parent) :
      parent (parent)
    {
    }
    const Macdef *getmacro (const macname_t &name) const;
    void setmacro (const macname_t &name, Macdef def);
    // FIXME net something to unset a macro

  private :
    static Context nullctx;	// The notional parent of the root context
    Context &parent;
    definitions_t macros;
};

Context Context::nullctx;


inline const Macdef *Context::getmacro (const macname_t &name) const
{
  definitions_t::const_iterator i;

  i = macros.find (name);
  if (i != macros.end ())
    return &i->second;
  if (&parent == &nullctx)
    return NULL;
  return parent.getmacro (name);
}


// FIXME prevent the redefinition or shadowing of the intrinsics
inline void Context::setmacro (const macname_t &name, Macdef def)
{
  macros[name] = def;
}


// Parser flags
typedef enum
{
  SCOPE_SPACE = 0x0001,			// Stop at whitespace
  SCOPE_SYN1  = 0x0002,			// Stop at syn_end1 (")")
  SCOPE_SYN2  = 0x0004			// Stop at syn_end2 ("}")
} scope_t;

inline scope_t &operator|= (scope_t &lhs, const scope_t &rhs)
{
  lhs = scope_t (lhs | rhs);
  return lhs;
}


/*
 *	Static functions
 */
static void register_intrinsics (Context &ctx);
static int expand_text (Context &ctx, int nest, Input &input,
    Output &output, scope_t scope);
static int expand_reference (Context &ctx, int nest, Input &input,
    Output &output);
static int locate_file (const char *base, const char *arg, std::string &result);
static void dependency (const char *pathname, bool recursive);
static int mac_ (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
#if 0
static int mac__hash (Input &input, Output &output,
    std::vector<std::string> &args);
#endif
static int mac_blob (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_date (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_define (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_echo (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_file (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_mtime (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_set (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_source (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);
static int mac_where (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args);


/*
 *	Extern globals
 */
int			debug		= 0;


/*
 *	Static globals
 */
static bool		datadeps	= false;
static char		mode		= '\0';
static const char	*opathname	= NULL;
static const char	*openmoder	= "r";
static const char	*openmodew	= "w";
static char		syn_meta	= '$';
static const char	syn_begin1	= '(';
static const char	syn_end1	= ')';
static const char	syn_begin2	= '{';
static const char	syn_end2	= '}';


/*
 *	main - process the arguments and exit
 */
int main (int argc, char *argv[])
{
  int		rc		= 0;
  const char	*ipathname	= NULL;
  struct stat	istat;
  FILE		*ifp		= NULL;
  FILE		*ofp		= NULL;
  bool		obufset		= false;
  size_t	obufsz		= 69;	// Placate GCC


  /* Parse the command line */
  if (argc == 2 && strcmp (argv[1], "--help") == 0)
  {
    printf (
"Usage:\n"
"  m7 --help\n"
"  m7 --version\n"
"  m7 [-abdM] [-B num] [-m char] [-o file] [file ...]\n"
"Options:\n"
"  --help    Print usage to standard output and exit sucessfully\n"
"  --version Print version number to standard output and exit sucessfully\n"
"  -a        With -M, also list the data dependencies\n"
"  -b        Open all files in binary mode\n"
"  -B num    Output buffer size in bytes. 0 is line-buffered, 1 is unbuffered.\n"
"  -d        Debug mode\n"
"  -m char   Use char as the metacharacter (default is %c)\n"
"  -M        List the dependencies to standard output and exit successfully\n"
"  -o file   Write output to file (default is standard output)\n"
      ,
      syn_meta
      );
    exit (0);
  }

  if (argc == 2 && strcmp (argv[1], "--version") == 0)
  {
    puts ("0.0.1");
    exit (0);
  }

  {
    int g;

    while ((g = getopt (argc, argv, "abB:dm:Mo:")) != EOF)
    {
      if (g == 'a')
      {
	datadeps = true;
      }
      else if (g == 'b')
      {
	openmoder = "rb";
	openmodew = "wb";
      }
      else if (g == 'B')
      {
	obufsz = atol (optarg);
	obufset = true;
      }
      else if (g == 'd')
      {
	debug = debug
	  | DEBUG_INPUT
	  | DEBUG_SYNTAX
	  | DEBUG_DEFINE
	  | DEBUG_SUBCTX
	  ;
      }
      else if (g == 'm')
      {
	if (strlen (optarg) != 1)
	{
	  err ("M7010", "-d: the argument length must be exactly 1");
	  exit (1);
	}
	syn_meta = *optarg;
      }
      else if (g == 'M')
      {
	mode = 'l';
      }
      else if (g == 'o')
      {
	opathname = optarg;
      }
      else
	exit (1);
    }
  }

  /* Initialise the root macro table with intrinsics */
  Context ctx;
  register_intrinsics (ctx);
  // FIXME add the environment variables

  /* Open the output file */
  if (mode != 'l')
  {
    if (opathname == NULL)
    {
      ofp = stdout;
    }
    else
    {
      ofp = fopen (opathname, openmodew);
      if (ofp == NULL)
      {
	err ("M7020", "%s: %s", opathname, strerror (errno));
	exit (1);
      }
    }
  }

  if (obufset)				// Honour -B
  {
    int r;
    errno = 0;
    if (obufsz == 0)
      r = setvbuf (ofp, NULL, _IOLBF, 0);
    else if (obufsz == 1)
      r = setvbuf (ofp, NULL, _IONBF, 0);
    else
    {
      char *buf = (char *) malloc (obufsz);
      if (buf == NULL)
      {
	err ("M7030", "%s", strerror (ENOMEM));
	exit (1);
      }
      r = setvbuf (ofp, buf, _IOFBF, obufsz);
    }
    if (r != 0)
    {
      err ("M7080", "%s: setvbuf error: %s", opathname, strerror (errno));
      exit (1);
    }
  }

  Output *output = NULL;
  if (mode == 'l')
  {
    Output_null output_null;
    output = new Output_null;
  }
  else
  {
    output = new Output_file (opathname == NULL ? "(stdout)" : opathname, ofp);
  }

  /* Process the arguments */
  if (optind >= argc)
  {
    Input_file input ("(stdin)", NULL, stdin);
    if (expand_text (ctx, 0, input, *output, scope_t (0)) != 0)
      rc = 2;
  }
  else
  {
    for (int a = optind; a < argc; a++)
    {
      ipathname = argv[a];

      dependency (ipathname, true);

      if (stat (ipathname, &istat) != 0)
      {
	err ("M7110", "%s: %s", ipathname, strerror (errno));
	exit (1);
      }
      ifp = fopen (ipathname, openmoder);
      if (ifp == NULL)
      {
	err ("M7120", "%s: %s", ipathname, strerror (errno));
	exit (1);
      }
      Input_file input (ipathname, &istat, ifp);
      int r = expand_text (ctx, 0, input, *output, scope_t (0));
      if (r > rc)
	rc = r;
      fclose (ifp);
    }
  }

  /* Exit point */
  if (ofp != NULL)
  {
    if (fflush (ofp) != 0)
    {
      err ("M7130", "%s: %s",
	opathname == NULL ? "(stdout)" : opathname, strerror (errno));
      if (rc < 2)
	rc = 2;
    }
    if (ofp != stdout)
      if (fclose (ofp) != 0)
      {
	err ("M7140", "%s: %s",
	  opathname == NULL ? "(stdout)" : opathname, strerror (errno));
	if (rc < 2)
	  rc = 2;
      }
  }
  exit (rc);
}


/*
 *	register_intrinsics - register all the intrinsics into a Context
 */
static void register_intrinsics (Context &ctx)
{
  const mdflags_t f = MDF_INTRINSIC;

  ctx.setmacro
  (
    nullmacro,
    Macdef
    (
      f,
      mac_,
      &Macargdef (MADF_GREEDY),		// $*
      NULL
    )
  );

  ctx.setmacro
  (
    argmacro,
    Macdef
    (
      f,
      mac_,
      NULL
    )
  );

  ctx.setmacro
  (
    "blob",
     Macdef
     (
       f | MDF_DNL,
       mac_blob,
       &Macargdef (MADF_MAND),		// file
       NULL
     )
  );

  ctx.setmacro
  (
    "date",
    Macdef
    (
      f,
      mac_date,
      &Macargdef (),			// [format]
      NULL
    )
  );

  ctx.setmacro
  (
    "define",
    Macdef
    (
      f | MDF_DNL,
      mac_define,
      &Macargdef (MADF_MAND | MADF_ID),		// name
      &Macargdef (MADF_MAND | MADF_UINT),	// argmin
      &Macargdef (MADF_MAND | MADF_UINT),	// argmax
      &Macargdef (MADF_GREEDY | MADF_NOEXPAND),	// value
      NULL
    )
  );

  ctx.setmacro
  (
    "echo",
    Macdef
    (
      f,
      mac_echo,
      &Macargdef (MADF_GREEDY),		// $*
      NULL
    )
  );

  ctx.setmacro
  (
    "file",
    Macdef
    (
      f | MDF_DNL,
      mac_file,
      &Macargdef (MADF_MAND),		// file
      NULL
    )
  );

  ctx.setmacro
  (
    "mtime",
    Macdef
    (
      f,
      mac_mtime,
      &Macargdef (),			// [format]
      NULL
    )
  );

  ctx.setmacro
  (
    "set",
    Macdef
    (
      f | MDF_DNL,
      mac_set,
      &Macargdef (MADF_MAND | MADF_ID),	// name
      &Macargdef (MADF_GREEDY),		// [value]
      NULL
    )
  );

  ctx.setmacro
  (
    "source",
    Macdef
    (
      f | MDF_DNL,
      mac_source,
      &Macargdef (MADF_MAND),		// file
      NULL
    )
  );

  ctx.setmacro
  (
    "where",
    Macdef
    (
      f,
      mac_where,
      NULL
    )
  );
}


/*
 *	expand_text - expand text which may contain macro references
 *
 *	On entry, input must be set so that the next call to getc()
 *	returns the first plain text character. If a metacharacter ("$")
 *	is seen, expand_reference() is called.
 *
 *	Returns 0 on success, non-zero on failure.
 */
static int expand_text (Context &ctx, int nest, Input &input,
    Output &output, scope_t scope)
{
  bool dnl = false;
  std::string tail;
  for (;;)
  {
    int c = input.peekc ();

    if (c == EOF
	|| (scope & SCOPE_SYN1)  && c == syn_end1
	|| (scope & SCOPE_SYN2)  && c == syn_end2
	|| (scope & SCOPE_SPACE) && isspace (c))
      return 0;
    else if (c == syn_meta)
    {
      int r = expand_reference (ctx, nest + 1, input, output);
      if (r < 0 || r > 1)
      {
	err ("M7150", "%s: parse_directive() error", input.where ());
	return 1;
      }
      if (r == 1)
      {
	dnl = true;
      }
    }
    else
    {
      c = input.getc ("txt1", nest);
      if (dnl && c == '\n')
      {
	tail.clear ();
	dnl = false;
      }
      else if (dnl && c != '\n' && isspace (c))
      {
	tail += c;
      }
      else if (dnl && ! isspace (c) && ! tail.empty ())
      {
	tail += c;
	output.write (tail.c_str (), tail.size (), 1);
	tail.clear ();
	dnl = false;
      }
      else
	output.putc (c);
    }

  }

  return 0;
}


/*
 *	expand_reference - expand a macro reference
 *
 *	On entry, input must be set so that the next call to getc()
 *	returns the "$" that marks the beginning on the macro reference.
 *	On exit, input is set so that the next call getc() returns the
 *	first character after the directive (for a long form reference,
 *	the character after ")"; for a short form reference, the
 *	character immediately following the last argument).
 *
 *	Returns 0 or 1 on success. Anything else mean failure. If the
 *	return value is 1, the caller should s/\s*?\n// the output.
 */
typedef enum
{
  MRF_GREEDY = 0x0001,
  MRF_IGNERR = 0x0002,
  MRF_DNL    = 0x0004
} flags_t;

static int expand_reference (Context &ctx, int nest, Input &input,
    Output &output)
{
  /* Get the initial meta ("$") */
  int c = input.getc ("ref0", nest);
  if (c != syn_meta)
  {
    err ("M7155", "%s: internal error, report this bug (%02Xh)",
	input.where (), c);
    exit (42);
  }
  const char *where_pathname = input.pathname ();
  unsigned long where_line = input.line ();
  unsigned long where_col = input.col ();

  /* Get the next character */
  c = input.peekc ();
  if (c == EOF)
  {
    err ("M7160", "%s: unexpected EOF after \"%c\"", input.where (), syn_meta);
    return 2;
  }
  /* Escape metacharacters ("$", ")", "}" and whitespace) */
  if (c == syn_meta || c == syn_end1 || c == syn_end2 || isspace (c))
  {
    input.getc ("ref1", nest);
    output.putc (c);
    return 0;
  }

  /* OK, this must be a macro reference */
  int flags = 0;		// Not flags_t or you can forget about |=
  bool explicit_dnl = false;
  int hasparen = 0;

  /* Get the "(" or "{" if it's there */
  if (c == syn_begin1)
  {
    hasparen = 1;
    c = input.getc ("ref(", nest);
  }
  else if (c == syn_begin2)
  {
    hasparen = 2;
    c = input.getc ("ref{", nest);
  }

  /* Get the flags */
  {
    for (;;)
    {
      int c = input.peekc ();
      if (c == '*')
      {
	flags |= MRF_GREEDY;
	if (hasparen != 0)
	  err ("M7170",
	      "%s: the \"*\" flag has no effect in long form references",
	      input.where ());
      }
      else if (c == '-')
      {
	flags |= MRF_IGNERR;
      }
      else if (c == '@')
      {
	flags |= MRF_DNL;
	explicit_dnl = true;
      }
      else
      {
	break;
      }
      input.getc ("reff", nest);
    }
  }

  /* Read the keyword (the name of the macro) into token[] */
  char token[21];
  {
    int c;
    char *p    = token;
    char *pmax = token + sizeof token - 1;

    while (isident (input.peekc ()))
    {
      int c = input.getc ("refn", nest);
      if (p >= pmax)
      {
	err ("M7180", "%s: directive too long", input.where ());
	return 2;
      }
      *p++ = c;
    }
    *p = '\0';
    if (debug & DEBUG_SYNTAX)
      err ("M7190", "%s: directive \"%s\"", input.where (), token);
    if (p == token)
    {
      err ("M7200", "%s: directive name missing", input.where ());
      return 2;
    }
#if 0
    c = input.peekc ();
    if (input.peekc () == EOF)
    {
      err ("M7210", "%s: unexpected EOF in directive", input.where ());
      return 2;
    }
    else if ((hasparen == 1 && c != syn_end1
	   || hasparen == 2 && c != syn_end2)
	&& ! isspace (c))
    {
      err ("M7220", "%s: unexpected character %02Xh in directive name",
	  input.where (), unsigned (c));
      return 2;
    }
#endif
  }

  /* Find the definition of this macro */
  const Macdef *md;
			      // Ugly special case for arguments in define
  if (input.echo () != NULL && isparm (token))
  {
    md = ctx.getmacro (argmacro);
    if (md == NULL)
    {
      err ("M7225", "argmacro not found, report this bug");
      exit (1);
    }
  }
  else
  {
    md = ctx.getmacro (token);
    if (md == NULL)
    {
      if (flags & MRF_IGNERR)
      {
	md = ctx.getmacro (nullmacro);
	if (md == NULL)			// Can't happen
	{
	  err ("M7230", "nullmacro not found, report this bug");
	  exit (1);
	}
      }
      else
      {
	err ("M7240", "%s: invalid macro \"%s\"", input.where (), token);
	return 2;
      }
    }
  }
  if (! explicit_dnl)
  {
    flags &= ~ MRF_DNL;
    if (md->flags & MDF_DNL)
      flags |= MRF_DNL;
  }

  /* Read (and perhaps expand) all the arguments till argmax or EOF/EOM
     and put them into args. */
  std::vector<std::string> args;

  size_t argmax;
  if (hasparen != 0 || (flags & MRF_GREEDY))
    argmax = md->argmax;
  else
    argmax = md->argmin;

  while (argmax >= 0 && args.size () < argmax)
  {
    Output_buf o;

    const Macargdef *mad = &(md->args[args.size ()]);  // FIXME

    /* Make sure we have some whitespace. This is so that we know
       when to stop with the "$name" form. */
    if (hasparen == 0 && ! isspace (input.peekc ()))
      break;

    /* Skip the whitespace. */
    while (isspace (input.peekc ()))
      c = input.getc ("refw", nest);
    if (input.peekc () == EOF
     ||	hasparen == 1 && input.peekc () == syn_end1
     || hasparen == 2 && input.peekc () == syn_end2)
      break;

    /* Add the (normally expanded) argument to args. */
    scope_t scope = SCOPE_SPACE;
    if ((mad->flags & MADF_GREEDY) && hasparen != 0)
      scope = scope_t (scope & ~SCOPE_SPACE);
    if (hasparen == 1)
      scope |= SCOPE_SYN1;
    if (hasparen == 2)
      scope |= SCOPE_SYN2;
    int r;
    if (mad->flags & MADF_NOEXPAND)
    {
      Output_null blackhole;
      Output *echo = input.echo ();
      input.echo (&o);
      r = expand_text (ctx, nest + 1, input, blackhole, scope);
      input.echo (echo);
    }
    else
    {
      r = expand_text (ctx, nest + 1, input, o, scope);
    }
    if (r != 0)
    {
      err ("M7270", "%s: an error occurred in argument %d of macro \"%s\"",
	  input.where (), args.size () + 1, token);
      return 2;
    }
    if (debug & DEBUG_SYNTAX)
      err ("M7290", "%s: arg \"%s\"", input.where (), o.buf.c_str ());

    if (mad->flags & MADF_ID)  // Enforce MADF_ID (valid macro name)
      for (size_t i = 0; i < o.buf.length (); i++)
	if (! isident (o.buf[i]))
	{
	  err ("M7300", "%s: not a valid macro name \"%s\"",
	      input.where (), o.buf.c_str ());
	  return 2;
	}
    if (mad->flags & MADF_UINT)	// Enforce MADF_UINT (unsigned integer)
      for (size_t i = 0; i < o.buf.length (); i++)
	if (! isdigit (o.buf[i]))
	{
	  err ("M7310", "%s: not an unsigned integer \"%s\"",
	      input.where (), o.buf.c_str ());
	  return 2;
	}

    args.push_back (o.buf);
  }

  /* Got all the arguments we could. Enough ? Too many ? */
  if (args.size () < md->argmin)
  {
    err ("M7320", "%s: directive \"%s\" needs at least %hu argument(s)",
	input.where (), token, md->argmin);
    return 2;
  }
  if (hasparen != 0)
  {
    /* Skip any whitespace between the last argument and the ")". */
    while (isspace (input.peekc ()))
      c = input.getc ("refW", nest);

    if (input.peekc () == EOF)
    {
      err ("M7330", "%s(%lu,%lu): unterminated macro reference",
	  where_pathname, where_line, where_col);
      return 2;
    }
    if (hasparen == 1 && input.peekc () != syn_end1
     || hasparen == 2 && input.peekc () != syn_end2)
    {
      err ("M7340", "%s: directive \"%s\" takes at most %lu argument(s)",
	  input.where (), token, (unsigned long) md->argmax);
      return 2;
    }
    input.getc ("ref)", nest);
  }

  /* Expand the macro */
  if (md->flags & MDF_INTRINSIC)
  {
    if (md->func != NULL)
      if (md->func (ctx, input, output, args) != 0)
	return 2;
  }
  else
  {
    if (md->flags & MDF_RECURSE)
    {
      /* The user macro is evaluated in a different context where
	 $1, $2, etc. are the arguments. As a desirable side-effect,
	 any macros defined by the user macro will be local. */
      Context subctx (ctx);
      for (size_t a = 0; a < args.size (); a++)
      {
	char name[20];
	sprintf (name, "%lu", (unsigned long) a + 1);
	if (debug & DEBUG_SUBCTX)
	  err ("M7345", "subctx set %d, name %s, value \"%s\"",
	    (int) a, name, args[a].c_str ());
	subctx.setmacro (name, Macdef (mdflags_t (0), args[a], NULL));
      }

      Input_buf definition (md->value);
      if (expand_text (subctx, nest + 1, definition, output, scope_t (0))
	  != 0)
      {
	err ("M7350", "%s: could not expand macro %s",
	    input.where (), token);
	return 2;
      }
    }
    else
    {
      output.write (md->value.c_str (),
	  md->value.size (), 1);  // FIXME error checking
    }
  }

  return (flags & MRF_DNL) ? 1 : 0;
}


/*
 *	locate_file - resolve the argument of "source" et al.
 *
 *	base is the name of the current source file. arg is the argument
 *	of "source", i.e. the name of the file to locate. The resulting
 *	file name is stored in result.
 *
 *	If base is a null pointer, result = arg.
 *
 *	If arg is absolute (i.e. begins with a slash) or explicitly
 *	relative to the current working directory (i.e. the first
 *	component is "."), result = arg.
 *
 *	Otherwise, arg is considered to be relative to the path of base.
 *
 *	Returns 0 on success, non-zero on failure. Success does not
 *	imply that the file actually exists or is readable.
 */
static int locate_file (const char *base, const char *arg, std::string &result)
{
  if (base == NULL)
  {
    result = arg;
    goto byebye;
  }
  if (arg[0] == '/')
  {
    result = arg;
    goto byebye;
  }
  if (arg[0] == '.' && (arg[1] == '\0' || arg[1] == '/'))
  {
    result = arg;
    goto byebye;
  }
  {
    const char *lastslash = strrchr (base, '/');
    if (lastslash == NULL)	//     a.m7 + b.m7 = b.m7
      result = arg;
    else				// dir/a.m7 + b.m7 = dir/b.m7
    {
      result.assign (base, lastslash - base + 1);
      result += arg;
    }
  }

byebye:
  return 0;
}


/*
 *	dependency - declare a dependency
 *
 *	The first argument is the pathname of a file whose content
 *	may influence the execution. The second argument specifies
 *	whether the file may itself depend on other files.
 */
static void dependency (const char *pathname, bool recursive)
{
  if (mode != 'l')
    return;

  if (! recursive && ! datadeps)
    return;

  // FIXME quote any "$"s
  if (opathname == NULL)
    puts (pathname);
  else
    printf ("%s: %s\n", opathname, pathname);
}


/*
 *	mac_ - implementation of the null macro
 *
 *	This macro accepts any number of arguments, does nothing with
 *	them and expands to an empty string. It's used for references to
 *	undefined macros.
 */
static int mac_ (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  (void) input;
  (void) output;
  (void) args;
  return 0;
}


#if 0
/*
 *	mac__hash - implementation of the "#" intrinsic
 */
static int mac__hash (Input &input, Output &output,
    std::vector<std::string> &args)
{
  // Skip until ")" or EOF
  for (;;)
  {
    int c = input.getc ("#");
    if (c == EOF)
    {
      err ("M7360", "%s: unexpected EOF in macro argument", input.where ());
      return 1;
    }
    if (c == syn_end)
      break;
  }

  return 0;
}
#endif


/*
 *	mac_blob - implementation of the "blob" intrinsic
 */
static int mac_blob (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  int rc = 0;

  std::string pathname;
  if (locate_file (input.pathname (), args[0].c_str (), pathname) != 0)
  {
    err ("M7370", "%s: cannot locate \"%s\"", input.where (), args[0].c_str ());
    return 1;
  }

  dependency (pathname.c_str (), false);

  FILE *fp = fopen (pathname.c_str (), openmoder);
  if (fp == NULL)
  {
    err ("M7380", "%s: %s: %s",
	input.where (), pathname.c_str (), strerror (errno));
    return 1;
  }

  char buf[4096];
  for (;;)
  {
    size_t nbytes = fread (buf, 1, sizeof buf, fp);
    if (nbytes == 0)
    {
      if (ferror (fp))
      {
	err ("M7390", "%s: read error", pathname.c_str ());
	rc = 1;
      }
      break;
    }
    if (output.write (buf, 1, nbytes) != nbytes)
    {
      err ("M7400", "%s: write error", output.pathname ());
      rc = 1;
      break;
    }
  }

  fclose (fp);
  return rc;
}


/*
 *	mac_date - implementation of the date intrinsic
 */
static int mac_date (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  const char *format;

  if (args.empty ())
    format = default_time_format;
  else
    format = args[0].c_str ();

  time_t t;
  time (&t);
  struct tm *tm;
  if (true)
    tm = gmtime (&t);
  else
    tm = localtime (&t);
  const size_t bufsz = 1025;
  char *buf = new char[bufsz];
  memset (buf, '\0', bufsz);
  strftime (buf, bufsz, format, tm);
  buf[bufsz - 1] = '\0';
  output.puts (buf);
  delete[] buf;
  return 0;
}


/*
 *	mac_define - implementation of the "define" intrinsic
 */
static int mac_define (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  if (args.size () < 3 || args.size () > 4)
  {
    err ("M7410", "%s: define: need three or four arguments", input.where ());
    return 1;
  }

  // Don't let users overwrite built-ins
  const Macdef *md;
  md = ctx.getmacro (args[0]);
  if (md != NULL && (md->flags & MDF_INTRINSIC))
  {
    err ("M7420", "%s: define: \"%s\" is an intrinsic",
	input.where (), args[0].c_str ());
    return 1;
  }

  int min = atoi (args[1].c_str ());
  int max = atoi (args[2].c_str ());
  if (debug & DEBUG_DEFINE)
  {
    const char *definition = "";
    if (args.size () >= 4)
      definition = args[3].c_str ();
    err ("M7430", "define \"%s\", argmin %d, argmax %d, definition \"%s\"",
	args[0].c_str(), min, max, definition);
  }
  std::vector<Macargdef> argdefs (max, Macargdef (madflags_t (0)));
  if (args.size () == 4)
    ctx.setmacro (args[0],
	Macdef (mdflags_t (MDF_RECURSE), args[3], min, max, argdefs));
  else
    ctx.setmacro (args[0],
	Macdef (mdflags_t (MDF_RECURSE), std::string (), min, max, argdefs));
  return 0;
}


/*
 *	mac_echo - implementation of the "echo" intrinsic
 */
static int mac_echo (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  if (! args.empty ())
    output.write (args[0].c_str (), args[0].size (), 1);

  return 0;
}


/*
 *	mac_file - implementation of the "file" intrinsic
 *
 *	This function differs from mac_blob() in that it strips any
 *	trailing newlines.
 */
static int mac_file (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  int rc = 0;

  std::string pathname;
  if (locate_file (input.pathname (), args[0].c_str (), pathname) != 0)
  {
    err ("M7440", "%s: cannot locate \"%s\"", input.where (), args[0].c_str ());
    return 1;
  }

  dependency (pathname.c_str (), false);

  FILE *fp = fopen (pathname.c_str (), openmoder);
  if (fp == NULL)
  {
    err ("M7450", "%s: %s: %s",
	input.where (), pathname.c_str (), strerror (errno));
    return 1;
  }

  char buf[4096];
  size_t pending_newlines = 0;
  for (;;)
  {
    size_t nbytes = fread (buf, 1, sizeof buf, fp);
    const char *p;
    for (p = buf + nbytes - 1; p >= buf && *p == '\n'; p--)
      ;
    size_t head = p + 1 - buf;
    size_t tail = nbytes - head;
    if (head != 0 && pending_newlines != 0)
    {
      static const char newlines[128] =
      {
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
	'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
      };
      while (pending_newlines != 0)
      {
	size_t chars = pending_newlines;
	if (chars > sizeof newlines)
	  chars = sizeof newlines;
	if (output.write (newlines, chars, 1) != 1)
	{
	  err ("M7460", "%s: write error", output.pathname ());
	  rc = 1;
	  break;
	}
	pending_newlines -= chars;
      }
    }
    output.write (buf, 1, head);		// FIXME error checking
    pending_newlines += tail;
    if (nbytes != sizeof buf)
    {
      if (ferror (fp))
      {
	err ("M7470", "%s: read error", pathname.c_str ());
	rc = 1;
      }
      break;
    }
  }

  fclose (fp);
  return rc;
}


/*
 *	mac_mtime - implementation of the "mtime" intrinsic
 */
static int mac_mtime (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  const char *format;

  if (args.empty ())
    format = default_time_format;
  else
    format = args[0].c_str ();

  time_t t;
  if (input.stat () == NULL)
    time (&t);
  else
    t = input.stat ()->st_mtime;
  struct tm *tm;
  if (true)
    tm = gmtime (&t);
  else
    tm = localtime (&t);
  const size_t bufsz = 1025;
  char *buf = new char[bufsz];
  memset (buf, '\0', bufsz);
  strftime (buf, bufsz, format, tm);
  buf[bufsz - 1] = '\0';
  output.puts (buf);
  delete[] buf;
  return 0;
}


/*
 *	mac_set - implementation of the "set" intrinsic
 */
static int mac_set (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  if (args.size () < 1 || args.size () > 2)
  {
    err ("M7480", "%s: set: need one or two arguments", input.where ());
    return 1;
  }

  // Don't let users overwrite built-ins
  const Macdef *md;
  md = ctx.getmacro (args[0]);
  if (md && NULL && (md->flags & MDF_INTRINSIC))
  {
    err ("M7490", "%s: set: \"%s\" is an intrinsic",
	input.where (), args[0].c_str ());
    return 1;
  }

  if (args.size () == 2)
    ctx.setmacro (args[0], Macdef (mdflags_t (0), args[1], 0, 0, NULL));
  else
    ctx.setmacro (args[0], Macdef (mdflags_t (0), std::string (), 0, 0, NULL));
  return 0;
}


/*
 *	mac_source - implementation of the "source" intrinsic
 */
static int mac_source (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  std::string pathname;
  if (locate_file (input.pathname (), args[0].c_str (), pathname) != 0)
  {
    err ("M7500", "%s: cannot locate \"%s\"", input.where (), args[0].c_str ());
    return 1;
  }

  dependency (pathname.c_str (), true);

  FILE *fp = fopen (pathname.c_str (), openmoder);
  if (fp == NULL)
  {
    err ("M7510", "%s: %s: %s",
	input.where (), pathname.c_str (), strerror (errno));
    return 1;
  }

  Input_file input_inc (pathname.c_str (), NULL, fp);
  int rc = 0;
  if (expand_text (ctx, 0/*FIXME*/, input_inc, output, scope_t (0)) != 0)
  {
    err ("M7520", "%s: %s: expand_text error",
	input.where (), pathname.c_str ());
    rc = 1;
  }

  fclose (fp);
  return rc;
}


/*
 *	mac_where - implementation of the "where" intrinsic
 */
static int mac_where (Context &ctx, Input &input, Output &output,
    std::vector<std::string> &args)
{
  output.puts (input.where ());
  return 0;
}


/*
 *	err - print an error message
 */
void err (const char *code, const char *fmt, ...)
{
  va_list argp;

  if (fileno (stdout) == fileno (stderr))
    fflush (stdout);			// FIXME report errors

  fputs ("m7: ", stderr);
  fputs (code, stderr);
  fputc (' ', stderr);
  va_start (argp, fmt);
  vfprintf (stderr, fmt, argp);
  va_end (argp);
  fputc ('\n', stderr);
}

