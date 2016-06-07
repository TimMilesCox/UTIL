#include <stdio.h>
#define	DATA	80

int main()
{
   double	 up;

   long long	*p = (long long *) &up;
   char		*q;

   char		 data[DATA];

   for (;;)
   {
      q = fgets(data, DATA-1, stdin);
      if (!q) break;
      if (*q == '.') break;
      sscanf(data, "%le", &up);
      printf("%16.16llx\n", *p);
   }

   return 0;
}
