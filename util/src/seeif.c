
/******************************************************************

		Copyright Tim Cox, 2015

		TimMilesCox@gmx.ch       

	This source code seeif.c is a multiple file compare
	used in POSIX bash shells and in DOS and Windows command
	prompt

	seeif is free software licensed
	with the GNU General Public Licence Version 3

	The same licence encompasses all accompanying software
	and documentation

	The full licence text is included with these materials

        See also the licensing notice at the foot of this document

*******************************************************************/

/******************************************************************

	seeif is similar to diff and also optionally recurses
	subdirectories

	the two command-line comparands are two files or two
	directories

	default is silent operation with output only on file
	difference. The intention is easy checking of many hundreds
	of files

	more or less output may be opted

	resynchronisation may be opted for text files in order to
	persist after a first mismatch

	whitespace fields in text files may be normalised to a
	single space for comparison and blank lines dropped

	differences in binary files may shown in a dump of a
	small containing page. Persistence may be opted to
	keep comparing after the first difference

	to build seeif for

	unix / POSIX / linux
	____________________

	[gcc -m32 -funsigned-char -c posix2tx.c]
	[gcc -m32 -funsigned-char -c argue.c]
	gcc -m32 -funsigned-char -o [PATH]/seeif argue.o posix2tx.o seeif.c


	Windows
	_______

	[pushd ../win32.src
	 cl /J /c dirent.c
	 popd]

	[cl /J /c posix2tx.c]
	[cl /J /c argue.c]
	cl /J /DMSW /Fe[PATH\]seeif argue.obj ../win32.src/dirent.obj posix2tx.obj seeif.c



******************************************************************/


#include <stdio.h>
#include <errno.h>

#ifdef DOS
#include <fcntl.h>
#include "../win32.src/dirent.h"
typedef long off_t;
#else
#include <fcntl.h>
#include "dirent.h"
#endif

#include "../include/argue.h"
#include "../include/posix2s.h"

#define	DATA	360
#define PATH	720
#define	BLOCK	32
#define	LINE	720
#define	SEARCH	8

static char		 left[PATH],
			right[PATH];

static char		*p1 = left,
			*p2 = right;

static int		 line1,
			 line2;


static char *pend(char *p, char *q)
{
   while (*p = *q++) p++;
   return p;
}

static char *append(char *p, char *q)
{
   *p++ = '/';
   return pend(p, q);
}

static int symbol(int x)
{
   if (x < ' ') return '.';
   if (x > 126) return '.';
   return x;
}


