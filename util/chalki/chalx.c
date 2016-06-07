#include <stdio.h>
#include <string.h>

#define DATA         160
#define TAGS         36

typedef struct { char                name[72];
                 float                    sum; } tag_tuple;

static tag_tuple     tag[TAGS];
static tag_tuple    *next_tag = tag;

int main()
{
   float             left  = 0,
                     right = 0;

   int               symbol,
                     x,
                     operator;

   char              data[DATA];

   char             *p;

   char              name[72];

   tag_tuple        *search;



   for (;;)
   {
      p = fgets(data, DATA, stdin);

      if (!p) break;

      while (symbol = *p)
      {
         if (symbol ^ ' ') break;
         p++;
      }


      if (symbol == '.') break;

      operator = 0;

      if ((symbol == '+')
      ||  (symbol == '-')
      ||  (symbol == '*')
      ||  (symbol == '/'))
      {
         operator = symbol;
         symbol = *p++;
      }

      for (;;)
      {
         while (symbol = *p++)
         {
            if (symbol ^ ' ') break;
         }

         if (symbol == 0) break;
         if (symbol == '\n') break;
         if (symbol == '\r') break;

         x = 0;

         for (;;)
         {
            if (symbol == ' ') break;
            if (symbol == 0) break;
            if (symbol == '\n') break;
            if (symbol == '\r') break;
            if (symbol == '+') break;
            if (symbol == '-') break;
            if (symbol == '*') break;
            if (symbol == '/') break;

            name[x++] = symbol;
            symbol = *p++;
         }

         name[x] = 0;
         while (symbol == ' ') symbol = *p++;


         if ((name[0] < '0') || (name[0] > '9'))
         {
            search = tag;
            while (search < next_tag)
            {
               x = strcmp(name, search->name);

               if (x)
               {
               }
               else break;

               search++;
            }

            if ((operator)
            ||  (symbol == '+') || (symbol == '-')
            ||  (symbol == '*') || (symbol == '/'))
            {
               right = search->sum;
            }
            else
            {
               search->sum = left;
               if (search < next_tag)
               {
               }
               else
               {
                  next_tag++;
                  strcpy(search->name, name);
               }
               printf("%s = %f\n", search->name, search->sum);
               break;
            }
         }
         else
         {
            sscanf(name, "%f", &right);
         }

         switch (operator)
         {
            case '+':
               left += right;
               break;
            case '-':
               left -= right;
               break;
            case '*':
               left *= right;
               break;
            case '/':
               left /= right;
               break;
            default:
               left = right;
         }

         if ((symbol == '+') || (symbol == '-')
         ||  (symbol == '*') || (symbol == '/')) operator = symbol;
      }

      printf("%f\n", left);
   }

   return 0;
}

