/* ====================================================================
 * Copyright (c) 1999-2001 Carnegie Mellon University.  All rights
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
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
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

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <fcntl.h>
#else
#include <sys/file.h>
#include <sys/fcntl.h>
#include <unistd.h>
#endif
#include "byteorder.h"

int
peek_length (char const *file)
{
  int             fd, len;
/*  char		  buff[4]; */
  int buff;

  if ((fd = open (file, O_RDONLY, 0644)) < 0)
  {
    char temp[200];

    sprintf (temp, "peek_length: '%s'",file);
    perror (temp);
    return -1;
  }
  if (read (fd, &buff, 4) != 4)
  {
    fprintf (stderr, "peek_length: %s: can't read length\n", file);
    close (fd);
    return -1;
  }
  SWAPL(&buff);
  close(fd);
/*  len = buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3]; */
  len = buff;
  if (len < 0)
    fprintf (stderr, "Warning: peek_length: %s: length(%d) < 0\n", file, len);
  return len;
}