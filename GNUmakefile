#
#	Makefile for Yadex
#	Copyright © André Majorel 1998-2000.
#	AYM 1998-06-10
#

# This version of Yadex is known to work on a Debian 2.0 Linux
# distribution (80x86, Linux 2.0.34, EGCS 1.0.3, glibc 2.0.7,
# XFree86 3.3.2 (= X11R6.3), GNU make 3.76 and 3.77). It might
# need modifications to work on other platforms.

# ATTENTION : GNU MAKE IS REQUIRED ! This makefile uses pattern
# rules, addprefix, addsuffix, etc. It's not named "GNUmakefile"
# for nothing.

# Don't change these lines.
DEFINES =
VERSION := $(shell cat VERSION)
VERPREV := $(shell test -f VERSION~ && cat VERSION~)

########################################################################
#
#	Definitions that end users
#	might want to change
#
########################################################################

# Where you want "make install" to put things.
# Typical values : "/usr", "/usr/local" and "/opt".
PREFIX = /usr/local

# Which OS ?
OS := $(shell uname -s | tr A-Z a-z)

# Can we use GCC to compile BSP ?
HAVE_GCC := $(shell if gcc --version; then echo OK; fi)

# Is that /usr/man or /usr/share/man ?
FHS_MAN := $(shell if [ -d "$(PREFIX)/share/man" ]; then echo OK; fi)

# Does your system have gettimeofday() ?
# Current rule: all systems have it.
HAVE_GETTIMEOFDAY = 1

# Does your system have nanosleep() ?
# Current rule: only Linux has it.
ifneq (,$(findstring $(OS), linux))
  HAVE_NANOSLEEP = 1
endif

# Does your system have snprintf() ?
# Current rule: only Linux has it.
ifneq (,$(findstring $(OS), linux))
  HAVE_SNPRINTF = 1
endif

# Does your system have usleep() ?
# Current rule: all unices have it.
ifdef OS
  HAVE_USLEEP = 1
endif

# Where your X11 libraries and headers reside.
# Current rule:
# - AIX has them in /usr/lpp/X11/{lib,include},
# - Solaris has them in /usr/openwin/{lib,include},
# - all other unices in /usr/X11R6/{lib,include}.
ifeq ($(findstring $(OS), aix), $(OS))
  X11LIBDIR     = /usr/lpp/X11/lib
  X11INCLUDEDIR = /usr/lpp/X11/include
else
  ifeq ($(findstring $(OS), solaris sunos), $(OS))
    X11LIBDIR     = /usr/openwin/lib
    X11INCLUDEDIR = /usr/openwin/include
  else
    X11LIBDIR     = /usr/X11R6/lib
    X11INCLUDEDIR = /usr/X11R6/include
  endif
endif

# Your C and C++ compilers.
CC = cc
CXX = c++

#
#	Definitions that only hackers
#	might want to change
#

# All the modules of Yadex without path or extension.
MODULES_YADEX =\
	acolours	aym		bitvec		cfgfile\
	checks		clearimg	colour1		colour2\
	colour3		colour4		dialog		dispimg\
	disppic		drawmap		edisplay	editgrid\
	editlev		editloop	editobj		editsave\
	endian		editzoom	entry		entry2\
	events		flats		game		gcolour1\
	gcolour2	geom		gfx		gfx2\
	gfx3		gotoobj		help1		help2\
	highlt		infobar		input		l_align\
	l_centre	l_flags		l_misc		l_prop\
	l_unlink	l_vertices	levels		lists\
	macro		memory		menubar		menu\
	mkpalette	mouse		names		nop\
	objects		objinfo		oldmenus	palview\
	patchdir	pic2img		prefer		s_centre\
	s_door		s_lift		s_linedefs	s_merge\
	s_misc		s_prop		s_split		s_vertices\
	sanity		savepic		scrnshot	selbox\
	selectn		selpath		selrect		spectimg\
	sprites		swapmem		t_centre	t_flags\
	t_prop		t_spin		textures	things\
	trace		v_centre	v_merge		v_polyg\
	verbmsg		version		wads		wads2\
	warn		x_centre	x_exchng	x_mirror\
	x_rotate	x11		xref		yadex\
	ytime

