#include <stdio.h>

int main()
{
   int		 x, y, sum;
   unsigned short carry;

   for(;;)
   {
      x = scanf("%x", &y);
      if (x == EOF) break;
      if (!x) break;
      sum += y;
   }

   carry = sum;
   while (sum >>= 16)
   {
      sum += carry;
      carry = sum;
   }

   carry ^= 0xFFFF;

   printf("%4.4x\n", carry);

   return 0;
}

