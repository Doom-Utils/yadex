<html>
<head>
<title>Yadex hacker's guide</title>
<style type="text/css">
body {
        color: black;
        background-color: rgb(176,144,104);
}

kbd {
        color: black;
        background-color: rgb(224,192,128);
}

code {
	background-color: rgb(192,160,120);
}

h1, h2, h3, h4 {
        font-family: helvetica, arial, sans-serif;
}

h1 {
        align: center;
}

/* Apparently, Mozilla doesn't support it. */
dl dt {
        font-weight: bold;
}
</style>
</head>
<body>
	<h1>Yadex hacker's guide</h1>

		<h2>Blah</h2>

			<h3>Foreword</h3>

This documents is aimed at people
who want to hack Yadex.
It is very incomplete,
partly due to lack of time,
partly because as some subsystems are going to be rewritten,
I'd rather not spend too much time documenting them.
But if you're interested in a particular area
of Yadex's bowels that does not appear here,
don't hesitate to let me know.
<p>
I apologize for the poor quality of Yadex's code
but it seemed to me it was better to release
something imperfect now
than something clean two years from now.
If you want to improve it, be my guest.

			<h3>Introduction</h3>

Yadex is written in a mixture of C and C++.
The Unix version interfaces with X through Xlib directly ;
it uses no toolkit.
The DOS version uses BGI (Borland Graphics Interface),
a rather low-level API to set the video mode, draw lines, text, etc.

			<h3>Original platform</h3>

The Unix version has been developped with
GCC 2.7.2 and XFree 3.3
on a PC K6/200 with Linux 2.0.29, 2.0.30 and 2.0.35.
<p>
The DOS version has been developped with
Borland C++ 4.0
on a PC 486 DX4/75 with MS-DOS 6.22.
<p>
Yadex should be compilable on all reasonable
Unix-and-X11 platforms provided that 
<ul>
<li>you have GNU make,
<li>you either have GCC or can hack the makefile
to change the GCC-specific options ($c -W*),
<li>$c short is 16-bit long,
<li>$c long is 32-bit long.
</ul>
To compile on platforms where $c short
or $c long don't have the needed size,
just change the definitions of $c u16,
$c i16, $c u32 and $c i32
in $c yadex.h.

			<h3>Historic background</h3>

Yadex descends from DEU 5.21.
<p>
DEU 5.21 was written by Raphaël Quinet, Brendon Wyber and others
and released on 1994-05-21.
As you probably already know,
DEU was a real-mode DOS program,
in C, compiled with Borland C++ 4.0 (I think)
and using BGI for its output.
<p>
In the mists of time (that is probably 1996),
I began to hack DEU for my own use.
In 1997, other people began to use my hack
and I gave it a name : "Yade"
(which meant Yet Another Doom Editor).
It was still a real-mode DOS program.
<p>
In june 1998, tired of rebooting to DOS
every time I wanted to do some Doom level editing,
I started porting Yade to Linux.
As there already was a Unix program called "Yade"
(Yet Another Diagram Editor),
I changed the name of my baby to "Yadex".
At the same time, I began to use C++ in places
so that's why Yadex is such an ugly mixture of languages.

		<h2>The programming environment</h2>

			<h3>Memory allocation</h3>

You're not supposed to use $c malloc() and $c free()
but $c GetMemory(), $c FreeMemory(),
$c GetFarMemory() and $c FreeFarMemory() instead.
Why ?
<p>
$c GetMemory() and friends manage more things for you.
They include an anti-fragmentation system,
they try to swap things out when memory is tight
(this is an only an issue for the 16-bit DOS version)
and if they fail, they call $c fatal_error()
so you don't need to check their return value.
<p>
The reason for $c GetFarMemory()
is that, for the 16-bit DOS version,
it can allocate more than 64 kB ($c GetMemory() cannot).
I must say that I don't use $c GetFarMemory() a lot myself
because I don't like the idea of having to use two different
memory allocation routines depending on the size I expect to allocate.
I modified $c GetMemory()
so that it accepts an <code>unsigned long</code>
but checks that the passed value fits in $c size_t.
In other words, if you call $c GetMemory()
with a size of 65,536 the 16-bit DOS version
will trigger a fatal error immediately
instead of silently allocating 1 byte
and letting you search afterwards why the program behaves strangely.
A better fix would be to make $c GetMemory()
call $c GetFarMemory() when the block is too large
for $c malloc().
Any volunteers ?
<p>
Memory allocated with $c GetMemory() is guaranteed
to be freeable with $c free().
On the other hand, memory allocated with $c GetFarMemory()
must be freeed with $c FreeFarMemory().

			<h3><a name="endianness">Endianness</a></h3>

