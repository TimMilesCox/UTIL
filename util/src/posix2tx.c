#include <stdio.h>
#include <errno.h>

#ifdef	DOS
#include <sys/types.h>
#else
#include <unistd.h>
#endif

#include "../include/posix2s.h"


off_t posix2s_page(posix2s_block *b)
{
   return b->position;
}

int posix2s_offset(posix2s_block *b)
{
   return b->rcursor;
}

int posix2s_point(int file, int offset, off_t position, posix2s_block *b)
{
   int		 x;
   off_t	 new_position;


   if (position ^ b->position)
   {
      new_position = lseek(file, position, SEEK_SET);

      if (new_position < 0)
      {
         printf("%d %lld %d seek error\n", file, position, errno);
         return -1;
      }

      b->wcursor = x = read(file, b->data, POSIX2S_BLOCK);

      if (x < 0)
      {
         printf("%d %lld %d read error\n", file, position, errno);
         return -1;
      }

      b->position = position;
   }

   b->rcursor = offset;
   return 0;
}

int posix2textline(int file, char *buffer, int maximum, int compress, posix2s_block *b)
{
   int			 final_count = maximum;
   int			 last_read = -1;

   int			 x = b->wcursor,
			 y = b->rcursor,
			 symbol;

   
   while (final_count)
   {
      if (y < x)
      {
      }
      else
      {
         y = 0;
         b->position = lseek(file, (off_t) 0, SEEK_CUR);
         b->wcursor = x = read(file, b->data, POSIX2S_BLOCK);
         if (x < 1) break;

         /* this also covers -1 */
      }

      symbol = b->data[y++];
      if (symbol == '\r') continue;

      if (compress)
      {
         if (symbol == '\t') symbol = ' ';
         if ((symbol == ' ') && (last_read == ' ')) continue;
         last_read = symbol;
      }

      *buffer++ = symbol;
      final_count--;
      if (symbol == '\n') break;
   }

   b->rcursor = y;

   #ifdef FORCE_FINAL_LF
   if (last_read < 0)
   {
   }
   else if ((final_count) && (last_read ^ '\n'))
   {
      final_count--;
      *buffer++ '\n';
   } 
   #endif

   *buffer = 0;
   return maximum - final_count;
}
 
