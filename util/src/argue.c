#include "../include/argument.h"

ARGUMENT
void argue(int argc, void *argv[])
{
   ARGUMENT_XYPQ
   ARGUE
}

void reflag(unsigned char *p)
{
   int		 x;

   while (x = *p++)
   {
      if ((x > 'a'-1) && (x < 'z'+1))  flag[x-'a'] ^= 1;
      if ((x > 'A'-1) && (x < 'Z'+1)) uflag[x-'A'] ^= 1;
   }
}

