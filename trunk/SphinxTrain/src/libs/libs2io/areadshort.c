/* ====================================================================
 * Copyright (c) 1989-2000 Carnegie Mellon University.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The names "Sphinx" and "Carnegie Mellon" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. To obtain permission, contact 
 *    sphinx@cs.cmu.edu.
 *
 * 4. Products derived from this software may not be called "Sphinx"
 *    nor may "Sphinx" appear in their names without prior written
 *    permission of Carnegie Mellon University. To obtain permission,
 *    contact sphinx@cs.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Carnegie
 *    Mellon University (http://www.speech.cs.cmu.edu/)."
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */
/*
	30 May 1989 David R. Fulmer (drf) updated to do byte order
		conversions when necessary.
 */

#include <sys_compat/file.h>

#include <s2/byteorder.h>
#include <s3/s2io.h>

#include <stdio.h>
#include <stdlib.h>

int
areadshort (char *file,
	    short **data_ref,
	    int *length_ref)
{
  int             fd;
  int             length;
  int             size;
  int             offset;
  char           *data;

  if ((fd = open (file, O_RDONLY, 0644)) < 0)
  {
    fprintf (stderr, "areadshort: %s: can't open\n", file);
    perror("");
    return -1;
  }
  if (read (fd, (char *) &length, 4) != 4)
  {
    fprintf (stderr, "areadshort: %s: can't read length (empty file?)\n", file);
    perror("");
    close (fd);
    return -1;
  }
  SWAPL(&length);
  size = length * sizeof (short);
  if (!(data = malloc ((unsigned) size)))
  {
    fprintf (stderr, "areadshort: %s: can't alloc data\n", file);
    perror("");
    close (fd);
    return -1;
  }
  if (read (fd, data, size) != size)
  {
    fprintf (stderr, "areadshort: %s: can't read data\n", file);
    perror("");
    close (fd);
    free (data);
    return -1;
  }
  close (fd);
  *data_ref = (short *) data;
  for(offset = 0; offset < length; offset++)
    SWAPW(*data_ref + offset);
  *length_ref = length;
  return length;
}
