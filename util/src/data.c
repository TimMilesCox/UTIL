
/******************************************************************

		Copyright Tim Cox, 2015

		TimMilesCox@gmx.ch       

	This source code data.c is a line-oriented 
	text editor used in POSIX bash shells and in
	DOS and Windows command prompt

	data is free software licensed
	with the GNU General Public Licence Version 3

	The same licence encompasses all accompanying software
	and documentation

	The full licence text is included with these materials

        See also the licensing notice at the foot of this document

*******************************************************************/

/******************************************************************

	data takes commands from the input line
	and does no in-page or in-line editing

	text is updated with single or multiline transform commands
	and with line insertion, replacement and deletion commands

	data is suitable for inclusion in scripts and a list of
	commands may be piped

	to build data for

	unix / POSIX / linux
	____________________

	gcc -m32 -funsigned-char -o [PATH]/data data.c


	Windows
	_______

	cl /J /DMSW /Fe[PATH\]data data.c


	DOS
	___

	bcc -K -DDOS -e[PATH\]data.exe data.c

******************************************************************/




#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include <errno.h>

extern void exit(int z);

#define DATA    0
#define COMMAND 1
#define LINE    8192
#define	MASK	360

#ifdef	MSW
#define	BLUE
#define	RED
#define	PRIMARY
#else
#define BLUE             if (flag['g'-'a']) printf("\033[44;1;37m");
#define RED              if (flag['g'-'a']) printf("\033[41;1;37m");
#define PRIMARY          if (flag['g'-'a']) printf("\033[0m");
#endif

static char              flag[26];

static char *lread(int MAX, char *p)
{
   int           x = MAX, y = 0, symbol = 0;
   char         *q = p;

   #ifdef MSW

   /******************************************************

	this is for spooling through a file
	with the return key on a Windows platform

	if the CR goes to stdin then LF is generated
	and the output is double spaced

	other platforms use scan up line delete
	which isn't available on MSW

	it would also be possible to drop the LF
	in the data. Then the next command would
	necessarily be input on the right of the line
	which would be confusing, or mixed up with
	the data at the start of the line, which...

	this solution is working with the same
	clean appearance as on Unix / Linux and DOS


   ******************************************************/

      #if 1
      printf("%p %2.2x %2.2x, ctr %x, b %p %2.2x %2.2x Fl %x %x b %x sz %x tf %p %2.2x %2.2x\n",
              stdin->_ptr,
              (stdin->_ptr) ? *stdin->_ptr : 0,
              (stdin->_ptr) ? *(stdin->_ptr + 1) : 0,  
              stdin->_cnt, stdin->_base,
              (stdin->_base) ? *stdin->_base : 0, 
              (stdin->_base) ? *(stdin->_base + 1) : 0,
              stdin->_flag, stdin->_file, stdin->_charbuf,
              stdin->_bufsiz, stdin->_tmpfname,
              (stdin->_tmpfname) ? stdin->_tmpfname : 0,
              (stdin->_tmpfname) ? *(stdin->_tmpfname + 1) : 0);
      #else    

   if (_isatty(0))
   {
      for (;;)
      {
         if (_kbhit())
         {
            symbol = _getch();

            if (symbol == '\r')
            {
               /*****************************************
			clear the leading 8 columns
			of the "terminal" line
			and go to column 1
               *****************************************/

               *p = 0;
               printf("\t\r");
               return q;
            }

            ungetc(symbol, stdin);
            break; 
         }
         else _sleep(20);
      }
   }

      #endif

   #endif

   while (x--)
   {
      symbol = getchar();

      #ifdef MSW
      printf("%p %2.2x %2.2x, ctr %x, b %p %2.2x %2.2x Fl %x %x b %x sz %x tf %p %2.2x %2.2x\n",
              stdin->_ptr,
              (stdin->_ptr) ? *stdin->i_ptr : 0,
              (stdin->_ptr) ? *(stdin->_ptr + 1) : 0,
              stdin->_cnt, stdin->_base,
              (stdin->_base) ? *stdin->_base : 0,
              (stdin->_base) ? *(stdin->_base + 1) : 0,
              stdin->_flag, stdin->_file, stdin->_charbuf,
              stdin->_bufsiz, stdin->_tmpfname,
              (stdin->_tmpfname) ? stdin->_tmpfname : 0,
              (stdin->_tmpfname) ? *(stdin->_tmpfname + 1) : 0);
      #endif 

      if (symbol ==  EOF) break;
      if (symbol ==    0) break;
      y++;
      if (symbol == 0x0D) continue;
      if (symbol == 0x0A) break;
      *p++ = symbol;
   }

   if (y)
   {
      *p = 0;
      if (flag['e'-'a']) printf("<%s", p);
      return q;
   }

   return NULL;
}

