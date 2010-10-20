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
 * prim_type.h -- Primitive types; more machine-independent.
 *
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1999 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * 
 * $Log$
 * Revision 1.5  2004/07/16  00:57:10  egouvea
 * Added Ravi's implementation of FSG support.
 * 
 * Revision 1.2  2004/05/27 14:22:57  rkm
 * FSG cross-word triphones completed (but for single-phone words)
 *
 * Revision 1.1.1.1  2004/03/01 14:30:20  rkm
 *
 *
 * Revision 1.3  2004/02/09 21:19:35  rkm
 * Added bool
 *
 * Revision 1.2  2004/01/23 19:20:03  rkm
 * *** empty log message ***
 *
 *
 * 02-Aug-1999  Kevin A. Lenzo (lenzo@cs.cmu.edu) at Carnegie Mellon
 *              Copied from s3 and cut out everything but the typedefs.
 * 
 * 12-Mar-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon
 * 		Added arraysize_t, point_t, fpoint_t.
 * 
 * 01-Feb-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon
 * 		Added anytype_t.
 * 
 * 08-31-95	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon
 * 		Created.
 */


#ifndef _S2TYPES_
#define _S2TYPES_

typedef int		int32;
typedef short		int16;
typedef char		int8;
typedef unsigned int	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
typedef float		float32;
typedef double		float64;
typedef unsigned char	boolean;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL (void *)0
#endif

typedef union anytype_s {
  boolean boolean;
  int8 int8;
  uint8 uint8;
  int16 int16;
  uint16 uint16;
  int32 int32;
  uint32 uint32;
  float32 float32;
  float64 float64;
  void *ptr;
} anytype_t;


#endif