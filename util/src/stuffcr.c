#include <stdio.h>

#define	CR	0x0D
#define LF	0x0A

int main()
{
   int		 x;

   for (;;)
   {
      x = getchar();
      if (x == EOF) break;
      if (x ==  LF) putchar(CR);
      putchar(x);
   }
   return 0;
}
