#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "argue.h"

#ifdef	NETWORK_BYTE_ORDER
#define	PORT(X)	X
#else
#define	PORT(X) ((unsigned short) X >> 8) | ((X & 255) << 8)
#endif

#define	VLAN	PORT(0x8100)
#define	IP	PORT(0x0800)
#define	ARP	PORT(0x0806)
#define	IP6	PORT(0x86DD)

#define PPPD	PORT(0x8863)
#define PPPS	PORT(0x8864)

#define	PPP_IP	PORT(0x0021)
#define PPP_IP6	PORT(0x0057)

#define	ICMP	1
#define	UDP	17
#define	TCP	6


typedef struct { unsigned short		     to[3],
					   from[3],
					ethertype1; } frame1;

typedef struct { unsigned short		      vlan,
					        id; } vlan;

typedef struct { unsigned short		       ppp,
					       vtc,
					   session,
                                            length,
                                          preamble; } ppp;

typedef struct { unsigned char		        vl,
                                               tos;
                 unsigned short		     bytes,
                                                id,
                                          frag_dnf;
                 unsigned char         	       ttl,
					  protocol;
                 unsigned short           checksum;
                 unsigned char		   from[4],
					     to[4]; } dgram;

typedef struct { unsigned short		        to,
					      from,
					     bytes,
					  checksum; } udgram;

static void analyse(unsigned short ethertype, unsigned short *q)
{
   int			 vlan,
			 vtc,
			 session,
			 payload,
			 tag;

   dgram		*ip;
   udgram		*udp;
   unsigned char	*p;

   switch (ethertype)
   {
      case VLAN:
         vlan = *q++;
         ethertype = *q++;
         printf("vlan %4.4x\n", PORT(vlan));
         analyse(ethertype, q);
         break;

      case PPPD:
         printf("%4.4x == %4.4x pppd\n", ethertype, PPPD);
         break;

      case PPPS:
         vtc = *q++;
         session = *q++;
         payload = *q++;
         tag = *q++;
         printf("ppps session %4.4x payload %d\n", PORT(session), PORT(payload));

         switch (tag)
         {
            case PPP_IP:
               ethertype = IP;
               break;
            case PPP_IP6:
               ethertype = IP6;
               break;
            default:
               ethertype = tag;
         }

         analyse(ethertype, q);
         break;

      case IP:
         ip = (dgram *) q;
         q += (ip->vl & 15) << 1;

         printf("%d.%d.%d.%d -> %d.%d.%d.%d\n",
                ip->from[0], ip->from[1], ip->from[2], ip->from[3],
                  ip->to[0],   ip->to[1],   ip->to[2],   ip->to[3]);

         switch (ip->protocol)
         {
            case UDP:
               udp = (udgram *) q;
               printf("udp %d -> %d %d\n", PORT(udp->from), PORT(udp->to), PORT(udp->bytes));
               q += 4;
               p = (unsigned char *) q;
               break;
            case TCP:
               printf("tcp\n");
               break;
            case ICMP:
               printf("icmp\n");
               break;
            default:
               printf("protocol %d\n", ip->protocol);
         }

         break;

      case IP6:
         printf("ip6\n");
         break;

      case ARP:
         printf("arp\n");
         break;

      default:
         printf("ethertype %d\n", ethertype);
   }
}

