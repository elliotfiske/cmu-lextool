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

/********************************************************************
 *
 * Adapted from src/programs/main_live_pretend.c
 *
 * Collects performance metrics, e.g., accuracy, runtimes
 *
 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <libutil/libutil.h>
#include <libs3decoder/new_fe.h>
#include "live_dump.h"

#define MAXSAMPLES 	1000000

int main (int argc, char *argv[])
{
    short *samps;

    int  i, j, buflen, endutt, blksize, nhypwds, nsamp;
    int numberMatches, numberFiles;

    float accuracy;

    char   *argsfile, *ctlfile, *indir, *resultsFile;
    char   filename[512], cepfile[512];
    char   hypothesis[512], referenceResult[512];
    char   *word;

    partialhyp_t *parthyp;
    FILE *fp, *sfp, *rfp;

    numberMatches = 0;
    numberFiles = 0;
    accuracy = 100;

    if (argc != 4) {
        E_FATAL("\nUSAGE: %s <ctlfile> <inrawdir> <argsfile>\n",
                argv[0]);
    }
    
    ctlfile = argv[1]; 
    indir = argv[2]; 
    argsfile = argv[3];

    samps = (short *) calloc(MAXSAMPLES,sizeof(short));
    blksize = 2000;

    if ((fp = fopen(ctlfile,"r")) == NULL)
	E_FATAL("Unable to read %s\n",ctlfile);

    rfp = stdout;

    fprintf(rfp, "BatchDecoder: decoding files in %s\n----------\n", ctlfile);

    live_initialize_decoder(argsfile);

    while (fscanf(fp,"%s %s", filename, referenceResult) != EOF){
        
        numberFiles++;
        nhypwds = 0;

	sprintf(cepfile,"%s/%s.raw",indir,filename);

        fprintf(rfp, "\nDecoding: %s\n\n", cepfile);

	if ((sfp = fopen(cepfile,"rb")) == NULL)
	    E_FATAL("Unable to read %s\n",cepfile);
        
        nsamp = fread(samps, sizeof(short), MAXSAMPLES, sfp);
        
        fflush(stdout); 
        fclose(sfp);

        for (i=0;i<nsamp;i+=blksize){
	    buflen = i+blksize < nsamp ? blksize : nsamp-i;
	    endutt = i+blksize <= nsamp-1 ? 0 : 1;
	    nhypwds = live_utt_decode_block(samps+i,buflen,endutt,&parthyp);

            if (endutt && nhypwds > 0) {

                hypothesis[0] = '\0';

                for (j=0; j < nhypwds; j++) {
                    word = parthyp[j].word;
                    if (strcmp(word, "<sil>") != 0 &&
                        (strcmp(word, "<s>") != 0 &&
                         strcmp(word, "</s>") != 0)) {
                        
                        if (strlen(hypothesis) > 0) {
                            strcat(hypothesis, " ");
                        }
                        strcat(hypothesis, word);
                    }
                }

                /* 
                 * If hypothesis is equal to referenceResult
                 * increment the number of matches
                 */
                fprintf(rfp, "REF:  %s\n", referenceResult);
                fprintf(rfp, "HYP:  %s\n", hypothesis);

                if (strcmp(hypothesis, referenceResult) == 0) {
                    numberMatches++;
                }
            }
        }

        accuracy = ((float) numberMatches)/((float) numberFiles) * 100.0;

        fprintf(rfp, "   Accuracy : %%%f   Errors: %d\n",
                accuracy, (numberFiles - numberMatches));
        fprintf(rfp, "   Sentences: %d      Words : %d   Matches: %d\n",
                numberFiles, numberFiles, numberMatches);
        fprintf(rfp, "--------------\n");
    }
    return 0;
}
