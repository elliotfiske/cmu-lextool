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
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "libs3decoder/fe.h"
#include "libs3decoder/fe_internal.h"
#include "fe_internal_dump.h"
#include "fe_dump.h"


void fe_frame_to_fea_dump(fe_t *FE, double *in, double *fea)
{
    double *spec, *mfspec;

    /* RAH, typo */
    if (FE->FB_TYPE == MEL_SCALE){

	spec = (double *)calloc(FE->FFT_SIZE, sizeof(double));
	mfspec = (double *)calloc(FE->MEL_FB->num_filters, sizeof(double));

	if (spec==NULL || mfspec==NULL){
	    fprintf(stderr,"memory alloc failed in fe_frame_to_fea()\n...exiting\n");
	    exit(0);
	}

        metricsStart("SpectrumMagnitude");
        
        fe_spec_magnitude(in, FE->FRAME_SIZE, spec, FE->FFT_SIZE);
        
        metricsStop("SpectrumMagnitude");

        if (fe_dump) {
            fe_dump_double_frame(fe_dumpfile, spec, FE->FFT_SIZE/2,
                                 "SPEC_MAGNITUDE");
        }

        metricsStart("MelSpectrum");

        fe_mel_spec(FE, spec, mfspec);

        metricsStop("MelSpectrum");

        if (fe_dump) {
            fe_dump_double_frame(fe_dumpfile, mfspec, FE->MEL_FB->num_filters,
                                 "MEL_SPECTRUM   ");
        }

        metricsStart("MelCepstrum");

	fe_mel_cep(FE, mfspec, fea);

        metricsStop("MelCepstrum");

        if (fe_dump) {
            fe_dump_double_frame(fe_dumpfile, fea, FE->NUM_CEPSTRA,
                             "MEL_CEPSTRUM   ");
        }

	free(spec);
	free(mfspec);
    }
    else {
	fprintf(stderr,"MEL SCALE IS CURRENTLY THE ONLY IMPLEMENTATION!\n");
	exit(0);
    }
    
}

