#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#include "argue.h"

#ifdef	NETWORK_BYTEORDER
#define	PORT(X)	X
#else
#define	PORT(X)	((unsigned short) X >> 8) | (X << 8)
#endif

#define	DATA	1024
#define	RXFILE	62000

typedef struct sockaddr		 neta;

#ifdef	OSX
static struct sockaddr_in        here  = { 16, AF_INET               } ;
static struct sockaddr_in        there = { 16, AF_INET, PORT(RXFILE) } ;
#else
static struct sockaddr_in	 here  = { AF_INET               } ;
static struct sockaddr_in	 there = { AF_INET, PORT(RXFILE) } ;
#endif

static unsigned char 		*p = (unsigned char *) &there.sin_addr;


int main(int argc, char *argv[])
{
   int			 s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   int			 x = bind(s, (neta *) &here, 16);
   int			 y;
   int			 f;
   int			 kbytes = 0;

   char			 data[DATA];
   char			 text[160];

   unsigned char	*q;


   argue(argc, argv);

   if (arguments <  1) printf("remote host address required\n");
   else
   {
      if (x < 0) printf("bind error %d\n", errno);
      else
      {
         x = sscanf(argument[0], "%hhd.%hhd.%hhd.%hhd", p, p+1, p+2, p+3);

         if (x < 4) printf("that does not look like an IPv4 network address\n");
         else
         {
            x = connect(s, (neta *) &there, 16);

            if (x < 0) printf("connect error %d\n", errno);
            else
            {
               for (;;)
               {
                  q = fgets(text, 160, stdin);
                  if (!q) break;
                  if (*q == '.') break;


                  x = send(s, text, strlen(text), 0);
                  if (x < 0) break;
               }

               if (x < 0) printf("tx error %d\n", errno);

               #if 0
               f = open(argument[1], O_RDWR, 0777);

               if (f < 0) printf("data file open error %d\n", errno);
               else
               {
                  for (;;)
                  {
                     x = read(f, data, DATA);
                     if (x < 1) break;
                     kbytes++;

                     y = send(s, data, x, 0);

                     if (y < 0) break;
                  }

                  printf("xmit %dK + %d bytes\n", kbytes, x);
                  if (x < 0) printf("data read error %d\n", errno);
                  if (y < 0) printf("tx error %d\n", errno);
                  close(f);

               }
               #endif


            }
         }
      }
   }

   close(s);
   return 0;
}

