#
#	Makefile for Yadex
#	Copyright © André Majorel 1998-2005.
#	AYM 1998-06-10
#

# ATTENTION : GNU MAKE IS REQUIRED ! This makefile uses pattern
# rules, addprefix, addsuffix, etc. It's not named "GNUmakefile"
# for nothing.

########################################################################
#
#	Definitions that only hackers
#	might want to change
#
########################################################################

# The name of the directory where objects and
# binaries are put. I include the output of
# "uname -a" to make it easier for me to build
# Yadex for different platforms from the same
# source tree.
SYSTEM := $(shell echo `uname -n`_`uname -a | cksum` | tr -dc '[:alnum:]._-')
OBJDIR             = obj/0
DOBJDIR            = dobj/0
OBJPHYSDIR         = obj/$(SYSTEM)
DOBJPHYSDIR        = dobj/$(SYSTEM)
OBJDIR_ATCLIB      = $(OBJDIR)/atclib
DOBJDIR_ATCLIB     = $(DOBJDIR)/atclib
OBJPHYSDIR_ATCLIB  = $(OBJPHYSDIR)/atclib
DOBJPHYSDIR_ATCLIB = $(DOBJPHYSDIR)/atclib

# Create all directories and make symlinks to
# config.cc and config.h. Doing it at the start
# makes things much simpler later on.
DUMMY := $(shell							\
	mkdir -p $(OBJPHYSDIR)  $(OBJPHYSDIR_ATCLIB);			\
	mkdir -p $(DOBJPHYSDIR) $(DOBJPHYSDIR_ATCLIB);			\
	[ ! -h $(OBJDIR)  ] || rm $(OBJDIR);				\
	[ ! -h $(DOBJDIR) ] || rm $(DOBJDIR);				\
	ln -s $(SYSTEM) $(OBJDIR);					\
	ln -s $(SYSTEM) $(DOBJDIR);					\
	)

include $(OBJDIR)/Makefile.config

X11CC := $(addprefix -I,$(X11INC))
X11LD := $(addprefix -L,$(X11LIB))

########################################################################
#
#	Definitions that end users
#	might want to change
#
########################################################################

# DEU-style round things. The angle is not shown. Little reason to use
# this.
#CXXFLAGS += -DROUND_THINGS

# Dark-on-light UI. Some parts need work.
#CXXFLAGS += -DWHITE_BACKGROUND

# Old drawmap() code. Slightly more precise but precludes hardware
# scrolling. Little reason to use this.
#CXXFLAGS += -DY_SWARM

# 1.7.900 is an alpha release
CXXFLAGS += -DY_ALPHA


# Options used to compile and link the debugging
# targets. Not used by normal end-user targets.
# Unlike CFLAGS, CXXFLAGS and LDFLAGS, assume
# GCC/EGCS.
DCFLAGS		= $(CFLAGS) -g -O
DCFLAGS		+= -Wall			# GCC warnings
DCFLAGS		+= -pedantic			# GCC warnings
DCFLAGS		+= -Wcast-qual			# GCC warnings
DCFLAGS		+= -Wcast-align			# GCC warnings
DCFLAGS		+= -Winline			# GCC warnings
DCFLAGS		+= -Wmissing-declarations	# GCC warnings
DCFLAGS		+= -Wmissing-prototypes		# GCC warnings
DCFLAGS		+= -Wno-parentheses		# GCC warnings
DCFLAGS		+= -Wpointer-arith		# GCC warnings
DCFLAGS		+= -Wwrite-strings		# GCC warnings
DCFLAGS		+= -pg				# Profiling

DCXXFLAGS	= $(CXXFLAGS) -g -O
DCXXFLAGS	+= -Wall			# GCC warnings
DCXXFLAGS	+= -pedantic			# GCC warnings
DCXXFLAGS	+= -Wcast-qual			# GCC warnings
DCXXFLAGS	+= -Wcast-align			# GCC warnings
#DCXXFLAGS	+= -Weffc++			# GCC warnings
#DCXXFLAGS	+= -Winline			# GCC warnings
DCXXFLAGS	+= -Wmissing-prototypes		# GCC warnings
DCXXFLAGS	+= -Wno-parentheses		# GCC warnings
DCXXFLAGS	+= -Wpointer-arith		# GCC warnings
DCXXFLAGS	+= -Wwrite-strings		# GCC warnings
DCXXFLAGS	+= -pg				# Profiling

DLDFLAGS	=
DLDFLAGS	+= -pg				# Profiling
#DLDFLAGS	+= -lefence			# Electric Fence


########################################################################
#
#	Definitions that only hackers
#	might want to change
#
########################################################################

MAKEFILE = GNUmakefile
VERSION := $(shell cat VERSION)
VERPREV := $(shell test -f VERPREV && cat VERPREV)

