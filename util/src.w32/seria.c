#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#ifdef WINDOWS
#include <Windows.h>
#include <WinBase.h>

static LPTHREAD_START_ROUTINE keyboard(int f)
{
   char		 data[160];
   int		 x;

   #if 0
   int		 f = open(file_label, O_RDWR|O_APPEND, 0777);
   #endif

   if (f < 0) printf("device write open fail %d\n", errno);
   else
   {
      for (;;)
      {
         x = getchar();
         if (x == EOF) break;
         putchar(x);
         data[0] = x;
         x = write(f, data, 1);
         if (x < 0)
         {
            printf("device write error %d\n", errno);
            break;
         }
         Sleep(20);
      }
   }
}
#endif

int main(int argc, char *argv[])
{
   unsigned char  data[1];
   int		  x;
   char		*p;
   int		  f = open(argv[1], O_RDONLY);

   if (f < 0) printf("device open fail\n");
   else
   {
      #ifdef WINDOWS
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) keyboard, f, 0, NULL);
      #endif

      for (;;)
      {
         x = read(f, data, 1);
         if (x < 1)
         {
            printf("rx fail %d\n", errno);
            break;
         }

         if (x)
         {
            x = data[0];
            if (((x > 0x1f) && (x < 127))
            ||  (x == '\r')
            ||  (x == '\n')
            ||  (x == '\t')) putchar(x);
            else printf("%2.2x", x);
         }

      }
   }

   return 0;
}
