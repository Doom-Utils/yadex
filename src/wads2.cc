/*
 *	wads2.cc
 *	Wads functions that are not needed during editing.
 *	AYM 1998-08-09
 */


/*
This file is part of Yadex.

Yadex incorporates code from DEU 5.21 that was put in the public
domain in 1994 by Raphaël Quinet and Brendon Wyber.

The rest of Yadex is Copyright © 1997-1999 André Majorel.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307, USA.
*/


#include "yadex.h"
#include "wads.h"


static char *locate_pwad (const char *filename);


/*
   open the main wad file, read in its directory and create the
   master directory
*/

void OpenMainWad (const char *filename)
{
MDirPtr lastp, newp;
long n;
WadPtr wad;

/* open the wad file */
printf ("Loading main wad file: %s...\n", filename);
wad = BasicWadOpen (filename);
if (strncmp (wad->type, "IWAD", 4))
   fatal_error ("\"%s\" is not an iwad file", filename);

/* create the master directory */
lastp = NULL;
for (n = 0; n < wad->dirsize; n++)
   {
   newp = (MDirPtr) GetMemory (sizeof (struct MasterDirectory));
   newp->next = NULL;
   newp->wadfile = wad;
   memcpy (&(newp->dir), &(wad->directory[n]), sizeof (struct Directory));
   if (MasterDir)
      lastp->next = newp;
   else
      MasterDir = newp;
   lastp = newp;
   }

/* check if registered version */
if (FindMasterDir (MasterDir, "E2M1") == NULL
 && FindMasterDir (MasterDir, "MAP01") == NULL)
   {
   printf ("   *-----------------------------------------------------*\n");
   printf ("   | Warning: this is the shareware version of the game. |\n");
   printf ("   |     You won't be allowed to save your changes.      |\n");
   printf ("   |       PLEASE REGISTER YOUR COPY OF THE GAME.        |\n");
   printf ("   *-----------------------------------------------------*\n");
   Registered = 0; /* If you remove this, bad things will happen to you... */
   }
else
   Registered = 1;
}



/*
   open a patch wad file, read in its directory and alter the master
   directory
*/