# All the modules of Yadex without path or extension.
MODULES_YADEX =								\
	acolours	aym		bench		bitvec		\
	cfgfile		checks		colour1		colour2		\
	colour3		colour4		config		credits		\
	dependcy	dialog		disppic		drawmap		\
	edisplay	editgrid	editlev		editloop	\
	editobj		editsave	endian		editzoom	\
	entry		entry2		events		flats		\
	game		gcolour1	gcolour2	gcolour3	\
	geom		gfx		gfx2		gfx3		\
	gotoobj		help1		help2		highlt		\
	img		imgscale	imgspect	infobar		\
	input		l_align		l_centre	l_flags		\
	l_merge		l_misc		l_prop		l_unlink	\
	l_vertices							\
	levels		lists		locate		lumpdir		\
	macro		memory		menubar		menu		\
	mkpalette	mouse		names		nop		\
	objects		objinfo		oldmenus	palview		\
	patchdir	pic2img		prefer		s_centre	\
	s_door		s_lift		s_linedefs	s_merge		\
	s_misc		s_prop		s_slice		s_split		\
	s_swapf		s_vertices	sanity		scrnshot	\
	selbox		selectn		selpath		selrect		\
	serialnum	spritdir	sticker		swapmem		\
	t_centre	t_flags		t_prop		t_spin		\
	textures	things		trace		v_centre	\
	v_merge		v_polyg		vectext		verbmsg		\
	version		wadfile		wadlist		wadnamec	\
	wadres		wads		wads2		warn		\
	windim		x_centre	x_exchng	x_hover		\
	x_mirror	x_rotate	x11		xref		\
	yadex		ytime

# All the modules of Atclib without path or extension.
MODULES_ATCLIB =							\
	al_adigits	al_aerrno	al_astrerror	al_fana		\
	al_fnature	al_lateol	al_lcount	al_lcreate	\
	al_ldelete	al_ldiscard	al_lgetpos	al_linsert	\
	al_linsertl	al_llength	al_lpeek	al_lpeekl	\
	al_lpoke	al_lpokel	al_lptr		al_lread	\
	al_lreadl	al_lrewind	al_lseek	al_lsetpos	\
	al_lstep	al_ltell	al_lwrite	al_lwritel	\
	al_sapc		al_saps		al_scps		al_scpslower	\
	al_sdup		al_sisnum	al_strolc

# The source files of Yadex and Atclib
SRC_YADEX  = $(addprefix src/,     $(addsuffix .cc, $(MODULES_YADEX)))
SRC_ATCLIB = $(addprefix atclib/,  $(addsuffix .c,  $(MODULES_ATCLIB)))

