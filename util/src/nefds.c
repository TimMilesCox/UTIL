#include <stdio.h>

#define	CR	'\r'
#define	LF	'\n'
#define	DISPLAYABLE	(127-1)
#define	SP	' '

#define	DATA	2048*2


static int pair(int symbol1, int symbol2)
{
   int		 x;
   char		 sample[] = "\0\0\0";

   if (symbol1 < SP) return EOF;
   if (symbol2 < SP) return EOF;

   sample[0] = symbol1;
   sample[1] = symbol2;
   
   sscanf(sample, "%x", &x);
   return x;
}

int main()
{
   int			 x,
			 symbol;

   char			*p;
   char			 data[DATA + 4];

   for (;;)
   {
      p = fgets(data, DATA, stdin);
      if (!p) break;

      while (symbol = *p++)
      {
         if (symbol == CR) continue;
         if (symbol == LF) break;
         x = pair(symbol, *p++);

         if (x == EOF) break;

         if ((x < SP) || (x > DISPLAYABLE)) printf("%2.2x:", x);
         else putchar(x);
      }

      putchar(LF);
      if (x == EOF) printf("unsync\n");
   }

   return 0;
}
