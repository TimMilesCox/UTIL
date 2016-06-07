#ifndef DIRENT_INCLUDED
#define DIRENT_INCLUDED

/*

    Declaration of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003.
    Rights:  See end of file.
    
*/

#ifdef __cplusplus
extern "C"
{
#endif

#define	DT_DIR	16
#define	DT_REG	32

typedef struct DIR DIR;


/*************************************************************

        d_type added 2015, TimMilesCox@gmx.ch
        to retrieve directory / file / other attribute

	definitions DT_DIR DT_REG added at the same time
	those are the Windows values, each equal to the
	POSIX value of the same name shifted left 2 positions

        many thanks to the original author
        kevlin@acm.org, kevlin@curbralan.com

*************************************************************/

struct dirent
{
      int d_type;
    char *d_name;
};

DIR           *opendir(const char *);
int           closedir(DIR *);
struct dirent *readdir(DIR *);
void          rewinddir(DIR *);

/*

    Copyright Kevlin Henney, 1997, 2003. All rights reserved.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.
    
    This software is supplied "as is" without express or implied warranty.

    But that said, if there are any problems please get in touch.

*/

#ifdef __cplusplus
}
#endif

#endif