# All the modules of Atclib without path or extension.
MODULES_ATCLIB =\
	al_adigits	al_aerrno	al_astrerror	al_fana\
	al_fnature	al_lateol	al_lcount	al_lcreate\
	al_ldelete	al_ldiscard	al_lgetpos	al_linsert\
	al_linsertl	al_llength	al_lpeek	al_lpeekl\
	al_lpoke	al_lpokel	al_lptr		al_lread\
	al_lreadl	al_lrewind	al_lseek	al_lsetpos\
	al_lstep	al_ltell	al_lwrite	al_lwritel\
	al_sapc		al_saps		al_scps		al_scpslower\
	al_sdup		al_sisnum	al_strolc

# All the modules of BSP without path or extension.
MODULES_BSP =\
	bsp		funcs		makenode	picknode

# The source files of Yadex.
SRC_YADEX = $(addprefix src/, $(addsuffix .cc, $(MODULES_YADEX)))

# The source files of Atclib.
SRC_ATCLIB = $(addprefix atclib/, $(addsuffix .c, $(MODULES_ATCLIB)))

# The source files of BSP.
SRC_BSP = $(addprefix bsp-2.3/, $(addsuffix .c, $(MODULES_BSP)))

# The headers of Yadex.
HEADERS_YADEX := $(wildcard src/*.h)

# The headers of Atclib.
HEADERS_ATCLIB = atclib/atclib.h

# The headers of BSP.
HEADERS_BSP = bsp-2.3/bsp.h bsp-2.3/structs.h

# All the source files of both, including the headers.
SRC = $(SRC_YADEX)  $(HEADERS_YADEX)\
      $(SRC_ATCLIB) $(HEADERS_ATCLIB)\
      $(SRC_BSP)    $(HEADERS_BSP)

# The files on which youngest is run.
SRC_NON_GEN = $(filter-out src/version.cc, $(SRC))

# Options used when compiling Atclib.
CFLAGS = -O

# Options used when compiling and linking Yadex.
# ld is invoked through the C++ compiler
# so LDFLAGS should not contain options that mean
# something to the C++ compiler.
CXXFLAGS = -O -I$(X11INCLUDEDIR)
LDFLAGS  =

# Options used to compile and link the debugging
# targets. Not used by normal end-user targets.
# Unlike CFLAGS, CXXFLAGS and LDFLAGS, assume
# GCC/EGCS.
DCFLAGS = -O -g -pedantic -Wall -Wno-parentheses -Wpointer-arith\
		-Wcast-qual -Wcast-align -Wwrite-strings\
		-Wmissing-declarations -Wmissing-prototypes -Winline
DCXXFLAGS = -O -I$(X11INCLUDEDIR)\
		-g -pedantic -Wall -Wno-parentheses -Wpointer-arith\
		-Wcast-qual -Wcast-align -Wwrite-strings\
		-Wmissing-declarations -Wmissing-prototypes -Winline
DLDFLAGS =

# Defines used when compiling Yadex.
# If you change Y_UNIX for Y_DOS or Y_X11 for Y_BGI,
# don't even expect it to compile. As of this release,
# the DOS/BGI version is _broken_.
DEFINES += -DY_UNIX -DY_X11
#DEFINES += -DY_ALPHA
#DEFINES += -DY_BETA
ifdef HAVE_GETTIMEOFDAY
  DEFINES += -DY_GETTIMEOFDAY
endif
ifdef HAVE_NANOSLEEP
  DEFINES += -DY_NANOSLEEP
endif
ifdef HAVE_SNPRINTF
  DEFINES += -DY_SNPRINTF
endif
ifdef HAVE_USLEEP
  DEFINES += -DY_USLEEP
endif

# The name of the directory where objects and binaries
# are put. I include the output of "uname -a" to make
# it easier for me to build Yadex for different platforms
# from the same source tree.
OBJSUBDIR          := $(shell uname -a | tr -c "[:alnum:]._-" "[_*]")
OBJDIR             = obj/0
DOBJDIR            = dobj/0
OBJPHYSDIR         = obj/$(OBJSUBDIR)
DOBJPHYSDIR        = dobj/$(OBJSUBDIR)
OBJDIR_ATCLIB      = $(OBJDIR)/atclib
DOBJDIR_ATCLIB     = $(DOBJDIR)/atclib
OBJPHYSDIR_ATCLIB  = $(OBJPHYSDIR)/atclib
DOBJPHYSDIR_ATCLIB = $(DOBJPHYSDIR)/atclib
OBJ_YADEX   = $(addprefix $(OBJDIR)/,  $(addsuffix .o, $(MODULES_YADEX)))
DOBJ_YADEX  = $(addprefix $(DOBJDIR)/, $(addsuffix .o, $(MODULES_YADEX)))
OBJ_ATCLIB  = $(addprefix $(OBJDIR_ATCLIB)/,  $(addsuffix .o, $(MODULES_ATCLIB)))
DOBJ_ATCLIB = $(addprefix $(DOBJDIR_ATCLIB)/, $(addsuffix .o, $(MODULES_ATCLIB)))

# The game definition files.
YGD = $(addprefix ygd/,\
	doom.ygd	doom02.ygd	doom04.ygd	doom05.ygd\
	doom2.ygd	doompr.ygd	heretic.ygd	hexen.ygd\
	strife.ygd	strife10.ygd)

# Files that are used with scripts/process to
# generate files that are included in the
# distribution archive.
DOC1_SRC =\
	docsrc/README\
	docsrc/README.doc\

# Files that are used with scripts/process to
# generate files that go in the doc/ directory
# and are NOT included in the archive.
DOC2_SRC_HTML =\
	docsrc/advanced.html\
	docsrc/contact.html\
	docsrc/credits.html\
	docsrc/deu_diffs.html\
	docsrc/editing_docs.html\
	docsrc/feedback.html\
	docsrc/getting_started.html\
	docsrc/hackers_guide.html\
	docsrc/help.html\
	docsrc/index.html\
	docsrc/keeping_up.html\
	docsrc/legal.html\
	docsrc/palette.html\
	docsrc/reporting.html\
	docsrc/tips.html\
	docsrc/trivia.html\
	docsrc/trouble.html\
	docsrc/users_guide.html\
	docsrc/wad_specs.html\
	docsrc/ygd.html\

DOC2_SRC_MISC =\
	bsp-2.3/ybsp.6\
	docsrc/yadex.6\
#	docsrc/yadex.lsm\

# Files that must be put in the distribution
# archive. Most (but not all) are generated from
# $(DOC1_SRC_*) into the base directory.
DOC1 = README doc/README

# Files that go in the doc/ directory and must
# NOT be put in the distribution archive. Most
# are either generated from $(DOC2_SRC_*) or
# symlinked for docsrc/*.png.
DOC2 = $(addprefix doc/, $(PIX) $(notdir $(DOC2_SRC_HTML) $(DOC2_SRC_MISC)))

# Misc. other files that must be put in the
# distribution archive.
MISC_FILES =\
	src/.srcdate\
	src/.uptodate\
	CHANGES\
	COPYING\
	COPYING.LIB\
	GNUmakefile\
	Makefile\
	TODO\
	VERSION\
	bsp-2.3/bsp23x.txt\
	bsp-2.3/transdor.wad\
	docsrc/.pixlist\
	yadex.cfg\
	yadex.dep\

# The images used in the HTML doc. FIXME: "<img"
# and "src=" have to be on the same line. These
# are symlinked into doc/ when $(DOC2) is made.
PIX = $(shell cat docsrc/.pixlist)

# The script files.
SCRIPTS = $(addprefix scripts/,\
	ftime.1\
	ftime.c\
	mkinstalldirs\
	process\
	youngest)

# All files that must be put in the distribution archive.
ARC_FILES = $(sort $(DOC1) $(DOC1_SRC) $(DOC2_SRC_HTML) $(DOC2_SRC_MISC)\
	$(MISC_FILES) $(addprefix docsrc/, $(PIX)) $(SCRIPTS) $(SRC) $(YGD))

# Where the normal and debugging binaries are
# put. Don't change this or you'll break things.
BINDIR  = $(OBJDIR)
DBINDIR = $(DOBJDIR)

# The "root" directory of the archives. The
# basename of the archives is also based on this.
ARCHIVE := yadex-$(VERSION)
ARCPREV := yadex-$(VERPREV)
ARCDIFF := yadex-$(VERSION).diff

# Where "make install" puts things.
INST_BINDIR = $(PREFIX)/bin
ifeq ($(PREFIX), /usr)	# Special case: not /usr/etc but /etc
  INST_CFGDIR = /etc/yadex/$(VERSION)
else
  INST_CFGDIR = $(PREFIX)/etc/yadex/$(VERSION)
endif
INST_YGDDIR = $(PREFIX)/share/games/yadex/$(VERSION)
ifdef FHS_MAN
  INST_MANDIR = $(PREFIX)/share/man/man6
else
  INST_MANDIR = $(PREFIX)/man/man6
endif

########################################################################
#
#	Targets for
#	end users.
#
########################################################################

.PHONY: all
all: doc yadex.dep dirs yadex ybsp $(YGD)

.PHONY: yadex
yadex: $(BINDIR)/yadex

$(BINDIR)/yadex: $(OBJ_YADEX) $(OBJ_ATCLIB)
	@echo
	@echo Linking Yadex
	@$(CXX) $(OBJ_YADEX) $(OBJ_ATCLIB) -o $@\
	  -L$(X11LIBDIR) -lc -lm -lX11 $(LDFLAGS) 2>&1

.PHONY: ybsp
ybsp: $(BINDIR)/ybsp

$(BINDIR)/ybsp: $(SRC_BSP) $(HEADERS_BSP)
	@echo
ifdef HAVE_GCC
	@echo Compiling and linking BSP with gcc
	@gcc bsp-2.3/bsp.c -DYADEX_VERSION=\"$(VERSION)\" \
		-O2 -Wall -Winline -finline-functions -ffast-math -lm -o $@
else
	@echo Compiling and linking BSP with cc
	@$(CC) bsp-2.3/bsp.c -DYADEX_VERSION=\"$(VERSION)\" -O2 -lm -o $@
endif

.PHONY: test
test:
	$(BINDIR)/yadex $(A)

.PHONY: install
install:
	@echo
	@scripts/mkinstalldirs $(INST_BINDIR)
	@scripts/mkinstalldirs $(INST_CFGDIR)
	@scripts/mkinstalldirs $(INST_MANDIR)
	@scripts/mkinstalldirs $(INST_YGDDIR)
	@echo Installing yadex and ybsp in $(INST_BINDIR)
	cp -p $(BINDIR)/yadex	$(INST_BINDIR)/yadex-$(VERSION)	2>&1
	ln -sf yadex-$(VERSION)	$(INST_BINDIR)/yadex		2>&1
	cp -p $(BINDIR)/ybsp	$(INST_BINDIR)/ybsp-$(VERSION)	2>&1
	ln -sf ybsp-$(VERSION)	$(INST_BINDIR)/ybsp		2>&1
	@echo Installing man pages in $(INST_MANDIR)
	cp -p doc/yadex.6	$(INST_MANDIR)	2>&1
	cp -p doc/ybsp.6	$(INST_MANDIR)	2>&1
	@echo Installing game definition files in $(INST_YGDDIR)
	cp -p $(YGD) 		$(INST_YGDDIR)	2>&1
	@echo Installing configuration file in $(INST_CFGDIR)
	cp -p yadex.cfg	$(INST_CFGDIR)		2>&1
	chmod a+w $(INST_CFGDIR)/yadex.cfg	2>&1
	@echo "---------------------------------------------------------------"
	@echo "  Yadex is now installed."
	@echo
	@echo "  Before you run it, enter the paths to your iwads in"
	@echo "  $(INST_CFGDIR)/yadex.cfg."
	@echo "  When you're done, type \"yadex\" to start."
	@echo "  If you're confused, take a look at index.html."
	@echo
	@echo "  Good editing !"
	@echo "---------------------------------------------------------------"

.PHONY: clean
clean:
	rm -r $(OBJPHYSDIR)

.PHONY: doc
doc: docsrc/.pixlist docdirs $(DOC1) doc2

# Have to put it separately because evaluation
# of $(DOC2) requires docsrc/.pixlist to exist.
.PHONY: doc2
doc2: $(DOC2)

.PHONY: help
help:
	@echo User targets:
	@echo "make [all]                 Build Yadex and BSP"
	@echo "make yadex                 Build Yadex"
	@echo "make ybsp                  Build BSP"
	@echo "make test [A=args]         Test Yadex"
	@echo "make install [PREFIX=dir]  Install Yadex and BSP"
	@echo
	@echo Hacker targets:
	@echo "make dall            Build debug version of Yadex and BSP"
	@echo "make dyadex          Build debug version of Yadex"
	@echo "make dybsp           Build debug version of BSP"
	@echo "make dtest [A=args]  Test debug version of Yadex"
	@echo "make dg              Run debug version of Yadex through gdb"
	@echo "make dd              Run debug version of Yadex through ddd"
	@echo "make doc             Update doc"
	@echo "make dist            Create distribution archive"
	@echo "make save            Create backup archive"
	@echo "make showconf        Show current configuration"


########################################################################
#
#	Targets meant for
#	hackers only.
#
########################################################################

# d: Compile and run
.PHONY: d
d: dyadex dtest

.PHONY: save
save:
	tar -zcvf yadex-$$(date '+%Y%m%d').tgz\
		--exclude "*.wad"\
		--exclude "*.zip"\
		--exclude "core"\
		--exclude "dos/*"\
		--exclude "obj"\
		--exclude "dobj"\
		--exclude "old/*"\
		--exclude "*~"\
		--exclude "*.bak"\
		--exclude "web/arc"\
		--exclude yadex-$$(date '+%Y%m%d').tgz\
		.

.PHONY: dall
dall: yadex.dep ddirs dyadex dybsp $(YGD)

.PHONY: dyadex
dyadex: ddirs $(DBINDIR)/yadex
	
$(DBINDIR)/yadex: $(DOBJ_YADEX) $(DOBJ_ATCLIB)
	@echo
	@echo Linking Yadex
	@$(CXX) $(DOBJ_YADEX) $(DOBJ_ATCLIB) -o $@\
	  -L$(X11LIBDIR) -lc -lm -lX11 $(DLDFLAGS) 2>&1

.PHONY: dybsp
dybsp: ddirs $(DBINDIR)/ybsp

$(DBINDIR)/ybsp: $(SRC_BSP) $(HEADERS_BSP)
	@echo
	@echo Compiling and linking BSP
	@$(CC) bsp-2.3/bsp.c -Wall -Winline -O2 -finline-functions\
		-ffast-math -lm -o $@

.PHONY: dtest
dtest:
	$(DBINDIR)/yadex $(A)

.PHONY: dg
dg:
	gdb $(DBINDIR)/yadex
	
.PHONY: dd
dd:
	ddd $(DBINDIR)/yadex

# Generate the distribution archives. Requires GNU tar,
# GNU cp, gzip and optionally bzip2. bzip2 is cool but,
# in this case, it's 900% slower than gzip while being
# only 15% more efficient. That's why the creation
# of the .tar.bz2 archive is commented out.
.PHONY: dist
dist: changes distimage distgz distdiff #distbz2
	@echo Removing distribution image tree $(ARCHIVE)
	@rm -r $(ARCHIVE)

.PHONY: distimage
distimage: all $(ARC_FILES)
	@echo Creating distribution image tree $(ARCHIVE)
	@if [ -e $(ARCHIVE) ]; then echo Error: $(ARCHIVE) already exists'!';\
		false; fi
	@scripts/mkinstalldirs $(ARCHIVE)
	@tar -cf - $(ARC_FILES) | (cd $(ARCHIVE); tar -xf -)

.PHONY: distgz
distgz: distimage
	tar -czf $(ARCHIVE).tar.gz $(ARCHIVE)

.PHONY: distbz2
distbz2: distimage
	tar -cIf $(ARCHIVE).tar.bz2 $(ARCHIVE)

.PHONY: distdiff
TMP0 = $$HOME/tmp
TMPDIFF = $(TMP0)/$(ARCDIFF)
TMPPREV = $(TMP0)/$(ARCPREV)
distdiff:
	@echo "> Building the diff distribution"
	@echo ">> Creating the diff"
	mkdir -p $(TMPDIFF)
	scripts/process docsrc/README.diff >$(TMPDIFF)/README
	tar -xzf ../yadex-arc/$(ARCPREV).tar.gz -C $(TMP0)
	export ARCDIR=$$PWD/$(ARCHIVE); cd $(TMPPREV)\
	  && diff -uNr . $$ARCDIR >$(TMPDIFF)/$(ARCDIFF) || true
	cd $(TMPDIFF)/.. && tar -czf $(ARCDIFF).tar.gz $(ARCDIFF)
	@echo ">> Verifying the diff"
	cd $(TMPPREV) && patch -i $(TMPDIFF)/$(ARCDIFF) -p 0
	diff -r $(TMPPREV) $(ARCHIVE)
	mv $(TMPDIFF).tar.gz .
	@echo ">> Cleaning up"
	rm -rf $(TMPDIFF)
	rm -rf $(TMPPREV)

.PHONY: showconf
showconf:
	@echo "ARCHIVE            \"$(ARCHIVE)\""
	@echo "CC                 \"$(CC)\""
	@echo "CFLAGS             \"$(CFLAGS)\""
	@echo "CXX                \"$(CXX)\""
	@echo "CXXFLAGS           \"$(CXXFLAGS)\""
	@echo "DCFLAGS            \"$(DCFLAGS)\""
	@echo "DCXXFLAGS          \"$(DCXXFLAGS)\""
	@echo "DEFINES            \"$(DEFINES)\""
	@echo "DLDFLAGS           \"$(DLDFLAGS)\""
	@echo "FHS_MAN            \"$(FHS_MAN)\""
	@echo "HAVE_GCC           \"$(HAVE_GCC)\""
	@echo "HAVE_GETTIMEOFDAY  \"$(HAVE_GETTIMEOFDAY)\""
	@echo "HAVE_NANOSLEEP     \"$(HAVE_NANOSLEEP)\""
	@echo "HAVE_SNPRINTF      \"$(HAVE_SNPRINTF)\""
	@echo "HAVE_USLEEP        \"$(HAVE_USLEEP)\""
	@echo "INST_BINDIR        \"$(INST_BINDIR)\""
	@echo "INST_CFGDIR        \"$(INST_CFGDIR)\""
	@echo "INST_MANDIR        \"$(INST_MANDIR)\""
	@echo "INST_YGDDIR        \"$(INST_YGDDIR)\""
	@echo "LDFLAGS            \"$(LDFLAGS)\""
	@echo "OBJSUBDIR          \"$(OBJSUBDIR)\""
	@echo "OS                 \"$(OS)\""
	@echo "PREFIX             \"$(PREFIX)\""
	@echo "SHELL              \"$(SHELL)\""
	@echo "VERSION            \"$(VERSION)\""
	@echo "X11INCLUDEDIR      \"$(X11INCLUDEDIR)\""
	@echo "X11LIBDIR          \"$(X11LIBDIR)\""
	@echo "c++ --version      \"`c++ --version`\""
	@echo "cc --version       \"`cc --version`\""
	@echo "shell              \"$$SHELL\""
	@echo "uname              \"`uname`\""

########################################################################
#
#	Internal targets, not meant
#	to be invoked directly
#
########################################################################

# Dependencies of the modules of Yadex
# -Y is here to prevent the inclusion of dependencies on
# /usr/include/*.h etc. As a side-effect, it generates many
# warnings, hence "2>/dev/null".
#
# The purpose of the awk script is to transform this input :
#
#   src/foo.o: src/whatever.h
#
# into this output :
#
#   obj/0/foo.o: src/whatever.h
#   dobj/0/foo.o: src/whatever.h
#
# Note: the modules of Atclib are not scanned as they all
# depend on $(HEADERS_ATCLIB) and nothing else.

yadex.dep: $(SRC_YADEX)
	@echo
	@echo makedepend
	@makedepend -f- -Y -Iatclib $(DEFINES) $(SRC_YADEX) 2>/dev/null\
		| awk 'sub (/^src/, "") == 1 {\
				print "'$(OBJDIR)'" $$0;\
				print "'$(DOBJDIR)'" $$0;\
				next;\
			}' >$@

# The YYYY-MM-DD date indicated in the parentheses after the
# version number is the mtime of the most recent source file
# (where "being a source file" is defined as "being listed in
# $(SRC_NON_GEN)"). That string is the output of a perl script,
# scripts/youngest. Since perl is not necessarily installed on
# all machines, we cache that string in the file src/.srcdate
# and include that file in the distribution archive. If we
# didn't do that, people who don't have perl would be unable to
# build Yadex.
#
# Conceptually, src/.srcdate depends on $(SRC_NON_GEN) and
# doc/*.html depend on src/.srcdate. However, we can't write the
# makefile that way because if we did, that would cause two
# problems. Firstly every time a source file is changed,
# scripts/youngest would be ran, most of the time for nothing
# since its output is always the same, unless it's never been
# run today. Secondly, src/.srcdate being just generated, it's
# more recent than the content of the doc/ directory. The result
# would be that the entire doc/ directory would be rebuilt every
# time a single source file is changed, which is guaranteed to
# have an unnerving effect on the hacker at the keyboard.
#
# Part of the solution is to systematically force the mtime of
# src/.srcdate to 00:00, today. Thus, src/.srcdate always looks
# older than the content of the doc/ directory, unless it's not
# been refreshed yet today.
#
# But that's not enough because then src/.srcdate also looks
# always older than the source files it depends on, and thus
# make attempts to regenerate it every time make is invoked at
# all, which would render the very existence of src/.srcdate
# useless. That's why we have another file, src/.uptodate, that
# we touch to keep track of the time when we last generated
# src/.srcdate.
#
# If there was a such thing as _date-only_ dependencies, I could
# get away with just this :
#
# src/.srcdate: scripts/youngest
# src/.srcdate <date_dependency_operator> $(SRC_NON_GEN)
#         if perl -v >/dev/null 2>&1; then\
#           scripts/youngest >$@;\
#         else\
#           blah...
# doc/*.html <date_dependency_operator> src/.srcdate
#         blah...
#
# That would save two calls to "touch", one intermediary
# dependency (src/.uptodate) and a lot of obfuscation.

src/.srcdate: src/.uptodate

src/.uptodate: scripts/youngest $(SRC_NON_GEN)
	@if perl -v >/dev/null 2>&1; then\
	  echo Generating src/.srcdate;\
	  scripts/youngest $(SRC_NON_GEN) >src/.srcdate;\
	  touch -t `date '+%m%d'`0000 src/.srcdate;\
	elif [ -f src/.srcdate ]; then\
	  echo Perl not available. Keeping old src/.srcdate;\
	else\
	  echo Perl not available. Creating bogus src/.srcdate;\
	  date '+%Y-%m-%d' >src/.srcdate;\
	fi
	@touch $@;

# Directories where objects and binaries are put.
# (normal and debugging versions)

.PHONY: dirs
dirs:
	@if [ ! -d $(OBJPHYSDIR)\
	  -o ! -d $(OBJPHYSDIR_ATCLIB) ];\
	then\
	  echo Creating object directories;\
	  scripts/mkinstalldirs $(OBJPHYSDIR)        2>&1;\
	  scripts/mkinstalldirs $(OBJPHYSDIR_ATCLIB) 2>&1;\
	fi
	@if [ -e $(OBJDIR) ]; then rm $(OBJDIR) 2>&1; fi
	@ln -sf $(OBJSUBDIR) $(OBJDIR) 2>&1

.PHONY: ddirs
ddirs:
	@if [ ! -d $(DOBJPHYSDIR)\
	  -o ! -d $(DOBJPHYSDIR_ATCLIB) ];\
	then\
	  echo Creating object directories;\
	  scripts/mkinstalldirs $(DOBJPHYSDIR)        2>&1;\
	  scripts/mkinstalldirs $(DOBJPHYSDIR_ATCLIB) 2>&1;\
	fi
	@if [ -e $(DOBJDIR) ]; then rm $(DOBJDIR) 2>&1; fi
	@ln -sf $(OBJSUBDIR) $(DOBJDIR) 2>&1

# To compile the modules of Yadex
# (normal and debugging versions)

include yadex.dep
$(OBJDIR)/%.o: src/%.cc
	@echo
	@echo $(CXX) $<
	@$(CXX) $(CXXFLAGS) -c -Iatclib $(DEFINES) $< -o $@ 2>&1

$(DOBJDIR)/%.o: src/%.cc
	@echo
	@echo $(CXX) $<
	@$(CXX) $(DCXXFLAGS) -c -Iatclib $(DEFINES) $< -o $@ 2>&1

# To compile the modules of Atclib
# (normal and debugging versions)

$(OBJDIR_ATCLIB)/%.o: atclib/%.c $(HEADERS_ATCLIB)
	@echo
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -c $< -o $@ 2>&1

$(DOBJDIR_ATCLIB)/%.o: atclib/%.c $(HEADERS_ATCLIB)
	@echo
	@echo $(CC) $<
	@$(CC) $(DCFLAGS) -c $< -o $@ 2>&1

# A source file containing just the date of the
# most recent source file and the version number
# (found in ./VERSION).

src/version.cc: $(SRC_NON_GEN) VERSION src/.srcdate
	@echo
	@echo Generating $@
	@printf "extern const char *const yadex_source_date = \"%s\";\n" \
		`cat src/.srcdate` >$@
	@printf "extern const char *const yadex_version = \"%s\";\n" \
		$(VERSION) >>$@

# -------- Doc-related stuff --------

docdirs:
	@if [ ! -d doc ]; then mkdir doc; fi

docsrc/.pixlist: $(DOC2_SRC_HTML)
	@echo Generating $@
	@if perl -v >/dev/null; then\
	  perl -ne '@l = m/<img\s[^>]*src="?([^\s">]+)/io;\
	    print "@l\n" if @l;' $(DOC2_SRC_HTML) | sort | uniq >$@;\
	elif [ -e $@ ]; then\
	  echo "Sorry, you need Perl to refresh $@. Keeping old $@.";\
	else\
	  echo "Sorry, you need Perl to create $@. Creating empty $@.";\
	  touch $@;\
	fi

events.html: ev evhtml
	evhtml -- -n $< >$@

events.txt: events.html
	lynx -dump $< >$@

changes/changes.html: changes/*.log log2html
	./log2html -- $$(ls -r changes/*.log) >$@
	
.PHONY: changes
changes: changes/changes.html
	lynx -dump $< >CHANGES

.PHONY: man
man: doc/yadex.6
	man -l $^

.PHONY: dvi
dvi: doc/yadex.dvi
	xdvi $^ 

doc/yadex.dvi: doc/yadex.6
	groff -Tdvi -man $^ >$@

.PHONY: ps
ps: doc/yadex.ps
	gv $^

doc/yadex.ps: doc/yadex.6
	groff -Tps -man $^ >$@
	

# Generate the doc by filtering them through scripts/process

PROCESS = VERSION src/.srcdate scripts/process scripts/ftime

doc/ybsp.6: bsp-2.3/ybsp.6 $(PROCESS)
	@echo
	@echo Generating $@
	@scripts/process $< >$@

doc/yadex.6: docsrc/yadex.6 $(PROCESS)
	@echo
	@echo Generating $@
	@scripts/process $< >$@

doc/README: docsrc/README.doc $(PROCESS)
	@echo
	@echo Generating $@
	@scripts/process $< >$@

%: docsrc/% $(PROCESS)
	@echo
	@echo Generating $@
	@scripts/process $< >$@

doc/%.html: docsrc/%.html $(PROCESS)
	@echo
	@echo Generating $@
	@scripts/process $< >$@

# The images are just symlinked from docsrc/ to doc/

doc/%.png: docsrc/%.png
	@ln -sf ../$< $@

#scripts/mdate: scripts/mdate.c
#	$(CC) $< -o $@

scripts/ftime: scripts/ftime.c
	$(CC) $< -o $@