int main(int argc, char *argv[])
{
   unsigned char		 preamble1[24];
   unsigned char		 preamble2[16];
   unsigned char		 frame[2008];

   unsigned char		*p;

   unsigned short		*q;

   int				 f;
   int				 x;
   int				 bucket;

   unsigned short		 ethertype,
                                 vtc,
                                 payload,
                                 session,
                                 tag,
				 vlan;

   dgram			*ip;
   udgram			*udp;

   unsigned long			 position;

   unsigned char		 command[72];

   argue(argc, argv);


   if (arguments)
   {
      f = open(argument[0], O_RDONLY);

      if (f < 0) printf("input file open error %d\n", errno);
      else
      {
         x = read(f, preamble1, 24);

         if (flag['w'-'a'])
         {
            p = preamble1;
            x = 24;
            while (x--) printf(" %2.2x", *p++);
            putchar('\n');
         }

         for (;;)
         {
            x = read(f, preamble2, 16);

            if (x == 0) break;

            if (x < 16)
            {
               printf("frame descriptor preamble incomplete\n");
               break;
            }

            q = (unsigned short *) preamble2;
            bucket = *(q + 4);

            if (flag['w'-'a'])
            {
               position = lseek(f, 0, SEEK_CUR);
               printf("%8.8lx", position);
               p = preamble2;
               x = 16;
               while (x--) printf(" %2.2x", *p++);
               putchar('\n');
            }

            x = read(f, frame, bucket);
            if (x == 0) break;

            q = (unsigned short *) frame;

            printf("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x <- %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                    frame[0], frame[1], frame[2],frame[3], frame[4], frame[5],
                    frame[6], frame[7], frame[8],frame[9], frame[10],frame[11]);

	    q += 6;

            ethertype = *q++;

            analyse(ethertype, q);

#if 0
static void analyse(unsigned short ethertype, unsigned char *q)
{
            switch (ethertype)
            {
               case VLAN:
                  vlan = *q++;
                  ethertype = *q++;
                  printf("vlan %4.4x\n", PORT(vlan));
                  analyse(ethertype, q);
                  break;

               case PPPD:
                  printf("%4.4x == %4.4x pppd\n", ethertype, PPPD);
                  break;

               case PPPS:
                  vtc = *q++;
                  session = PORT(*q++);
                  payload = PORT(*q++);
                  tag = *q++;
                  printf("ppps session %4.4x payload %d\n", session, payload);
                  if (tag == PPP_IP) ethertype = IP;
                  if (tag == PPP_IP6) ethertype = IP6;
                  analyse(ethertype, q);
                  break;

               case IP:
                  ip = (dgram *) q;
                  q += (ip->vl & 15) << 1;

                  printf("%d.%d.%d.%d -> %d.%d.%d.%d\n",
                         ip->from[0], ip->from[1], ip->from[2], ip->from[3],
                           ip->to[0],   ip->to[1],   ip->to[2],   ip->to[3]);

                  switch (ip->protocol)
                  {
                     case UDP:
                        udp = (udgram *) q;
                        printf("udp %d -> %d %d\n", PORT(udp->from), PORT(udp->to), PORT(udp->bytes));
                        q += 4;
                        p = (unsigned char *) q;
                        break;
                     case TCP:
                        printf("tcp\n");
                        break;
                     case ICMP:
                        printf("icmp\n");
                        break;
                     default:
                        printf("protocol %d\n", ip->protocol);
                  }

                  break;

               case IP6:
                  printf("ip6\n");
                  break;

               case ARP:
                  printf("arp\n");
                  break;

               default:
                  printf("ethertype %d\n", ethertype);
            }
}
#endif

            if (flag['x'-'a'])
            {
               x = bucket >> 1;

               if (x &= 7)
               {
                  x = 8 - x;
                  while (x--) printf("     ");
               }

               p = frame;
               x = bucket;

               /******************************************************

		a bit intricate but prints network-order-pairs:space
		for easy reading irrespective of machine endian
		plus handles last byte of odd number

               ******************************************************/

               while (x--)
               {
                  printf("%2.2x", *p++);
                  if (x--) printf("%2.2x", *p++);
                  else
                  {
                     putchar('\n');
                     break;
                  }

                  if (x & 14) putchar(' ');
                  else        putchar('\n');
               }

               putchar('\n');
            }

            if (flag['h'-'a']) break;
            if (flag['i'-'a'])
            {
               command[0] = 0;
               p = fgets(command, 71, stdin);
               if (p == NULL) break;
               x = *p;
               if (x == '.') break;

               if ((x > '0' - 1) && (x < '9' + 1))
               {
                  if (x ^ 0) sscanf(command, "%ld", &position);
                  else       sscanf(command, "%lx", &position);

                  lseek(f, position, SEEK_SET);
               }

               if (x == '/')
               {
                  p = command + 1;
                  while (x = *p++)
                  {
                     if ((x > 'a' - 1) && (x < 'z' + 1))  flag[x-'a'] ^= 1;
                     if ((x > 'A' - 1) && (x < 'Z' + 1)) uflag[x-'A'] ^= 1;
                  }
               }
            } 
         }

         close(f);
      }
   }
   else printf("argument 1 must be a filename from tcpdump -w file\n");

   return 0;
}

