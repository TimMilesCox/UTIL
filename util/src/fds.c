
/******************************************************************

		Copyright Tim Cox, 2015

		TimMilesCox@gmx.ch       

	This source code fds.c is a binary file viewer
	used in POSIX bash shells and in
	DOS and Windows command prompt

	fds is free software licensed
	with the GNU General Public Licence Version 3

	The same licence encompasses all accompanying software
	and documentation

	The full licence text is included with these materials

        See also the licensing notice at the foot of this document

*******************************************************************/

/******************************************************************

	fds takes commands from the input line

	the most frequent command is the return key which advances
	the view to next row of bytes. File end or a period . exits

	a file position may be entered to start viewing from a new
	file address with optionally a search string. Display resumes
	where the search string is matched. Search is forward

	to build fds for


	unix / POSIX / linux
	____________________

	[gcc -m32 -funsigned-char -c argue.c]

	gcc -m32 -funsigned-char -o [PATH]/fds argue.o fds.c


	Windows
	_______

	[cl /J /c argue.c]

	cl /J /Fe[PATH\]fds argue.obj fds.c


	DOS
	___

	[bcc -K -c [-oargue.obj] argue.c]

	bcc -K -e[PATH\]fds.exe argue.obj fds.c

******************************************************************/

#ifdef DOS
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#else
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#endif

#include "../include/argue.h"

int main(int argc, char *argv[])
{
   long long			 cursor = 0;
   int				 f, x, y, symbol;

   char				 command[60];
   char				 mask[60];
   unsigned char		 data[16];

   argue(argc, argv);


   if (arguments == 0)
   {
      printf("one file argument required\n");
      return 0;
   }

   #ifdef DOS
   f = open(argument[0], O_RDONLY | O_BINARY, 0);
   #else
   f = open(argument[0], O_RDONLY, 0);
   #endif

   if (f < 0)
   {
      printf("%s not read\n", argument[0]);
      return 0;
   }

   for (;;)
   {
      y = read(f, data, 16);

      if (y < 0) break;

      printf("%8.8llx: ", cursor);
      for (x = 0; x < y; x++) printf("%2.2x", data[x]);
      while (x++ < 16) printf("  ");

      printf("   \"");

      for (x = 0; x < y; x++)
      {
         symbol = data[x];
         if ((symbol > 0x1F) && (symbol < 0x7F)) putchar(symbol);
         else                                    putchar('.');
      }

      while (x++ < 16) putchar(' ');

      putchar('\"');
      fflush(stdout);

      if (y < 16) break;
      cursor += 16;

      if (uflag['Z'-'A'])
      {
         if (flag['e'-'a'] == 0) putchar('\n');
         continue;
      }

      x = read(0, command, 60);

      if (x > 0)
      {
         if (command[0] == '.') break;

         if ((command[0] > '0' - 1) && (command[0] < '9' + 1))
         {
            if (command[0] == '0') y = sscanf(command, "%llx %[^\r\n]", &cursor, mask);
            else                   y = sscanf(command, "%lld %[^\r\n]", &cursor, mask);

            x = lseek(f, cursor, SEEK_SET);

            if (x < 0)
            {
               printf("position not taken\n");
               break;
            }

            if (y > 1)
            {
               y = strlen(mask);
               if (y > 16) y = 16;

               x = read(f, data, y);
               if (x < y) break;
               symbol = y - 1;

               for (;;) 
               {
                  x = memcmp(data, mask, y);

                  if (x == 0) break;

                  memcpy(data, data + 1, symbol);
                  x = read(f, data + symbol, 1);
                  if (x < 1) break;
               }

               cursor = lseek(f, (off_t) 0, SEEK_CUR);
               cursor -= y;
               x = lseek(f, cursor, SEEK_SET);
               

               if (x < 0) break;
            }
         }

         if (command[0] == '+')
         {
            y = strlen(mask);
            if (y > 16) y = 16;

            x = read(f, data, y);
            if (x < y) break;
            symbol = y - 1;

            for (;;)
            {
               x = memcmp(data, mask, y);

               if (x == 0) break;

               memcpy(data, data + 1, symbol);
               x = read(f, data + symbol, 1);
               if (x < 1) break;
            }

            cursor = lseek(f, (off_t) 0, SEEK_CUR);
            cursor -= y;
            x = lseek(f, cursor, SEEK_SET);


            if (x < 0) break;
         }

         if (command[0] == '-')
         {
            reflag(command + 1);
            putchar('\n');
         }
      }
   }

   putchar('\n');
   close(f);
   return 0;
}

