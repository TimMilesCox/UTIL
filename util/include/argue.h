#define	ARGUMENTS 8

extern int	 arguments;
extern char	*argument[];
extern char	 flag[];
extern char	 uflag[];

extern void argue(int argc, char *argv[]);
extern void reflag(char *flags);