void OpenPatchWad (const char *filename)
{
WadPtr wad;
MDirPtr mdir;
long n;
int state;
char entryname[9];
int replaces;
const char *entry_type;
char *real_name;
int nitems;		// Number of items in group of flats/patches

// Look for the file and ignore it if it doesn't exist
real_name = locate_pwad (filename);
if (real_name == NULL)
   {
   printf ("Warning: patch wad file \"%s\" doesn't exist. Ignored.\n",
      filename);
   return;
   }

/* open the wad file */
printf ("Loading patch wad file: %s...\n", real_name);
wad = BasicWadOpen (real_name);
if (strncmp (wad->type, "PWAD", 4))
   fatal_error ("\"%s\" is not a pwad file", real_name);
FreeMemory (real_name);

/* alter the master directory */

/* AYM 19970816 FIXME I think there is a big problem in this routine,
   it replaces entries too often. Entries within (S_START,S_END),
   (P_START,P_END), (F_START,F_END) can only replace entries in the
   same group. */
/* AYM: now, while the directory is scanned, a state variable is
   updated. its values are :
   0    no special state
   1-11 reading level lumps */
/* AYM 1998-11-15: FIXME: to be on the safe side, should consider
   FF_END to end a group of flats if the following entry is neither
   F_END nor F?_START. */

state = 0;
replaces = 0;
for (n = 0; n < wad->dirsize; n++)
   {
   strncpy (entryname, wad->directory[n].name, 8);
   entryname[8] = '\0';
   if (state == 0)
      {
      if (! strcmp (entryname, "F_START")
       || ! strcmp (entryname, "P_START")
       || ! strcmp (entryname, "S_START"))
	 {
	 entry_type = "marker";
	 replaces   = 0;
	 }
      // Deutex puts flats between FF_START and FF_END.
      // All lumps between those markers are assumed
      // to be patches.
      else if (*entryname == 'F' && ! strncmp (entryname + 2, "_START", 8))
	 {
	 state      = 'f';
	 entry_type = "group of flats";
	 replaces   = 0;
         nitems     = 0;
	 }
      // Deutex puts patches between PP_START and PP_END.
      // All lumps between those markers are assumed
      // to be patches.
      else if (! strncmp (entryname, "PP_START", 8))
	 {
	 state      = 'p';
	 entry_type = "group of patches";
	 replaces   = 0;
         nitems     = 0;
	 }
      else
	 {
	 mdir = FindMasterDir (MasterDir, entryname);
	 replaces = mdir != NULL;
	 /* if it is a level, do the same thing for the next 10 entries too */
	 if (levelname2levelno (entryname))
	    {
	    state = 11;
	    entry_type = "level";
	    }
	 else
	    entry_type = "entry";
	 }
      printf  ("  %s %s %s",
	 replaces ? "Updating" : "Adding new", entry_type, entryname);
      if ((*entry_type == 'm' || *entry_type == 'l') && wad->directory[n].size)
	 printf (" warning: non-zero length (%ld)", wad->directory[n].size);
      if (state != 'f' && state != 'p')
         putchar ('\n');
      }
   // F_END marks the end of a Deutex-generated group of flats.
   // Actually, Deutex uses FF_END instead but
   // you are expected to have replaced that with
   // F_END, as FF_END doesn't work with Doom.
   else if (state == 'f')
      {
      /* we ignore F?_END and F?_START */
      if (! strncmp (entryname, "F_END", 8))
         {
	 state = 0;
         printf (" (%d flats)\n", nitems);
         }
      // Of course, F?_START and F?_END don't count
      // toward the number of flats in the group.
      else if (! (*entryname == 'F'
                  && (! strncmp (entryname + 2, "_START", 6)
                      || ! strcmp (entryname + 2, "_END"))))
         nitems++;
      }
   // PP_END marks the end of a Deutex-generated group of patches.
   else if (state == 'p')
      {
      if (! strncmp (entryname, "PP_END", 8))
         {
         state = 0;
         printf (" (%d patches)\n", nitems);
         }
      // Of course, P?_START and P?_END don't count
      // toward the number of flats in the group.
      else if (! (*entryname == 'P'
                  && (! strncmp (entryname + 2, "_START", 6)
                      || ! strcmp (entryname + 2, "_END"))))
         nitems++;
      }
     
   /* if this entry is not in the master directory, then add it */
   if (!replaces)
      {
      mdir = MasterDir;
      while (mdir->next)
	 mdir = mdir->next;
      mdir->next = (MDirPtr) GetMemory (sizeof (struct MasterDirectory));
      mdir = mdir->next;
      mdir->next = NULL;
      }
   /* else, simply replace it */
   mdir->wadfile = wad;
   memcpy (&(mdir->dir), &(wad->directory[n]), sizeof (struct Directory));
   mdir = mdir->next;

   if (state > 0 && state <= 11)
      state--;
   }

}



/*
   close all the wad files, deallocating the wad file structures
*/

void CloseWadFiles ()
{
WadPtr curw, nextw;
MDirPtr curd, nextd;

/* close the wad files */
curw = WadFileList;
WadFileList = NULL;
while (curw)
   {
   nextw = curw->next;
   if (curw->fd != NULL)
      fclose (curw->fd);
   if (curw->directory != NULL)
      FreeMemory (curw->directory);
   if (curw->filename != NULL)
      FreeMemory (curw->filename);
   FreeMemory (curw);
   curw = nextw;
   }

/* delete the master directory */
curd = MasterDir;
MasterDir = NULL;
while (curd)
   {
   nextd = curd->next;
   FreeMemory (curd);
   curd = nextd;
   }
}



/*
   forget unused patch wad files
*/

void CloseUnusedWadFiles ()
{
WadPtr curw, prevw;
MDirPtr mdir;

prevw = NULL;
curw = WadFileList;
while (curw)
   {
   /* check if the wad file is used by a directory entry */
   mdir = MasterDir;
   while (mdir && mdir->wadfile != curw)
      mdir = mdir->next;
   if (mdir)
      prevw = curw;
   else
      {
      /* if this wad file is never used, close it */
      if (prevw)
	 prevw->next = curw->next;
      else
	 WadFileList = curw->next;
      fclose (curw->fd);
      FreeMemory (curw->directory);
      FreeMemory (curw->filename);
      FreeMemory (curw);
      }
   curw = prevw->next;
   }
}



/*
   basic opening of wad file and creation of node in Wad linked list
*/

