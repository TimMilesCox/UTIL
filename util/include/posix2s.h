
#define	POSIX2S_BLOCK	512

typedef struct { int		    wcursor,
				    rcursor;
		 off_t		   position;
		 char	data[POSIX2S_BLOCK]; } posix2s_block;

/***********************************************************

	the structure is placed in static
	the two byte counters are zero
	the first read causes a refill

***********************************************************/

extern int posix2textline(int file, char *buffer, int maximum, int compress, posix2s_block *b);
int posix2s_point(int file, int offset, off_t position, posix2s_block *b);
off_t posix2s_page(posix2s_block *b);
int posix2s_offset(posix2s_block *b);