static int realign(int f1, int f2,
                   posix2s_block *b1, posix2s_block *b2,
                   char *txline1, char *txline2)
{
   char			 text1[LINE],
			 text2[LINE];

   char			 text2here[LINE];

   int			 x,
                         lines2_display = 1,
			 traverse, 
			 search = SEARCH;

   off_t		 departure2 = posix2s_page(b2);
   int			 offset2 = posix2s_offset(b2);
   int			 position2 = line2;

   char			*p = txline1;



   while (search--)
   {
      traverse = SEARCH;

      while (traverse--)
      {
         for (;;)
         {
            line2++;
            x = posix2textline(f2, text2, LINE-1, uflag['W'-'A'], b2);

            if (x == 0) break;

            if (text2[0] == '\n') continue;
            if ((text2[1] == '\n') && (text2[0] == ' ')) continue;
            break;
         }

         if (x == 0) break;

         x = strcmp(p, text2);

         if (x == 0)
         {
            if (flag['v'-'a'])
            {
               x = posix2s_point(f2, offset2, (off_t) departure2, b2);
               if (x < 0) printf("[*%d]\n", errno);
               else
               {
                  lines2_display = position2 + 1;

                  while (lines2_display < line2)
                  {
                     x = posix2textline(f2, text2here, LINE-1, uflag['W'-'A'], b2);
                     printf("            %s: %d: %s", right, lines2_display, text2here);
                     lines2_display++;
                  }

                  x = posix2textline(f2, text2here, LINE-1, uflag['W'-'A'], b2);
               }

               printf("          | %s: %d: %s", right, line2, text2);
               printf("realigned | %s: %d: %s",  left, line1, p);
            }

            return 0;		/* equal lines have been found and consumed */
         }
         #if 0
         else if (flag['v'-'a'] & lines2_display) printf("            %s: %d: %s", right, line2, text2);
         #endif
      }

      #if 0
      lines2_display = 0;
      #endif

      for (;;)
      {
         line1++;
         x = posix2textline(f1, text1, LINE-1, uflag['W'-'A'], b1);

         if (x == 0) break;

         if (text1[0] == '\n') continue;
         if ((text1[1] == '\n') && (text1[0] == ' ')) continue;
         break;
      }

      if (x == 0) break;

      x = posix2s_point(f2, offset2, (off_t) departure2, b2);
      if (x < 0) printf("[*%d]\n", errno);
      line2 = position2;

      x = strcmp(text1, txline2);

      if (x == 0)
      {
         if (flag['v'-'a'])
         {
            printf("          | %s: %d: %s", left, line1, text1);
            printf("realigned | %s: %d: %s", right, position2, txline2);
         }
         
         return 0;           /* equal lines have been found and consumed */
      }
      else if (flag['v'-'a']) printf("            %s: %d: %s", left, line1, text1);

      p = text1;
   }

   traverse = SEARCH;

   while (traverse--)
   {
      for (;;)
      {
         line2++;
         x = posix2textline(f2, text2, LINE-1, uflag['W'-'A'], b2);
         if (x == 0) break;

         if (text2[0] == '\n') continue;
         if ((text2[1] == '\n') && (text2[0] == ' ')) continue;
         break;
      }

      if (x == 0) break;
      printf("            %s: %d: %s", right, line2, text2);
   }

   return -1;
}

static int text_compare(int f1, int f2)
{
   static posix2s_block	 block1,
			 block2;

   char			 text1[LINE],
			 text2[LINE];

   int			  status = 0;

   int			 x,
			 y,
			 j;


   /******************************************************************

	force a restart of the static buffer blocks

   ******************************************************************/

   block1.wcursor = 0;
   block2.wcursor = 0;

   line1 = 0;
   line2 = 0;

   for (;;)
   {
      for (;;)
      {
         line1++;
         x = posix2textline(f1, text1, LINE-1, uflag['W'-'A'], &block1);
         if (x == 0) break;
         if (uflag['W'-'A'] == 0) break;
         if (text1[0] == '\n') continue;
         if ((text1[1] == '\n') && (text1[0] == ' ')) continue;
         break;
      }

      for (;;)
      {
         line2++;
         y = posix2textline(f2, text2, LINE-1, uflag['W'-'A'], &block2);
         if (y == 0) break;
         if (uflag['W'-'A'] == 0) break;
         if (text2[0] == '\n') continue;
         if ((text2[1] == '\n') && (text2[0] == ' ')) continue;
         break;
      }

      status |= (x ^ y);

      if (j = strcmp(text1, text2))
      {
         status |= j;

         if (flag['v'-'a'])

         {
            if      (y) printf("          * %s: %d: %s", right, line2, text2);
            else if (x) printf("       eof* %s\n", right);
            if      (x) printf("difference* %s: %d: %s",  left, line1, text1);
            else if (y) printf("       eof* %s\n",  left);
         }

         if (!uflag['V'-'A']) break;
         j = realign(f1, f2, &block1, &block2, text1, text2);
         if (j < 0) return j;
      }

      if (x == 0) break;
      if (y == 0) break;
   }

   return status;
}

static int binary_compare(int f1, int f2)
{
   char			 b1[BLOCK],
			 b2[BLOCK];

   int			 x,
                         y;

   int			 j;
   long			 total = 0;

   int			 status = 0;

   for (;;)
   {
      x = read(f1, b1, BLOCK);
      y = read(f2, b2, BLOCK);

      if (x ^ y)
      {
         if (x < y)
         {
            y = x;
            printf("less data read from %s\n", left);
         }
         else printf("less data read from %s\n", right);

         status |= 1;
      }

      if (y < 1) break;		/* this covers -1 */

      x = memcmp(b1, b2, y);

      if (x)
      {
         status |= x;

         if (flag['v'-'a'])
         {
            printf("pointer %lx \n", total);
            for (j = 0; j < y; j++) printf("%2.2x", b1[j]);
            printf("    ");
            for (j = 0; j < y; j++) printf("%c", symbol(b1[j]));
            putchar('\n');
            for (j = 0; j < y; j++) printf("%2.2x", b2[j]);
            printf("    ");
            for (j = 0; j < y; j++) printf("%c", symbol(b2[j]));
            putchar('\n');
         }

         if (!uflag['V'-'A']) break;
      }

      total += y;
      if (y < BLOCK) break;
   }

   return status;
}