WadPtr BasicWadOpen (const char *filename)
{
WadPtr curw, prevw;


/* find the wad file in the wad file list */
prevw = WadFileList;
if (prevw)
   {
   curw = prevw->next;
   while (curw && fncmp (filename, curw->filename))
      {
      prevw = curw;
      curw = prevw->next;
      }
   }
else
   curw = NULL;

/* if this entry doesn't exist, add it to the WadFileList */
if (curw == NULL)
   {
   curw = (WadPtr) GetMemory (sizeof (struct WadFileInfo));
   if (prevw == NULL)
      WadFileList = curw;
   else
      prevw->next = curw;
   curw->next = 0;	// NULL
   curw->directory = 0;	// NULL
   {
   static int time;
   time++;
   }
   curw->filename = (char *) GetMemory (strlen (filename) + 1);
   strcpy (curw->filename, filename);
   }


/* open the file */
if ((curw->fd = fopen (filename, "rb")) == NULL)
   fatal_error ("error opening \"%s\"", filename);

/* read in the wad directory info */
wad_read_bytes (curw, curw->type, 4);
if (strncmp (curw->type, "IWAD", 4) && strncmp (curw->type, "PWAD", 4))
   fatal_error ("\"%s\" is not a valid wad file", filename);
wad_read_i32 (curw, &curw->dirsize );
wad_read_i32 (curw, &curw->dirstart);
verbmsg ("  Type %.4s, directory has %ld entries at offset %08lXh\n",
   curw->type, (long) curw->dirsize, (long) curw->dirstart);

/* read in the wad directory itself */
curw->directory = (DirPtr) GetMemory ((long) sizeof (struct Directory)
   * curw->dirsize);
wad_seek (curw, curw->dirstart);
DirPtr d = curw->directory;
for (i32 n = 0; n < curw->dirsize; n++)
   {
   wad_read_i32 (curw, &d->start);
   wad_read_i32 (curw, &d->size );
   wad_read_bytes (curw, d->name, 8);
   //verbmsg ("    %-8.8s %08lXh %d\n", d->name, (long) d->start, (long) d->size);
   d++;
   }

/* all done */
return curw;
}


/*
   list the master directory
*/

void ListMasterDirectory (FILE *file)
{
char dataname[9];
MDirPtr dir;
char key;
int lines = 3;

dataname[8] = '\0';
fprintf (file, "The Master Directory\n");
fprintf (file, "====================\n\n");
fprintf (file, "NAME____  FILE______________________________________________"
   "  SIZE__  START____\n");
for (dir = MasterDir; dir; dir = dir->next)
   {
   strncpy (dataname, dir->dir.name, 8);
   fprintf (file, "%-8s  %-50s  %6ld  x%08lx\n",
     dataname, dir->wadfile->filename, dir->dir.size, dir->dir.start);
   if (file == stdout && lines++ > screen_lines - 4)
      {
      lines = 0;
      printf ("['Q' followed by Return to abort, Return only to continue]");
      key = getchar ();
      printf ("\r                                                         \r");
      if (key == 'Q' || key == 'q')
	 {
         getchar ();  // Read the '\n'
         break;
         }
      }
   }
}



/*
   list the directory of a file
*/

void ListFileDirectory (FILE *file, WadPtr wad)
{
char dataname[9];
char key;
int lines = 5;
long n;

dataname[8] = '\0';
fprintf (file, "Wad File Directory\n");
fprintf (file, "==================\n\n");
fprintf (file, "Wad File: %s\n\n", wad->filename);
fprintf (file, "NAME____  SIZE__  START____  END______\n");
for (n = 0; n < wad->dirsize; n++)
   {
   strncpy (dataname, wad->directory[n].name, 8);
   fprintf (file, "%-8s  %6ld  x%08lx  x%08lx\n", dataname,
     wad->directory[n].size, wad->directory[n].start,
     wad->directory[n].size + wad->directory[n].start - 1);
   if (file == stdout && lines++ > screen_lines - 4)
      {
      lines = 0;
      printf ("['Q' followed by Return to abort, Return only to continue]");
      key = getchar ();
      printf ("\r                                                         \r");
      if (key == 'Q' || key == 'q')
         {
         getchar ();  // Read the '\n'
	 break;
         }
      }
   }
}



/*
   build a new wad file from master dictionary
*/

