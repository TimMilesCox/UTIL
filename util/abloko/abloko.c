#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define	BLOCK	512
#define	BLOCKS	5

int main()
{
   int			 f = open("/dev/rdisk0", O_RDONLY, 0);
   char			 data[BLOCK];

   int			 x = BLOCKS,
                	 y;

   unsigned char	*p;


   if (f < 0)
   {
      printf("e = %d\n", errno);
   }
   else
   {
      printf("f = %d\n", f);

      while (x--)
      {
         y = read(f, data, BLOCK);
         if (y < 0)
         {
            printf("e %d\n", errno);
            break;
         }
         p = data;
         while (y--) printf("%2.2x", *p++);
         putchar('\n');
      }

      close(f);
   }
   return 0;
}

