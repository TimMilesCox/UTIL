#include <stdio.h>
#include <string.h>
#include "argue.h"

int main(int argc, char *argv[])
{
   unsigned char	*p;
   char			 data[240];

   int			 aline,
			 alouette;


   argument[2] = "";
   argue(argc, argv);

   if (arguments < 2)
   {
      fprintf(stderr, "a left path stem and a right path stem are needed, like\n"
                      "skyd this/stem that/other/stem [/additional/prefix] < text_list_of_directories\n");

      return 0;
   }

   aline = strlen(argument[0]);

   for (;;)
   {
      p = fgets(data, 159, stdin);
      if (!p) break;
      if (*p == '.') break;

      if (memcmp(argument[0], data, aline))
      {
         fprintf(stderr, "leading path symbols are not %s in %s\n", argument[0], data);
         break;
      }

      alouette = strlen(data);
      alouette--;
      if (data[alouette] ^ '\n') printf("LF not encountered %s\n", data);
      if (data[alouette - 1] == '\r') alouette--;
      data[alouette] = 0;

      if (flag['u'-'a']) printf("diff -w --brief %s/%s/%s %s/%s\n", argument[2], argument[1], data + aline, argument[2], data);
      else               printf("diff -w --brief %s/%s %s/%s/%s\n", argument[2], data, argument[2], argument[1], data + aline);
   }

   return 0;
}

