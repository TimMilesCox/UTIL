
/******************************************************************

		Copyright Tim Cox, 2015

		TimMilesCox@gmx.ch       

	This source code is usea.c usea searches text files
	for combinations of search strings. usea is used in
	POSIX bash shells and in DOS and Windows command
	prompt

	usea is free software licensed
	with the GNU General Public Licence Version 3

	The same licence encompasses all accompanying software
	and documentation

	The full licence text is included with these materials

        See also the licensing notice at the foot of this document

*******************************************************************/

/******************************************************************

	usea may either have a small number of file arguments
	or more usually receives file names from stdin, where
	they are piped from POSIX find or Microsoft dir /b /s

		find top_directory -type f | usea -z string1 string2 string...
		find top_directory -name '*.[]' | usea -zai string1 string2 string...

		dir /B /S top_directory | usea -z strings strings...

	command line options after -z are search arguments and may
	be case-transparent. Lines containing all or one search argument
	may be sought

	usea is similar to recursive grep but less critical about what
	characters are in the search strings
	

	to build data for

	unix / POSIX / linux
	____________________

	gcc -m32 -funsigned-char -o [PATH]/usea usea.c


	Windows
	_______

	cl /J /Fe[PATH\]usea usea.c


	DOS
	___

	bcc -K -e[PATH\]usea.exe usea.c

******************************************************************/


#include <stdio.h>

static int               names;
static char		 flag[28];
static int		 files;
static char		*name[384];

#define	DATA	8192

int upper(int y)
{
   int		 x = y;

   if ((x > 0x60) && (x < 0x7B)) x &= 0x5F;
   return x;
}

int main(int argc, char *_argv[])
{
   int			 x = 0, y, symbol, line, mask_words, mask_sum, scale;

   int			 touches[384/32],
			 mask[384/32];

   char			*p, *q, *r, *s;

   char			 filename[1600];
   char			 data[DATA];

   FILE			*f;

   for (;;)
   {
      x++;
      if (x == argc) break;

      p = _argv[x];

      if ((*p == '-') && (flag['z' - 'a'] == 0))
      {
         p++;
         while (symbol = *p++)
         {
            if ((symbol > 0x60) && (symbol < 0x7B)) symbol &= 0x5F;
            if ((symbol > 0x40) && (symbol < 0x5B)) flag[symbol - 'A'] = 1;
         }
      }
      else
      {
         if (names < 384)
         {
            name[names++] = p;

            if (flag['z' - 'a'] == 0)
            {
               if ((files == 0) || (flag['x' - 'a'] == 1))
               {
                  if (files < 31) files++;
                  else
                  {
                     printf("only 31 file names may be commanded\n"
                            "for more files than that pipe their names to stdin\n");
                     return 0;
                  }
               }
            }
         }
         else printf("only 384 search strings and file names may be commanded\n");
      }
   }

   x = 0;
   y = names >> 5;
   mask_words = y + 1;
   mask[y] = ((1 << (names & 31)) - 1);
   while (y--) mask[y] = 0xFFFFFFFF;
   mask[0] ^= ((1 << files) - 1);

   if (flag['v' - 'a']) printf("%d files named on command line\n", files);

   for (;;)
   {
      if (files)
      {
         if (x < files)
         {
            p = name[x];
            x++;
         }
         else break;
      }
      else
      {
         p = gets(filename);
         if (!p) break;
      }

      f = fopen(p, "r");
      y = mask_words;
      while (y--) touches[y] = 0;

      if (f)
      {
         if (flag['v'-'a']) printf("reading %s\n", p);
         line = 0;

         for (;;)
         {
            q = fgets(data, DATA - 1, f);
            if (!q) break;
            line++;

            if (flag['f'-'a'] == 0)
            {
               y = mask_words;
               while (y--) touches[y] = 0;
            }

            while (*q)
            {
               for (y = files; y < names; y++)
               {
                  r = name[y];
                  s = q;

                  while (symbol = *r++)
                  {
                     if (flag['i' - 'a'])
                     {
                        if (upper(symbol) - upper(*s)) break;
                     }
                     else
                     {
                        if (symbol - *s) break;
                     }

                     s++;
                  }

                  if (!symbol)
                  {
                     if (flag['a'-'a']) touches[y >> 5] |= 1 << (y & 31);
                     else
                     {
                        y = mask_words;
                        while (y--) touches[y] = mask[y];
                        break;
                     }
                  }
               }
               q++;
            }

            mask_sum = 0;
            y = mask_words;
            while (y--) mask_sum |= mask[y] ^ touches[y];

            if (mask_sum == 0)
            {
               if (flag['f'-'a'])
               {
                  printf("requested strings are in %s\n", p);
                  break;
               }

               printf("%s:%d %s\n", p, line, data);
            }
         }
         if (flag['v'-'a']) printf("close %s lines %d\n", p, line);

         if (flag['n'-'a'] & flag['f'-'a'])
         {
            if (mask_sum)
            {
               y = mask_words;
               printf("\nstrings not found in %s\n", p);

               while (y--)
               {
                  mask_sum = mask[y] ^ touches[y];
                  scale = 32;

                  while(scale--)
                  {
                     if ((mask_sum >> scale) & 1) printf(" %s\n", name[y * 32 + scale]);
                  }
               }
            }
         }

         fclose(f);
      }
      else
      {
         printf("%s not opened\n", p);
      }
   }
   return 0;
}
