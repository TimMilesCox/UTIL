#include <stdio.h>

int main()
{
   int	 y;
   int	 b4 = 0;

   for (;;)
   {
      y = getchar();
      if (y == EOF) break;
      if (y ^ '\n')
      {
      }
      else if (b4 ^ '\r') putchar('\r');
      putchar(y);      
      b4 = y;
   }

   return 0;
}