The 16-bit and 32-bit integers in a wad file
are always little-endian, whatever the platform.
<p>
On the other hand, Yadex keeps all its in-core
integer in the platform's native endianness,
i.e. in little-endian format on little-endian machines
and in big-endian format on big-endian machines.
<p>
The wad endianness &lt;-&gt; native endianness conversion
is done automagically by $c wad_read_i16() and $c wad_read_i32().
<p>
<u>To maintain compatibility with big-endian platforms, all I/O of
multibyte integers should be done with those functions.</u>

		<h2>The main directory</h2>

TBD

		<h2>The wad data</h2>

TBD

		<h2>The level data</h2>

			<h3>Structure</h3>

The data for a level is stored in 10 variables
that are declared in <code>levels.h</code> and
defined in <code>levels.cc</code>. Here they are :
<pre>int   NumThings;         /* number of things */
TPtr  Things;            /* things data */
int   NumLineDefs;       /* number of linedefs */
LDPtr LineDefs;          /* linedefs data */
int   NumSideDefs;       /* number of sidedefs */
SDPtr SideDefs;          /* sidedefs data */
int   NumVertices;       /* number of vertices */
VPtr  Vertices;          /* vertices data */
int   NumSectors;        /* number of sectors */
SPtr  Sectors;           /* sectors data */</pre>

			<h3>Scope and lifetime</h3>

Since those variables (and other critical ones)
are unfortunately static, it's not possible to
open editing windows on several different levels
simultaneously.
This should be fixed in the future by making the
level data a class and turning those variables
into members of that class.
<p>
I think that the level data class should be separate
from the editing window class because it might be
useful to open several editing windows on the same
level.
Separate class should also make the design of the read
level and write level routines cleaner and simpler.

			<h3>Maintenance</h3>

It's of paramount importance for the stability and
reliability of Yadex that the level data be maintained
in a consistent state at all times. In particular,
<ul>
<li>the $c Num* variables must remain accurate,
<li>vertex references in linedefs must be either
	$c OBJ_NO_NONE or the number of an existing vertex,
<li>sidedef references in linedefs must be either
	$c OBJ_NO_NONE or the number of an existing sidedef,
<li>sector references in sidedefs must be either
	$c OBJ_NO_NONE or the number of an existing sector.
</ul>

			<h3>Loading</h3>

The SEGS, SSECTORS, NODES, BLOCKMAP and REJECT lumps are
ignored. The other lumps are read into the level data
variables with a special case for VERTEXES ; vertices
that are not used by any linedef are ignored (such vertices
are believed to come from the nodes builder and therefore
be irrelevant to level editing). The linedefs vertices
references are updated if necessary.
<p>
Since the endianness of the wad files is fixed (little
endian) and thus not necessarily identical to the endianness
of the CPU, reading 2- and 4-byte integers from the file is
done through special endianness-independant routines.

			<h3>Saving</h3>

If $c MadeMapChanges is false, the SEGS, SSECTORS, NODES,
BLOCKMAP, REJECT and VERTEXES lumps are copied from the
original file. Else, they are output with a length of
zero bytes, except the VERTEXES lump that is created
from the the level data ($c NumVertices and $c Vertices).
<p>
Since the endianness of the wad files is fixed (little
endian) and thus not necessarily identical to the endianness
of the CPU, writing 2- and 4-byte integers to the file is
done through special endianness-independant routines.

		<h2>Editing windows, or the lack of it</h2>

Too many global variables...
<p>
See "<code>_edit.h</code>".

		<h2>The editor loop</h2>

