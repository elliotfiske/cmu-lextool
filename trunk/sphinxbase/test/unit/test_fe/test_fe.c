#include <stdio.h>

#include "fe.h"
#include "cmd_ln.h"
#include "ckd_alloc.h"

#include "test_macros.h"

int
main(int argc, char *argv[])
{
	static const arg_t fe_args[] = {
		waveform_to_cepstral_command_line_macro(),
		{ NULL, 0, NULL, NULL }
	};
	FILE *raw;
	cmd_ln_t *config;
	fe_t *fe;
	int16 buf[2048];
	int16 const *inptr;
	int32 frame_shift, frame_size;
	mfcc_t **cepbuf1, **cepbuf2;
	int32 nfr, i;
	size_t nsamp;

	TEST_ASSERT(config = cmd_ln_parse_r(NULL, fe_args, argc, argv, FALSE));
	TEST_ASSERT(fe = fe_init_auto_r(config));

	TEST_EQUAL(fe_get_output_size(fe), DEFAULT_NUM_CEPSTRA);

	fe_get_input_size(fe, &frame_shift, &frame_size);
	TEST_EQUAL(frame_shift, DEFAULT_FRAME_SHIFT);
	TEST_EQUAL(frame_size, (int)(DEFAULT_WINDOW_LENGTH*DEFAULT_SAMPLING_RATE));

	TEST_ASSERT(raw = fopen(DATADIR "/chan3.raw", "rb"));

	TEST_EQUAL(0, fe_start_utt(fe));
	TEST_EQUAL(1024, fread(buf, sizeof(int16), 1024, raw));

	nsamp = 1024;
	TEST_EQUAL(0, fe_process_frames(fe, NULL, &nsamp, NULL, &nfr));
	TEST_EQUAL(1024, nsamp);
	TEST_EQUAL(4, nfr);

	cepbuf1 = ckd_calloc_2d(5, DEFAULT_NUM_CEPSTRA, sizeof(**cepbuf1));
	inptr = &buf[0];
	nfr = 1;

	printf("frame_size %d frame_shift %d\n", frame_size, frame_shift);
	/* Process the first frame. */
	TEST_EQUAL(0, fe_process_frames(fe, &inptr, &nsamp, &cepbuf1[0], &nfr));
	printf("inptr %d nsamp %d nfr %d\n", inptr - buf, nsamp, nfr);
	TEST_EQUAL(nfr, 1);

	/* Note that this next one won't actually consume any frames
	 * of input, because it already got sufficient overflow
	 * samples last time around.  This is implementation-dependent
	 * so we shouldn't actually test for it. */
	TEST_EQUAL(0, fe_process_frames(fe, &inptr, &nsamp, &cepbuf1[1], &nfr));
	printf("inptr %d nsamp %d nfr %d\n", inptr - buf, nsamp, nfr);
	TEST_EQUAL(nfr, 1);

	TEST_EQUAL(0, fe_process_frames(fe, &inptr, &nsamp, &cepbuf1[2], &nfr));
	printf("inptr %d nsamp %d nfr %d\n", inptr - buf, nsamp, nfr);
	TEST_EQUAL(nfr, 1);

	TEST_EQUAL(0, fe_process_frames(fe, &inptr, &nsamp, &cepbuf1[3], &nfr));
	printf("inptr %d nsamp %d nfr %d\n", inptr - buf, nsamp, nfr);
	TEST_EQUAL(nfr, 1);

	TEST_EQUAL(0, fe_end_utt(fe, cepbuf1[4], &nfr));
	printf("nfr %d\n", nfr);
	TEST_EQUAL(nfr, 1);

	/* What we *should* test is that the output we get by
	 * processing one frame at a time is exactly the same as what
	 * we get from doing them all at once.  So let's do that */
	cepbuf2 = ckd_calloc_2d(5, DEFAULT_NUM_CEPSTRA, sizeof(**cepbuf2));
	inptr = &buf[0];
	nfr = 5;
	nsamp = 1024;
	TEST_EQUAL(0, fe_process_frames(fe, &inptr, &nsamp, cepbuf2, &nfr));
	printf("nfr %d\n", nfr);
	TEST_EQUAL(nfr, 4);
	nfr = 1;
	TEST_EQUAL(0, fe_end_utt(fe, cepbuf2[4], &nfr));
	printf("nfr %d\n", nfr);
	TEST_EQUAL(nfr, 1);

	for (i = 0; i < 5; ++i) {
		int j;
		printf("%d: ", i);
		for (j = 0; j < DEFAULT_NUM_CEPSTRA; ++j) {
			printf("%.2f,%.2f ",
			       MFCC2FLOAT(cepbuf1[i][j]),
			       MFCC2FLOAT(cepbuf2[i][j]));
			TEST_EQUAL_FLOAT(cepbuf1[i][j], cepbuf2[i][j]);
		}
		printf("\n");
	}

	ckd_free_2d(cepbuf1);
	ckd_free_2d(cepbuf2);
	fclose(raw);
	fe_close(fe);
	cmd_ln_free_r(config);

	return 0;
}
