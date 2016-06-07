#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
   int		 f = open(argv[1], O_RDWR);
   int		 x;
   unsigned char data[1];

   if (f < 0) printf("device not opened %d\n", errno);
   else
   {
      x = fcntl(0, F_SETFD, O_NONBLOCK);
      if (x < 0) printf("stdin not nonblock %d\n", errno);
      else
      {
         x = fcntl(f, F_SETFD, O_NONBLOCK);
         if (x < 0) printf("device not nonblock\n", errno);
         else
         {
            for (;;)
            {
               x = read(f, data, 1);
               if (x < 0)
               {
                  if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
                  {
                  }
                  else
                  {
                     printf("device read error %d\n", errno);
                     break;
                  }
               }
               else if (x) putchar(data[0]);

               #if 0
               x = read(0, data, 1);

               if (x < 0)
               {
                  if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
                  {
                  }
                  else
                  {
                     printf("console read error %d\n", errno);
                     break;
                  }
               }
               else if (x)
               {
                  putchar(data[0]);
                  x = write(f, data, 1);

                  if (x < 0)
                  {
                     printf("device write errror %d\n", errno);
                     break;
                  }
               }
               #endif
            }
         } 
      }

      close(f); 
   }

   return 0;
}

