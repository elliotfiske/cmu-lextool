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
 * muLaw.h -- Table for converting mu-law data into 16-bit linear PCM format.
 *
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1996 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * 
 * 21-Jul-97	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Created from Sunil Issar's version.
 */


#ifndef _MULAW_H_
#define _MULAW_H_


static int16 muLaw[256] = {
 -0x1f5f, -0x1e5f, -0x1d5f, -0x1c5f, -0x1b5f, -0x1a5f, -0x195f, -0x185f,
 -0x175f, -0x165f, -0x155f, -0x145f, -0x135f, -0x125f, -0x115f, -0x105f,
 -0x0f9f, -0x0f1f, -0x0e9f, -0x0e1f, -0x0d9f, -0x0d1f, -0x0c9f, -0x0c1f,
 -0x0b9f, -0x0b1f, -0x0a9f, -0x0a1f, -0x099f, -0x091f, -0x089f, -0x081f,
 -0x07bf, -0x077f, -0x073f, -0x06ff, -0x06bf, -0x067f, -0x063f, -0x05ff,
 -0x05bf, -0x057f, -0x053f, -0x04ff, -0x04bf, -0x047f, -0x043f, -0x03ff,
 -0x03cf, -0x03af, -0x038f, -0x036f, -0x034f, -0x032f, -0x030f, -0x02ef,
 -0x02cf, -0x02af, -0x028f, -0x026f, -0x024f, -0x022f, -0x020f, -0x01ef,
 -0x01d7, -0x01c7, -0x01b7, -0x01a7, -0x0197, -0x0187, -0x0177, -0x0167,
 -0x0157, -0x0147, -0x0137, -0x0127, -0x0117, -0x0107, -0x00f7, -0x00e7,
 -0x00db, -0x00d3, -0x00cb, -0x00c3, -0x00bb, -0x00b3, -0x00ab, -0x00a3,
 -0x009b, -0x0093, -0x008b, -0x0083, -0x007b, -0x0073, -0x006b, -0x0063,
 -0x005d, -0x0059, -0x0055, -0x0051, -0x004d, -0x0049, -0x0045, -0x0041,
 -0x003d, -0x0039, -0x0035, -0x0031, -0x002d, -0x0029, -0x0025, -0x0021,
 -0x001e, -0x001c, -0x001a, -0x0018, -0x0016, -0x0014, -0x0012, -0x0010,
 -0x000e, -0x000c, -0x000a, -0x0008, -0x0006, -0x0004, -0x0002,  0x0000,
  0x1f5f,  0x1e5f,  0x1d5f,  0x1c5f,  0x1b5f,  0x1a5f,  0x195f,  0x185f,
  0x175f,  0x165f,  0x155f,  0x145f,  0x135f,  0x125f,  0x115f,  0x105f,
  0x0f9f,  0x0f1f,  0x0e9f,  0x0e1f,  0x0d9f,  0x0d1f,  0x0c9f,  0x0c1f,
  0x0b9f,  0x0b1f,  0x0a9f,  0x0a1f,  0x099f,  0x091f,  0x089f,  0x081f,
  0x07bf,  0x077f,  0x073f,  0x06ff,  0x06bf,  0x067f,  0x063f,  0x05ff,
  0x05bf,  0x057f,  0x053f,  0x04ff,  0x04bf,  0x047f,  0x043f,  0x03ff,
  0x03cf,  0x03af,  0x038f,  0x036f,  0x034f,  0x032f,  0x030f,  0x02ef,
  0x02cf,  0x02af,  0x028f,  0x026f,  0x024f,  0x022f,  0x020f,  0x01ef,
  0x01d7,  0x01c7,  0x01b7,  0x01a7,  0x0197,  0x0187,  0x0177,  0x0167,
  0x0157,  0x0147,  0x0137,  0x0127,  0x0117,  0x0107,  0x00f7,  0x00e7,
  0x00db,  0x00d3,  0x00cb,  0x00c3,  0x00bb,  0x00b3,  0x00ab,  0x00a3,
  0x009b,  0x0093,  0x008b,  0x0083,  0x007b,  0x0073,  0x006b,  0x0063,
  0x005d,  0x0059,  0x0055,  0x0051,  0x004d,  0x0049,  0x0045,  0x0041,
  0x003d,  0x0039,  0x0035,  0x0031,  0x002d,  0x0029,  0x0025,  0x0021,
  0x001e,  0x001c,  0x001a,  0x0018,  0x0016,  0x0014,  0x0012,  0x0010,
  0x000e,  0x000c,  0x000a,  0x0008,  0x0006,  0x0004,  0x0002,  0x0000,
};


#endif