void BuildNewMainWad (const char *filename, Bool patchonly)
{
FILE *file;
long counter = 12;
MDirPtr cur;
long size;
long dirstart;
long dirnum;

/* open the file and store signatures */
if (patchonly)
   printf ("Building a compound Patch Wad file \"%s\".\n", filename);
else
   printf ("Building a new Main Wad file \"%s\" (size approx 10000K)\n",
      filename);
if (FindMasterDir (MasterDir, "E2M4") == NULL
 && FindMasterDir (MasterDir, "MAP01") == NULL)  /* AYM */
   fatal_error ("You were warned: you are not allowed to do this.");
if ((file = fopen (filename, "wb")) == NULL)
   fatal_error ("unable to open file \"%s\"", filename);
if (patchonly)
   WriteBytes (file, "PWAD", 4);
else
   WriteBytes (file, "IWAD", 4);
WriteBytes (file, &counter, 4L);      /* put true value in later */
WriteBytes (file, &counter, 4L);      /* put true value in later */

/* output the directory data chuncks */
for (cur = MasterDir; cur; cur = cur->next)
   {
   if (patchonly && cur->wadfile == WadFileList)
      continue;
   size = cur->dir.size;
   counter += size;
   wad_seek (cur->wadfile, cur->dir.start);
   CopyBytes (file, cur->wadfile->fd, size);
   printf ("Size: %luK\r", counter / 1024);
   }

/* output the directory */
dirstart = counter;
counter = 12;
dirnum = 0;
for (cur = MasterDir; cur; cur = cur->next)
   {
   if (patchonly && cur->wadfile == WadFileList)
      continue;
   if (dirnum % 100 == 0)
      printf ("Outputting directory %04ld...\r", dirnum);
   if (cur->dir.start)
      WriteBytes (file, &counter, 4L);
   else
      WriteBytes (file, &(cur->dir.start), 4L);
   WriteBytes (file, &(cur->dir.size), 4L);
   WriteBytes (file, &(cur->dir.name), 8L);
   counter += cur->dir.size;
   dirnum++;
   }

/* fix up the number of entries and directory start information */
if (fseek (file, 4L, 0))
   fatal_error ("error writing to file");
WriteBytes (file, &dirnum, 4L);
WriteBytes (file, &dirstart, 4L);

/* close the file */
printf ("                            \r");
fclose (file);
}


/*
   dump a directory entry in hex
*/

void DumpDirectoryEntry (FILE *file, const char *entryname)
{
MDirPtr entry;
char dataname[9];
char key;
int lines = 5;
long n, c, i;
unsigned char buf[16];

c = 0;
entry = MasterDir;
while (entry)
   {
   if (!strnicmp (entry->dir.name, entryname, 8))
      {
      strncpy (dataname, entry->dir.name, 8);
      dataname[8] = '\0';
      fprintf (file, "Contents of entry %s (size = %ld bytes):\n", dataname, entry->dir.size);
      wad_seek (entry->wadfile, entry->dir.start);
      n = 0;
      i = -1;
      for (c = 0; c < entry->dir.size; c += i)
	 {
	 fprintf (file, "%04lX: ", n);
	 for (i = 0; i < 16; i++)
	    {
	    wad_read_bytes (entry->wadfile, &(buf[i]), 1);
	    fprintf (file, " %02X", buf[i]);
	    n++;
	    }
	 fprintf (file, "   ");
	 for (i = 0; i < 16; i++)
	    {
	    if (buf[i] >= 0x20
               && buf[i] != 0x7f
#ifdef Y_UNIX
               && ! (buf[i] >= 0x80 && buf[i] <= 0xa0)  // ISO 8859-1
#endif
               )
	       fprintf (file, "%c", buf[i]);
	    else
	       fprintf (file, ".");
	    }
	 fprintf (file, "\n");
	 if (file == stdout && lines++ > screen_lines - 4)
	    {
	    lines = 0;
            /* FIXME: the message should indicate that Return is needed */
	    printf ("[%d%% - Q + Return to abort,"
		" S + Return to skip this entry,"
		" Return to continue]", (int) (n * 100 / entry->dir.size));
	    key = getchar ();
	    printf ("\r                                                                    \r");
	    if (key == 'S' || key == 's')
               {
               getchar ();  // Read the '\n'
	       break;
               }
	    if (key == 'Q' || key == 'q')
               {
               getchar ();  // Read the '\n'
	       return;
               }
	    }
	 }
      }
   entry = entry->next;
   }
if (! c)
   {
   printf ("[Entry not in master directory]\n");
   return;
   }
}



/*
   save a directory entry to disk
*/

void SaveDirectoryEntry (FILE *file, const char *entryname)
{
MDirPtr entry;
long    counter;
long    size;

for (entry = MasterDir; entry; entry = entry->next)
   if (!strnicmp (entry->dir.name, entryname, 8))
      break;
if (entry)
   {
   WriteBytes (file, "PWAD", 4L);     /* PWAD file */
   counter = 1L;
   WriteBytes (file, &counter, 4L);   /* 1 entry */
   counter = 12L;
   WriteBytes (file, &counter, 4L);
   counter = 28L;
   WriteBytes (file, &counter, 4L);
   size = entry->dir.size;
   WriteBytes (file, &size, 4L);
   WriteBytes (file, &(entry->dir.name), 8L);
   wad_seek (entry->wadfile, entry->dir.start);
   CopyBytes (file, entry->wadfile->fd, size);
   }
else
   {
   printf ("[Entry not in master directory]\n");
   return;
   }
}



