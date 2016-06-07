#include <stdio.h>

int main(int argc, char *argv[])
{
   char			 data[240];
   char			 prompt[12];
   char			*p;
   FILE			*f = fopen(argv[1], "r");


   if (f)
   {
      for (;;)
      {
         p = fgets(data, 239, f);
         if (!p) break;
         printf("%s\n", data);
         p = fgets(prompt, 11,  stdin);
         if (!p) break;
         if (*p == '.') break;
         system(data);
      }
   }
   else printf("file at argument 1 not opened %s\n", argv[1]);

   return 0;
}