static long copyend(FILE *f, FILE *g)
{
   char                  data[LINE];
   char                 *p;
   long                  lines = 0;

   for (;;)
   {
      p = fgets(data, LINE-1, f);
      if (!p) break;
      fputs(data, g);
      lines++;
   }
   return lines;
}

static FILE *align(long x, FILE *f, char *target)
{
   char                  data[LINE];
   char                 *p;
   FILE                 *g = fopen(target, "w");

   if (!g)
   {
      RED
      printf("data file problem 4 %s\n", target);
      PRIMARY
      exit(-1);
   }
   
   fseek(f, 0, SEEK_SET);
   if (flag['v'-'a']) printf("source output load %ld lines %s\n", x, target);
   
   while (x--)
   {
      p = fgets(data, LINE-1, f);

      if (!p)
      {
	 RED
	 printf("alignment paradox %ld, abandon\n", x);
	 PRIMARY

	 exit(-1);
      }

      fputs(data, g);
   }

   return g;
}

static void print_help()
{
   printf("commands are newline           advance to next line\n"
	  "             number            go to line number\n"
	  "             number,number [file] delete lines [save to file]\n"
	  "             a    file         add lines from a file\n"
	  "             +[number] [file]  skip [copy lines to file]\n"
	  "             +:[line] [file]   advance to line + 1 [copy lines to file]\n"
	  "             -[number]         go back\n" 
	  "             n                 on/off line number display\n"
	  "             i    text line    insert text line\n"
	  "             i                 enter input mode\n"
	  "                  text lines:  newline to return from\n"
	  "                               input mode to command mode\n"
	  "             b    text line    insert line before current line\n"
	  "             r    text line    replace current line\n"
	  "             d                 delete current line\n"
	  "             d-                delete current line, go up 1\n"
	  "  C [no]case c   /old/new/     change current line / is any character\n"
	  "             c //new/[-]       insert text at line start or //end/-\n"
	  "  T [no]case t/old/new/[lines] change [lines] or all from current line\n"
	  "  L [no]case l mask            locate next instance of mask\n"
	  "  LC [no]case lc mask          count lines containing mask\n"
	  "             q                 abandon -don't save\n"
	  "             .                 save and exit\n");
}

