#include <stdio.h>

int main()
{
   int	 y;
   int	 x = 0;
   int	 b4 = 0;

   for (;;)
   {
      y = getchar();
      if (y == EOF) break;
      if (y ^ '\n')
      {
      }
      else if (b4 ^ '\r') x++;
      b4 = y;
   }

   printf("%d LFs without preceding CR\n", x);
   return 0;
}