# The headers of Yadex and Atclib
HEADERS_YADEX  := $(wildcard src/*.h)
HEADERS_ATCLIB =  atclib/atclib.h

# All the source files, including the headers.
SRC = $(filter-out src/config.cc, $(SRC_YADEX))				\
      $(filter-out src/config.h, $(HEADERS_YADEX))			\
      $(SRC_ATCLIB) $(HEADERS_ATCLIB)

# The files on which youngest is run.
SRC_NON_GEN = $(filter-out src/credits.cc src/prefix.cc src/version.cc, $(SRC))

# The object files
OBJ_YADEX   = $(addprefix $(OBJDIR)/,  $(addsuffix .o, $(MODULES_YADEX)))
DOBJ_YADEX  = $(addprefix $(DOBJDIR)/, $(addsuffix .o, $(MODULES_YADEX)))
OBJ_ATCLIB  = $(addprefix $(OBJDIR_ATCLIB)/,  $(addsuffix .o,$(MODULES_ATCLIB)))
DOBJ_ATCLIB = $(addprefix $(DOBJDIR_ATCLIB)/, $(addsuffix .o,$(MODULES_ATCLIB)))

# The game definition files.
YGDDIR = ygd
YGD = $(addprefix $(YGDDIR)/,						\
	doom.ygd	doom02.ygd	doom04.ygd	doom05.ygd	\
	doom2.ygd	doompr.ygd	heretic.ygd	hexen.ygd	\
	strife.ygd	strife10.ygd)

# Files included in the distribution archive. Most (but not all) are
# generated from $(DOC1_SRC_*) into the base directory.
DOC1 =					\
	FAQ				\
	README				\
	doc/README

# Files not included in the distribution archive.
DOC2 = $(DOC2_M7) $(DOC2_CP)

# Files not included in the distribution archive and generated with m7
DOC2_M7 = 				\
	doc/advanced.html		\
	doc/contact.html		\
	doc/credits.html		\
	doc/deu_diffs.html		\
	doc/editing_docs.html		\
	doc/faq.html			\
	doc/feedback.html		\
	doc/getting_started.html	\
	doc/hackers_guide.html		\
	doc/help.html			\
	doc/index.html			\
	doc/keeping_up.html		\
	doc/legal.html			\
	doc/packagers_guide.html	\
	doc/palette.html		\
	doc/reporting.html		\
	doc/tips.html			\
	doc/trivia.html			\
	doc/trouble.html		\
	doc/users_guide.html		\
	doc/wad_specs.html		\
	doc/yadex.6			\
	doc/ygd.html

# Misc. other files that must be put in the
# distribution archive.
MISC_FILES =								\
	boost/boost/config.hpp						\
	boost/boost/config/compiler/borland.hpp				\
	boost/boost/config/compiler/comeau.hpp				\
	boost/boost/config/compiler/common_edg.hpp			\
	boost/boost/config/compiler/compaq_cxx.hpp			\
	boost/boost/config/compiler/gcc.hpp				\
	boost/boost/config/compiler/greenhills.hpp			\
	boost/boost/config/compiler/hp_acc.hpp				\
	boost/boost/config/compiler/intel.hpp				\
	boost/boost/config/compiler/kai.hpp				\
	boost/boost/config/compiler/metrowerks.hpp			\
	boost/boost/config/compiler/mpw.hpp				\
	boost/boost/config/compiler/sgi_mipspro.hpp			\
	boost/boost/config/compiler/sunpro_cc.hpp			\
	boost/boost/config/compiler/vacpp.hpp				\
	boost/boost/config/compiler/visualc.hpp				\
	boost/boost/config/platform/aix.hpp				\
	boost/boost/config/platform/beos.hpp				\
	boost/boost/config/platform/bsd.hpp				\
	boost/boost/config/platform/cygwin.hpp				\
	boost/boost/config/platform/hpux.hpp				\
	boost/boost/config/platform/irix.hpp				\
	boost/boost/config/platform/linux.hpp				\
	boost/boost/config/platform/macos.hpp				\
	boost/boost/config/platform/solaris.hpp				\
	boost/boost/config/platform/win32.hpp				\
	boost/boost/config/posix_features.hpp				\
	boost/boost/config/select_compiler_config.hpp			\
	boost/boost/config/select_platform_config.hpp			\
	boost/boost/config/select_stdlib_config.hpp			\
	boost/boost/config/stdlib/dinkumware.hpp			\
	boost/boost/config/stdlib/libstdcpp3.hpp			\
	boost/boost/config/stdlib/modena.hpp				\
	boost/boost/config/stdlib/msl.hpp				\
	boost/boost/config/stdlib/roguewave.hpp				\
	boost/boost/config/stdlib/sgi.hpp				\
	boost/boost/config/stdlib/stlport.hpp				\
	boost/boost/config/stdlib/vacpp.hpp				\
	boost/boost/config/suffix.hpp					\
	boost/boost/config/user.hpp					\
	boost/boost/smart_ptr.hpp					\
	boost/boost/static_assert.hpp					\
	boost/boost/utility.hpp						\
	boost/boost/utility/base_from_member.hpp			\
	boost/boost/utility_fwd.hpp					\
	cache/copyright.man						\
	cache/copyright.txt						\
	cache/doc.dep							\
	cache/m7src							\
	cache/pixlist							\
	cache/srcdate							\
	cache/uptodate							\
	cache/yadex.dep							\
	configure							\
	docsrc/copyright						\
	CHANGES								\
	COPYING								\
	COPYING.LIB							\
	GNUmakefile							\
	Makefile							\
	TODO								\
	VERSION								\
	yadex.cfg

# The images used in the HTML doc. FIXME: "<img"
# and "src=" have to be on the same line. These
# are symlinked into doc/ when $(DOC2) is made.
PIX = $(shell cat cache/pixlist)

DOC2_CP = $(addprefix doc/, $(PIX))

# docsrc - returns the primary source of any doc file.
define docsrc
$(if $(filter doc/README, $1),
  docsrc/README.doc.m7,
  $(if $(filter README, $1),
    docsrc/README.m7,
    $(if $(filter FAQ, $1),
      docsrc/faq.html.m7,
      $(patsubst doc/%, docsrc/%.m7, $1)
    )
  )
)
endef

# M7ROOTS lists all the primary m7 source files, primary meaning that
# they are mentioned in an m7 command line. sourced files don't count.
# If any of these files changes, we must refresh M7SRC.
M7ROOTS = $(strip							\
	  docsrc/common.m7						\
	  $(foreach f, $(DOC1) $(DOC2_M7), $(call docsrc, $f))		\
	)

# M7SRC lists all the files that are used by m7, directly or indirectly.
# If any of these files files changes, we must refresh doc.dep.
include cache/m7src

# The script files.
SCRIPTS = $(addprefix scripts/,	\
	copyright		\
	htmlimg			\
	input.h			\
	input_buf.h		\
	input_file.h		\
	install.c		\
	m7.1			\
	m7.cc			\
	m7.h			\
	mkinstalldirs		\
	notexist.c		\
	output.h		\
	output_buf.h		\
	output_file.h		\
	output_null.h		\
	youngest)

# The patches
PATCHES = $(addprefix patches/,	\
	README			\
	1.5.0_gcc27.diff)

# All files that must be put in the distribution archive.
ARC_FILES = $(sort $(DOC1) $(M7SRC) $(MISC_FILES) $(addprefix docsrc/, $(PIX))\
	$(SCRIPTS) $(SRC) $(YGD) $(PATCHES))

# The "root" directory of the archives. The
# basename of the archives is also based on this.
ARCHIVE := yadex-$(VERSION)
ARCPREV := yadex-$(VERPREV)
ARCDIFF := yadex-$(VERSION).diff

# Cosmetic
CFLAGS    := $(strip $(CFLAGS))
DCFLAGS   := $(strip $(DCFLAGS))
CXXFLAGS  := $(strip $(CXXFLAGS))
DCXXFLAGS := $(strip $(DCXXFLAGS))
LDFLAGS   := $(strip $(LDFLAGS))
DLDFLAGS  := $(strip $(DLDFLAGS))


########################################################################
#
#	Targets for
#	end users.
#
########################################################################

.PHONY: all
all: doc yadex $(YGD)

.PHONY: yadex
yadex: $(OBJDIR)/yadex

$(OBJDIR)/yadex: $(OBJ_YADEX) $(OBJ_ATCLIB) $(MAKEFILE)
	@echo "** Linking Yadex"
	$(CXX) $(LDFLAGS) -o $@	$(OBJ_YADEX) $(OBJ_ATCLIB) \
	  $(X11LD) -lX11 -lm -lc

.PHONY: test
test:
	$(OBJDIR)/yadex -G $(YGDDIR) $(A)

.PHONY: install
install: $(OBJDIR)/install
	@scripts/mkinstalldirs $(BINDIR)
	@scripts/mkinstalldirs $(ETCDIR)
	@scripts/mkinstalldirs $(MANDIR)
	@scripts/mkinstalldirs $(MANDIR)/man6
	@scripts/mkinstalldirs $(SHAREDIR)
	$(OBJDIR)/install -m 755 $(OBJDIR)/yadex $(BINDIR)/yadex-$(VERSION)
	rm -f $(BINDIR)/yadex
	ln -s yadex-$(VERSION) $(BINDIR)/yadex
	$(OBJDIR)/install -m 644 doc/yadex.6 $(MANDIR)/man6/yadex-$(VERSION).6
	rm -f $(MANDIR)/man6/yadex.6
	ln -s yadex-$(VERSION).6 $(MANDIR)/man6/yadex.6
	$(OBJDIR)/install -m 644 -d $(SHAREDIR) $(YGD)
	$(OBJDIR)/install -m 644 -d $(ETCDIR) yadex.cfg
	@echo "---------------------------------------------------------------"
	@echo "  Yadex is now installed."
	@echo
	@echo "  Before you run it, enter the paths to your iwads in"
	@echo "  $(ETCDIR)/yadex.cfg or ~/.yadex/yadex.cfg."
	@echo "  When you're done, type \"yadex\" to start."
	@echo "  If you're confused, take a look at doc/index.html."
	@echo
	@echo "  Happy editing !"
	@echo "---------------------------------------------------------------"

.PHONY: clean
clean:
	rm -f $(OBJ_YADEX) $(OBJ_ATCLIB) $(OBJDIR)/yadex
	rm -f $(DOBJ_YADEX) $(DOBJ_ATCLIB) $(DOBJDIR)/yadex
	rm -f $(OBJDIR)/install
	rm -f $(OBJDIR)/notexist
	rm -f $(OBJDIR)
	rm -f $(DOBJDIR)
	rm -rf doc

.PHONY: dclean
dclean:
	rm -rf $(DOBJPHYSDIR)
	rm -f $(DOBJDIR)

.PHONY: doc
doc: cache/pixlist docdirs $(DOC1) doc2

# Have to put it separately because evaluation
# of $(DOC2) requires cache/pixlist to exist.
.PHONY: doc2
doc2: $(DOC2)

.PHONY: help
help:
	@echo User targets:
	@echo "make [all]           Build everything"
	@echo "make yadex           Build Yadex"
	@echo "make test [A=args]   Test Yadex"
	@echo "make install         Install everything"
	@echo "make showconf        Show current configuration"
	@echo
	@echo Hacker targets:
	@echo "make dall            Build debug version of everything"
	@echo "make dyadex          Build debug version of Yadex"
	@echo "make dtest [A=args]  Test debug version of Yadex"
	@echo "make dg              Run debug version of Yadex through gdb"
	@echo "make dd              Run debug version of Yadex through ddd"
	@echo "make doc             Update doc"
	@echo "make man             View man page with man"
	@echo "make dvi             View man page with xdvi"
	@echo "make ps              View man page with gv"
	@echo "make dist            Create distribution archive"
	@echo "make save            Create backup archive"


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
	tar -cjvf yadex-$$(date '+%Y%m%d').tar.bz2			\
		--exclude "*.wad"					\
		--exclude "*.zip"					\
		--exclude "core"					\
		--exclude "dos/*"					\
		--exclude "obj"						\
		--exclude "dobj"					\
		--exclude "old/*"					\
		--exclude "*~"						\
		--exclude "*.bak"					\
		--exclude "web/arc"					\
		--exclude yadex-$$(date '+%Y%m%d').tar.bz2		\
		.

.PHONY: dall
dall: dyadex $(YGD)

.PHONY: dyadex
dyadex: $(DOBJDIR)/yadex
	
$(DOBJDIR)/yadex: $(DOBJ_CONFIG) $(DOBJ_YADEX) $(DOBJ_ATCLIB) $(MAKEFILE)
	@echo "** Linking Yadex (debug)"
	$(CXX) $(DLDFLAGS) -o $@ $(DOBJ_CONFIG) $(DOBJ_YADEX) $(DOBJ_ATCLIB) \
	  $(X11LD) -lX11 -lm -lc

.PHONY: dtest
dtest:
	$(DOBJDIR)/yadex -G $(YGDDIR) $(A)
	gprof $(DOBJDIR)/yadex >gprof.out

.PHONY: dg
dg:
	gdb $(DOBJDIR)/yadex
	
.PHONY: dd
dd:
	ddd $(DOBJDIR)/yadex

.PHONY: asm
asm: $(addprefix $(OBJDIR)/, $(addsuffix .S, $(MODULES_YADEX)))

# Generate the distribution archives. Requires GNU tar,
# GNU cp, gzip and optionally bzip2 (if distbz2 is
# uncommented).
.PHONY: dist
dist: all changes distimage distgz distdiff #distbz2
	@echo "** Removing distribution image tree $(ARCHIVE)"
	rm -r $(ARCHIVE)

.PHONY: distimage
distimage: $(ARC_FILES)
	@echo "** Creating distribution image tree $(ARCHIVE)"
	rm -rf $(ARCHIVE)
	scripts/mkinstalldirs $(ARCHIVE)
	@tar -cf - $(ARC_FILES) | (cd $(ARCHIVE); tar -xf -)


distgz: distimage
	@echo "** Creating tar.gz distribution"
	tar -czf $(ARCHIVE).tar.gz $(ARCHIVE)

.PHONY: distbz2
distbz2: distimage
	@echo "** Creating .tar.bz2 distribution"
	tar -cIf $(ARCHIVE).tar.bz2 $(ARCHIVE)

.PHONY: distdiff
TMP0    = $$HOME/tmp
TMPPREV = $(TMP0)/$(ARCPREV)
TMPCURR = $(TMP0)/$(ARCHIVE)
distdiff:
	@echo "** Building the diff distribution"
	@echo "Creating the diff"
	rm -rf $(TMPPREV) $(TMPCURR) $(TMPDIFF)
	mkdir -p $(TMP0)
	tar -xzf                  $(ARCHIVE).tar.gz -C $(TMP0)
	tar -xzf ../yadex-arc/pub/$(ARCPREV).tar.gz -C $(TMP0)
	$(OBJDIR)/m7 docsrc/common.m7 docsrc/README.diff.m7 >$(TMP0)/$(ARCDIFF)
	echo >>$(TMP0)/$(ARCDIFF)
	cd $(TMP0) && (diff -uaNr $(ARCPREV) $(ARCHIVE) >>$(ARCDIFF) || true)
	@# KLUDGE - On my system, just "! grep" makes make choke
	true; ! grep "^Binary files .* and .* differ" $(TMP0)/$(ARCDIFF)
	gzip -f $(TMP0)/$(ARCDIFF)
	@echo "Verifying the diff"
	cd $(TMPPREV) && gzip -d <../$(ARCDIFF).gz | patch -p1
	@# FIXME remove -N after 1.6 is done, it's there because
	@# uptodate has been moved between 1.5 and 1.6 and since
	@# it's empty it remains in $(ARCPREV).
	cd $(TMP0) && diff -rP $(ARCHIVE) $(ARCPREV)
	mv $(TMP0)/$(ARCDIFF).gz .
	@echo "Cleaning up"
	cd $(TMP0) && rm -rf $(ARCPREV)
	cd $(TMP0) && rm -rf $(ARCHIVE)

.PHONY: showconf
showconf:
	@awk 'FNR == 1 { if (NR > 1) print ""; print FILENAME ":" }	\
	  { printf("%d\t%s\n", FNR, $$0) } END { print "" }'		\
	  $(OBJDIR)/Makefile.config					\
	  $(OBJDIR)/config.h						\
	  $(OBJDIR)/config.cc						\
	  $(OBJDIR)/config.etc						\
	  $(OBJDIR)/config.share					\
	  
	@echo "ARCHIVE            \"$(ARCHIVE)\""
	@echo "BINDIR             \"$(BINDIR)\""
	@echo "CC                 \"$(CC)\""
	@echo "CFLAGS             \"$(CFLAGS)\""
	@echo "CXX                \"$(CXX)\""
	@echo "CXXFLAGS           \"$(CXXFLAGS)\""
	@echo "DCFLAGS            \"$(DCFLAGS)\""
	@echo "DCXXFLAGS          \"$(DCXXFLAGS)\""
	@echo "DLDFLAGS           \"$(DLDFLAGS)\""
	@echo "ETCDIR             \"$(ETCDIR)\""
	@echo "ETCDIRNV           \"$(ETCDIRNV)\""
	@echo "LDFLAGS            \"$(LDFLAGS)\""
	@echo "MANDIR             \"$(MANDIR)\""
	@echo "SHAREDIR           \"$(SHAREDIR)\""
	@echo "SHAREDIRNV         \"$(SHAREDIRNV)\""
	@echo "SHELL              \"$(SHELL)\""
	@echo "SYSTEM             \"$(SYSTEM)\""
	@echo "VERSION            \"$(VERSION)\""
	@echo "X11INC             \"$(X11INC)\""
	@echo "X11LIB             \"$(X11LIB)\""
	@echo "CXX --version      \"`$(CXX) --version`\""
	@echo "CC --version       \"`$(CC) --version`\""
	@echo "shell              \"$$SHELL\""
	@echo "uname -a           \"`uname -a`\""


########################################################################
#
#	Internal targets, not meant
#	to be invoked directly
#
########################################################################

# If Makefile.config or config.h don't exist, give a hint...
$(OBJDIR)/Makefile.config:
$(OBJDIR)/config.h:
	@echo "Sorry guv'nor, but... did you run ./configure ?" >&2
	@false

$(OBJDIR)/files_etc.man: $(OBJDIR)/config.etc $(MAKEFILE)
	@echo "Generating $@"
	@sed 's/%v/$(VERSION)/g; s,.*,.B &/yadex.cfg,' $< >$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

$(OBJDIR)/files_share.man: $(OBJDIR)/config.share $(MAKEFILE)
	@echo "Generating $@"
	@sed 's/%v/$(VERSION)/g; s,.*,.BI &/ game .ygd,' $< >$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

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

cache/yadex.dep: $(SRC_NON_GEN) src/config.h
	@echo "Generating $@"
	@test -d cache || mkdir cache
	@makedepend -f- -Y -Iatclib $(SRC_NON_GEN) 2>/dev/null		\
		| $(AWK) 'sub (/^src/, "") == 1 {			\
				print "'$(OBJDIR)'" $$0;		\
				print "'$(DOBJDIR)'" $$0;		\
				next;					\
			}' >$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

cache/copyright.man: $(MAKEFILE) scripts/copyright docsrc/copyright
	@echo "Generating $@"
	@test -d cache || mkdir cache
	@scripts/copyright -m docsrc/copyright >$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

cache/copyright.txt: $(MAKEFILE) scripts/copyright docsrc/copyright
	@echo "Generating $@"
	@test -d cache || mkdir cache
	@scripts/copyright -t docsrc/copyright | sed 's/^./    &/' >$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

# The YYYY-MM-DD date indicated in the parentheses after the
# version number is the mtime of the most recent source file
# (where "being a source file" is defined as "being listed in
# $(SRC_NON_GEN)"). That string is the output of a perl script,
# scripts/youngest. Since perl is not necessarily installed on
# all machines, we cache that string in the file cache/srcdate
# and include that file in the distribution archive. If we
# didn't do that, people who don't have perl would be unable to
# build Yadex.
#
# Conceptually, cache/srcdate depends on $(SRC_NON_GEN) and
# doc/*.html depend on cache/srcdate. However, we can't write the
# makefile that way because if we did, that would cause two
# problems. Firstly every time a source file is changed,
# scripts/youngest would be ran, most of the time for nothing
# since its output is always the same, unless it's never been
# run today. Secondly, cache/srcdate being just generated, it's
# more recent than the content of the doc/ directory. The result
# would be that the entire doc/ directory would be rebuilt every
# time a single source file is changed, which is guaranteed to
# have an unnerving effect on the hacker at the keyboard.
#
# Part of the solution is to systematically force the mtime of
# cache/srcdate to 00:00, today. Thus, cache/srcdate always looks
# older than the content of the doc/ directory, unless it's not
# been refreshed yet today.
#
# But that's not enough because then cache/srcdate also looks
# always older than the source files it depends on, and thus
# make attempts to regenerate it every time make is invoked at
# all, which would render the very existence of cache/srcdate
# useless. That's why we have another file, cache/uptodate, that
# we touch to keep track of the time when we last generated
# cache/srcdate.
#
# If there was a such thing as _date-only_ dependencies, I could
# get away with just this :
#
# cache/srcdate: scripts/youngest
# cache/srcdate <date_dependency_operator> $(SRC_NON_GEN)
#         if perl -v >/dev/null 2>&1; then\
#           scripts/youngest >$@;\
#         else\
#           blah...
# doc/*.html <date_dependency_operator> cache/srcdate
#         blah...
#
# That would save two calls to "touch", one intermediary
# dependency (cache/uptodate) and a lot of obfuscation.
cache/srcdate: cache/uptodate

cache/uptodate: scripts/youngest $(SRC_NON_GEN)
	@test -d cache || mkdir cache
	@if perl -v >/dev/null 2>&1; then				\
	  echo Generating cache/srcdate;				\
	  scripts/youngest $(SRC_NON_GEN) >cache/srcdate;		\
	  touch -t `date '+%m%d'`0000 cache/srcdate;			\
	elif [ -r cache/srcdate ]; then					\
	  echo Perl not available. Keeping old cache/srcdate;		\
	else								\
	  echo Perl not available. Creating bogus cache/srcdate;	\
	  date '+%Y-%m-%d' >cache/srcdate;				\
	fi
	@touch $@;

# To compile the modules of Yadex
# (normal and debugging versions)
include cache/yadex.dep

# It's simpler to copy config.cc into src/ than to have a
# compilation rule for just one file.
src/config.cc: $(OBJDIR)/config.cc
	cp -p $< $@

src/config.h: $(OBJDIR)/config.h
	cp -p $< $@

$(OBJDIR)/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -Iatclib -Iboost $(X11CC) -c $< -o $@

$(DOBJDIR)/%.o: src/%.cc
ifeq "$(filter -Weffc++,$(DCXXFLAGS))" ""
	$(CXX) $(DCXXFLAGS) -Iatclib -Iboost $(X11CC) -c $< -o $@
else
	$(CXX) $(DCXXFLAGS) -Iatclib -Iboost $(X11CC) -c $< -o $@ 2>&1	\
	  | awk '/^[^ ]/ { c = 0 } /^\/usr\/include\// { c = 1 } !c'
endif

# To compile the modules of Atclib
# (normal and debugging versions)
$(OBJDIR_ATCLIB)/%.o: atclib/%.c $(HEADERS_ATCLIB) $(OBJDIR)/Makefile.config
	$(CC) -c $(CFLAGS) $< -o $@

$(DOBJDIR_ATCLIB)/%.o: atclib/%.c $(HEADERS_ATCLIB) $(OBJDIR)/Makefile.config
	$(CC) -c $(DCFLAGS) $< -o $@

# To see the generated assembly code
# for the modules of Yadex
$(OBJDIR)/%.S: src/%.cc $(MAKEFILE)
	$(CXX) $(CXXFLAGS) -fverbose-asm -Iatclib -Iboost $(X11CC) -S $< -o $@

# A source file containing the credits
src/credits.cc: $(MAKEFILE) docsrc/copyright scripts/copyright
	@echo "Generating $@"
	@echo '// DO NOT EDIT -- generated from docsrc/copyright' >$(@D)/.$(@F)
	@scripts/copyright -c docsrc/copyright >>$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

# A source file containing just the date of the
# most recent source file and the version number
# (found in ./VERSION)
src/version.cc: $(SRC_NON_GEN) VERSION cache/srcdate $(MAKEFILE)
	@echo "Generating $@"
	@printf '// DO NOT EDIT -- generated from VERSION\n\n' >$(@D)/.$(@F)
	@printf "extern const char *const yadex_source_date = \"%s\";\n" \
		`cat cache/srcdate` >>$(@D)/.$(@F)
	@printf "extern const char *const yadex_version = \"%s\";\n" 	\
		"$(VERSION)" >>$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@


# -------- Doc-related stuff --------

docdirs:
	@if [ ! -d doc ]; then mkdir doc; fi

# FIXME if $(M7SRC) changes, pixlist is not regenerated.
cache/pixlist: $(filter %.html, $(DOC2_M7))
	@echo "Generating $@"
	@test -d cache || mkdir cache
	@touch cache/.htmlimg
	@scripts/htmlimg $^ >$(@D)/.$(@F)
	@test -f cache/.htmlimg || mv $(@D)/.$(@F) $@

dummy:
	@if perl -v >/dev/null 2>/dev/null; then			\
	  perl -we '							\
	    use strict;							\
	    my %imgsrc;							\
	    undef $$/;							\
	    while (defined (my $$data = <>))				\
	    {								\
	      map { $$imgsrc{$$_} = undef }				\
		grep defined $$_ && $$_ !~ m{^\w+://},			\
		  $$data =~ m/<img\s[^>]*\bsrc=(?:"([[:graph:]]+?)"|([[:graph:]]+?)[\s>])/gis;\
	    }								\
	    print map "$$_\n", sort keys %imgsrc;			\
	    ' $^ >$(@D)/.$(@F)						\
	    && mv $(@D)/.$(@F) $@;					\
	elif [ -f $@ ]; then						\
	  echo "Sorry, you need Perl to refresh $@. Keeping old $@.";	\
	else								\
	  echo "Sorry, you need Perl to create $@. Creating empty $@.";	\
	  touch $@;							\
	fi

events.html: ev evhtml
	evhtml -- -n $< >$@

events.txt: events.html
	lynx -dump $< >$@

changes/changes.html: changes/*.log log2html RELEASE
	./log2html -- -r `cat RELEASE` -- $$(ls -r changes/*.log) >$@
	
# changes - update the changelog
.PHONY: changes
changes: changes/changes.html
	w3m -dump -cols 72 $< >CHANGES

# cns - view the changelog with Netscape
.PHONY: cns
cns:
	netscape -remote "openURL(file:$$(pwd)/changes/changes.html,new-window)"

# clynx - view the changelog with Lynx
.PHONY: clynx
clynx:
	lynx changes/changes.html

# cless - view the changelog with less
.PHONY: cless
cless:
	less CHANGES

# man - view the man page with man
.PHONY: man
man: doc/yadex.6
	man -l $^

# dvi - view the man page with xdvi
.PHONY: dvi
dvi: doc/yadex.dvi
	xdvi $^ 

# ps - view the man page with gv
.PHONY: ps
ps: doc/yadex.ps
	gv $^

# Use docsrc/faq.html and not directly
# doc/faq.html because we don't want FAQ to be
# remade at first build time.
FAQ: docsrc/faq.html.m7
	@test -d cache || mkdir cache
	$(OBJDIR)/m7 docsrc/common.m7 $< >cache/faq.html
	links -dump-width 72 -dump cache/faq.html >$@
	rm cache/faq.html

doc/yadex.dvi: doc/yadex.6
	groff -Tdvi -man $^ >$@

doc/yadex.ps: doc/yadex.6
	groff -Tps -man $^ >$@
	
#
#	Doc generation by processing with m7
#

# FIXME since m7src depends on the m7 executable, it's always remade the
# first time.
cache/m7src: $(OBJDIR)/m7 $(M7ROOTS) $(M7SRC)
	@echo "Generating $@"
	@test -d cache || mkdir cache
	@$(OBJDIR)/m7 -M $(M7ROOTS)					\
	  | sort							\
	  | uniq							\
	  | awk 'BEGIN{print"M7SRC = \\"}{print"\t"$$0" \\"}END{print""}'\
	  >$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

PROCESS =				\
	VERSION				\
	cache/copyright.man		\
	cache/copyright.txt		\
	cache/srcdate			\
	docsrc/common.m7		\
	$(OBJDIR)/files_etc.man		\
	$(OBJDIR)/files_share.man	\
	$(OBJDIR)/m7			\
	$(OBJDIR)/notexist

doc/README: docsrc/README.doc.m7 $(PROCESS)
	@echo "Generating $@"
	@test -d doc || mkdir doc
	@$(OBJDIR)/m7 docsrc/common.m7 $< >$(@D)/.$(@F) && mv $(@D)/.$(@F) $@

doc/%: docsrc/%.m7 $(PROCESS)
	@echo "Generating $@"
	@test -d doc || mkdir doc
	@$(OBJDIR)/m7 docsrc/common.m7 $< >$(@D)/.$(@F) && mv $(@D)/.$(@F) $@

%: docsrc/%.m7 $(PROCESS)
	@echo "Generating $@"
	@$(OBJDIR)/m7 docsrc/common.m7 $< >$(@D)/.$(@F) && mv $(@D)/.$(@F) $@

# The images are just copied from docsrc/ to doc/
doc/%.png: docsrc/%.png
	@echo "Creating $@"
	@rm -f $@ && cp -p $< $@

include cache/doc.dep

cache/doc.dep: $(PROCESS) cache/m7src $(M7SRC)
	@echo "Generating $@"
	@test -d cache || mkdir cache
	@for f in $(DOC2_M7); do					\
	  files="docsrc/common.m7 docsrc/$${f#doc/}.m7";		\
	  $(OBJDIR)/m7 -Ma -o $$f $$files				\
	    || {							\
	      echo "m7: error processing $$files" >&2;			\
	      exit 1;							\
	    };								\
	  done >$(@D)/.$(@F)
	@mv $(@D)/.$(@F) $@

#
#	Compile the "scripts"
#
$(OBJDIR)/%: $(OBJDIR)/%.o
	$(CC) $(LDFLAGS) $< -o $@
	
$(OBJDIR)/%: $(OBJDIR)/%.oo
	$(CXX) $(LDFLAGS) $< -o $@

$(OBJDIR)/%.o: scripts/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# -g because m7 is shaky. -O costs more than it buys.
$(OBJDIR)/%.oo: scripts/%.cc
	$(CXX) $(filter-out -O%, $(CXXFLAGS)) -g -c $< -o $@