int main(int argc, char *argv[])
{
   char                  next[] = "temp0.txt";
   char                 *filename[2]= { NULL, NULL } ;

   char                 *source;
   char                 *target = next;

   FILE                 *f = NULL, *g = NULL;

   char                  data[LINE];
   char                  command[LINE];
   char                  mask1[MASK];
   char                  mask2[MASK];
   char                  mask3[MASK];

   char                  swapfile[MASK] = "swap.txt";
   FILE                 *swap = NULL;   

   long                  cursor = 0,
			 line_last_found = 0,
			 from,
			 to;

   int                   mode = DATA, x = 0, y = 0, z = 0,
			 symbol, symbol1, symbol2, mflag;

   char                  symbolz;

   char                 *p = NULL, *q, *r;

   long                  scope,
                         hits,
			 change,
			 changes,
			 changed;
   
   long                  update = 0;
   
   while (x++ < argc-1)    
   {
      q = argv[x];

      if (*q == '-')
      {
	 q++;

	 while (symbol = *q++)
	 {
	    if ((symbol > 0x60) && (symbol < 0x7B)) symbol &= 0x5F;
	    if ((symbol > 0x40) && (symbol < 0x5B)) flag[symbol - 'A'] = 1; 
	    
	    if (symbol == 'I')
	    {
	       if (y)
	       {
		  RED
		  printf("flag -I must precede any filenames\n");
		  PRIMARY

		  return 0;
	       }
	       update = 1;
	       y = 1;
	    }
	 }

      }
      else
      {
	 if (y < 2) filename[y++] = q;
	 else printf("file %d %s not used\n", y + 1, q);
      }
   }
   
   BLUE

   if (flag['h'-'a'])
   {
      RED
      printf("data [-n] [file] [file]        flag -G for colour -v for verbose\n");
      
      #ifdef DOS
      printf("data [-i]        [file]        flag -A if ANSI.SYS absent\n");
      #else
      printf("data [-i]        [file]\n");
      #endif

      BLUE

      print_help();
      
      PRIMARY
      
      return 0;
   }
   
   source = filename[0];

   if (!source)
   {
      printf("input [CR to enter command mode H = help]>\n");
      y = 2;
      g = fopen(target, "w");

      if (!g)
      {
	 RED
	 printf("initial space not open\n");
	 PRIMARY

	 exit(-1);
      }
   }
   else
   {
      mode = COMMAND;
      f = fopen(source, "r");

      if (!f)
      {
	 RED
	 printf("access failed %s\n", source);
	 PRIMARY

	 return -1;
      }
   }

   for (;;)
   {
      if (mode == DATA)
      {
	 if (flag['n'-'a']) printf("%ld:I>", cursor + 1);

	 q = lread(LINE-1, command);
	 if ((!q) || (*q == 10) || (*q == 13) || (*q == 0))
	 {
	    mode = COMMAND;
	    
	    if (!f)
	    {
	       printf("%s address %ld\n", target, ftell(g));
	       fclose(g);
	       source = target;
	       f = fopen(source, "r");
	       
	       if (!f)
	       {
		  RED
		  printf("data file problem I %p\n", f);
		  PRIMARY

		  exit(-1);
	       }

	       next[4] ^= 1;

	       g = NULL;
	       if (cursor) g = align(cursor - 1, f, target);

	       if (flag['g'-'a'])
	       {
		  printf("%s address %ld\n", source, ftell(f));
	       }
	    }
	 }
	 else
	 {
	    update = 1;

	    if (p) fputs(p, g);
	    sprintf(data, "%s\n", command);
	    p = data;
	    cursor++;
	    continue;
	 }
      }

      printf("%ld>", cursor);
      q = lread(LINE-1, command);

      /*******************************************************
      gets() == NULL should only happen when reading command
      stream from a file
      *******************************************************/

      if (!q) break;

      if (flag['e'-'a']) printf("%s\n", q);

      if (command[0] == '.') break;
      
      if ((command[0] > '0' - 1) && (command[0] < '9' + 1))
      {
         /**************************************************************

		number			a line number on its own just
					means scan to that line

		number,number		means delete a range of lines

		number,number filename	means save the deleted lines
					on a swapout file. They can
					be read back to another position
					with the A (add) command

         ***************************************************************/

         x = sscanf(command, "%ld,%ld %s", &from, &to, swapfile);
         y = -1;

         if ((from == 0) && (x > 1))
         {
            RED
            printf("there is no line numbered zero\n");
            PRIMARY
            continue;
         }

         if (x > 1) y = to - from;
         swap = NULL;

         if (x > 2)
         {
            swap = fopen(swapfile, "w");
            if (!swap)
            {
               RED
               printf("%s cannot be written, line swapout abandoned\n",
                      swapfile);
               BLUE
               continue;
            }
         }


         if (cursor > from)
         {
            /***********************************************************

		you have to go backwards. If there are already updates
		on this pass, you have to finish copying input to output
		until the file end. Otherwise just rewind

            ***********************************************************/

            if (g)
            {
               if (p) fputs(p, g);

               copyend(f, g);
               fclose(f);
               fclose(g);
               source = target;
               g = NULL;

               /*******************************************************

		there is now no open output file, g = NULL marks it absent.
		This makes it possible to decide whether to open a new
		output file or to maintain the one that is open.

		The completed output file becomes the input file

               *******************************************************/

               f = fopen(source, "r");

               if (!f)
               {
                  RED
                  printf("data file problem G %p\n", f);
                  PRIMARY
                  exit(-1);
               }

               next[4] ^= 1;
            }
            else fseek(f, 0, SEEK_SET);
            cursor = 0;
            p = NULL;
         }

         if (y < 0)
         {
            while (cursor < from)
            {
               /*******************************************************

		you're not planning a new update, just scanning forward
		but if there are already updates on this pass you have
		to keep writing the output file

		the target line remains buffered after you read it

               *******************************************************/

               if (g)
               {
                  if (p) fputs(p, g);
               }

               p = fgets(data, LINE - 1, f);
               if (!p) break;
               cursor++;
            }
         }
         else
         {
            update = 1;

            if (g)
            {
               /*******************************************************

		you are going to update and you have an output file
		already because you started updating further back on
		this pass

               *******************************************************/

               while (cursor < from)
               {
                  /***************************************************

			when you arrive at the target line number
			that line remains buffered

                  ***************************************************/

                  if (p) fputs(p, g);
                  p = fgets(data, LINE - 1, f);
                  if (!p) break;
                  cursor++;
               }

               if (!p) continue;
            }
            else
            {
               /*******************************************************

		you are going to update and you had no output file
		copy into the output file from the start of the input file

               *******************************************************/

               g = align(from - 1, f, target);
               p = fgets(data, LINE - 1, f);

               /*******************************************************

		buffer the first line for deletion

               *******************************************************/
            }

            cursor = from - 1;

            if (!p) continue;

            for (;;)
            {
               /*******************************************************

		value of y is one less than the delete range
		i.e delete from 5 to 5 gives value zero in y
		the first line for delete/swapout is buffered
		already.

		The next line is read until the line after the delete
		range has also been read. That one remains buffered

               *******************************************************/

               if (swap) fputs(data, swap);
               p = fgets(data, LINE - 1, f);
               if (!y) break;
               y--;
               if (!p) break;
            }

            if (swap) fclose(swap);
         }

         if (!p) continue;
         cursor = from;
         if (flag['n'-'a']) printf("%ld: ", cursor);
         printf("%s", p);
         continue;
      }      

      switch (command[0])
      {
	 case 0:
	 case 10:
	 case 13:
	    if ((p) && (g)) fputs(p, g);
	    cursor++;

	    p = fgets(data, LINE-1, f);
	    
	    if (!p)
	    {
	       if (g)
	       {
		  if (f) fclose(f);
		  fclose(g);
		  source = target;
		  f = fopen(source, "r");
		  if (flag['v'-'a']) printf("source open read %s\n", source);
		  next[4] ^= 1;

		  #if 0
		  target = next;
		  #endif

		  g = NULL;
    
		  if (!f)
		  {
		     RED
		     printf("data file problem 2 %p\n", f);
		     PRIMARY

		     exit(-1);
		  }
	       }
	       else
	       {
		  fseek(f, 0, SEEK_SET);
	       }
	       cursor = 0;
	       line_last_found = 0;
	       continue;
	    }

	    #ifdef DOS
	    
	    if (flag['a'-'a'])
	    {
	       gotoxy(1, 24);
	       clreol();
	    }
	    else

	    #endif
	    
            #ifdef MSW
            {
            }
            #else
	    printf("\033[A\033[K");
            #endif

	    if (flag['n'-'a']) printf("%ld: ", cursor);
	    printf("%s", p);
	    break;

	 case 'c':
	 case 'C':
	    q++;

	    while (symbol = *q++)
	    {
	       if ((symbol != 32) && (symbol != 9)) break;
	    }

	    if (symbol1 = symbol)
	    {
	       symbolz = 0;
	       x = 0;
	       while (x < MASK)
	       {
		  symbol = *q++;
		  if (symbol == symbol1) break; 
		  if (!symbol) break;

		  if ((command[0] == 'C')
		  &&  (symbol > 0x60) 
		  &&  (symbol < 0x7B)) symbol &= 0x5F;
		  
		  mask1[x++] = symbol;
	       }

	       mask1[x] = 0;

	       if (!symbol) break;

	       x = 0;
	       while (x < MASK)
	       {
		  symbol = *q++;
		  if (symbol == symbol1) break; 
		  if (!symbol) break;

		  mask2[x++] = symbol;
	       }
	       mask2[x] = 0;
	       if (symbol == symbol1) symbolz = *q;
	    }

	    if (!p) break;
	    mflag = mask1[0];
	    x = 2;
	    
	    if (mflag)
	    {
	       q = p;

	       for (;;)
	       {
		  symbol = *q++;   
		  if (symbol ==  0) break;
		  if (symbol == 13) break;
		  if (symbol == 10) break;

		  symbol1 = symbol;

		  if ((command[0] == 'C')
		  &&  (symbol1 > 0x60)
		  &&  (symbol1 < 0x7B)) symbol1 &= 0x5F;

		  if (symbol1 == mflag)
		  {
		     r = q;
		     y = 1;

		     for (;;)
		     {
			symbol1 = mask1[y++];
			if (!symbol1) break;
			symbol2 = *r++; 
			if (!symbol2) break;


			if ((command[0] == 'C')
			&&  (symbol2 > 0x60) 
			&&  (symbol2 < 0x7B)) symbol2 &= 0x5F;

			if (symbol1 ^ symbol2) break;
		     }

		     if (symbol1)
		     {
			command[x++] = symbol;
		     }
		     else
		     {
			update = 1;
			y = 0;
			while (symbol = mask2[y++]) command[x++] = symbol;
			q = r;
		     }
		  }
		  else
		  {
		    command[x++] = symbol;
		  }
	       }
	    }
	    else
	    {
	       update = 1;

	       if (symbolz == '-')
	       {               
		  y = 0;
		  while (symbol = data[y++])
		  {
		     if (symbol == 10) break;
		     if (symbol == 13) break;
		     command[x++] = symbol;
		  }
		  y = 0;
		  while (symbol = mask2[y++]) command[x++] = symbol;
	       }
	       else
	       {
		  y = 0;
		  while (symbol = mask2[y++]) command[x++] = symbol;
		  y = 0;
		  while (symbol = data[y++])
		  {
		     if (symbol == 10) break;
		     if (symbol == 13) break;
		     command[x++] = symbol;
		  }
	       }
	    }

	    command[x] = 0;

	    if (flag['n'-'a']) printf("%ld: ", cursor);
	    printf("%s\n", command + 2);
	    if (!update) break;

	    /************************************
	    fall through to replace the old line
	    with the changed line
	    ************************************/
	 
	 case 'r':
	 case 'R':
	    if (!p) break;
	    
	    if ((command[0] == 'r') || (command[0] == 'R'))
	    {
	       if ((command[1] == 32) || (command[1] == 9))
	       {
	       }
	       else
	       {
		  RED
		  printf("r new text line\n");
		  BLUE
		  break;
	       }
	    }
	    
	    update = 1;

	    if (!g)
	    {
	       g = align(cursor - 1, f, target); 
	       fgets(data, LINE-1, f);
	    }
	    sprintf(data, "%s\n", command+2);

	    break;

	 case 'i':
	 case 'I':

	    if (g)
	    {
	       if (p) fputs(p, g);
	    }
	    else
	    {
	       g = align(cursor, f, target);
	    }
	    
	    if (command[1] == 32)          
	    {
	       update = 1;

	       p = data;
	       sprintf(data, "%s\n", command+2);            
	       cursor++;
	       break;
	    }
	    
	    if (command[1] == 9)          
	    {
	       update = 1;

	       p = data;
	       sprintf(data, "%s\n", command+1);            
	       cursor++;
	       break;
	    }
	    
	    p = NULL;

	    if (command[1] > 32)
	    {
	       RED
	       printf("i\n  OR\ni text\n");
	       BLUE

	       break;
	    }

	    printf("input>\n");
	    mode = DATA;
	    break;

	 case 'b':
	 case 'B':

	    if (!g)
	    {
	       g = align(cursor - 1, f, target);
	       p = fgets(data, LINE-1, f);
	    }

	    update = 1;

	    fputs(command+2, g);            
	    fputc('\n', g);

	    printf("%ld: %s\n%ld: %s", cursor, command+2, cursor + 1, p);
	    cursor++;
	    break;

	 case 'd':
	 case 'D':

	    if (!p)
	    {
	       RED
	       printf("line %ld not in scope\n", cursor);
	       BLUE
	       break;
	    }

	    update = 1;
	    
	    if (!g)
	    {
	       g = align(cursor - 1, f, target); 
	       fgets(data, LINE-1, f);
	    }
	    
	    if (command[1] == '-')
	    {
	       p = NULL;
	    }
	    else
	    {
	       p = fgets(data, LINE-1, f);
	    }
	    
	    if (!p)
	    {
	       if (cursor)
	       {
		  cursor--;
		  copyend(f, g);
		  fclose(f);
		  fclose(g);
		  g = NULL;

		  source = target;
		  f = fopen(source, "r");
		  
		  if (!f)
		  {
		     RED
		     printf("file error\n");
		     PRIMARY

		     exit(-1);
		  }

		  next[4] ^= 1;
		  
		  to = cursor;
		  cursor = 0;

		  while (cursor < to)
		  {
		     p = fgets(data, LINE-1, f);
		     if (!p)
		     {
			RED
			printf("cycle anomaly\n");
			PRIMARY

			exit(-1);
		     }
		     cursor++;
		  }
	       }
	    }
	    
	    if (p)
	    {
	       if (flag['n'-'a']) printf("%ld: ", cursor);
	       printf("%s", p);
	    }

	    break;

	 case '+':

            scope = 0x7FFFFFFF;

	    if (command[1] == ':')
            {
               x = sscanf(command + 2, "%ld %s", &scope, swapfile);
               scope -= cursor - 1;
	    }
	    else x = sscanf(command + 1, "%ld %s", &scope, swapfile);

	    swap = NULL;

	    if (x > 1)
	    {
	       swap = fopen(swapfile, "w");
	       if (!swap)
	       {
		  RED
		  printf("swapfile %s error, data not written\n", swapfile);
		  BLUE
	       }
	    }

	    while (scope--)
	    {
	       if (p)
	       {
		  /*********************************************
			output the line you were on
			if you were on one
			equally to the edit file
			and the lookaside file
		  *********************************************/

		  if (g)    fputs(data, g);
		  if (swap) fputs(data, swap);

		  /*********************************************
			so when (swapfile): the saved fragment includes
			from    the line your were on
			to      the line before 
				that line where you arrive
				excluding line 0 which does not exist
				and lines after the last line, which...

			1>+2    save.txt <- line 1
					    line 2
			3>
				but

			0>+2    save.txt <- line 1
			2>

			zero is not a line number, but the state
			of being positioned before the first data
			in the file
				
		  *********************************************/
	       }

	       p = fgets(data, LINE-1, f);
	       if (!p) break;
	       cursor++;
	    }

	    if (swap) fclose(swap);
            if (!p) break;
            if (flag['n'-'a']) printf("%ld: ", cursor);
	    printf("%s", p);
	    break;

	 case '-':
	    sscanf(command+1, "%ld", &from);
	    to = cursor - from;
	    if (to < 0) to = 0;
	    
	    if (g)
	    {
	       if (p) fputs(p, g);
	       copyend(f, g);
	       fclose(g);
	       fclose(f);

               source = target;
	       f = fopen(source, "r");

               #if 0
	       source = target;
               #endif

	       next[4] ^= 1;

	       #if 0
	       target = next;
	       #endif

	       g = NULL;

	       if (!f)
	       {
		  RED
		  printf("data file problem 3\n");
		  BLUE
		  exit(-1);
	       }
	    }
	    else
	    {
	       fseek(f, 0, SEEK_SET);
	    }

	    cursor = 0;
	    line_last_found = 0;
	    p = 0;

	    while (cursor < to)
	    {
	       p = fgets(data, LINE-1, f);
	       cursor++;
	    }

	    if (p)
            {
               if (flag['n'-'a']) printf("%ld: ", cursor);
               printf("%s", data);
            }

	    break;

	 case 'n':
	 case 'N':
	    flag['n'-'a'] ^= 1;
	    break;

	 case 'a':
	 case 'A':
	    
	    update = 1;

	    swap = NULL;
	    if (command[1] == ' ') sscanf(command+2, "%s", swapfile);
	    swap = fopen(swapfile, "r");
	    
	    if (!swap)
	    {
	       RED
	       printf("add data %s not available\n", swapfile);
	       BLUE

	       break;
	    }

	    if ((p) && (g)) fputs(p, g);
	    
	    if (!g) g = align(cursor, f, target);
	    
	    for (;;)
	    {
	       p = fgets(data, LINE-1, swap);
	       if (!p) break;
	       fputs(p, g);
	       cursor++;
	    }
	    fclose(swap);

            if (flag['n'-'a']) printf("%ld: ", cursor);
            printf("%s", data);

	    break;

	 case 'l':
	 case 'L':

	    q++;
	    symbol = *q++;

            hits = 0;

            if ((symbol == 'c') || (symbol == 'C')) symbol = *q++;

	    if (symbol == ' ')
	    {
	       /**********************************************
	       line_last_found = 0: new search mask, so no hit 
				    already made on this line
		in case you are a script
	       **********************************************/

	       line_last_found = 0;

	       x = 0;

	       while (x < MASK)
	       {
		  symbol = *q++;
		  if (!symbol) break;

		  if ((command[0] == 'L')
		  &&  (symbol > 0x60) 
		  &&  (symbol < 0x7B)) symbol &= 0x5F;
		  
		  mask3[x++] = symbol;
	       }
	       mask3[x] = 0;
	    }
	       
	    mflag = mask3[0];

	    if (line_last_found == cursor)
	    {
	       /***********************************
	       match on current line already made.
	       Therefore examine later lines
	       ***********************************/

	       if ((p) && (g)) fputs(p, g);
	       p = fgets(data, LINE-1, f);
	       if (!p) break;
	       cursor++;
	    }

	    
	    for (;;)
	    {
	       z = 0;

	       if (p)
	       {
		  q = p;

		  for (;;)
		  {
		     symbol = *q++;   
		     if (symbol ==  0) break;
		     if (symbol == 13) break;
		     if (symbol == 10) break;

		     symbol1 = symbol;
		     
		     if ((command[0] == 'L')
		     &&  (symbol1 > 0x60)
		     &&  (symbol1 < 0x7B)) symbol1 &= 0x5F;

		     if (symbol1 == mflag)
		     {
			r = q;
			y = 1;

			for (;;)
			{
			   symbol1 = mask3[y++];
			   if (!symbol1) break;
			   symbol2 = *r++; 
			   if (!symbol2) break;

			   if ((command[0] == 'L')
			   &&  (symbol2 > 0x60) 
			   &&  (symbol2 < 0x7B)) symbol2 &= 0x5F;

			   if (symbol1 ^ symbol2) break;
			}

			if (symbol1 == 0)
			{
                           hits++;
			   if (flag['n'-'a']) printf("%ld: ", cursor);
			   printf("%s", p);
			   line_last_found = cursor;
			   if ((command[1] ^ 'c')
                           &&  (command[1] ^ 'C')) z = 1;
			}
		     }
		  }

		  if (z) break;
		  if (g) fputs(p, g);
	       }

	       if (z) break;
	       p = fgets(data, LINE-1, f);              
	       if (!p) break;
	       cursor++;
	    }

            if ((command[1] == 'c') || (command[1] == 'C'))
            {
               printf("%ld lines found with match\n", hits);
            }

	    break;

	 case 't':
	 case 'T':
	    scope = 0x7FFFFFFF;
	    changes = 0;
	    changed = 0;

	    q++;

	    while (symbol = *q++)
	    {
	       if ((symbol != 32) && (symbol != 9)) break;
	    }

	    if (symbol1 = symbol)
	    {
	       symbolz = 0;
	       x = 0;
	       while (x < MASK)
	       {
		  symbol = *q++;
		  if (symbol == symbol1) break; 
		  if (!symbol) break;

		  if ((command[0] == 'T')
		  &&  (symbol > 0x60) 
		  &&  (symbol < 0x7B)) symbol &= 0x5F;
		  
		  mask1[x++] = symbol;
	       }
	       
	       mask1[x] = 0;

	       if (!symbol) break;

	       x = 0;
	       while (x < MASK)
	       {
		  symbol = *q++;
		  if (symbol == symbol1) break; 
		  if (!symbol) break;

		  mask2[x++] = symbol;
	       }
	       mask2[x] = 0;

	       if (symbol == symbol1)
	       {
		  symbol = *q;
		  if (symbol)
		  {
		     if (symbol == '-') symbolz = '-'; 
		     else sscanf(q, "%ld%c", &scope, &symbolz);
		  }
	       }
	    }

	    mflag = mask1[0];
	    
	    for (;;)
	    {
	       if (p)
	       {
		  change = 0;

		  x = 2;

		  if (mflag)
		  {
		     q = p;

		     for (;;)
		     {
			symbol = *q++;   
			if (symbol ==  0) break;
			if (symbol == 13) break;
			if (symbol == 10) break;

			symbol1 = symbol;

			if ((command[0] == 'T')
			&&  (symbol1 > 0x60)
			&&  (symbol1 < 0x7B)) symbol1 &= 0x5F;

			if (symbol1 == mflag)
			{
			   r = q;
			   y = 1;
		    
			   for (;;)
			   {
			      symbol1 = mask1[y++];
			      if (!symbol1) break;
			      symbol2 = *r++; 
			      if (!symbol2) break;

			      if ((command[0] == 'T')
			      &&  (symbol2 > 0x60) 
			      &&  (symbol2 < 0x7B)) symbol2 &= 0x5F;

			      if (symbol1 ^ symbol2) break;
			   }

			   if (symbol1)
			   {
			      command[x++] = symbol;
			   }
			   else
			   {
			      update = 1;
		  
			      change++;
			      y = 0;
			      
			      while (symbol = mask2[y++])
			      {
				 command[x++] = symbol;
			      }

			      q = r;
			   }
			}
			else
			{
			  command[x++] = symbol;
			}
		     }
		  }
		  else
		  {
		     change = 1;
		     update = 1;

		     if (symbolz == '-')
		     {
			y = 0;
			while (symbol = data[y++])
			{
			   if (symbol == 10) break;
			   if (symbol == 13) break;
			   command[x++] = symbol;
			}
			y = 0;
			while (symbol = mask2[y++]) command[x++] = symbol;
		     }
		     else
		     {
			y = 0;
			while (symbol = mask2[y++]) command[x++] = symbol;
			y = 0;
			while (symbol = data[y++])
			{
			   if (symbol == 10) break;
			   if (symbol == 13) break;
			   command[x++] = symbol;
			}
		     }
		  }

		  command[x] = 0;
	       
		  if (change)
		  {
		     changes += change;
		     changed++;
		     printf("%ld: %s\n", cursor, command + 2);
		     
		     if (!g)
		     {
			g = align(cursor - 1, f, target);
			fgets(data, LINE-1, f);
		     }

		     sprintf(data, "%s\n", command + 2);
		  }
	       }

	       scope--;
	       if (!scope) break;
	       if ((p) && (g)) fputs(p, g);
	       p = fgets(data, LINE-1, f);
	       if (!p) break;
	       cursor++;
	    }

	    printf("%ld translations on %ld lines\n", changes, changed);
	    break;

	 case 'q':
	 case 'Q':
	    if (f) fclose(f);
	    if (g) fclose(g);
	    PRIMARY
	    return 0;
	 
	 case 'h':
	 case 'H':
	    print_help();
	    break;
	 
	 case 'p':
	 case 'P':
         case 'o':
         case 'O':
            scope = 1;

            if (command[1] == '!')   scope = 0x7FFFFFF;
            else x = sscanf(&command[1], "%ld", &scope);

            if (((command[0] & 0x5F) == 'O') || (cursor == 0))
            {
               while (scope--)
               {
                  if (g)
                  {
                     if (p) fputs(p, g);
                  }

                  p = fgets(data, LINE - 1, f);
                  if (!p) break;
                  cursor++;
                  if (flag['n'-'a']) printf("%ld: ", cursor);
                  printf("%s", p); 
               }
               break;
            }

	    while (scope--)
	    {
               if (!p) break;
	       if (flag['n'-'a']) printf("%ld: ", cursor);
	       printf("%s", p);
               if (!scope) break;
               if (g) fputs(p, g);
               p = fgets(data, LINE - 1, f);
               if (p) cursor++;
	    }
	    break;

	 default:
	    RED
	    printf("command h or H for help\n");
	    BLUE
      }
   }

   if (g)
   {
      if (p) fputs(p, g);
      cursor += copyend(f, g);
      fclose(g);
      if (flag['v'-'a']) printf("%s %ld lines\n", target, cursor);
   }
   else
   {
      next[4] ^= 1;
   }

   fclose(f);

   if (flag['v'-'a'])
   {
     if (filename[1]) printf("2nd filename %s\n", filename[1]);
     if (update) printf("updates present\n");
   }

   if (update)
   {
      if (!filename[1]) filename[1] = filename[0];
      y = remove(filename[1]);
      x = rename(target, filename[1]);

      if ((x < 0) || (flag['v'-'a']))
      {
	 printf("%s->%s %d\n", target, filename[1], x);
	 if (x < 0) printf("%d\n", errno);
      }

      PRIMARY
      return 0;
   }

   if (filename[1])
   {
      if (update) filename[0] = target;
      f = fopen(filename[0], "r");
      if (!f)
      {
	 RED
	 printf("%s not read\n", filename[0]);
	 PRIMARY

	 exit(-1);
      }

      g = fopen(filename[1], "w");
      if (!g)
      {
	 RED
	 printf("%s not written\n", filename[1]);
	 PRIMARY

	 exit(-1);
      }

      cursor = copyend(f, g);
      fclose(f);
      fclose(g);
      if (flag['v'-'a']) printf("%s %ld lines\n", filename[1], cursor);
   }

   PRIMARY
   return 0;
}

