#include <stdio.h>

#define	CR	0x0D

int main()
{
   int		 x;

   for (;;)
   {
      x = getchar();
      if (x == EOF) break;
      if (x ==  CR) continue;
      putchar(x);
   }
   return 0;
}
