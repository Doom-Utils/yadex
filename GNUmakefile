#
#	Makefile for Yadex
#	Copyright © André Majorel 1998.
#	AYM 1998-06-10
#

# Requires GNU make ! (uses pattern rules, addprefix, addsuffix, etc.)
# Tested with Linux 2.0.29/2.0.34 and GCC 2.7.2.1/2.7.2.3.
# Might need modifications to work on other platforms.

DEFINES=

#
#	Definitions that end users
#	might want to change
#

# Where you want "make install" to put things.
# Typical values : "/usr", "/usr/local" and "/opt".
PREFIX=/usr/local

# Other compilers than GCC will probably not understand this.
WARNINGS=-Wall -Wno-parentheses -Wpointer-arith -Wcast-qual -Wcast-align\
	-Wwrite-strings -Wmissing-declarations -Wmissing-prototypes -Winline

# If your C library doesn't have snprintf(), comment this line out.
DEFINES+=-DY_SNPRINTF

# Your C and C++ compilers.
CC=cc
CXX=c++

#
#	Definitions that only hackers
#	might want to change
#

# All the modules of Yadex without path or extension.
MODULES_YADEX=\
acolours	aym		cfgfile		checks\
clearimg	colour1		colour2		colour3\
colour4		dialog		dispimg		disppic\
drawmap		edisplay	editgrid	editlev\
editloop	editobj		editzoom	entry\
events		flats		game		gcolour1\
gcolour2	geom		gfx		gfx2\
gotoobj		help1		help2		highlt\
infobar		input		l_align		l_centre\
l_flags		l_misc		l_prop		l_unlink\
l_vertices	levels		lists		memory\
menubar		menu		mkpalette	mouse\
names\
nop		objects		objinfo		oldmenus\
pic2img		prefer		s_centre	s_door\
s_lift		s_linedefs	s_merge		s_misc\
s_prop		s_split		s_vertices	sanity\
savepic		scrnshot	selbox		selectn\
sprites		swapmem		t_centre	t_flags\
t_prop		t_spin		textures	things\
v_centre	v_merge		v_polyg		verbmsg\
version		wads		wads2		x_centre\
x_rotate	yadex

# All the modules of Atclib without path or extension.
MODULES_ATCLIB=\
al_adigits	al_aerrno	al_astrerror	al_fana\
al_fnature	al_lateol	al_lcount	al_lcreate\
al_ldelete	al_ldiscard	al_lgetpos	al_linsert\
al_linsertl	al_llength	al_lpeek	al_lpeekl\
al_lpoke	al_lpokel	al_lptr		al_lread\
al_lreadl	al_lrewind	al_lseek	al_lsetpos\
al_lstep	al_ltell	al_lwrite	al_lwritel\
al_sapc		al_saps		al_scps		al_scpslower\
al_strolc

# All the modules of BSP without path or extension.
MODULES_BSP=\
bsp		funcs		makenode	picknode

# The source files of Yadex.
SRC_YADEX=$(addprefix src/,$(addsuffix .cc,$(MODULES_YADEX)))

# The source files of Atclib.
SRC_ATCLIB=$(addprefix atclib/,$(addsuffix .c,$(MODULES_ATCLIB)))

# The source files of BSP.
SRC_BSP=$(addprefix bsp-2.3/,$(addsuffix .c,$(MODULES_BSP)))

