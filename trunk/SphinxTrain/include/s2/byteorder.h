/* ====================================================================
 * Copyright (c) 1996-2000 Carnegie Mellon University.  All rights 
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
/* in place byte order conversion
   nothing is promised to be returned
   currently only works for suns and Vax MIPS machines
 */

/* defines needed for little endian machines */
#if defined(mips) || defined(__alpha) || defined(i386) || defined(WIN32)
#define NEEDS_SWAP

#define SWAPBYTES

#define SWAPW(x)	*(x) = ((0xff & (*(x))>>8) | (0xff00 & (*(x))<<8))
#define SWAPL(x)	*(x) = ((0xff & (*(x))>>24) | (0xff00 & (*(x))>>8) |\
			(0xff0000 & (*(x))<<8) | (0xff000000 & (*(x))<<24))
#define SWAPF(x)	SWAPL((int *) x)
#define SWAPP(x)	SWAPL((int *) x)
#define SWAPD(x)	{ int *low = (int *) (x), *high = (int *) (x) + 1,\
			      temp;\
			  SWAPL(low);  SWAPL(high);\
			  temp = *low; *low = *high; *high = temp;}

#else	/* don't need byte order conversion, do nothing */

#undef NEEDS_SWAP

#define SWAPW(x)
#define SWAPL(x)
#define SWAPF(x)
#define SWAPP(x)
#define SWAPD(x)

#endif