/*
   save a directory entry to disk, without a pwad header
*/

void SaveEntryToRawFile (FILE *file, const char *entryname)
{
MDirPtr entry;

for (entry = MasterDir; entry; entry = entry->next)
   if (!strnicmp (entry->dir.name, entryname, 8))
      break;
if (entry)
   {
   wad_seek (entry->wadfile, entry->dir.start);
   CopyBytes (file, entry->wadfile->fd, entry->dir.size);
   }
else
   {
   printf ("[Entry not in master directory]\n");
   return;
   }
}


/*
   encapsulate a raw file in a pwad file
*/

void SaveEntryFromRawFile (FILE *file, FILE *raw, const char *entryname)
{
long    counter;
long    size;
char    name8[8];

for (counter = 0L; counter < 8L; counter++)
   name8[counter] = '\0';
strncpy (name8, entryname, 8);
WriteBytes (file, "PWAD", 4L);     /* PWAD file */
counter = 1L;
WriteBytes (file, &counter, 4L);   /* 1 entry */
counter = 12L;
WriteBytes (file, &counter, 4L);
counter = 28L;
WriteBytes (file, &counter, 4L);
if (fseek (raw, 0L, SEEK_END) != 0)
   fatal_error ("error reading from raw file");
size = ftell (raw);
if (size < 0)
   fatal_error ("error reading from raw file");
if (fseek (raw, 0L, SEEK_SET) != 0)
   fatal_error ("error reading from raw file");
WriteBytes (file, &size, 4L);
WriteBytes (file, name8, 8L);
CopyBytes (file, raw, size);
}


/*
 *	locate_pwad
 *	Look for a PWAD in the standard directories
 *	and returns its name in a GetMemory'd buffer
 *	(or NULL if not found). It's up to the caller
 *	to free the buffer after use.
 */


/* Directories that are searched for PWADs */
static const char *standard_directories[] =
   {
   "",
   "~/",                            // "~" means "the user's home directory"
   "/usr/local/share/games/%s/",    // %s is replaced by <Game>
   "/usr/share/games/%s/",          // %s is replaced by <Game>
   "/usr/local/share/games/wads/",
   "/usr/share/games/wads/",
   0
   };


static char *locate_pwad (const char *filename)
{
al_fext_t ext;
const char **dirname;
char *real_basename;
char *real_name;

// Get the extension in <ext>
al_fana (filename, NULL, NULL, NULL, ext);

// If it's an absolute name, stop there.
if (is_absolute (filename))
   {
   real_name = (char *) GetMemory (strlen (filename) + 1 + (*ext ? 0 : 4));
   strcpy (real_name, filename);
   if (! *ext)
      strcat (real_name, ".wad");
   int r = Exists (real_name);
   if (! r)
      {
      FreeMemory (real_name);
      return 0;
      }
   return real_name;
   }

// It's a relative name.
// If no extension given, append ".wad"
real_basename = (char *) GetMemory (strlen (filename) + 1 + (*ext ? 0 : 4));
strcpy (real_basename, filename);
if (! *ext)
   strcat (real_basename, ".wad");

// Then search for a file of that
// name in the standard directories.
real_name = (char *) GetMemory (257);
for (dirname = standard_directories; *dirname; dirname++)
   {
   if (! strcmp (*dirname, "~/"))
      if (getenv ("HOME"))
         {
	 al_scps (real_name, getenv ("HOME"), 256);
         al_sapc (real_name, '/', 256);
         }
      else
	 continue;
   else
#ifdef Y_SNPRINTF
      snprintf (real_name, 257, *dirname, Game ? Game : "");
#else
      sprintf (real_name, *dirname, Game ? Game : "");
#endif
   al_saps (real_name, real_basename, 256);
   verbmsg ("  Trying \"%s\"... ", real_name);
   if (Exists (real_name))
      {
      verbmsg ("right on !\n");
      FreeMemory (real_basename);
      return real_name;
      }
   verbmsg ("nuts\n");
   }
FreeMemory (real_name);
FreeMemory (real_basename);
return NULL;
}


/* end of file */