# The headers of Yadex.
HEADERS_YADEX:=$(wildcard src/*.h)

# The headers of Atclib.
HEADERS_ATCLIB=atclib/atclib.h

# The headers of BSP.
HEADERS_BSP=bsp-2.3/bsp.h bsp-2.3/structs.h

# All the source files of both, including the headers.
SRC=$(SRC_YADEX)  $(HEADERS_YADEX)\
    $(SRC_ATCLIB) $(HEADERS_ATCLIB)\
    $(SRC_BSP)    $(HEADERS_BSP)

# The files on which youngest is run.
SRC_NON_GEN=$(filter-out src/version.cc,$(SRC))

# Options used when compiling Atclib.
CFLAGS=

# Options used when compiling and linking Yadex.
# ld is invoked through the C++ compiler
# so LDFLAGS should not contain options that mean
# something to the C++ compiler.
CXXFLAGS=
LDFLAGS=

# Defines used when compiling Yadex.
# If you change Y_UNIX for Y_DOS or Y_X11 for Y_BGI,
# don't even expect it to compile. As of this release,
# the DOS/BGI version is _broken_.
DEFINES+=-DY_UNIX -DY_X11 -DY_BETA

# The name of the directory where objects and binaries
# are put. I include the output of "uname -a" to make
# it easier for me to build Yadex for different platforms
# from the same source tree.
OBJDIR:=obj_$(shell uname -a | tr -c "[:alnum:]._-" "[_*]")
OBJDIR_ATCLIB=$(OBJDIR)/atclib
OBJ_YADEX=$(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODULES_YADEX)))
OBJ_ATCLIB=$(addprefix $(OBJDIR_ATCLIB)/,$(addsuffix .o,$(MODULES_ATCLIB)))

# The game definition files.
YGD=doom.ygd doom2.ygd heretic.ygd

# The doc files.
DOC=README COPYING COPYING.LIB HISTORY TODO VERSION\
	yadex.6\
	yadex.lsm\
	users_guide.html\
	hackers_guide.html\
	bsp-2.3/bsp.6

# The source of the doc files.
DOC_SRC=$(addprefix docsrc/,\
	README yadex.6 yadex.lsm users_guide.html hackers_guide.ch)

# The script files.
SCRIPTS=$(addprefix scripts/,mkinstalldirs youngest)

# Where the binary is put.
BINDIR=$(OBJDIR)

# The name of the archive.
ARCHIVE:=yadex-$(shell cat VERSION)

# Where "make install" puts things.
INST_BINDIR=$(PREFIX)/bin
ifeq ($(PREFIX),/usr)	# Special case: not /usr/etc but /etc
INST_CFGDIR=/etc
else
INST_CFGDIR=$(PREFIX)/etc
endif
INST_YGDDIR=$(PREFIX)/share/games
INST_MANDIR=$(PREFIX)/man/man6

#
#	Targets for
#	end users.
#

.PHONY: all
all: yadex.dep dirs $(BINDIR)/yadex $(BINDIR)/bsp $(YGD)

.PHONY: install
install: all
	@echo
	@scripts/mkinstalldirs $(INST_BINDIR)
	@scripts/mkinstalldirs $(INST_CFGDIR)
	@scripts/mkinstalldirs $(INST_MANDIR)
	@scripts/mkinstalldirs $(INST_YGDDIR)
	@echo Installing binaries in $(INST_BINDIR)
	@cp $(BINDIR)/yadex	$(INST_BINDIR)	2>&1
	@cp $(BINDIR)/bsp	$(INST_BINDIR)	2>&1
	@echo Installing man pages in $(INST_MANDIR)
	@cp yadex.6		$(INST_MANDIR)	2>&1
	@cp bsp-2.3/bsp.6	$(INST_MANDIR)	2>&1
	@echo Installing game definition files in $(INST_YGDDIR)
	@cp $(YGD) 		$(INST_YGDDIR)	2>&1
	@echo Installing configuration file in $(INST_CFGDIR)
	@cp yadex.cfg		$(INST_CFGDIR)	2>&1
	@chmod a+w $(INST_CFGDIR)/yadex.cfg	2>&1
	@echo "----------------------------------------------------------------"
	@echo "  Yadex is now installed."
	@echo
	@echo "  Before you run it, you're going to have to make sure it finds"
	@echo "  your iwads. You can do this by editing the lines \"iwad1=\","
	@echo "  \"iwad2=\" and \"iwad3=\" in $(INST_CFGDIR)/yadex.cfg."
	@echo
	@echo "  When you're done, type \"yadex\" to start."
	@echo "  If you're confused, take a look at \"users_guide.html\"."
	@echo
	@echo "  Good editing !"
	@echo "----------------------------------------------------------------"

$(BINDIR)/yadex: $(OBJ_YADEX) $(OBJ_ATCLIB)
	@echo
	@echo Linking Yadex
	@$(CXX) $(OBJ_YADEX) $(OBJ_ATCLIB) -o $@\
	  -L/usr/X11R6/lib -L/usr/local/lib -lc -lm -lX11 $(LDFLAGS) 2>&1

$(BINDIR)/bsp: $(SRC_BSP) $(HEADERS_BSP)
	@echo
	@echo Compiling and linking BSP
	@$(CC) -Wl,-s bsp-2.3/bsp.c -Wall -Winline -O2 -finline-functions\
		-ffast-math -lm -o $@

.PHONY: clean
clean:
	rm -r $(OBJDIR)

.PHONY: doc
doc: $(DOC)

#
#	Targets meant for
#	hackers only.
#

.PHONY: save
save:
	#echo '*.wad'				>/var/tmp/yadex.tmp
	#echo '*~'				>>/var/tmp/yadex.tmp
	#echo 'core'				>>/var/tmp/yadex.tmp
	#echo 'obj/*'				>>/var/tmp/yadex.tmp
	#echo 'yadex'				>>/var/tmp/yadex.tmp
	#echo yadex-$$(date '+%Y%m%d').tgz	>>/var/tmp/yadex.tmp
	#cat /var/tmp/yadex.tmp
	tar -zcvf yadex-$$(date '+%Y%m%d').tgz\
		--exclude "*.wad"\
		--exclude "core"\
		--exclude yadex-$$(date '+%Y%m%d').tgz\
		--exclude "dos/*"\
		--exclude "obj_*/*"\
		--exclude "*~"\
		--exclude "*.zip"\
		--exclude "old/*"\
		--exclude "*.bak"\
		--exclude yadex\
		.

# Recompile with debugging info, install and run.
# You need GNU make >= 3.77.
.PHONY: db
db: CFLAGS+=-g
db: CXXFLAGS+=-g
db: all install
	yadex

# Recompile with debugging info + Electric Fence,
# install and run. You need GNU make >= 3.77.
.PHONY: ef
ef: CFLAGS+=-g
ef: CXXFLAGS+=-g
ef: LDFLAGS+=-lefence
ef: all install
	yadex

# So that GNU make < 3.77 doesn't barf.
.PHONY: CFLAGS+=-g
.PHONY: CXXFLAGS+=-g
.PHONY: LDFLAGS+=-lefence

# Generate the distribution archive.
# Requires gzip, GNU tar and GNU cp.
.PHONY: dist
dist: all doc
	@echo Creating distribution tree $(ARCHIVE)
	@if [ -e $(ARCHIVE) ]; then echo Error: $(ARCHIVE) already exists'!';\
		false; fi
	@scripts/mkinstalldirs $(ARCHIVE)
	@cp -P $(sort $(DOC) $(DOC_SRC) $(SRC) $(YGD) $(SCRIPTS)\
		GNUmakefile yadex.cfg) $(ARCHIVE)
	@chmod -R 666 $(ARCHIVE)
	@chmod -R 777 $(addprefix $(ARCHIVE)/,$(SCRIPTS))
	@find $(ARCHIVE) -type d -exec chmod 777 '{}' ';'
	@chown -R 1000 $(ARCHIVE)
	@chgrp -R 1000 $(ARCHIVE)
	@echo Creating archive $(ARCHIVE).tar.gz
	@tar -czf $(ARCHIVE).tar.gz $(ARCHIVE)
	@echo Removing distribution tree $(ARCHIVE)
	@rm -r $(ARCHIVE)

#
#	Internal targets, not meant
#	to be invoked directly
#

# Dependencies of the modules of Yadex
# -Y is here to prevent the inclusion of dependencies on
# /usr/include/*.h etc. As a side-effect, it generates many
# warnings. We use "grep -v" to filter them out and "|| true"
# to prevent the non-zero exit status of grep from being
# seen by make.
# Note: the modules of Atclib are not scanned as they all
# depend on $(HEADERS_ATCLIB) and nothing else.

yadex.dep: $(SRC_YADEX)
	@echo
	@echo makedepend
	@makedepend -f- -p$(OBJDIR)/ -Y -Iatclib $(DEFINES) $(SRC_YADEX)\
		2>/dev/null | sed -e "s:/src::" >$@

# Directories where objects and binaries are put.
.PHONY: dirs
dirs:
	@if [ ! -d $(OBJDIR) -o ! -d $(OBJDIR_ATCLIB) -o ! -d $(BINDIR) ];\
	then\
		echo Creating object directories;\
		scripts/mkinstalldirs $(OBJDIR)        2>&1;\
		scripts/mkinstalldirs $(OBJDIR_ATCLIB) 2>&1;\
		scripts/mkinstalldirs $(BINDIR)        2>&1;\
	fi

# To compile the modules of Yadex
include yadex.dep
$(OBJDIR)/%.o: src/%.cc
	@echo
	@echo $<
	@$(CXX) $(CXXFLAGS) -c -Iatclib $(DEFINES) $(WARNINGS) $< -o $@ 2>&1

# To compile the modules of Atclib
$(OBJDIR_ATCLIB)/%.o: atclib/%.c $(HEADERS_ATCLIB)
	@echo
	@echo $<
	@$(CC) $(CFLAGS) -c -O $(WARNINGS) $< -o $@ 2>&1

# A source file containing just the date of the
# most recent source file and the version number
# (found in ./VERSION).
src/version.cc: $(SRC_NON_GEN) VERSION
	@echo
	@echo Generating $@
	@echo -n "extern const char *const yadex_source_date = \"" >$@
	@scripts/youngest $(SRC_NON_GEN) >>$@
	@echo "\";" >>$@
	@echo -n "extern const char *const yadex_version = \"" >>$@
	@echo -n $$(cat VERSION) >>$@
	@echo "\";" >>$@

# Generate the text format doc by replacing all occurences of
# - "$DATE" by the system date,
# - "$SOURCE_DATE" by the date of the last modified source file,
# - "$VERSION" by the version number.
%: docsrc/% VERSION
	@echo
	@echo Generating $@
	@sed -e "s/\$$DATE/$$(date +%Y-%m-%d)/;\
		s/\$$SOURCE_DATE/$$(scripts/youngest $(SRC_NON_GEN))/;\
		s/\$$VERSION/$$(cat VERSION)/" $< >$@

%.html: docsrc/%.html VERSION
	@echo
	@echo Generating $@
	@sed -e "s/\$$DATE/$$(date +%Y-%m-%d)/;\
		s/\$$SOURCE_DATE/$$(scripts/youngest $(SRC_NON_GEN))/;\
		s/\$$VERSION/$$(cat VERSION)/" $< >$@

# Generate the HTML format doc by processing it with chtml.
# Well, for the moment, just cp :-|.
%.html: docsrc/%.ch
	@echo
	@echo Generating $@
	@cp $< $@