All the time the user spends editing a level is
spent within a certain function, the <dfn>editor loop</dfn>,
a.k.a. $c EditorLoop() in $c editloop.c.
It's essential for you to understand it
if you want to get how Yadex works right.
<p>
The $c EditorLoop() is an endless loop (okay, not
<em>really</em> endless) which, for each iteration,
first, refreshes the display,
second, waits for an event,
third, processes that event.
I could have put things in a different order
but I liked the idea of displaying something
<em>before</em> waiting for user input.
<p>
Because the event input and the graphical output
are complex and not-quite-synchronous processes,
I've tried to separate them.
$c EditorLoop() gets input events and processes them
and calls another function, $c edisplay_c::refresh(),
to take care of everything display related.
If you replaced $c edisplay_c::refresh() by a stub
(and did the same with a couple of functions in $c gfx.c
and $c input.c), you could perfectly well, if blindly,
run Yadex without a connection to an X server.
While you may object that this would be a pointless exercise
(to which I agree), it still proves the modularity of the design.
<p>
The $c edisplay_c::refresh() function
is also a very important one to understand,
at least if you work on graphical output.
It is discussed in another section but,
just to settle ideas, I thought I'd give you
here a bird's eye view of the whole thing.
If there is a single paragraph in this document
that you need to read, it's probably this one :
<ul>
<li>$c EditorLoop()
	<ul>
	<li>Call $c edisplay_c::refresh(),
		<ul>
		<li>Do some basic geometry management,
		<li>setup widgets according to editing session,
		<li>call the $c need_to_clear() method of the widgets
			to determine whether we need to redisplay
			everything from scratch,
		<li>if so, call the $c clear() method of the widgets
			and call $c ClearScreen() to clear the pixmap
			(if available) and make
			sure all subsequent graphical output will be sent
			to the pixmap (if available),
		<li>else, make sure all subsequent graphical ouput will
			be sent to the window and call the $c undraw()
			method of the widgets from the top down,
		<li>call the $c draw() method of the widgets from the bottom up,
		<li>call $c update_display() to refresh the physical display
			(if we're using the window, it's a no-op,
			if we're using the pixmap, call $c XCopyArea()).
		</ul>
	<li>call $c get_input_status() to get the next event,
	<li>process that event.
	</ul>
</ul>

Note that all graphical output is done from within $c edisplay_c::refresh().

		<h2>The display</h2>

			<h3>Logical and physical display : widgets</h3>

The display can be seen at two levels ;
the logical level and the physical level.
The physical level is just a rectangular
matrix of pixels. It's the contents of
the window/screen.
The logical level is more like "oh, there's
a window displayed at those coordinates".
<p>
There's obviously more to say on this...

			<h3>The pixmap</h3>

To further complicate matters,
there are two physical displays :
a window and a pixmap.
The role of the pixmap is to help avoid flicker.
Here's how it works :
<p>
As long as we do incremental changes to the display
(E.G. "undisplaying" the highlight on a vertex
or redisplaying the pointer coordinates),
we do it directly on the window.
<p>
But, if we have to redraw everything from scratch,
we have to clear the window first which generates
an annoying "flashing" of the screen.
To avoid this, we instead clear a pixmap,
do our display thing on it and then
put the pixmap onto the window, with $c XCopyArea().
The result is a flicker-less refresh.
<p>
The graphical routines from $c gfx.c
switch automatically to the pixmap if
$c ClearScreen() was called.
Thanks to this, that window vs. pixmap thing
is nearly transparent to the application functions.
$c edisplay_c::refresh() just forces
widgets that can undraw themselves to use the
window, not the pixmap.
<p>
But a pixmap is large.
For a 800x600 window in 64k colours, 937 kB.
And copying it to the window is obviously long.
So, on machines with little memory or a slow CPU,
the user might prefer to do without it.
That's what $c no_pixmap is for.

		<h2>The selection</h2>

			<h3>Introduction</h3>

From the user's point of view, the selection is a "list" of
objects.  I use the term "list" instead of "collection" because,
for certain operations, the order in which objects were added to
the selection is significant.
<p>
From the programmer's point of view, the selection is a
singly linked list of objects of this type :
<p>
<pre>typedef struct SelectionList *SelPtr;
struct SelectionList
   {
   SelPtr next;                 /* next in list */
   int objnum;                  /* object number */
   };</pre>
<p>
Note that the $c SelectionList structure
has no $c objtype field ;
the type of the object (THING, vertex...)
is implicit from the current mode
(the $c obj_type field from the $c edit_t
structure).
As a consequence, the selection cannot contain
objects of different types.
<p>
The selection manipulation functions are supposed to be
defined in $c selectn.c and declared in
$c selectn.h. Here they are :

<dl>
<dt><code>void SelectObject (SelPtr *s, int n)</code>
<dd>Adds object $c n at the beginning of list $c *s.
$c *s can be $c NULL ; it means the list is empty.
Warning : does not check that object $c n is not already
in the list.

<dt><code>void UnSelectObject (SelPtr *s, int n)</code>
<dd>Removes from list $c *s all occurences of
object $c n.
If all objects are removed, sets $c *s to $c NULL.

<dt><code>void select_unselect_obj (SelPtr *s, int n)</code>
<dd>If the object $c n is already in the list $c *n, remove it.
If it's not, insert it at the beginning.
$c *s can be $c NULL ; it means the list is empty.

<dt><code>Bool IsSelected (SelPtr s, int n)</code>
<dd>Tells whether object $c n is in selection $c s.
$c s can be $c NULL ; it means the list is empty.

<dt><code>void ForgetSelection (SelPtr *s)</code>
<dd>Frees all memory allocated to list $c *s
and sets $c *s to $c NULL.

<dt><code>void DumpSelection (SelPtr s)</code>
<dd>Debugging function ; prints the contents of the
selection to $c stdout.
</dl>

Note that there is not selection iteration function.
Indeed, iterating through a selection is always done
by the application functions themselves, usually with
something like :
<p>
<pre>SelPtr cur;
for (cur = list; cur; cur = cur-&gt;next)
   do_something_with_object (cur-&gt;objnum);
</pre>

			<h3>Selecting in/out</h3>

When you draw a box with [<kbd>Ctrl</kbd>] depressed, the objects in
the box are added to the selection.
However, if some of those objects were already selected,
they are unselected.
So $c SelectObjectsInBox() cannot just add all
the objects in the box to the list or we would end up with
multiply selected objects. Wouldn't do us much good when
displaying the selection or dragging objects.
<p>
That's when $c select_unselect_obj() is used.

		<h2>The highlight</h2>

TBD

		<h2>Colours</h2>

The colour management system in very complex.
There are lots of things to say on that topic.
However, for most uses, you need to know only three functions :
<dl>
<dt>$c set_colour()
<dd>Set the current colour to a new value.
<dt>$c push_colour()
<dd>Save the current colour on the colour stack
and set the current colour to a new value.
<dt>$c pop_colour()
<dd>Set the current colour to the value
it had at the moment of the last call to $c push_colour().
</dl>

		<h2>Menus and pop-up windows</h2>

TBD

		<h2>Compile-time variables (defines)</h2>

<dl>
<dt>$c AYM_MOUSE_HACKS
<dd>Some experimental code by me to try to understand why,
under DOS, the mouse pointer moves 8 pixels at a time
(seems to depend on the mouse driver ?).
<p>
<dt>$c CIRRUS_PATCH
<dd>Dates back to DEU 5.21. Apparently, some code specific to
Cirrus VGA boards.
Does nothing unless $c Y_BGI is defined.
<p>
<dt>$c DEBUG
<dd>The obvious.
<p>
<dt>$c DIALOG
<dd>Experimental code by me to test the dialog box function
that Jim Flynn wrote for Deth in the beginning of 1998.
<p>
<dt>$c NO_CIRCLES
<dd>If your BGI driver does not support drawing circles,
define this and Yadex will draw squares instead.
<p>
<dt>$c OLD
<dd>Misc. obsolete stuff I didn't want to delete at the time.
Never define it or you'll break Yadex !
Code under <code>#ifdef OLD</code> should probably be removed.
<p>
<dt>$c OLD_METHOD
<dd>My cruft. Code thus <code>#ifdef</code>'d should probably be removed.
<p>
<dt>$c OLD_MESSAGE
<dd>My cruft. Code thus <code>#ifdef</code>'d should probably be removed.
<p>
<dt>$c ROUND_THINGS
<dd>Draw THINGS as circles (like DEU did), not as squares.
<p>
<dt>$c SWAP_TO_XMS
<dd>Comes from DEU : related to code supposed to use XMS as
"swap space". Apparently, was never used ?
<p>
<dt>$c Y_BGI
<dd>Use BGI for graphics output and BIOS for keyboard and mouse
input. Makes senses only for DOS + Borland C++ 4.0.
Exactly one of ($c Y_BGI, $c Y_X11) must be defined.
<p>
<dt>$c Y_DOS
<dd>Compile for DOS (with Borland C++ 4.0). Allows, among others,
the $c huge and $c far pointer modifiers.
Exactly one of ($c Y_DOS, $c Y_UNIX) must be defined.
<p>
<dt>$c Y_UNIX
<dd>Compile for Unix (with GCC 2.7.2). Causes, among other
things, "huge" and "far" to be <code>#define</code>'d to "".
Exactly one of ($c Y_DOS, $c Y_UNIX) must be defined.
<p>
<dt>$c Y_X11
<dd>Use X11 (Xlib) for graphics output, keyboard and mouse
input and other events.
Exactly one of ($c Y_BGI, $c Y_X11) must be defined.
</dl>

		<h2>Coding standards</h2>

Warning : this section was written when Yadex was still plain C.
Some of it may be inadequate or incomplete with C++.

			<h3>Indent style</h3>

I use the Whitesmiths style with an indent width of 3.
<ul>
<li>tab stops every 8 characters,
<li>indent width is 3 spaces,
<li>line width limited to 80 characters,
<li>the braces have the same indentation as the text they contain,
<li>one space between the name of the function/statement and the "(",
<li>no space between the "(" and the first argument,
<li>no space between the end of the argument and the "," or ";",
<li>one space between the "," or ";" and the next argument,
<li>no space between the end of the argument and the ")",
<li>no space between the ")" and the ";",
<li>the body of a function is not indented.
</ul>
<p>
Example :
<p>
<pre>static const char *foo (int n)
{
for (stuff; stuff; stuff)
   {
   if (thingie || gadget ())
      call_this_one (with, three, exactly[arguments]);
   else
      dont ();
   call_that_one ();
   }
return NULL;
}</pre>

			<h3>Identifiers</h3>

For variables and functions (and certain macros),
I use all-lower-case identifiers
with underscores where it seems appropriate.
For enums and most macros,
I use all-upper-case identifiers.
<p>
I consistently use the English spelling (E.G. "colour", not "color").

			<h3>General style</h3>

My general style is to try to make it look clear and pretty. If there
are several similar consecutive statements, I try to align what I can.
<p>
<pre>/* This is a long comment. A long comment is a comment
   that spans over several lines. See how I "typeset" it. */
func         (object, mutter    );
another_func (object, mumble, 46);
yet_another  (object, grunt     );  // Short comment.</pre>

			<h3>Code</h3>

Good code ;-).
<p>
<ul>
<li>Use $c const for all arguments passed by reference
unless they're modified.
<li>Use $c static for all functions and variables
unless they're extern.
<li>Be careful with $c int,
on some platforms, it's equivalent to $c short,
on others, it's equivalent to $c long.
<li>Whenever you need a fixed number of bytes,
use $c u16, $c i32, etc.
<li>Use prototypes.
<li>Prototype functions that take no args with "<code>foo (void);</code>",
 not "<code>foo ();</code>" (author's note : this is C-ish !).
<li>Try to avoid buffer overruns
by using $c snprintf() instead of $c sprintf(),
$c al_scps() instead of $c strcpy(),
$c al_saps() instead of $c strcat(), etc.
</ul>


			<h3>Includes</h3>

Put $c yadex.h first,
then any standard headers needed (E.G. $c math.h)
then all the Yadex headers needed by alphabetical order.
<p>
If the need arises to protect a Yadex header against multiple inclusion,
use this :
<p>
<pre>#ifndef YH_FOO
#define YH_FOO
(put the contents of the header here)
#endif</pre>

			<h3>File format</h3>

I use the standard Unix text file format (lines separated by LF)
with the character set ISO 8895-1 a.k.a. Latin-1
and a tab width of 8.

		<h2>Notes</h2>

Here is the text of the notes in the source code.
<p>
<dl>
<dt>1
<dd>It's important to set the $c line_width to 1
and not 0 for GCs that have the $c GXxor function,
for the following reason.
<p>
Those GCs are used for objects that should be "undrawn" by
drawing them again, E.G. the highlight.
Now, imagine the following scenario :
you highlight a linedef and then press, say, page up to
make the window scroll. This is not an incremental change
to the display so everything is redrawn from scratch onto
the pixmap. The pixmap is <code>XCopyArea()</code>'d onto the window,
the linedef still highlighted. Then, Yadex realizes that
the map coordinates of the pointer have changed so the
linedef is not highlighted anymore. It dutifully unhighlights
the linedef. But $c XDrawLine() on a window does not use the
same algorithm as on a pixmap (attempts to use the blitter
on the video card). So the first line and the second don't
coincide exactly and the result is a trail of yellow pixels
where the highlight used to be.
<p>
That's why I use a $c line_width
sure that the same algorithm is used both for pixmap output
and for window output.

</dl>

</body>
</html>