static void file_compare()
{
   #ifdef DOS
   int			 f1 = open(left, O_BINARY | O_RDONLY);
   int			 f2 = open(right,O_BINARY | O_RDONLY);
   #else
   int                   f1 = open(left, O_RDONLY);
   int                   f2 = open(right,O_RDONLY);
   #endif

   int			 status;

   if (f1 < 0)
   {
      printf("%s not available or not a data file\n", left);
      return;
   }

   if (f2 < 0)
   {
      printf("%s not available or not a data file\n", right);
      close(f1);
      return;
   }

   if (flag['a'-'a']) status = text_compare(f1, f2);
   else             status = binary_compare(f1, f2);

   close(f1);
   close(f2);

   if (status) printf("%s and %s differ\n", left, right);

   else if (flag['s'-'a']) printf("%s and %s are identical\n", left, right);
}

static void tree_compare()
{
   DIR			*d1 = opendir(left),
   			*d2,
			*d3;

   struct dirent	*p;

   char			*walkback1,
                        *walkback2;

   int			 f;



   if (d1)
   {
      d2 = opendir(right);

      if (d2)
      {
         if (flag['v'-'a']) printf("comparing trees %s and %s\n", left, right);

         for (;;)
         {
            p = readdir(d1);
            if (!p) break;
            if (p->d_name[0] == '.') continue;

            walkback1 = p1;
            walkback2 = p2;

            p1 = append(p1, p->d_name);
            p2 = append(p2, p->d_name);

            if (p->d_type == DT_DIR)
            {
               if (uflag['R'-'A']) tree_compare();
               else printf("-R not opted. Subdirectory %s skipped\n", p->d_name);
            }
            else file_compare();

            p1 = walkback1;
            p2 = walkback2;
            *walkback1 = 0;
            *walkback2 = 0;
         }

         for (;;)
         {
            p = readdir(d2);
            if (!p) break;
            if (p->d_name[0] == '.') continue;
            walkback1 = p1;
            p1 = append(p1, p->d_name);
            if (p->d_type == DT_DIR)
            {
               d3 = opendir(left);
               if (d3) closedir(d3);
               else printf("directory %s is only in %s\n", p->d_name, right);
            }
            else if (p->d_type == DT_REG)
            {
               f = open(left, O_RDONLY);
               if (f < 0) printf("file %s is only in %s\n", p->d_name, right);
               else close(f);
            }
            else printf("[%x]\n", p->d_type);
            p1 = walkback1;
            *walkback1 = 0;
         }

         closedir(d2);
      }
      else printf("directory %s is present but not directory %s\n", left, right);

      closedir(d1);
   }

   else file_compare();
}

int main(int argc, char *argv[])
{
   argue(argc, argv);

   if (arguments > 1)
   {
      p1 = pend(p1, argument[0]);
      p2 = pend(p2, argument[1]);
      tree_compare();
   }
   else
   {
      printf("\narguments must be two files or two directories\n\n");

      printf("	seeif [-avVWRs] path path\n\n");

      printf("All  options are single letter and can be in all\n"
             "positions of the line, all at once if desired\n\n");

      printf("paths are either two files or two directories\n\n"

              "        -a	text, ignore stuff like CR\n\n"

              "        -v      display differences\n\n"

              "        -V      keep comparing after the first difference\n"
              "                try to resync text files\n\n"

              "        -W      treat whitespace fields as one space\n"
              "                ignore blank lines\n\n"

              "        -R      recurse subdirectories\n\n"

              "        -s      say if files are equal\n\n");
        
   }

   return 0;
}
