/* $Header$
 * DESCRIPTION
 *	CMPSL functions to make error handling a little easier.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#ifdef WIN32
#include <posixwin32.h>
#endif

#include <sys/types.h>

#include "s2types.h"
#include "CM_macros.h"

FILE *
_CM_fopen (char const *file, char const *mode, char const *srcfile, int32 srcline)
{
    FILE *fs = fopen (file, mode);

    if (fs == NULL) {
	fprintf (stdout, "%s(%d): fopen(%s,%s) failed\n",
		 srcfile, srcline, file, mode);
	perror ("fopen");
	exit (-1);
    }

    return (fs);
}

FILE *
_CM_fopenp (char const *dirl, char const *file, char const *mode,
	    char const *srcfile, int32 srcline)
{
    char buffer[2048];
    FILE *fs;

    sprintf (buffer, "%s/%s", dirl, file);
    fs = (FILE *) fopen (buffer, mode);

    if (fs == NULL) {
	fprintf (stdout, "%s(%d): fopen(%s,%s) failed\n", srcfile, srcline,
		 buffer, mode);
	perror ("fopen");
	exit (-1);
    }

    return (fs);
}

void *_CM_calloc (int32 cnt, int32 size, char const *file, int32 line)
{
    void *ret;

    if (cnt == 0)
	return 0;

    ret = calloc ((size_t)cnt, (size_t)size);
    if (ret == 0) {
	fprintf (stdout, "%s(%d): calloc(%d,%d) failed\n", file, line, cnt, size);
	exit (-1);
    }
    return (ret);
}


void *_CM_2dcalloc (int32 rcnt, int32 ccnt, int32 size,
		    char const *srcfile, int32 srcline)
/*------------------------------------------------------------*
 * DESCRIPTION - allocate row pointers and data in one chunk
 */
{
    char 	*ret;
    caddr_t 	*rowPtr;
    int32	r;

    if ((rcnt == 0) || (ccnt == 0))
	return 0;

    ret = calloc ((size_t)(rcnt * ccnt * size) +
		  rcnt * sizeof(caddr_t), 1);
    rowPtr = (caddr_t *) ret;

    if (ret == 0) {
	fprintf (stdout, "%s(%d): CM_2dcalloc(%d,%d,%d) failed\n", srcfile, srcline,
	  	 rcnt, ccnt, size);
	exit (-1);
    }

    for (r = 0; r < rcnt; r++)
	rowPtr[r] = (caddr_t)(ret + (rcnt * sizeof(caddr_t)) + (r * ccnt * size));

    return (ret);
}

void *_CM_3dcalloc (int32 lcnt, int32 rcnt, int32 ccnt, int32 size,
		    char const *srcfile, int32 srcline)
/*------------------------------------------------------------*
 * DESCRIPTION - allocate row pointers and data in one chunk
 */
{
    char 	*ret;
    caddr_t 	*rowPtr;
    caddr_t 	*lvlPtr;
    int32	r, l;

    ret = (char *) calloc ((size_t)(lcnt * rcnt * ccnt * size) +
			   (size_t)(lcnt * rcnt * sizeof(caddr_t)) + 
			   (size_t)(lcnt * sizeof(caddr_t)), 1);
    rowPtr = (caddr_t *) ret;
    lvlPtr = (caddr_t *) ret;

    if (ret == 0) {
	fprintf (stdout, "%s(%d): CM_3dcalloc(%d,%d,%d) failed\n",
		 srcfile, srcline, rcnt, ccnt, size);
	exit (-1);
    }

    for (l = 0; l < lcnt; l++) {
	lvlPtr[l] = ret + (lcnt * sizeof(caddr_t)) + (rcnt * sizeof(caddr_t) * l);
	rowPtr = (caddr_t *) lvlPtr[l];
	for (r = 0; r < rcnt; r++) {
	    rowPtr[r] = (caddr_t)(ret + (lcnt * sizeof(caddr_t)) +
		        (lcnt * rcnt * sizeof(caddr_t)) +
			(l * rcnt * ccnt * size) +
			(r * ccnt * size));
	}
    }

    return (ret);
}


void *_CM_recalloc (void *ptr, int32 cnt, int32 size,
		    char const *srcfile, int32 srcline)
{
    void *ret;

    if (ptr == 0)
	ret = calloc ((size_t)cnt, (size_t)size);
    else
	ret = realloc (ptr, (size_t)size * (size_t)cnt);

    if (ret == 0) {
	fprintf (stdout, "%s(%d): recalloc(0x%lX,%d,%d) failed\n", srcfile, srcline, 
		 (unsigned long) ptr, cnt, size);
	exit (-1);
    }
    return (ret);
}
