
#define ARGUMENTS       8

#define	ARGUMENT							\
									\
int		 arguments;						\
char		*argument[ARGUMENTS];					\
									\
char		 flag[32];						\
char		uflag[32];



   #define		ARGUMENT_XYPQ					\
   int			 x,						\
			 y = 1;						\
									\
   unsigned char	*p,						\
			*q;


   #define	ARGUE							\
   while (y < argc)							\
   {									\
      q = argv[y];							\
      x = *q;								\
									\
      if (x == '-')							\
      {									\
         q++;								\
									\
         while (x = *q++)						\
         {								\
            if ((x > 'a' - 1) && (x < 'z' + 1))  flag[x - 'a'] = 1;	\
            if ((x > 'A' - 1) && (x < 'Z' + 1)) uflag[x - 'A'] = 1;	\
         }								\
      }									\
      else								\
      {									\
         if (arguments < ARGUMENTS) argument[arguments++] = q;		\
      }									\
 									\
      y++;								\
   }