void fe_mel_spectrum(fe_t *FE, float64 *spec, float64 *mfspec)
{
    int32 whichfilt, start, i;
    float32 dfreq;

float64 input[] = {208244297.8809708000,
150194053.2556398000,
63353932.0553724100,
76475657.2798322300,
207974929.3207777000,
435692434.6629422000,
582050548.2094913000,
386274207.1834890000,
52404318.6084984700,
824530488.5846024000,
4236634598.9117556000,
8701101696.6651800000,
10177282247.1461010000,
8360225661.7272930000,
6174602249.5711050000,
5352819100.8781030000,
6878638301.1094260000,
9675533641.1175580000,
9840592242.9784400000,
6449778956.5336830000,
3747126424.0378730000,
5287152760.9952900000,
9639409046.2818260000,
12521246269.2746120000,
11342064162.1707080000,
7487405688.3067090000,
4428331476.4313080000,
3956251834.4681916000,
5334490383.2926930000,
7208524779.4032290000,
8379744135.5411530000,
7317218296.0159280000,
3973832889.7362046000,
2082842776.8078580000,
4624113385.4337090000,
8791778059.8659080000,
11244900159.7630860000,
12715664219.7257160000,
13686607167.7305100000,
12669084560.6043130000,
9805417291.0471200000,
7520747133.6218820000,
7246316518.6427820000,
7542261750.4259090000,
6705857936.6774730000,
5111825934.1674230000,
3896654456.0120745000,
4015588808.6853213000,
6436059243.2477560000,
11828392911.0368270000,
18288486246.2822100000,
19529804808.9370000000,
12582490876.7575340000,
4203427419.7740283000,
536674491.3778897500,
55553311.2154398700,
921294836.6847890000,
3562806413.6449304000,
7594349025.9298150000,
12368317707.9489500000,
17728539099.1391680000,
23724444843.3279840000,
30207374223.4150850000,
35683153509.2909100000,
37253706662.3634700000,
32614086858.9912800000,
22471795917.4778370000,
11949117978.1534540000,
9830214251.0468240000,
26596406918.4641840000,
68672681392.3471100000,
121373970743.0254800000,
146804117621.3218700000,
125478270577.3376500000,
83738687035.1862200000,
57573353199.0367300000,
53864676047.4961700000,
55360898240.0752100000,
44960061704.9144900000,
25249477495.8228600000,
11989181648.1783920000,
8159075274.6312870000,
4440751077.7967350000,
483901120.4576503000,
1154670314.2486840000,
3567215660.8434396000,
4586355795.3627930000,
5678393994.6827760000,
7369750887.6922580000,
8489316128.1397930000,
6753231879.3515620000,
2604633793.2668440000,
427182261.2578422400,
344348853.6154430500,
608787924.9757499000,
3651153655.4375677000,
8570836201.8408970000,
10797151409.7857900000,
10496383563.4553530000,
10863267731.8722270000,
13668562733.8099290000,
20162979154.8462370000,
30518361960.9642370000,
38042251760.1516300000,
31096232524.9147260000,
12608252998.9645460000,
1864574049.5617807000,
3061446004.4145530000,
2600263228.3163834000,
2369205993.1101775000,
19403375451.0254000000,
62167416836.5791500000,
126362356222.5021500000,
190717397709.5987500000,
219807830575.9661300000,
194833893380.8711200000,
132526805847.7476800000,
68539381360.1516800000,
33379905914.0027850000,
35394176498.8087700000,
52974293110.3780700000,
55593343162.4005900000,
41059579877.1131800000,
29467169258.2013100000,
23889496598.1664600000,
14807004399.7440830000,
7909773844.9877660000,
10499336096.2597200000,
15210174779.6939300000,
15312409853.7033180000,
13052774981.5254520000,
17900924833.3053320000,
44298838669.5259800000,
91852632863.6499300000,
135541829606.5383600000,
153184466271.7933300000,
147322274032.3498200000,
132325493135.9170200000,
116240381638.8082000000,
95544595081.6543400000,
60779350085.3672900000,
19156483091.0900900000,
425298510.1226288700,
13682529579.7436050000,
29782203525.7993850000,
31938833625.4513100000,
34737953377.3304750000,
44418156247.4566350000,
51421725371.7041500000,
54004961496.9731300000,
53797665916.1943500000,
45385853308.1791200000,
28915338028.0352630000,
14649829782.9483180000,
8019893546.3350000000,
4933161336.9167390000,
886331186.8587878000,
3550674078.6215400000,
36283648464.4235500000,
97274174975.0022600000,
127969037164.0332800000,
92239745429.7350500000,
51484082143.2224040000,
69266721631.8218700000,
101968249581.8823500000,
79169533993.6162400000,
22171473249.2878400000,
1868330444.7539530000,
37267601882.0895700000,
88856323366.2930800000,
125414795639.5707400000,
171280145922.9753400000,
259308317972.4980500000,
335757166736.7597700000,
299416082312.7789300000,
159091802109.5334000000,
36698816162.6470950000,
89372602.8836650800,
9551991656.6778560000,
15584808174.1552430000,
10645620902.0181640000,
4303159922.3712840000,
2189474167.0893500000,
3058096746.3397527000,
3811908506.3193192000,
2810716065.4481034000,
7703800562.5890930000,
51860507975.0960600000,
163015456971.8400600000,
287377631355.6397000000,
313623629844.9573000000,
223832548418.5860300000,
140111219394.2227800000,
153301782469.5626000000,
196555353154.4832800000,
167451626780.8584000000,
78404771660.7688300000,
12385274419.8512540000,
9441854525.5480580000,
39527986839.8265200000,
52249532639.9857600000,
43295929168.2097500000,
45835898265.1746140000,
60554971999.1720400000,
65434986115.3161500000,
71211059934.9247400000,
91452529200.2591600000,
102802387957.8366100000,
84429738186.4710200000,
48305983086.4539400000,
16792308818.1086540000,
6764076903.4941810000,
22704168862.9046020000,
46800487906.2943650000,
54287105136.3183200000,
39541695822.7563200000,
17688067399.3346600000,
10132690369.8853630000,
23959991729.3636400000,
33434363774.7628800000,
17255734572.3249660000,
17214324673.3392600000,
70147951191.8300600000,
113384268611.9005100000,
87200263495.4089800000,
43819083251.3123800000,
42842192972.8598250000,
59932992151.7966160000,
55557036481.4975400000,
33713506674.4024350000,
17588646758.6520600000,
14196506535.5216540000,
10723747917.5590630000,
2480548838.5492710000,
4186450866.0145160000,
20632910243.7510500000,
39671919828.6849200000,
57545008222.9349200000,
78742522397.6481000000,
99586464340.5505400000,
105888291999.3323400000,
89236962064.2860700000,
62446316196.1759900000,
38979670382.6668550000,
18078256911.1858560000,
5561915702.8691560000,
15253166900.2952000000,
43117279029.6594240000,
55264662580.4185200000,
29657111019.3613470000,
1885187941.2624726000,
7538324800.1062150000,
21821386697.7576940000,
16978419105.7248420000,
5192415489.2596655000,
432160151.3543333000
};

    for (i = 0; i < 256; i++) {
      spec[i] = input[i];
    }
    printf("\n");
    dfreq = FE->SAMPLING_RATE/(float32)FE->FFT_SIZE;
    
    for (whichfilt = 0; whichfilt<FE->MEL_FB->num_filters; whichfilt++){
	start = (int32)(FE->MEL_FB->left_apex[whichfilt]/dfreq) + 1;
	mfspec[whichfilt] = 0;
	for (i=0; i< FE->MEL_FB->width[whichfilt]; i++) {
	  mfspec[whichfilt] += FE->MEL_FB->filter_coeffs[whichfilt][i]*spec[start+i];
	  printf("TEMP: %f %f %f %d\n", FE->MEL_FB->filter_coeffs[whichfilt][i], spec[start+i], mfspec[whichfilt], FE->MEL_FB->fft_size);
	}
	printf("FILT: %f\n", mfspec[whichfilt]);
    }
    printf("\n");
    exit(0);
}
