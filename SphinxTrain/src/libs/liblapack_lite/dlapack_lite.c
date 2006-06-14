/*
NOTE: This is generated code. Look in Misc/lapack_lite for information on
      remaking this file.
*/
#include "s3/f2c.h"

#ifdef HAVE_CONFIG
#include "config.h"
#else
extern doublereal dlamch_(char *);
#define EPSILON dlamch_("Epsilon")
#define SAFEMINIMUM dlamch_("Safe minimum")
#define PRECISION dlamch_("Precision")
#define BASE dlamch_("Base")
#endif

extern doublereal dlapy2_(doublereal *x, doublereal *y);



/* Table of constant values */

static integer c__9 = 9;
static integer c__0 = 0;
static doublereal c_b15 = 1.;
static integer c__1 = 1;
static doublereal c_b29 = 0.;
static doublereal c_b94 = -.125;
static doublereal c_b151 = -1.;
static integer c_n1 = -1;
static integer c__3 = 3;
static integer c__2 = 2;
static integer c__8 = 8;
static integer c__4 = 4;
static integer c__65 = 65;
static integer c__6 = 6;
static integer c__15 = 15;
static logical c_false = FALSE_;
static integer c__10 = 10;
static integer c__11 = 11;
static doublereal c_b2804 = 2.;
static logical c_true = TRUE_;
static real c_b3825 = 0.f;
static real c_b3826 = 1.f;

/* Subroutine */ int dbdsdc_(char *uplo, char *compq, integer *n, doublereal *
	d__, doublereal *e, doublereal *u, integer *ldu, doublereal *vt,
	integer *ldvt, doublereal *q, integer *iq, doublereal *work, integer *
	iwork, integer *info)
{
    /* System generated locals */
    integer u_dim1, u_offset, vt_dim1, vt_offset, i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *), log(doublereal);

    /* Local variables */
    static integer i__, j, k;
    static doublereal p, r__;
    static integer z__, ic, ii, kk;
    static doublereal cs;
    static integer is, iu;
    static doublereal sn;
    static integer nm1;
    static doublereal eps;
    static integer ivt, difl, difr, ierr, perm, mlvl, sqre;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dlasr_(char *, char *, char *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *), dcopy_(integer *, doublereal *, integer *
	    , doublereal *, integer *), dswap_(integer *, doublereal *,
	    integer *, doublereal *, integer *);
    static integer poles, iuplo, nsize, start;
    extern /* Subroutine */ int dlasd0_(integer *, integer *, doublereal *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    integer *, integer *, doublereal *, integer *);

    extern /* Subroutine */ int dlasda_(integer *, integer *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     doublereal *, integer *, integer *, integer *, integer *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *), dlascl_(char *, integer *, integer *, doublereal *,
	    doublereal *, integer *, integer *, doublereal *, integer *,
	    integer *), dlasdq_(char *, integer *, integer *, integer
	    *, integer *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *), dlaset_(char *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *), dlartg_(doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static integer givcol;
    extern doublereal dlanst_(char *, integer *, doublereal *, doublereal *);
    static integer icompq;
    static doublereal orgnrm;
    static integer givnum, givptr, qstart, smlsiz, wstart, smlszp;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       December 1, 1999


    Purpose
    =======

    DBDSDC computes the singular value decomposition (SVD) of a real
    N-by-N (upper or lower) bidiagonal matrix B:  B = U * S * VT,
    using a divide and conquer method, where S is a diagonal matrix
    with non-negative diagonal elements (the singular values of B), and
    U and VT are orthogonal matrices of left and right singular vectors,
    respectively. DBDSDC can be used to compute all singular values,
    and optionally, singular vectors or singular vectors in compact form.

    This code makes very mild assumptions about floating point
    arithmetic. It will work on machines with a guard digit in
    add/subtract, or on those binary machines without guard digits
    which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or Cray-2.
    It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.  See DLASD3 for details.

    The code currently call DLASDQ if singular values only are desired.
    However, it can be slightly modified to compute singular values
    using the divide and conquer method.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            = 'U':  B is upper bidiagonal.
            = 'L':  B is lower bidiagonal.

    COMPQ   (input) CHARACTER*1
            Specifies whether singular vectors are to be computed
            as follows:
            = 'N':  Compute singular values only;
            = 'P':  Compute singular values and compute singular
                    vectors in compact form;
            = 'I':  Compute singular values and singular vectors.

    N       (input) INTEGER
            The order of the matrix B.  N >= 0.

    D       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the n diagonal elements of the bidiagonal matrix B.
            On exit, if INFO=0, the singular values of B.

    E       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the elements of E contain the offdiagonal
            elements of the bidiagonal matrix whose SVD is desired.
            On exit, E has been destroyed.

    U       (output) DOUBLE PRECISION array, dimension (LDU,N)
            If  COMPQ = 'I', then:
               On exit, if INFO = 0, U contains the left singular vectors
               of the bidiagonal matrix.
            For other values of COMPQ, U is not referenced.

    LDU     (input) INTEGER
            The leading dimension of the array U.  LDU >= 1.
            If singular vectors are desired, then LDU >= max( 1, N ).

    VT      (output) DOUBLE PRECISION array, dimension (LDVT,N)
            If  COMPQ = 'I', then:
               On exit, if INFO = 0, VT' contains the right singular
               vectors of the bidiagonal matrix.
            For other values of COMPQ, VT is not referenced.

    LDVT    (input) INTEGER
            The leading dimension of the array VT.  LDVT >= 1.
            If singular vectors are desired, then LDVT >= max( 1, N ).

    Q       (output) DOUBLE PRECISION array, dimension (LDQ)
            If  COMPQ = 'P', then:
               On exit, if INFO = 0, Q and IQ contain the left
               and right singular vectors in a compact form,
               requiring O(N log N) space instead of 2*N**2.
               In particular, Q contains all the DOUBLE PRECISION data in
               LDQ >= N*(11 + 2*SMLSIZ + 8*INT(LOG_2(N/(SMLSIZ+1))))
               words of memory, where SMLSIZ is returned by ILAENV and
               is equal to the maximum size of the subproblems at the
               bottom of the computation tree (usually about 25).
            For other values of COMPQ, Q is not referenced.

    IQ      (output) INTEGER array, dimension (LDIQ)
            If  COMPQ = 'P', then:
               On exit, if INFO = 0, Q and IQ contain the left
               and right singular vectors in a compact form,
               requiring O(N log N) space instead of 2*N**2.
               In particular, IQ contains all INTEGER data in
               LDIQ >= N*(3 + 3*INT(LOG_2(N/(SMLSIZ+1))))
               words of memory, where SMLSIZ is returned by ILAENV and
               is equal to the maximum size of the subproblems at the
               bottom of the computation tree (usually about 25).
            For other values of COMPQ, IQ is not referenced.

    WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK)
            If COMPQ = 'N' then LWORK >= (4 * N).
            If COMPQ = 'P' then LWORK >= (6 * N).
            If COMPQ = 'I' then LWORK >= (3 * N**2 + 4 * N).

    IWORK   (workspace) INTEGER array, dimension (8*N)

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  The algorithm failed to compute an singular value.
                  The update process of divide and conquer failed.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    --q;
    --iq;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    iuplo = 0;
    if (lsame_(uplo, "U")) {
	iuplo = 1;
    }
    if (lsame_(uplo, "L")) {
	iuplo = 2;
    }
    if (lsame_(compq, "N")) {
	icompq = 0;
    } else if (lsame_(compq, "P")) {
	icompq = 1;
    } else if (lsame_(compq, "I")) {
	icompq = 2;
    } else {
	icompq = -1;
    }
    if (iuplo == 0) {
	*info = -1;
    } else if (icompq < 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ldu < 1 || (icompq == 2 && *ldu < *n)) {
	*info = -7;
    } else if (*ldvt < 1 || (icompq == 2 && *ldvt < *n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DBDSDC", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    smlsiz = ilaenv_(&c__9, "DBDSDC", " ", &c__0, &c__0, &c__0, &c__0, (
	    ftnlen)6, (ftnlen)1);
    if (*n == 1) {
	if (icompq == 1) {
	    q[1] = d_sign(&c_b15, &d__[1]);
	    q[smlsiz * *n + 1] = 1.;
	} else if (icompq == 2) {
	    u[u_dim1 + 1] = d_sign(&c_b15, &d__[1]);
	    vt[vt_dim1 + 1] = 1.;
	}
	d__[1] = abs(d__[1]);
	return 0;
    }
    nm1 = *n - 1;

/*
       If matrix lower bidiagonal, rotate to be upper bidiagonal
       by applying Givens rotations on the left
*/

    wstart = 1;
    qstart = 3;
    if (icompq == 1) {
	dcopy_(n, &d__[1], &c__1, &q[1], &c__1);
	i__1 = *n - 1;
	dcopy_(&i__1, &e[1], &c__1, &q[*n + 1], &c__1);
    }
    if (iuplo == 2) {
	qstart = 5;
	wstart = ((*n) << (1)) - 1;
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    if (icompq == 1) {
		q[i__ + ((*n) << (1))] = cs;
		q[i__ + *n * 3] = sn;
	    } else if (icompq == 2) {
		work[i__] = cs;
		work[nm1 + i__] = -sn;
	    }
/* L10: */
	}
    }

/*     If ICOMPQ = 0, use DLASDQ to compute the singular values. */

    if (icompq == 0) {
	dlasdq_("U", &c__0, n, &c__0, &c__0, &c__0, &d__[1], &e[1], &vt[
		vt_offset], ldvt, &u[u_offset], ldu, &u[u_offset], ldu, &work[
		wstart], info);
	goto L40;
    }

/*
       If N is smaller than the minimum divide size SMLSIZ, then solve
       the problem with another solver.
*/

    if (*n <= smlsiz) {
	if (icompq == 2) {
	    dlaset_("A", n, n, &c_b29, &c_b15, &u[u_offset], ldu);
	    dlaset_("A", n, n, &c_b29, &c_b15, &vt[vt_offset], ldvt);
	    dlasdq_("U", &c__0, n, n, n, &c__0, &d__[1], &e[1], &vt[vt_offset]
		    , ldvt, &u[u_offset], ldu, &u[u_offset], ldu, &work[
		    wstart], info);
	} else if (icompq == 1) {
	    iu = 1;
	    ivt = iu + *n;
	    dlaset_("A", n, n, &c_b29, &c_b15, &q[iu + (qstart - 1) * *n], n);
	    dlaset_("A", n, n, &c_b29, &c_b15, &q[ivt + (qstart - 1) * *n], n);
	    dlasdq_("U", &c__0, n, n, n, &c__0, &d__[1], &e[1], &q[ivt + (
		    qstart - 1) * *n], n, &q[iu + (qstart - 1) * *n], n, &q[
		    iu + (qstart - 1) * *n], n, &work[wstart], info);
	}
	goto L40;
    }

    if (icompq == 2) {
	dlaset_("A", n, n, &c_b29, &c_b15, &u[u_offset], ldu);
	dlaset_("A", n, n, &c_b29, &c_b15, &vt[vt_offset], ldvt);
    }

/*     Scale. */

    orgnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (orgnrm == 0.) {
	return 0;
    }
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, n, &c__1, &d__[1], n, &ierr);
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, &nm1, &c__1, &e[1], &nm1, &
	    ierr);

    eps = EPSILON;

    mlvl = (integer) (log((doublereal) (*n) / (doublereal) (smlsiz + 1)) /
	    log(2.)) + 1;
    smlszp = smlsiz + 1;

    if (icompq == 1) {
	iu = 1;
	ivt = smlsiz + 1;
	difl = ivt + smlszp;
	difr = difl + mlvl;
	z__ = difr + ((mlvl) << (1));
	ic = z__ + mlvl;
	is = ic + 1;
	poles = is + 1;
	givnum = poles + ((mlvl) << (1));

	k = 1;
	givptr = 2;
	perm = 3;
	givcol = perm + mlvl;
    }

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) < eps) {
	    d__[i__] = d_sign(&eps, &d__[i__]);
	}
/* L20: */
    }

    start = 1;
    sqre = 0;

    i__1 = nm1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = e[i__], abs(d__1)) < eps || i__ == nm1) {

/*
          Subproblem found. First determine its size and then
          apply divide and conquer on it.
*/

	    if (i__ < nm1) {

/*        A subproblem with E(I) small for I < NM1. */

		nsize = i__ - start + 1;
	    } else if ((d__1 = e[i__], abs(d__1)) >= eps) {

/*        A subproblem with E(NM1) not too small but I = NM1. */

		nsize = *n - start + 1;
	    } else {

/*
          A subproblem with E(NM1) small. This implies an
          1-by-1 subproblem at D(N). Solve this 1-by-1 problem
          first.
*/

		nsize = i__ - start + 1;
		if (icompq == 2) {
		    u[*n + *n * u_dim1] = d_sign(&c_b15, &d__[*n]);
		    vt[*n + *n * vt_dim1] = 1.;
		} else if (icompq == 1) {
		    q[*n + (qstart - 1) * *n] = d_sign(&c_b15, &d__[*n]);
		    q[*n + (smlsiz + qstart - 1) * *n] = 1.;
		}
		d__[*n] = (d__1 = d__[*n], abs(d__1));
	    }
	    if (icompq == 2) {
		dlasd0_(&nsize, &sqre, &d__[start], &e[start], &u[start +
			start * u_dim1], ldu, &vt[start + start * vt_dim1],
			ldvt, &smlsiz, &iwork[1], &work[wstart], info);
	    } else {
		dlasda_(&icompq, &smlsiz, &nsize, &sqre, &d__[start], &e[
			start], &q[start + (iu + qstart - 2) * *n], n, &q[
			start + (ivt + qstart - 2) * *n], &iq[start + k * *n],
			 &q[start + (difl + qstart - 2) * *n], &q[start + (
			difr + qstart - 2) * *n], &q[start + (z__ + qstart -
			2) * *n], &q[start + (poles + qstart - 2) * *n], &iq[
			start + givptr * *n], &iq[start + givcol * *n], n, &
			iq[start + perm * *n], &q[start + (givnum + qstart -
			2) * *n], &q[start + (ic + qstart - 2) * *n], &q[
			start + (is + qstart - 2) * *n], &work[wstart], &
			iwork[1], info);
		if (*info != 0) {
		    return 0;
		}
	    }
	    start = i__ + 1;
	}
/* L30: */
    }

/*     Unscale */

    dlascl_("G", &c__0, &c__0, &c_b15, &orgnrm, n, &c__1, &d__[1], n, &ierr);
L40:

/*     Use Selection Sort to minimize swaps of singular vectors */

    i__1 = *n;
    for (ii = 2; ii <= i__1; ++ii) {
	i__ = ii - 1;
	kk = i__;
	p = d__[i__];
	i__2 = *n;
	for (j = ii; j <= i__2; ++j) {
	    if (d__[j] > p) {
		kk = j;
		p = d__[j];
	    }
/* L50: */
	}
	if (kk != i__) {
	    d__[kk] = d__[i__];
	    d__[i__] = p;
	    if (icompq == 1) {
		iq[i__] = kk;
	    } else if (icompq == 2) {
		dswap_(n, &u[i__ * u_dim1 + 1], &c__1, &u[kk * u_dim1 + 1], &
			c__1);
		dswap_(n, &vt[i__ + vt_dim1], ldvt, &vt[kk + vt_dim1], ldvt);
	    }
	} else if (icompq == 1) {
	    iq[i__] = i__;
	}
/* L60: */
    }

/*     If ICOMPQ = 1, use IQ(N,1) as the indicator for UPLO */

    if (icompq == 1) {
	if (iuplo == 1) {
	    iq[*n] = 1;
	} else {
	    iq[*n] = 0;
	}
    }

/*
       If B is lower bidiagonal, update U by those Givens rotations
       which rotated B to be upper bidiagonal
*/

    if ((iuplo == 2 && icompq == 2)) {
	dlasr_("L", "V", "B", n, n, &work[1], &work[*n], &u[u_offset], ldu);
    }

    return 0;

/*     End of DBDSDC */

} /* dbdsdc_ */

/* Subroutine */ int dbdsqr_(char *uplo, integer *n, integer *ncvt, integer *
	nru, integer *ncc, doublereal *d__, doublereal *e, doublereal *vt,
	integer *ldvt, doublereal *u, integer *ldu, doublereal *c__, integer *
	ldc, doublereal *work, integer *info)
{
    /* System generated locals */
    integer c_dim1, c_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1,
	    i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal), d_sign(
	    doublereal *, doublereal *);

    /* Local variables */
    static doublereal f, g, h__;
    static integer i__, j, m;
    static doublereal r__, cs;
    static integer ll;
    static doublereal sn, mu;
    static integer nm1, nm12, nm13, lll;
    static doublereal eps, sll, tol, abse;
    static integer idir;
    static doublereal abss;
    static integer oldm;
    static doublereal cosl;
    static integer isub, iter;
    static doublereal unfl, sinl, cosr, smin, smax, sinr;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *), dlas2_(
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *), dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    static doublereal oldcs;
    extern /* Subroutine */ int dlasr_(char *, char *, char *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *);
    static integer oldll;
    static doublereal shift, sigmn, oldsn;
    extern /* Subroutine */ int dswap_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer maxit;
    static doublereal sminl, sigmx;
    static logical lower;
    extern /* Subroutine */ int dlasq1_(integer *, doublereal *, doublereal *,
	     doublereal *, integer *), dlasv2_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *);

    extern /* Subroutine */ int dlartg_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *), xerbla_(char *,
	    integer *);
    static doublereal sminoa, thresh;
    static logical rotate;
    static doublereal sminlo, tolmul;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DBDSQR computes the singular value decomposition (SVD) of a real
    N-by-N (upper or lower) bidiagonal matrix B:  B = Q * S * P' (P'
    denotes the transpose of P), where S is a diagonal matrix with
    non-negative diagonal elements (the singular values of B), and Q
    and P are orthogonal matrices.

    The routine computes S, and optionally computes U * Q, P' * VT,
    or Q' * C, for given real input matrices U, VT, and C.

    See "Computing  Small Singular Values of Bidiagonal Matrices With
    Guaranteed High Relative Accuracy," by J. Demmel and W. Kahan,
    LAPACK Working Note #3 (or SIAM J. Sci. Statist. Comput. vol. 11,
    no. 5, pp. 873-912, Sept 1990) and
    "Accurate singular values and differential qd algorithms," by
    B. Parlett and V. Fernando, Technical Report CPAM-554, Mathematics
    Department, University of California at Berkeley, July 1992
    for a detailed description of the algorithm.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            = 'U':  B is upper bidiagonal;
            = 'L':  B is lower bidiagonal.

    N       (input) INTEGER
            The order of the matrix B.  N >= 0.

    NCVT    (input) INTEGER
            The number of columns of the matrix VT. NCVT >= 0.

    NRU     (input) INTEGER
            The number of rows of the matrix U. NRU >= 0.

    NCC     (input) INTEGER
            The number of columns of the matrix C. NCC >= 0.

    D       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the n diagonal elements of the bidiagonal matrix B.
            On exit, if INFO=0, the singular values of B in decreasing
            order.

    E       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the elements of E contain the
            offdiagonal elements of the bidiagonal matrix whose SVD
            is desired. On normal exit (INFO = 0), E is destroyed.
            If the algorithm does not converge (INFO > 0), D and E
            will contain the diagonal and superdiagonal elements of a
            bidiagonal matrix orthogonally equivalent to the one given
            as input. E(N) is used for workspace.

    VT      (input/output) DOUBLE PRECISION array, dimension (LDVT, NCVT)
            On entry, an N-by-NCVT matrix VT.
            On exit, VT is overwritten by P' * VT.
            VT is not referenced if NCVT = 0.

    LDVT    (input) INTEGER
            The leading dimension of the array VT.
            LDVT >= max(1,N) if NCVT > 0; LDVT >= 1 if NCVT = 0.

    U       (input/output) DOUBLE PRECISION array, dimension (LDU, N)
            On entry, an NRU-by-N matrix U.
            On exit, U is overwritten by U * Q.
            U is not referenced if NRU = 0.

    LDU     (input) INTEGER
            The leading dimension of the array U.  LDU >= max(1,NRU).

    C       (input/output) DOUBLE PRECISION array, dimension (LDC, NCC)
            On entry, an N-by-NCC matrix C.
            On exit, C is overwritten by Q' * C.
            C is not referenced if NCC = 0.

    LDC     (input) INTEGER
            The leading dimension of the array C.
            LDC >= max(1,N) if NCC > 0; LDC >=1 if NCC = 0.

    WORK    (workspace) DOUBLE PRECISION array, dimension (4*N)

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  If INFO = -i, the i-th argument had an illegal value
            > 0:  the algorithm did not converge; D and E contain the
                  elements of a bidiagonal matrix which is orthogonally
                  similar to the input matrix B;  if INFO = i, i
                  elements of E have not converged to zero.

    Internal Parameters
    ===================

    TOLMUL  DOUBLE PRECISION, default = max(10,min(100,EPS**(-1/8)))
            TOLMUL controls the convergence criterion of the QR loop.
            If it is positive, TOLMUL*EPS is the desired relative
               precision in the computed singular values.
            If it is negative, abs(TOLMUL*EPS*sigma_max) is the
               desired absolute accuracy in the computed singular
               values (corresponds to relative accuracy
               abs(TOLMUL*EPS) in the largest singular value.
            abs(TOLMUL) should be between 1 and 1/EPS, and preferably
               between 10 (for fast convergence) and .1/EPS
               (for there to be some accuracy in the results).
            Default is to lose at either one eighth or 2 of the
               available decimal digits in each computed singular value
               (whichever is smaller).

    MAXITR  INTEGER, default = 6
            MAXITR controls the maximum number of passes of the
            algorithm through its inner loop. The algorithms stops
            (and so fails to converge) if the number of passes
            through the inner loop exceeds MAXITR*N**2.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    lower = lsame_(uplo, "L");
    if ((! lsame_(uplo, "U") && ! lower)) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ncvt < 0) {
	*info = -3;
    } else if (*nru < 0) {
	*info = -4;
    } else if (*ncc < 0) {
	*info = -5;
    } else if ((*ncvt == 0 && *ldvt < 1) || (*ncvt > 0 && *ldvt < max(1,*n)))
	    {
	*info = -9;
    } else if (*ldu < max(1,*nru)) {
	*info = -11;
    } else if ((*ncc == 0 && *ldc < 1) || (*ncc > 0 && *ldc < max(1,*n))) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DBDSQR", &i__1);
	return 0;
    }
    if (*n == 0) {
	return 0;
    }
    if (*n == 1) {
	goto L160;
    }

/*     ROTATE is true if any singular vectors desired, false otherwise */

    rotate = *ncvt > 0 || *nru > 0 || *ncc > 0;

/*     If no singular vectors desired, use qd algorithm */

    if (! rotate) {
	dlasq1_(n, &d__[1], &e[1], &work[1], info);
	return 0;
    }

    nm1 = *n - 1;
    nm12 = nm1 + nm1;
    nm13 = nm12 + nm1;
    idir = 0;

/*     Get machine constants */

    eps = EPSILON;
    unfl = SAFEMINIMUM;

/*
       If matrix lower bidiagonal, rotate to be upper bidiagonal
       by applying Givens rotations on the left
*/

    if (lower) {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    work[i__] = cs;
	    work[nm1 + i__] = sn;
/* L10: */
	}

/*        Update singular vectors if desired */

	if (*nru > 0) {
	    dlasr_("R", "V", "F", nru, n, &work[1], &work[*n], &u[u_offset],
		    ldu);
	}
	if (*ncc > 0) {
	    dlasr_("L", "V", "F", n, ncc, &work[1], &work[*n], &c__[c_offset],
		     ldc);
	}
    }

/*
       Compute singular values to relative accuracy TOL
       (By setting TOL to be negative, algorithm will compute
       singular values to absolute accuracy ABS(TOL)*norm(input matrix))

   Computing MAX
   Computing MIN
*/
    d__3 = 100., d__4 = pow_dd(&eps, &c_b94);
    d__1 = 10., d__2 = min(d__3,d__4);
    tolmul = max(d__1,d__2);
    tol = tolmul * eps;

/*     Compute approximate maximum, minimum singular values */

    smax = 0.;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__2 = smax, d__3 = (d__1 = d__[i__], abs(d__1));
	smax = max(d__2,d__3);
/* L20: */
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__2 = smax, d__3 = (d__1 = e[i__], abs(d__1));
	smax = max(d__2,d__3);
/* L30: */
    }
    sminl = 0.;
    if (tol >= 0.) {

/*        Relative accuracy desired */

	sminoa = abs(d__[1]);
	if (sminoa == 0.) {
	    goto L50;
	}
	mu = sminoa;
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    mu = (d__2 = d__[i__], abs(d__2)) * (mu / (mu + (d__1 = e[i__ - 1]
		    , abs(d__1))));
	    sminoa = min(sminoa,mu);
	    if (sminoa == 0.) {
		goto L50;
	    }
/* L40: */
	}
L50:
	sminoa /= sqrt((doublereal) (*n));
/* Computing MAX */
	d__1 = tol * sminoa, d__2 = *n * 6 * *n * unfl;
	thresh = max(d__1,d__2);
    } else {

/*
          Absolute accuracy desired

   Computing MAX
*/
	d__1 = abs(tol) * smax, d__2 = *n * 6 * *n * unfl;
	thresh = max(d__1,d__2);
    }

/*
       Prepare for main iteration loop for the singular values
       (MAXIT is the maximum number of passes through the inner
       loop permitted before nonconvergence signalled.)
*/

    maxit = *n * 6 * *n;
    iter = 0;
    oldll = -1;
    oldm = -1;

/*     M points to last element of unconverged part of matrix */

    m = *n;

/*     Begin main iteration loop */

L60:

/*     Check for convergence or exceeding iteration count */

    if (m <= 1) {
	goto L160;
    }
    if (iter > maxit) {
	goto L200;
    }

/*     Find diagonal block of matrix to work on */

    if ((tol < 0. && (d__1 = d__[m], abs(d__1)) <= thresh)) {
	d__[m] = 0.;
    }
    smax = (d__1 = d__[m], abs(d__1));
    smin = smax;
    i__1 = m - 1;
    for (lll = 1; lll <= i__1; ++lll) {
	ll = m - lll;
	abss = (d__1 = d__[ll], abs(d__1));
	abse = (d__1 = e[ll], abs(d__1));
	if ((tol < 0. && abss <= thresh)) {
	    d__[ll] = 0.;
	}
	if (abse <= thresh) {
	    goto L80;
	}
	smin = min(smin,abss);
/* Computing MAX */
	d__1 = max(smax,abss);
	smax = max(d__1,abse);
/* L70: */
    }
    ll = 0;
    goto L90;
L80:
    e[ll] = 0.;

/*     Matrix splits since E(LL) = 0 */

    if (ll == m - 1) {

/*        Convergence of bottom singular value, return to top of loop */

	--m;
	goto L60;
    }
L90:
    ++ll;

/*     E(LL) through E(M-1) are nonzero, E(LL-1) is zero */

    if (ll == m - 1) {

/*        2 by 2 block, handle separately */

	dlasv2_(&d__[m - 1], &e[m - 1], &d__[m], &sigmn, &sigmx, &sinr, &cosr,
		 &sinl, &cosl);
	d__[m - 1] = sigmx;
	e[m - 1] = 0.;
	d__[m] = sigmn;

/*        Compute singular vectors, if desired */

	if (*ncvt > 0) {
	    drot_(ncvt, &vt[m - 1 + vt_dim1], ldvt, &vt[m + vt_dim1], ldvt, &
		    cosr, &sinr);
	}
	if (*nru > 0) {
	    drot_(nru, &u[(m - 1) * u_dim1 + 1], &c__1, &u[m * u_dim1 + 1], &
		    c__1, &cosl, &sinl);
	}
	if (*ncc > 0) {
	    drot_(ncc, &c__[m - 1 + c_dim1], ldc, &c__[m + c_dim1], ldc, &
		    cosl, &sinl);
	}
	m += -2;
	goto L60;
    }

/*
       If working on new submatrix, choose shift direction
       (from larger end diagonal element towards smaller)
*/

    if (ll > oldm || m < oldll) {
	if ((d__1 = d__[ll], abs(d__1)) >= (d__2 = d__[m], abs(d__2))) {

/*           Chase bulge from top (big end) to bottom (small end) */

	    idir = 1;
	} else {

/*           Chase bulge from bottom (big end) to top (small end) */

	    idir = 2;
	}
    }

/*     Apply convergence tests */

    if (idir == 1) {

/*
          Run convergence test in forward direction
          First apply standard test to bottom of matrix
*/

	if ((d__2 = e[m - 1], abs(d__2)) <= abs(tol) * (d__1 = d__[m], abs(
		d__1)) || (tol < 0. && (d__3 = e[m - 1], abs(d__3)) <= thresh)
		) {
	    e[m - 1] = 0.;
	    goto L60;
	}

	if (tol >= 0.) {

/*
             If relative accuracy desired,
             apply convergence criterion forward
*/

	    mu = (d__1 = d__[ll], abs(d__1));
	    sminl = mu;
	    i__1 = m - 1;
	    for (lll = ll; lll <= i__1; ++lll) {
		if ((d__1 = e[lll], abs(d__1)) <= tol * mu) {
		    e[lll] = 0.;
		    goto L60;
		}
		sminlo = sminl;
		mu = (d__2 = d__[lll + 1], abs(d__2)) * (mu / (mu + (d__1 = e[
			lll], abs(d__1))));
		sminl = min(sminl,mu);
/* L100: */
	    }
	}

    } else {

/*
          Run convergence test in backward direction
          First apply standard test to top of matrix
*/

	if ((d__2 = e[ll], abs(d__2)) <= abs(tol) * (d__1 = d__[ll], abs(d__1)
		) || (tol < 0. && (d__3 = e[ll], abs(d__3)) <= thresh)) {
	    e[ll] = 0.;
	    goto L60;
	}

	if (tol >= 0.) {

/*
             If relative accuracy desired,
             apply convergence criterion backward
*/

	    mu = (d__1 = d__[m], abs(d__1));
	    sminl = mu;
	    i__1 = ll;
	    for (lll = m - 1; lll >= i__1; --lll) {
		if ((d__1 = e[lll], abs(d__1)) <= tol * mu) {
		    e[lll] = 0.;
		    goto L60;
		}
		sminlo = sminl;
		mu = (d__2 = d__[lll], abs(d__2)) * (mu / (mu + (d__1 = e[lll]
			, abs(d__1))));
		sminl = min(sminl,mu);
/* L110: */
	    }
	}
    }
    oldll = ll;
    oldm = m;

/*
       Compute shift.  First, test if shifting would ruin relative
       accuracy, and if so set the shift to zero.

   Computing MAX
*/
    d__1 = eps, d__2 = tol * .01;
    if ((tol >= 0. && *n * tol * (sminl / smax) <= max(d__1,d__2))) {

/*        Use a zero shift to avoid loss of relative accuracy */

	shift = 0.;
    } else {

/*        Compute the shift from 2-by-2 block at end of matrix */

	if (idir == 1) {
	    sll = (d__1 = d__[ll], abs(d__1));
	    dlas2_(&d__[m - 1], &e[m - 1], &d__[m], &shift, &r__);
	} else {
	    sll = (d__1 = d__[m], abs(d__1));
	    dlas2_(&d__[ll], &e[ll], &d__[ll + 1], &shift, &r__);
	}

/*        Test if shift negligible, and if so set to zero */

	if (sll > 0.) {
/* Computing 2nd power */
	    d__1 = shift / sll;
	    if (d__1 * d__1 < eps) {
		shift = 0.;
	    }
	}
    }

/*     Increment iteration count */

    iter = iter + m - ll;

/*     If SHIFT = 0, do simplified QR iteration */

    if (shift == 0.) {
	if (idir == 1) {

/*
             Chase bulge from top to bottom
             Save cosines and sines for later singular vector updates
*/

	    cs = 1.;
	    oldcs = 1.;
	    i__1 = m - 1;
	    for (i__ = ll; i__ <= i__1; ++i__) {
		d__1 = d__[i__] * cs;
		dlartg_(&d__1, &e[i__], &cs, &sn, &r__);
		if (i__ > ll) {
		    e[i__ - 1] = oldsn * r__;
		}
		d__1 = oldcs * r__;
		d__2 = d__[i__ + 1] * sn;
		dlartg_(&d__1, &d__2, &oldcs, &oldsn, &d__[i__]);
		work[i__ - ll + 1] = cs;
		work[i__ - ll + 1 + nm1] = sn;
		work[i__ - ll + 1 + nm12] = oldcs;
		work[i__ - ll + 1 + nm13] = oldsn;
/* L120: */
	    }
	    h__ = d__[m] * cs;
	    d__[m] = h__ * oldcs;
	    e[m - 1] = h__ * oldsn;

/*           Update singular vectors */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "F", &i__1, ncvt, &work[1], &work[*n], &vt[
			ll + vt_dim1], ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		dlasr_("R", "V", "F", nru, &i__1, &work[nm12 + 1], &work[nm13
			+ 1], &u[ll * u_dim1 + 1], ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "F", &i__1, ncc, &work[nm12 + 1], &work[nm13
			+ 1], &c__[ll + c_dim1], ldc);
	    }

/*           Test convergence */

	    if ((d__1 = e[m - 1], abs(d__1)) <= thresh) {
		e[m - 1] = 0.;
	    }

	} else {

/*
             Chase bulge from bottom to top
             Save cosines and sines for later singular vector updates
*/

	    cs = 1.;
	    oldcs = 1.;
	    i__1 = ll + 1;
	    for (i__ = m; i__ >= i__1; --i__) {
		d__1 = d__[i__] * cs;
		dlartg_(&d__1, &e[i__ - 1], &cs, &sn, &r__);
		if (i__ < m) {
		    e[i__] = oldsn * r__;
		}
		d__1 = oldcs * r__;
		d__2 = d__[i__ - 1] * sn;
		dlartg_(&d__1, &d__2, &oldcs, &oldsn, &d__[i__]);
		work[i__ - ll] = cs;
		work[i__ - ll + nm1] = -sn;
		work[i__ - ll + nm12] = oldcs;
		work[i__ - ll + nm13] = -oldsn;
/* L130: */
	    }
	    h__ = d__[ll] * cs;
	    d__[ll] = h__ * oldcs;
	    e[ll] = h__ * oldsn;

/*           Update singular vectors */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "B", &i__1, ncvt, &work[nm12 + 1], &work[
			nm13 + 1], &vt[ll + vt_dim1], ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		dlasr_("R", "V", "B", nru, &i__1, &work[1], &work[*n], &u[ll *
			 u_dim1 + 1], ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "B", &i__1, ncc, &work[1], &work[*n], &c__[
			ll + c_dim1], ldc);
	    }

/*           Test convergence */

	    if ((d__1 = e[ll], abs(d__1)) <= thresh) {
		e[ll] = 0.;
	    }
	}
    } else {

/*        Use nonzero shift */

	if (idir == 1) {

/*
             Chase bulge from top to bottom
             Save cosines and sines for later singular vector updates
*/

	    f = ((d__1 = d__[ll], abs(d__1)) - shift) * (d_sign(&c_b15, &d__[
		    ll]) + shift / d__[ll]);
	    g = e[ll];
	    i__1 = m - 1;
	    for (i__ = ll; i__ <= i__1; ++i__) {
		dlartg_(&f, &g, &cosr, &sinr, &r__);
		if (i__ > ll) {
		    e[i__ - 1] = r__;
		}
		f = cosr * d__[i__] + sinr * e[i__];
		e[i__] = cosr * e[i__] - sinr * d__[i__];
		g = sinr * d__[i__ + 1];
		d__[i__ + 1] = cosr * d__[i__ + 1];
		dlartg_(&f, &g, &cosl, &sinl, &r__);
		d__[i__] = r__;
		f = cosl * e[i__] + sinl * d__[i__ + 1];
		d__[i__ + 1] = cosl * d__[i__ + 1] - sinl * e[i__];
		if (i__ < m - 1) {
		    g = sinl * e[i__ + 1];
		    e[i__ + 1] = cosl * e[i__ + 1];
		}
		work[i__ - ll + 1] = cosr;
		work[i__ - ll + 1 + nm1] = sinr;
		work[i__ - ll + 1 + nm12] = cosl;
		work[i__ - ll + 1 + nm13] = sinl;
/* L140: */
	    }
	    e[m - 1] = f;

/*           Update singular vectors */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "F", &i__1, ncvt, &work[1], &work[*n], &vt[
			ll + vt_dim1], ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		dlasr_("R", "V", "F", nru, &i__1, &work[nm12 + 1], &work[nm13
			+ 1], &u[ll * u_dim1 + 1], ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "F", &i__1, ncc, &work[nm12 + 1], &work[nm13
			+ 1], &c__[ll + c_dim1], ldc);
	    }

/*           Test convergence */

	    if ((d__1 = e[m - 1], abs(d__1)) <= thresh) {
		e[m - 1] = 0.;
	    }

	} else {

/*
             Chase bulge from bottom to top
             Save cosines and sines for later singular vector updates
*/

	    f = ((d__1 = d__[m], abs(d__1)) - shift) * (d_sign(&c_b15, &d__[m]
		    ) + shift / d__[m]);
	    g = e[m - 1];
	    i__1 = ll + 1;
	    for (i__ = m; i__ >= i__1; --i__) {
		dlartg_(&f, &g, &cosr, &sinr, &r__);
		if (i__ < m) {
		    e[i__] = r__;
		}
		f = cosr * d__[i__] + sinr * e[i__ - 1];
		e[i__ - 1] = cosr * e[i__ - 1] - sinr * d__[i__];
		g = sinr * d__[i__ - 1];
		d__[i__ - 1] = cosr * d__[i__ - 1];
		dlartg_(&f, &g, &cosl, &sinl, &r__);
		d__[i__] = r__;
		f = cosl * e[i__ - 1] + sinl * d__[i__ - 1];
		d__[i__ - 1] = cosl * d__[i__ - 1] - sinl * e[i__ - 1];
		if (i__ > ll + 1) {
		    g = sinl * e[i__ - 2];
		    e[i__ - 2] = cosl * e[i__ - 2];
		}
		work[i__ - ll] = cosr;
		work[i__ - ll + nm1] = -sinr;
		work[i__ - ll + nm12] = cosl;
		work[i__ - ll + nm13] = -sinl;
/* L150: */
	    }
	    e[ll] = f;

/*           Test convergence */

	    if ((d__1 = e[ll], abs(d__1)) <= thresh) {
		e[ll] = 0.;
	    }

/*           Update singular vectors if desired */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "B", &i__1, ncvt, &work[nm12 + 1], &work[
			nm13 + 1], &vt[ll + vt_dim1], ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		dlasr_("R", "V", "B", nru, &i__1, &work[1], &work[*n], &u[ll *
			 u_dim1 + 1], ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		dlasr_("L", "V", "B", &i__1, ncc, &work[1], &work[*n], &c__[
			ll + c_dim1], ldc);
	    }
	}
    }

/*     QR iteration finished, go back and check convergence */

    goto L60;

/*     All singular values converged, so make them positive */

L160:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (d__[i__] < 0.) {
	    d__[i__] = -d__[i__];

/*           Change sign of singular vectors, if desired */

	    if (*ncvt > 0) {
		dscal_(ncvt, &c_b151, &vt[i__ + vt_dim1], ldvt);
	    }
	}
/* L170: */
    }

/*
       Sort the singular values into decreasing order (insertion sort on
       singular values, but only one transposition per singular vector)
*/

    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Scan for smallest D(I) */

	isub = 1;
	smin = d__[1];
	i__2 = *n + 1 - i__;
	for (j = 2; j <= i__2; ++j) {
	    if (d__[j] <= smin) {
		isub = j;
		smin = d__[j];
	    }
/* L180: */
	}
	if (isub != *n + 1 - i__) {

/*           Swap singular values and vectors */

	    d__[isub] = d__[*n + 1 - i__];
	    d__[*n + 1 - i__] = smin;
	    if (*ncvt > 0) {
		dswap_(ncvt, &vt[isub + vt_dim1], ldvt, &vt[*n + 1 - i__ +
			vt_dim1], ldvt);
	    }
	    if (*nru > 0) {
		dswap_(nru, &u[isub * u_dim1 + 1], &c__1, &u[(*n + 1 - i__) *
			u_dim1 + 1], &c__1);
	    }
	    if (*ncc > 0) {
		dswap_(ncc, &c__[isub + c_dim1], ldc, &c__[*n + 1 - i__ +
			c_dim1], ldc);
	    }
	}
/* L190: */
    }
    goto L220;

/*     Maximum number of iterations exceeded, failure to converge */

L200:
    *info = 0;
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.) {
	    ++(*info);
	}
/* L210: */
    }
L220:
    return 0;

/*     End of DBDSQR */

} /* dbdsqr_ */

/* Subroutine */ int dgebak_(char *job, char *side, integer *n, integer *ilo,
	integer *ihi, doublereal *scale, integer *m, doublereal *v, integer *
	ldv, integer *info)
{
    /* System generated locals */
    integer v_dim1, v_offset, i__1;

    /* Local variables */
    static integer i__, k;
    static doublereal s;
    static integer ii;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dswap_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static logical leftv;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static logical rightv;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DGEBAK forms the right or left eigenvectors of a real general matrix
    by backward transformation on the computed eigenvectors of the
    balanced matrix output by DGEBAL.

    Arguments
    =========

    JOB     (input) CHARACTER*1
            Specifies the type of backward transformation required:
            = 'N', do nothing, return immediately;
            = 'P', do backward transformation for permutation only;
            = 'S', do backward transformation for scaling only;
            = 'B', do backward transformations for both permutation and
                   scaling.
            JOB must be the same as the argument JOB supplied to DGEBAL.

    SIDE    (input) CHARACTER*1
            = 'R':  V contains right eigenvectors;
            = 'L':  V contains left eigenvectors.

    N       (input) INTEGER
            The number of rows of the matrix V.  N >= 0.

    ILO     (input) INTEGER
    IHI     (input) INTEGER
            The integers ILO and IHI determined by DGEBAL.
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    SCALE   (input) DOUBLE PRECISION array, dimension (N)
            Details of the permutation and scaling factors, as returned
            by DGEBAL.

    M       (input) INTEGER
            The number of columns of the matrix V.  M >= 0.

    V       (input/output) DOUBLE PRECISION array, dimension (LDV,M)
            On entry, the matrix of right or left eigenvectors to be
            transformed, as returned by DHSEIN or DTREVC.
            On exit, V is overwritten by the transformed eigenvectors.

    LDV     (input) INTEGER
            The leading dimension of the array V. LDV >= max(1,N).

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    =====================================================================


       Decode and Test the input parameters
*/

    /* Parameter adjustments */
    --scale;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1 * 1;
    v -= v_offset;

    /* Function Body */
    rightv = lsame_(side, "R");
    leftv = lsame_(side, "L");

    *info = 0;
    if ((((! lsame_(job, "N") && ! lsame_(job, "P")) && ! lsame_(job, "S"))
	     && ! lsame_(job, "B"))) {
	*info = -1;
    } else if ((! rightv && ! leftv)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ilo < 1 || *ilo > max(1,*n)) {
	*info = -4;
    } else if (*ihi < min(*ilo,*n) || *ihi > *n) {
	*info = -5;
    } else if (*m < 0) {
	*info = -7;
    } else if (*ldv < max(1,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEBAK", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*m == 0) {
	return 0;
    }
    if (lsame_(job, "N")) {
	return 0;
    }

    if (*ilo == *ihi) {
	goto L30;
    }

/*     Backward balance */

    if (lsame_(job, "S") || lsame_(job, "B")) {

	if (rightv) {
	    i__1 = *ihi;
	    for (i__ = *ilo; i__ <= i__1; ++i__) {
		s = scale[i__];
		dscal_(m, &s, &v[i__ + v_dim1], ldv);
/* L10: */
	    }
	}

	if (leftv) {
	    i__1 = *ihi;
	    for (i__ = *ilo; i__ <= i__1; ++i__) {
		s = 1. / scale[i__];
		dscal_(m, &s, &v[i__ + v_dim1], ldv);
/* L20: */
	    }
	}

    }

/*
       Backward permutation

       For  I = ILO-1 step -1 until 1,
                IHI+1 step 1 until N do --
*/

L30:
    if (lsame_(job, "P") || lsame_(job, "B")) {
	if (rightv) {
	    i__1 = *n;
	    for (ii = 1; ii <= i__1; ++ii) {
		i__ = ii;
		if ((i__ >= *ilo && i__ <= *ihi)) {
		    goto L40;
		}
		if (i__ < *ilo) {
		    i__ = *ilo - ii;
		}
		k = (integer) scale[i__];
		if (k == i__) {
		    goto L40;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L40:
		;
	    }
	}

	if (leftv) {
	    i__1 = *n;
	    for (ii = 1; ii <= i__1; ++ii) {
		i__ = ii;
		if ((i__ >= *ilo && i__ <= *ihi)) {
		    goto L50;
		}
		if (i__ < *ilo) {
		    i__ = *ilo - ii;
		}
		k = (integer) scale[i__];
		if (k == i__) {
		    goto L50;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L50:
		;
	    }
	}
    }

    return 0;

/*     End of DGEBAK */

} /* dgebak_ */

/* Subroutine */ int dgebal_(char *job, integer *n, doublereal *a, integer *
	lda, integer *ilo, integer *ihi, doublereal *scale, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    doublereal d__1, d__2;

    /* Local variables */
    static doublereal c__, f, g;
    static integer i__, j, k, l, m;
    static doublereal r__, s, ca, ra;
    static integer ica, ira, iexc;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dswap_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static doublereal sfmin1, sfmin2, sfmax1, sfmax2;

    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static logical noconv;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DGEBAL balances a general real matrix A.  This involves, first,
    permuting A by a similarity transformation to isolate eigenvalues
    in the first 1 to ILO-1 and last IHI+1 to N elements on the
    diagonal; and second, applying a diagonal similarity transformation
    to rows and columns ILO to IHI to make the rows and columns as
    close in norm as possible.  Both steps are optional.

    Balancing may reduce the 1-norm of the matrix, and improve the
    accuracy of the computed eigenvalues and/or eigenvectors.

    Arguments
    =========

    JOB     (input) CHARACTER*1
            Specifies the operations to be performed on A:
            = 'N':  none:  simply set ILO = 1, IHI = N, SCALE(I) = 1.0
                    for i = 1,...,N;
            = 'P':  permute only;
            = 'S':  scale only;
            = 'B':  both permute and scale.

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the input matrix A.
            On exit,  A is overwritten by the balanced matrix.
            If JOB = 'N', A is not referenced.
            See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    ILO     (output) INTEGER
    IHI     (output) INTEGER
            ILO and IHI are set to integers such that on exit
            A(i,j) = 0 if i > j and j = 1,...,ILO-1 or I = IHI+1,...,N.
            If JOB = 'N' or 'S', ILO = 1 and IHI = N.

    SCALE   (output) DOUBLE PRECISION array, dimension (N)
            Details of the permutations and scaling factors applied to
            A.  If P(j) is the index of the row and column interchanged
            with row and column j and D(j) is the scaling factor
            applied to row and column j, then
            SCALE(j) = P(j)    for j = 1,...,ILO-1
                     = D(j)    for j = ILO,...,IHI
                     = P(j)    for j = IHI+1,...,N.
            The order in which the interchanges are made is N to IHI+1,
            then 1 to ILO-1.

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The permutations consist of row and column interchanges which put
    the matrix in the form

               ( T1   X   Y  )
       P A P = (  0   B   Z  )
               (  0   0   T2 )

    where T1 and T2 are upper triangular matrices whose eigenvalues lie
    along the diagonal.  The column indices ILO and IHI mark the starting
    and ending columns of the submatrix B. Balancing consists of applying
    a diagonal similarity transformation inv(D) * B * D to make the
    1-norms of each row of B and its corresponding column nearly equal.
    The output matrix is

       ( T1     X*D          Y    )
       (  0  inv(D)*B*D  inv(D)*Z ).
       (  0      0           T2   )

    Information about the permutations P and the diagonal matrix D is
    returned in the vector SCALE.

    This subroutine is based on the EISPACK routine BALANC.

    Modified by Tzu-Yi Chen, Computer Science Division, University of
      California at Berkeley, USA

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --scale;

    /* Function Body */
    *info = 0;
    if ((((! lsame_(job, "N") && ! lsame_(job, "P")) && ! lsame_(job, "S"))
	     && ! lsame_(job, "B"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEBAL", &i__1);
	return 0;
    }

    k = 1;
    l = *n;

    if (*n == 0) {
	goto L210;
    }

    if (lsame_(job, "N")) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    scale[i__] = 1.;
/* L10: */
	}
	goto L210;
    }

    if (lsame_(job, "S")) {
	goto L120;
    }

/*     Permutation to isolate eigenvalues if possible */

    goto L50;

/*     Row and column exchange. */

L20:
    scale[m] = (doublereal) j;
    if (j == m) {
	goto L30;
    }

    dswap_(&l, &a[j * a_dim1 + 1], &c__1, &a[m * a_dim1 + 1], &c__1);
    i__1 = *n - k + 1;
    dswap_(&i__1, &a[j + k * a_dim1], lda, &a[m + k * a_dim1], lda);

L30:
    switch (iexc) {
	case 1:  goto L40;
	case 2:  goto L80;
    }

/*     Search for rows isolating an eigenvalue and push them down. */

L40:
    if (l == 1) {
	goto L210;
    }
    --l;

L50:
    for (j = l; j >= 1; --j) {

	i__1 = l;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (i__ == j) {
		goto L60;
	    }
	    if (a[j + i__ * a_dim1] != 0.) {
		goto L70;
	    }
L60:
	    ;
	}

	m = l;
	iexc = 1;
	goto L20;
L70:
	;
    }

    goto L90;

/*     Search for columns isolating an eigenvalue and push them left. */

L80:
    ++k;

L90:
    i__1 = l;
    for (j = k; j <= i__1; ++j) {

	i__2 = l;
	for (i__ = k; i__ <= i__2; ++i__) {
	    if (i__ == j) {
		goto L100;
	    }
	    if (a[i__ + j * a_dim1] != 0.) {
		goto L110;
	    }
L100:
	    ;
	}

	m = k;
	iexc = 2;
	goto L20;
L110:
	;
    }

L120:
    i__1 = l;
    for (i__ = k; i__ <= i__1; ++i__) {
	scale[i__] = 1.;
/* L130: */
    }

    if (lsame_(job, "P")) {
	goto L210;
    }

/*
       Balance the submatrix in rows K to L.

       Iterative loop for norm reduction
*/

    sfmin1 = SAFEMINIMUM / PRECISION;
    sfmax1 = 1. / sfmin1;
    sfmin2 = sfmin1 * 8.;
    sfmax2 = 1. / sfmin2;
L140:
    noconv = FALSE_;

    i__1 = l;
    for (i__ = k; i__ <= i__1; ++i__) {
	c__ = 0.;
	r__ = 0.;

	i__2 = l;
	for (j = k; j <= i__2; ++j) {
	    if (j == i__) {
		goto L150;
	    }
	    c__ += (d__1 = a[j + i__ * a_dim1], abs(d__1));
	    r__ += (d__1 = a[i__ + j * a_dim1], abs(d__1));
L150:
	    ;
	}
	ica = idamax_(&l, &a[i__ * a_dim1 + 1], &c__1);
	ca = (d__1 = a[ica + i__ * a_dim1], abs(d__1));
	i__2 = *n - k + 1;
	ira = idamax_(&i__2, &a[i__ + k * a_dim1], lda);
	ra = (d__1 = a[i__ + (ira + k - 1) * a_dim1], abs(d__1));

/*        Guard against zero C or R due to underflow. */

	if (c__ == 0. || r__ == 0.) {
	    goto L200;
	}
	g = r__ / 8.;
	f = 1.;
	s = c__ + r__;
L160:
/* Computing MAX */
	d__1 = max(f,c__);
/* Computing MIN */
	d__2 = min(r__,g);
	if (c__ >= g || max(d__1,ca) >= sfmax2 || min(d__2,ra) <= sfmin2) {
	    goto L170;
	}
	f *= 8.;
	c__ *= 8.;
	ca *= 8.;
	r__ /= 8.;
	g /= 8.;
	ra /= 8.;
	goto L160;

L170:
	g = c__ / 8.;
L180:
/* Computing MIN */
	d__1 = min(f,c__), d__1 = min(d__1,g);
	if (g < r__ || max(r__,ra) >= sfmax2 || min(d__1,ca) <= sfmin2) {
	    goto L190;
	}
	f /= 8.;
	c__ /= 8.;
	g /= 8.;
	ca /= 8.;
	r__ *= 8.;
	ra *= 8.;
	goto L180;

/*        Now balance. */

L190:
	if (c__ + r__ >= s * .95) {
	    goto L200;
	}
	if ((f < 1. && scale[i__] < 1.)) {
	    if (f * scale[i__] <= sfmin1) {
		goto L200;
	    }
	}
	if ((f > 1. && scale[i__] > 1.)) {
	    if (scale[i__] >= sfmax1 / f) {
		goto L200;
	    }
	}
	g = 1. / f;
	scale[i__] *= f;
	noconv = TRUE_;

	i__2 = *n - k + 1;
	dscal_(&i__2, &g, &a[i__ + k * a_dim1], lda);
	dscal_(&l, &f, &a[i__ * a_dim1 + 1], &c__1);

L200:
	;
    }

    if (noconv) {
	goto L140;
    }

L210:
    *ilo = k;
    *ihi = l;

    return 0;

/*     End of DGEBAL */

} /* dgebal_ */

/* Subroutine */ int dgebd2_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *d__, doublereal *e, doublereal *tauq, doublereal *
	taup, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int dlarf_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *), dlarfg_(integer *, doublereal *,
	    doublereal *, integer *, doublereal *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DGEBD2 reduces a real general m by n matrix A to upper or lower
    bidiagonal form B by an orthogonal transformation: Q' * A * P = B.

    If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows in the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns in the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the m by n general matrix to be reduced.
            On exit,
            if m >= n, the diagonal and the first superdiagonal are
              overwritten with the upper bidiagonal matrix B; the
              elements below the diagonal, with the array TAUQ, represent
              the orthogonal matrix Q as a product of elementary
              reflectors, and the elements above the first superdiagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors;
            if m < n, the diagonal and the first subdiagonal are
              overwritten with the lower bidiagonal matrix B; the
              elements below the first subdiagonal, with the array TAUQ,
              represent the orthogonal matrix Q as a product of
              elementary reflectors, and the elements above the diagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors.
            See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    D       (output) DOUBLE PRECISION array, dimension (min(M,N))
            The diagonal elements of the bidiagonal matrix B:
            D(i) = A(i,i).

    E       (output) DOUBLE PRECISION array, dimension (min(M,N)-1)
            The off-diagonal elements of the bidiagonal matrix B:
            if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1;
            if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1.

    TAUQ    (output) DOUBLE PRECISION array dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix Q. See Further Details.

    TAUP    (output) DOUBLE PRECISION array, dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix P. See Further Details.

    WORK    (workspace) DOUBLE PRECISION array, dimension (max(M,N))

    INFO    (output) INTEGER
            = 0: successful exit.
            < 0: if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrices Q and P are represented as products of elementary
    reflectors:

    If m >= n,

       Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i);
    u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    If m < n,

       Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i);
    u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    The contents of A on exit are illustrated by the following examples:

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):

      (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 )
      (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 )
      (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 )
      (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 )
      (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 )
      (  v1  v2  v3  v4  v5 )

    where d and e denote diagonal and off-diagonal elements of B, vi
    denotes an element of the vector defining H(i), and ui an element of
    the vector defining G(i).

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("DGEBD2", &i__1);
	return 0;
    }

    if (*m >= *n) {

/*        Reduce to upper bidiagonal form */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector H(i) to annihilate A(i+1:m,i) */

	    i__2 = *m - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[min(i__3,*m) + i__ *
		    a_dim1], &c__1, &tauq[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;

/*           Apply H(i) to A(i:m,i+1:n) from the left */

	    i__2 = *m - i__ + 1;
	    i__3 = *n - i__;
	    dlarf_("Left", &i__2, &i__3, &a[i__ + i__ * a_dim1], &c__1, &tauq[
		    i__], &a[i__ + (i__ + 1) * a_dim1], lda, &work[1]);
	    a[i__ + i__ * a_dim1] = d__[i__];

	    if (i__ < *n) {

/*
                Generate elementary reflector G(i) to annihilate
                A(i,i+2:n)
*/

		i__2 = *n - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + (i__ + 1) * a_dim1], &a[i__ + min(
			i__3,*n) * a_dim1], lda, &taup[i__]);
		e[i__] = a[i__ + (i__ + 1) * a_dim1];
		a[i__ + (i__ + 1) * a_dim1] = 1.;

/*              Apply G(i) to A(i+1:m,i+1:n) from the right */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dlarf_("Right", &i__2, &i__3, &a[i__ + (i__ + 1) * a_dim1],
			lda, &taup[i__], &a[i__ + 1 + (i__ + 1) * a_dim1],
			lda, &work[1]);
		a[i__ + (i__ + 1) * a_dim1] = e[i__];
	    } else {
		taup[i__] = 0.;
	    }
/* L10: */
	}
    } else {

/*        Reduce to lower bidiagonal form */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector G(i) to annihilate A(i,i+1:n) */

	    i__2 = *n - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[i__ + min(i__3,*n) *
		    a_dim1], lda, &taup[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;

/*           Apply G(i) to A(i+1:m,i:n) from the right */

	    i__2 = *m - i__;
	    i__3 = *n - i__ + 1;
/* Computing MIN */
	    i__4 = i__ + 1;
	    dlarf_("Right", &i__2, &i__3, &a[i__ + i__ * a_dim1], lda, &taup[
		    i__], &a[min(i__4,*m) + i__ * a_dim1], lda, &work[1]);
	    a[i__ + i__ * a_dim1] = d__[i__];

	    if (i__ < *m) {

/*
                Generate elementary reflector H(i) to annihilate
                A(i+2:m,i)
*/

		i__2 = *m - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[min(i__3,*m) +
			i__ * a_dim1], &c__1, &tauq[i__]);
		e[i__] = a[i__ + 1 + i__ * a_dim1];
		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Apply H(i) to A(i+1:m,i+1:n) from the left */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dlarf_("Left", &i__2, &i__3, &a[i__ + 1 + i__ * a_dim1], &
			c__1, &tauq[i__], &a[i__ + 1 + (i__ + 1) * a_dim1],
			lda, &work[1]);
		a[i__ + 1 + i__ * a_dim1] = e[i__];
	    } else {
		tauq[i__] = 0.;
	    }
/* L20: */
	}
    }
    return 0;

/*     End of DGEBD2 */

} /* dgebd2_ */

/* Subroutine */ int dgebrd_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *d__, doublereal *e, doublereal *tauq, doublereal *
	taup, doublereal *work, integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer i__, j, nb, nx;
    static doublereal ws;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    static integer nbmin, iinfo, minmn;
    extern /* Subroutine */ int dgebd2_(integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, integer *), dlabrd_(integer *, integer *, integer *
	    , doublereal *, integer *, doublereal *, doublereal *, doublereal
	    *, doublereal *, doublereal *, integer *, doublereal *, integer *)
	    , xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwrkx, ldwrky, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DGEBRD reduces a general real M-by-N matrix A to upper or lower
    bidiagonal form B by an orthogonal transformation: Q**T * A * P = B.

    If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows in the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns in the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the M-by-N general matrix to be reduced.
            On exit,
            if m >= n, the diagonal and the first superdiagonal are
              overwritten with the upper bidiagonal matrix B; the
              elements below the diagonal, with the array TAUQ, represent
              the orthogonal matrix Q as a product of elementary
              reflectors, and the elements above the first superdiagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors;
            if m < n, the diagonal and the first subdiagonal are
              overwritten with the lower bidiagonal matrix B; the
              elements below the first subdiagonal, with the array TAUQ,
              represent the orthogonal matrix Q as a product of
              elementary reflectors, and the elements above the diagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors.
            See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    D       (output) DOUBLE PRECISION array, dimension (min(M,N))
            The diagonal elements of the bidiagonal matrix B:
            D(i) = A(i,i).

    E       (output) DOUBLE PRECISION array, dimension (min(M,N)-1)
            The off-diagonal elements of the bidiagonal matrix B:
            if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1;
            if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1.

    TAUQ    (output) DOUBLE PRECISION array dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix Q. See Further Details.

    TAUP    (output) DOUBLE PRECISION array, dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix P. See Further Details.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The length of the array WORK.  LWORK >= max(1,M,N).
            For optimum performance LWORK >= (M+N)*NB, where NB
            is the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrices Q and P are represented as products of elementary
    reflectors:

    If m >= n,

       Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i);
    u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    If m < n,

       Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i);
    u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    The contents of A on exit are illustrated by the following examples:

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):

      (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 )
      (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 )
      (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 )
      (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 )
      (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 )
      (  v1  v2  v3  v4  v5 )

    where d and e denote diagonal and off-diagonal elements of B, vi
    denotes an element of the vector defining H(i), and ui an element of
    the vector defining G(i).

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    --work;

    /* Function Body */
    *info = 0;
/* Computing MAX */
    i__1 = 1, i__2 = ilaenv_(&c__1, "DGEBRD", " ", m, n, &c_n1, &c_n1, (
	    ftnlen)6, (ftnlen)1);
    nb = max(i__1,i__2);
    lwkopt = (*m + *n) * nb;
    work[1] = (doublereal) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = max(1,*m);
	if ((*lwork < max(i__1,*n) && ! lquery)) {
	    *info = -10;
	}
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("DGEBRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    minmn = min(*m,*n);
    if (minmn == 0) {
	work[1] = 1.;
	return 0;
    }

    ws = (doublereal) max(*m,*n);
    ldwrkx = *m;
    ldwrky = *n;

    if ((nb > 1 && nb < minmn)) {

/*
          Set the crossover point NX.

   Computing MAX
*/
	i__1 = nb, i__2 = ilaenv_(&c__3, "DGEBRD", " ", m, n, &c_n1, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = max(i__1,i__2);

/*        Determine when to switch from blocked to unblocked code. */

	if (nx < minmn) {
	    ws = (doublereal) ((*m + *n) * nb);
	    if ((doublereal) (*lwork) < ws) {

/*
                Not enough work space for the optimal NB, consider using
                a smaller block size.
*/

		nbmin = ilaenv_(&c__2, "DGEBRD", " ", m, n, &c_n1, &c_n1, (
			ftnlen)6, (ftnlen)1);
		if (*lwork >= (*m + *n) * nbmin) {
		    nb = *lwork / (*m + *n);
		} else {
		    nb = 1;
		    nx = minmn;
		}
	    }
	}
    } else {
	nx = minmn;
    }

    i__1 = minmn - nx;
    i__2 = nb;
    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {

/*
          Reduce rows and columns i:i+nb-1 to bidiagonal form and return
          the matrices X and Y which are needed to update the unreduced
          part of the matrix
*/

	i__3 = *m - i__ + 1;
	i__4 = *n - i__ + 1;
	dlabrd_(&i__3, &i__4, &nb, &a[i__ + i__ * a_dim1], lda, &d__[i__], &e[
		i__], &tauq[i__], &taup[i__], &work[1], &ldwrkx, &work[ldwrkx
		* nb + 1], &ldwrky);

/*
          Update the trailing submatrix A(i+nb:m,i+nb:n), using an update
          of the form  A := A - V*Y' - X*U'
*/

	i__3 = *m - i__ - nb + 1;
	i__4 = *n - i__ - nb + 1;
	dgemm_("No transpose", "Transpose", &i__3, &i__4, &nb, &c_b151, &a[
		i__ + nb + i__ * a_dim1], lda, &work[ldwrkx * nb + nb + 1], &
		ldwrky, &c_b15, &a[i__ + nb + (i__ + nb) * a_dim1], lda);
	i__3 = *m - i__ - nb + 1;
	i__4 = *n - i__ - nb + 1;
	dgemm_("No transpose", "No transpose", &i__3, &i__4, &nb, &c_b151, &
		work[nb + 1], &ldwrkx, &a[i__ + (i__ + nb) * a_dim1], lda, &
		c_b15, &a[i__ + nb + (i__ + nb) * a_dim1], lda);

/*        Copy diagonal and off-diagonal elements of B back into A */

	if (*m >= *n) {
	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j + j * a_dim1] = d__[j];
		a[j + (j + 1) * a_dim1] = e[j];
/* L10: */
	    }
	} else {
	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j + j * a_dim1] = d__[j];
		a[j + 1 + j * a_dim1] = e[j];
/* L20: */
	    }
	}
/* L30: */
    }

/*     Use unblocked code to reduce the remainder of the matrix */

    i__2 = *m - i__ + 1;
    i__1 = *n - i__ + 1;
    dgebd2_(&i__2, &i__1, &a[i__ + i__ * a_dim1], lda, &d__[i__], &e[i__], &
	    tauq[i__], &taup[i__], &work[1], &iinfo);
    work[1] = ws;
    return 0;

/*     End of DGEBRD */

} /* dgebrd_ */

/* Subroutine */ int dgeev_(char *jobvl, char *jobvr, integer *n, doublereal *
	a, integer *lda, doublereal *wr, doublereal *wi, doublereal *vl,
	integer *ldvl, doublereal *vr, integer *ldvr, doublereal *work,
	integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1,
	    i__2, i__3, i__4;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, k;
    static doublereal r__, cs, sn;
    static integer ihi;
    static doublereal scl;
    static integer ilo;
    static doublereal dum[1], eps;
    static integer ibal;
    static char side[1];
    static integer maxb;
    static doublereal anrm;
    static integer ierr, itau;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    static integer iwrk, nout;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern doublereal dlapy2_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *), dgebak_(
	    char *, char *, integer *, integer *, integer *, doublereal *,
	    integer *, doublereal *, integer *, integer *),
	    dgebal_(char *, integer *, doublereal *, integer *, integer *,
	    integer *, doublereal *, integer *);
    static logical scalea;

    static doublereal cscale;
    extern doublereal dlange_(char *, integer *, integer *, doublereal *,
	    integer *, doublereal *);
    extern /* Subroutine */ int dgehrd_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    integer *), dlascl_(char *, integer *, integer *, doublereal *,
	    doublereal *, integer *, integer *, doublereal *, integer *,
	    integer *);
    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *),
	    dlartg_(doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *), xerbla_(char *, integer *);
    static logical select[1];
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static doublereal bignum;
    extern /* Subroutine */ int dorghr_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    integer *), dhseqr_(char *, char *, integer *, integer *, integer
	    *, doublereal *, integer *, doublereal *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, integer *), dtrevc_(char *, char *, logical *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, integer *, integer *, doublereal *, integer *);
    static integer minwrk, maxwrk;
    static logical wantvl;
    static doublereal smlnum;
    static integer hswork;
    static logical lquery, wantvr;


/*
    -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       December 8, 1999


    Purpose
    =======

    DGEEV computes for an N-by-N real nonsymmetric matrix A, the
    eigenvalues and, optionally, the left and/or right eigenvectors.

    The right eigenvector v(j) of A satisfies
                     A * v(j) = lambda(j) * v(j)
    where lambda(j) is its eigenvalue.
    The left eigenvector u(j) of A satisfies
                  u(j)**H * A = lambda(j) * u(j)**H
    where u(j)**H denotes the conjugate transpose of u(j).

    The computed eigenvectors are normalized to have Euclidean norm
    equal to 1 and largest component real.

    Arguments
    =========

    JOBVL   (input) CHARACTER*1
            = 'N': left eigenvectors of A are not computed;
            = 'V': left eigenvectors of A are computed.

    JOBVR   (input) CHARACTER*1
            = 'N': right eigenvectors of A are not computed;
            = 'V': right eigenvectors of A are computed.

    N       (input) INTEGER
            The order of the matrix A. N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the N-by-N matrix A.
            On exit, A has been overwritten.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    WR      (output) DOUBLE PRECISION array, dimension (N)
    WI      (output) DOUBLE PRECISION array, dimension (N)
            WR and WI contain the real and imaginary parts,
            respectively, of the computed eigenvalues.  Complex
            conjugate pairs of eigenvalues appear consecutively
            with the eigenvalue having the positive imaginary part
            first.

    VL      (output) DOUBLE PRECISION array, dimension (LDVL,N)
            If JOBVL = 'V', the left eigenvectors u(j) are stored one
            after another in the columns of VL, in the same order
            as their eigenvalues.
            If JOBVL = 'N', VL is not referenced.
            If the j-th eigenvalue is real, then u(j) = VL(:,j),
            the j-th column of VL.
            If the j-th and (j+1)-st eigenvalues form a complex
            conjugate pair, then u(j) = VL(:,j) + i*VL(:,j+1) and
            u(j+1) = VL(:,j) - i*VL(:,j+1).

    LDVL    (input) INTEGER
            The leading dimension of the array VL.  LDVL >= 1; if
            JOBVL = 'V', LDVL >= N.

    VR      (output) DOUBLE PRECISION array, dimension (LDVR,N)
            If JOBVR = 'V', the right eigenvectors v(j) are stored one
            after another in the columns of VR, in the same order
            as their eigenvalues.
            If JOBVR = 'N', VR is not referenced.
            If the j-th eigenvalue is real, then v(j) = VR(:,j),
            the j-th column of VR.
            If the j-th and (j+1)-st eigenvalues form a complex
            conjugate pair, then v(j) = VR(:,j) + i*VR(:,j+1) and
            v(j+1) = VR(:,j) - i*VR(:,j+1).

    LDVR    (input) INTEGER
            The leading dimension of the array VR.  LDVR >= 1; if
            JOBVR = 'V', LDVR >= N.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.  LWORK >= max(1,3*N), and
            if JOBVL = 'V' or JOBVR = 'V', LWORK >= 4*N.  For good
            performance, LWORK must generally be larger.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = i, the QR algorithm failed to compute all the
                  eigenvalues, and no eigenvectors have been computed;
                  elements i+1:N of WR and WI contain eigenvalues which
                  have converged.

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --wr;
    --wi;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1 * 1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1 * 1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    wantvl = lsame_(jobvl, "V");
    wantvr = lsame_(jobvr, "V");
    if ((! wantvl && ! lsame_(jobvl, "N"))) {
	*info = -1;
    } else if ((! wantvr && ! lsame_(jobvr, "N"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < max(1,*n)) {
	*info = -5;
    } else if (*ldvl < 1 || (wantvl && *ldvl < *n)) {
	*info = -9;
    } else if (*ldvr < 1 || (wantvr && *ldvr < *n)) {
	*info = -11;
    }

/*
       Compute workspace
        (Note: Comments in the code beginning "Workspace:" describe the
         minimal amount of workspace needed at that point in the code,
         as well as the preferred amount for good performance.
         NB refers to the optimal block size for the immediately
         following subroutine, as returned by ILAENV.
         HSWORK refers to the workspace preferred by DHSEQR, as
         calculated below. HSWORK is computed assuming ILO=1 and IHI=N,
         the worst case.)
*/

    minwrk = 1;
    if ((*info == 0 && (*lwork >= 1 || lquery))) {
	maxwrk = ((*n) << (1)) + *n * ilaenv_(&c__1, "DGEHRD", " ", n, &c__1,
		n, &c__0, (ftnlen)6, (ftnlen)1);
	if ((! wantvl && ! wantvr)) {
/* Computing MAX */
	    i__1 = 1, i__2 = *n * 3;
	    minwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = ilaenv_(&c__8, "DHSEQR", "EN", n, &c__1, n, &c_n1, (ftnlen)
		    6, (ftnlen)2);
	    maxb = max(i__1,2);
/*
   Computing MIN
   Computing MAX
*/
	    i__3 = 2, i__4 = ilaenv_(&c__4, "DHSEQR", "EN", n, &c__1, n, &
		    c_n1, (ftnlen)6, (ftnlen)2);
	    i__1 = min(maxb,*n), i__2 = max(i__3,i__4);
	    k = min(i__1,i__2);
/* Computing MAX */
	    i__1 = k * (k + 2), i__2 = (*n) << (1);
	    hswork = max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + 1, i__1 = max(i__1,i__2), i__2 = *n +
		    hswork;
	    maxwrk = max(i__1,i__2);
	} else {
/* Computing MAX */
	    i__1 = 1, i__2 = (*n) << (2);
	    minwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = ((*n) << (1)) + (*n - 1) * ilaenv_(&c__1,
		    "DORGHR", " ", n, &c__1, n, &c_n1, (ftnlen)6, (ftnlen)1);
	    maxwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = ilaenv_(&c__8, "DHSEQR", "SV", n, &c__1, n, &c_n1, (ftnlen)
		    6, (ftnlen)2);
	    maxb = max(i__1,2);
/*
   Computing MIN
   Computing MAX
*/
	    i__3 = 2, i__4 = ilaenv_(&c__4, "DHSEQR", "SV", n, &c__1, n, &
		    c_n1, (ftnlen)6, (ftnlen)2);
	    i__1 = min(maxb,*n), i__2 = max(i__3,i__4);
	    k = min(i__1,i__2);
/* Computing MAX */
	    i__1 = k * (k + 2), i__2 = (*n) << (1);
	    hswork = max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + 1, i__1 = max(i__1,i__2), i__2 = *n +
		    hswork;
	    maxwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = (*n) << (2);
	    maxwrk = max(i__1,i__2);
	}
	work[1] = (doublereal) maxwrk;
    }
    if ((*lwork < minwrk && ! lquery)) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEEV ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = PRECISION;
    smlnum = SAFEMINIMUM;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, dum);
    scalea = FALSE_;
    if ((anrm > 0. && anrm < smlnum)) {
	scalea = TRUE_;
	cscale = smlnum;
    } else if (anrm > bignum) {
	scalea = TRUE_;
	cscale = bignum;
    }
    if (scalea) {
	dlascl_("G", &c__0, &c__0, &anrm, &cscale, n, n, &a[a_offset], lda, &
		ierr);
    }

/*
       Balance the matrix
       (Workspace: need N)
*/

    ibal = 1;
    dgebal_("B", n, &a[a_offset], lda, &ilo, &ihi, &work[ibal], &ierr);

/*
       Reduce to upper Hessenberg form
       (Workspace: need 3*N, prefer 2*N+N*NB)
*/

    itau = ibal + *n;
    iwrk = itau + *n;
    i__1 = *lwork - iwrk + 1;
    dgehrd_(n, &ilo, &ihi, &a[a_offset], lda, &work[itau], &work[iwrk], &i__1,
	     &ierr);

    if (wantvl) {

/*
          Want left eigenvectors
          Copy Householder vectors to VL
*/

	*(unsigned char *)side = 'L';
	dlacpy_("L", n, n, &a[a_offset], lda, &vl[vl_offset], ldvl)
		;

/*
          Generate orthogonal matrix in VL
          (Workspace: need 3*N-1, prefer 2*N+(N-1)*NB)
*/

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, &ilo, &ihi, &vl[vl_offset], ldvl, &work[itau], &work[iwrk],
		 &i__1, &ierr);

/*
          Perform QR iteration, accumulating Schur vectors in VL
          (Workspace: need N+1, prefer N+HSWORK (see comments) )
*/

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("S", "V", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &
		vl[vl_offset], ldvl, &work[iwrk], &i__1, info);

	if (wantvr) {

/*
             Want left and right eigenvectors
             Copy Schur vectors to VR
*/

	    *(unsigned char *)side = 'B';
	    dlacpy_("F", n, n, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr);
	}

    } else if (wantvr) {

/*
          Want right eigenvectors
          Copy Householder vectors to VR
*/

	*(unsigned char *)side = 'R';
	dlacpy_("L", n, n, &a[a_offset], lda, &vr[vr_offset], ldvr)
		;

/*
          Generate orthogonal matrix in VR
          (Workspace: need 3*N-1, prefer 2*N+(N-1)*NB)
*/

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, &ilo, &ihi, &vr[vr_offset], ldvr, &work[itau], &work[iwrk],
		 &i__1, &ierr);

/*
          Perform QR iteration, accumulating Schur vectors in VR
          (Workspace: need N+1, prefer N+HSWORK (see comments) )
*/

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("S", "V", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &
		vr[vr_offset], ldvr, &work[iwrk], &i__1, info);

    } else {

/*
          Compute eigenvalues only
          (Workspace: need N+1, prefer N+HSWORK (see comments) )
*/

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("E", "N", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &
		vr[vr_offset], ldvr, &work[iwrk], &i__1, info);
    }

/*     If INFO > 0 from DHSEQR, then quit */

    if (*info > 0) {
	goto L50;
    }

    if (wantvl || wantvr) {

/*
          Compute left and/or right eigenvectors
          (Workspace: need 4*N)
*/

	dtrevc_(side, "B", select, n, &a[a_offset], lda, &vl[vl_offset], ldvl,
		 &vr[vr_offset], ldvr, n, &nout, &work[iwrk], &ierr);
    }

    if (wantvl) {

/*
          Undo balancing of left eigenvectors
          (Workspace: need N)
*/

	dgebak_("B", "L", n, &ilo, &ihi, &work[ibal], n, &vl[vl_offset], ldvl,
		 &ierr);

/*        Normalize left eigenvectors and make largest component real */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wi[i__] == 0.) {
		scl = 1. / dnrm2_(n, &vl[i__ * vl_dim1 + 1], &c__1);
		dscal_(n, &scl, &vl[i__ * vl_dim1 + 1], &c__1);
	    } else if (wi[i__] > 0.) {
		d__1 = dnrm2_(n, &vl[i__ * vl_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vl[(i__ + 1) * vl_dim1 + 1], &c__1);
		scl = 1. / dlapy2_(&d__1, &d__2);
		dscal_(n, &scl, &vl[i__ * vl_dim1 + 1], &c__1);
		dscal_(n, &scl, &vl[(i__ + 1) * vl_dim1 + 1], &c__1);
		i__2 = *n;
		for (k = 1; k <= i__2; ++k) {
/* Computing 2nd power */
		    d__1 = vl[k + i__ * vl_dim1];
/* Computing 2nd power */
		    d__2 = vl[k + (i__ + 1) * vl_dim1];
		    work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
/* L10: */
		}
		k = idamax_(n, &work[iwrk], &c__1);
		dlartg_(&vl[k + i__ * vl_dim1], &vl[k + (i__ + 1) * vl_dim1],
			&cs, &sn, &r__);
		drot_(n, &vl[i__ * vl_dim1 + 1], &c__1, &vl[(i__ + 1) *
			vl_dim1 + 1], &c__1, &cs, &sn);
		vl[k + (i__ + 1) * vl_dim1] = 0.;
	    }
/* L20: */
	}
    }

    if (wantvr) {

/*
          Undo balancing of right eigenvectors
          (Workspace: need N)
*/

	dgebak_("B", "R", n, &ilo, &ihi, &work[ibal], n, &vr[vr_offset], ldvr,
		 &ierr);

/*        Normalize right eigenvectors and make largest component real */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wi[i__] == 0.) {
		scl = 1. / dnrm2_(n, &vr[i__ * vr_dim1 + 1], &c__1);
		dscal_(n, &scl, &vr[i__ * vr_dim1 + 1], &c__1);
	    } else if (wi[i__] > 0.) {
		d__1 = dnrm2_(n, &vr[i__ * vr_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vr[(i__ + 1) * vr_dim1 + 1], &c__1);
		scl = 1. / dlapy2_(&d__1, &d__2);
		dscal_(n, &scl, &vr[i__ * vr_dim1 + 1], &c__1);
		dscal_(n, &scl, &vr[(i__ + 1) * vr_dim1 + 1], &c__1);
		i__2 = *n;
		for (k = 1; k <= i__2; ++k) {
/* Computing 2nd power */
		    d__1 = vr[k + i__ * vr_dim1];
/* Computing 2nd power */
		    d__2 = vr[k + (i__ + 1) * vr_dim1];
		    work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
/* L30: */
		}
		k = idamax_(n, &work[iwrk], &c__1);
		dlartg_(&vr[k + i__ * vr_dim1], &vr[k + (i__ + 1) * vr_dim1],
			&cs, &sn, &r__);
		drot_(n, &vr[i__ * vr_dim1 + 1], &c__1, &vr[(i__ + 1) *
			vr_dim1 + 1], &c__1, &cs, &sn);
		vr[k + (i__ + 1) * vr_dim1] = 0.;
	    }
/* L40: */
	}
    }

/*     Undo scaling if necessary */

L50:
    if (scalea) {
	i__1 = *n - *info;
/* Computing MAX */
	i__3 = *n - *info;
	i__2 = max(i__3,1);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[*info +
		1], &i__2, &ierr);
	i__1 = *n - *info;
/* Computing MAX */
	i__3 = *n - *info;
	i__2 = max(i__3,1);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[*info +
		1], &i__2, &ierr);
	if (*info > 0) {
	    i__1 = ilo - 1;
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[1],
		    n, &ierr);
	    i__1 = ilo - 1;
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[1],
		    n, &ierr);
	}
    }

    work[1] = (doublereal) maxwrk;
    return 0;

/*     End of DGEEV */

} /* dgeev_ */

/* Subroutine */ int dgehd2_(integer *n, integer *ilo, integer *ihi,
	doublereal *a, integer *lda, doublereal *tau, doublereal *work,
	integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__;
    static doublereal aii;
    extern /* Subroutine */ int dlarf_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *), dlarfg_(integer *, doublereal *,
	    doublereal *, integer *, doublereal *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DGEHD2 reduces a real general matrix A to upper Hessenberg form H by
    an orthogonal similarity transformation:  Q' * A * Q = H .

    Arguments
    =========

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    ILO     (input) INTEGER
    IHI     (input) INTEGER
            It is assumed that A is already upper triangular in rows
            and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally
            set by a previous call to DGEBAL; otherwise they should be
            set to 1 and N respectively. See Further Details.
            1 <= ILO <= IHI <= max(1,N).

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the n by n general matrix to be reduced.
            On exit, the upper triangle and the first subdiagonal of A
            are overwritten with the upper Hessenberg matrix H, and the
            elements below the first subdiagonal, with the array TAU,
            represent the orthogonal matrix Q as a product of elementary
            reflectors. See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    TAU     (output) DOUBLE PRECISION array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace) DOUBLE PRECISION array, dimension (N)

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrix Q is represented as a product of (ihi-ilo) elementary
    reflectors

       Q = H(ilo) H(ilo+1) . . . H(ihi-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on
    exit in A(i+2:ihi,i), and tau in TAU(i).

    The contents of A are illustrated by the following example, with
    n = 7, ilo = 2 and ihi = 6:

    on entry,                        on exit,

    ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      h   h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h )
    (                         a )    (                          a )

    where a denotes an element of the original matrix A, h denotes a
    modified element of the upper Hessenberg matrix H, and vi denotes an
    element of the vector defining H(i).

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*ilo < 1 || *ilo > max(1,*n)) {
	*info = -2;
    } else if (*ihi < min(*ilo,*n) || *ihi > *n) {
	*info = -3;
    } else if (*lda < max(1,*n)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEHD2", &i__1);
	return 0;
    }

    i__1 = *ihi - 1;
    for (i__ = *ilo; i__ <= i__1; ++i__) {

/*        Compute elementary reflector H(i) to annihilate A(i+2:ihi,i) */

	i__2 = *ihi - i__;
/* Computing MIN */
	i__3 = i__ + 2;
	dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[min(i__3,*n) + i__ *
		a_dim1], &c__1, &tau[i__]);
	aii = a[i__ + 1 + i__ * a_dim1];
	a[i__ + 1 + i__ * a_dim1] = 1.;

/*        Apply H(i) to A(1:ihi,i+1:ihi) from the right */

	i__2 = *ihi - i__;
	dlarf_("Right", ihi, &i__2, &a[i__ + 1 + i__ * a_dim1], &c__1, &tau[
		i__], &a[(i__ + 1) * a_dim1 + 1], lda, &work[1]);

/*        Apply H(i) to A(i+1:ihi,i+1:n) from the left */

	i__2 = *ihi - i__;
	i__3 = *n - i__;
	dlarf_("Left", &i__2, &i__3, &a[i__ + 1 + i__ * a_dim1], &c__1, &tau[
		i__], &a[i__ + 1 + (i__ + 1) * a_dim1], lda, &work[1]);

	a[i__ + 1 + i__ * a_dim1] = aii;
/* L10: */
    }

    return 0;

/*     End of DGEHD2 */

} /* dgehd2_ */

/* Subroutine */ int dgehrd_(integer *n, integer *ilo, integer *ihi,
	doublereal *a, integer *lda, doublereal *tau, doublereal *work,
	integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer i__;
    static doublereal t[4160]	/* was [65][64] */;
    static integer ib;
    static doublereal ei;
    static integer nb, nh, nx, iws;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    static integer nbmin, iinfo;
    extern /* Subroutine */ int dgehd2_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *),
	    dlarfb_(char *, char *, char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, integer *), dlahrd_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DGEHRD reduces a real general matrix A to upper Hessenberg form H by
    an orthogonal similarity transformation:  Q' * A * Q = H .

    Arguments
    =========

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    ILO     (input) INTEGER
    IHI     (input) INTEGER
            It is assumed that A is already upper triangular in rows
            and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally
            set by a previous call to DGEBAL; otherwise they should be
            set to 1 and N respectively. See Further Details.
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the N-by-N general matrix to be reduced.
            On exit, the upper triangle and the first subdiagonal of A
            are overwritten with the upper Hessenberg matrix H, and the
            elements below the first subdiagonal, with the array TAU,
            represent the orthogonal matrix Q as a product of elementary
            reflectors. See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    TAU     (output) DOUBLE PRECISION array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details). Elements 1:ILO-1 and IHI:N-1 of TAU are set to
            zero.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The length of the array WORK.  LWORK >= max(1,N).
            For optimum performance LWORK >= N*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrix Q is represented as a product of (ihi-ilo) elementary
    reflectors

       Q = H(ilo) H(ilo+1) . . . H(ihi-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on
    exit in A(i+2:ihi,i), and tau in TAU(i).

    The contents of A are illustrated by the following example, with
    n = 7, ilo = 2 and ihi = 6:

    on entry,                        on exit,

    ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      h   h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h )
    (                         a )    (                          a )

    where a denotes an element of the original matrix A, h denotes a
    modified element of the upper Hessenberg matrix H, and vi denotes an
    element of the vector defining H(i).

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
/* Computing MIN */
    i__1 = 64, i__2 = ilaenv_(&c__1, "DGEHRD", " ", n, ilo, ihi, &c_n1, (
	    ftnlen)6, (ftnlen)1);
    nb = min(i__1,i__2);
    lwkopt = *n * nb;
    work[1] = (doublereal) lwkopt;
    lquery = *lwork == -1;
    if (*n < 0) {
	*info = -1;
    } else if (*ilo < 1 || *ilo > max(1,*n)) {
	*info = -2;
    } else if (*ihi < min(*ilo,*n) || *ihi > *n) {
	*info = -3;
    } else if (*lda < max(1,*n)) {
	*info = -5;
    } else if ((*lwork < max(1,*n) && ! lquery)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEHRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Set elements 1:ILO-1 and IHI:N-1 of TAU to zero */

    i__1 = *ilo - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tau[i__] = 0.;
/* L10: */
    }
    i__1 = *n - 1;
    for (i__ = max(1,*ihi); i__ <= i__1; ++i__) {
	tau[i__] = 0.;
/* L20: */
    }

/*     Quick return if possible */

    nh = *ihi - *ilo + 1;
    if (nh <= 1) {
	work[1] = 1.;
	return 0;
    }

/*
       Determine the block size.

   Computing MIN
*/
    i__1 = 64, i__2 = ilaenv_(&c__1, "DGEHRD", " ", n, ilo, ihi, &c_n1, (
	    ftnlen)6, (ftnlen)1);
    nb = min(i__1,i__2);
    nbmin = 2;
    iws = 1;
    if ((nb > 1 && nb < nh)) {

/*
          Determine when to cross over from blocked to unblocked code
          (last block is always handled by unblocked code).

   Computing MAX
*/
	i__1 = nb, i__2 = ilaenv_(&c__3, "DGEHRD", " ", n, ilo, ihi, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = max(i__1,i__2);
	if (nx < nh) {

/*           Determine if workspace is large enough for blocked code. */

	    iws = *n * nb;
	    if (*lwork < iws) {

/*
                Not enough workspace to use optimal NB:  determine the
                minimum value of NB, and reduce NB or force use of
                unblocked code.

   Computing MAX
*/
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGEHRD", " ", n, ilo, ihi, &
			c_n1, (ftnlen)6, (ftnlen)1);
		nbmin = max(i__1,i__2);
		if (*lwork >= *n * nbmin) {
		    nb = *lwork / *n;
		} else {
		    nb = 1;
		}
	    }
	}
    }
    ldwork = *n;

    if (nb < nbmin || nb >= nh) {

/*        Use unblocked code below */

	i__ = *ilo;

    } else {

/*        Use blocked code */

	i__1 = *ihi - 1 - nx;
	i__2 = nb;
	for (i__ = *ilo; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = nb, i__4 = *ihi - i__;
	    ib = min(i__3,i__4);

/*
             Reduce columns i:i+ib-1 to Hessenberg form, returning the
             matrices V and T of the block reflector H = I - V*T*V'
             which performs the reduction, and also the matrix Y = A*V*T
*/

	    dlahrd_(ihi, &i__, &ib, &a[i__ * a_dim1 + 1], lda, &tau[i__], t, &
		    c__65, &work[1], &ldwork);

/*
             Apply the block reflector H to A(1:ihi,i+ib:ihi) from the
             right, computing  A := A - Y * V'. V(i+ib,ib-1) must be set
             to 1.
*/

	    ei = a[i__ + ib + (i__ + ib - 1) * a_dim1];
	    a[i__ + ib + (i__ + ib - 1) * a_dim1] = 1.;
	    i__3 = *ihi - i__ - ib + 1;
	    dgemm_("No transpose", "Transpose", ihi, &i__3, &ib, &c_b151, &
		    work[1], &ldwork, &a[i__ + ib + i__ * a_dim1], lda, &
		    c_b15, &a[(i__ + ib) * a_dim1 + 1], lda);
	    a[i__ + ib + (i__ + ib - 1) * a_dim1] = ei;

/*
             Apply the block reflector H to A(i+1:ihi,i+ib:n) from the
             left
*/

	    i__3 = *ihi - i__;
	    i__4 = *n - i__ - ib + 1;
	    dlarfb_("Left", "Transpose", "Forward", "Columnwise", &i__3, &
		    i__4, &ib, &a[i__ + 1 + i__ * a_dim1], lda, t, &c__65, &a[
		    i__ + 1 + (i__ + ib) * a_dim1], lda, &work[1], &ldwork);
/* L30: */
	}
    }

/*     Use unblocked code to reduce the rest of the matrix */

    dgehd2_(n, &i__, ihi, &a[a_offset], lda, &tau[1], &work[1], &iinfo);
    work[1] = (doublereal) iws;

    return 0;

/*     End of DGEHRD */

} /* dgehrd_ */

/* Subroutine */ int dgelq2_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *tau, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, k;
    static doublereal aii;
    extern /* Subroutine */ int dlarf_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *), dlarfg_(integer *, doublereal *,
	    doublereal *, integer *, doublereal *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DGELQ2 computes an LQ factorization of a real m by n matrix A:
    A = L * Q.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the m by n matrix A.
            On exit, the elements on and below the diagonal of the array
            contain the m by min(m,n) lower trapezoidal matrix L (L is
            lower triangular if m <= n); the elements above the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of elementary reflectors (see Further Details).

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) DOUBLE PRECISION array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace) DOUBLE PRECISION array, dimension (M)

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(k) . . . H(2) H(1), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n),
    and tau in TAU(i).

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELQ2", &i__1);
	return 0;
    }

    k = min(*m,*n);

    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Generate elementary reflector H(i) to annihilate A(i,i+1:n) */

	i__2 = *n - i__ + 1;
/* Computing MIN */
	i__3 = i__ + 1;
	dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[i__ + min(i__3,*n) * a_dim1]
		, lda, &tau[i__]);
	if (i__ < *m) {

/*           Apply H(i) to A(i+1:m,i:n) from the right */

	    aii = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;
	    i__2 = *m - i__;
	    i__3 = *n - i__ + 1;
	    dlarf_("Right", &i__2, &i__3, &a[i__ + i__ * a_dim1], lda, &tau[
		    i__], &a[i__ + 1 + i__ * a_dim1], lda, &work[1]);
	    a[i__ + i__ * a_dim1] = aii;
	}
/* L10: */
    }
    return 0;

/*     End of DGELQ2 */

} /* dgelq2_ */

/* Subroutine */ int dgelqf_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *tau, doublereal *work, integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer i__, k, ib, nb, nx, iws, nbmin, iinfo;
    extern /* Subroutine */ int dgelq2_(integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *), dlarfb_(char *,
	     char *, char *, char *, integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, integer *), dlarft_(char *, char *, integer *, integer *, doublereal
	    *, integer *, doublereal *, doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DGELQF computes an LQ factorization of a real M-by-N matrix A:
    A = L * Q.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, the elements on and below the diagonal of the array
            contain the m-by-min(m,n) lower trapezoidal matrix L (L is
            lower triangular if m <= n); the elements above the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of elementary reflectors (see Further Details).

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) DOUBLE PRECISION array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.  LWORK >= max(1,M).
            For optimum performance LWORK >= M*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(k) . . . H(2) H(1), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n),
    and tau in TAU(i).

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)
	    1);
    lwkopt = *m * nb;
    work[1] = (doublereal) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    } else if ((*lwork < max(1,*m) && ! lquery)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELQF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    k = min(*m,*n);
    if (k == 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *m;
    if ((nb > 1 && nb < k)) {

/*
          Determine when to cross over from blocked to unblocked code.

   Computing MAX
*/
	i__1 = 0, i__2 = ilaenv_(&c__3, "DGELQF", " ", m, n, &c_n1, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = max(i__1,i__2);
	if (nx < k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *m;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*
                Not enough workspace to use optimal NB:  reduce NB and
                determine the minimum value of NB.
*/

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGELQF", " ", m, n, &c_n1, &
			c_n1, (ftnlen)6, (ftnlen)1);
		nbmin = max(i__1,i__2);
	    }
	}
    }

    if (((nb >= nbmin && nb < k) && nx < k)) {

/*        Use blocked code initially */

	i__1 = k - nx;
	i__2 = nb;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = min(i__3,nb);

/*
             Compute the LQ factorization of the current block
             A(i:i+ib-1,i:n)
*/

	    i__3 = *n - i__ + 1;
	    dgelq2_(&ib, &i__3, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[
		    1], &iinfo);
	    if (i__ + ib <= *m) {

/*
                Form the triangular factor of the block reflector
                H = H(i) H(i+1) . . . H(i+ib-1)
*/

		i__3 = *n - i__ + 1;
		dlarft_("Forward", "Rowwise", &i__3, &ib, &a[i__ + i__ *
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(i+ib:m,i:n) from the right */

		i__3 = *m - i__ - ib + 1;
		i__4 = *n - i__ + 1;
		dlarfb_("Right", "No transpose", "Forward", "Rowwise", &i__3,
			&i__4, &ib, &a[i__ + i__ * a_dim1], lda, &work[1], &
			ldwork, &a[i__ + ib + i__ * a_dim1], lda, &work[ib +
			1], &ldwork);
	    }
/* L10: */
	}
    } else {
	i__ = 1;
    }

/*     Use unblocked code to factor the last or only block. */

    if (i__ <= k) {
	i__2 = *m - i__ + 1;
	i__1 = *n - i__ + 1;
	dgelq2_(&i__2, &i__1, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[1]
		, &iinfo);
    }

    work[1] = (doublereal) iws;
    return 0;

/*     End of DGELQF */

} /* dgelqf_ */

/* Subroutine */ int dgelsd_(integer *m, integer *n, integer *nrhs,
	doublereal *a, integer *lda, doublereal *b, integer *ldb, doublereal *
	s, doublereal *rcond, integer *rank, doublereal *work, integer *lwork,
	 integer *iwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3, i__4;

    /* Builtin functions */
    double log(doublereal);

    /* Local variables */
    static integer ie, il, mm;
    static doublereal eps, anrm, bnrm;
    static integer itau, nlvl, iascl, ibscl;
    static doublereal sfmin;
    static integer minmn, maxmn, itaup, itauq, mnthr, nwork;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *), dgebrd_(
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *,
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dgelqf_(integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *, integer *),
	    dlalsd_(char *, integer *, integer *, integer *, doublereal *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *, integer *), dlascl_(char *,
	    integer *, integer *, doublereal *, doublereal *, integer *,
	    integer *, doublereal *, integer *, integer *), dgeqrf_(
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *, integer *), dlacpy_(char *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *), dlaset_(char *, integer *, integer *, doublereal *,
	    doublereal *, doublereal *, integer *), xerbla_(char *,
	    integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static doublereal bignum;
    extern /* Subroutine */ int dormbr_(char *, char *, char *, integer *,
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, integer *);
    static integer wlalsd;
    extern /* Subroutine */ int dormlq_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *, integer *);
    static integer ldwork;
    extern /* Subroutine */ int dormqr_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *, integer *);
    static integer minwrk, maxwrk;
    static doublereal smlnum;
    static logical lquery;
    static integer smlsiz;


/*
    -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DGELSD computes the minimum-norm solution to a real linear least
    squares problem:
        minimize 2-norm(| b - A*x |)
    using the singular value decomposition (SVD) of A. A is an M-by-N
    matrix which may be rank-deficient.

    Several right hand side vectors b and solution vectors x can be
    handled in a single call; they are stored as the columns of the
    M-by-NRHS right hand side matrix B and the N-by-NRHS solution
    matrix X.

    The problem is solved in three steps:
    (1) Reduce the coefficient matrix A to bidiagonal form with
        Householder transformations, reducing the original problem
        into a "bidiagonal least squares problem" (BLS)
    (2) Solve the BLS using a divide and conquer approach.
    (3) Apply back all the Householder tranformations to solve
        the original least squares problem.

    The effective rank of A is determined by treating as zero those
    singular values which are less than RCOND times the largest singular
    value.

    The divide and conquer algorithm makes very mild assumptions about
    floating point arithmetic. It will work on machines with a guard
    digit in add/subtract, or on those binary machines without guard
    digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
    Cray-2. It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of A. M >= 0.

    N       (input) INTEGER
            The number of columns of A. N >= 0.

    NRHS    (input) INTEGER
            The number of right hand sides, i.e., the number of columns
            of the matrices B and X. NRHS >= 0.

    A       (input) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, A has been destroyed.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
            On entry, the M-by-NRHS right hand side matrix B.
            On exit, B is overwritten by the N-by-NRHS solution
            matrix X.  If m >= n and RANK = n, the residual
            sum-of-squares for the solution in the i-th column is given
            by the sum of squares of elements n+1:m in that column.

    LDB     (input) INTEGER
            The leading dimension of the array B. LDB >= max(1,max(M,N)).

    S       (output) DOUBLE PRECISION array, dimension (min(M,N))
            The singular values of A in decreasing order.
            The condition number of A in the 2-norm = S(1)/S(min(m,n)).

    RCOND   (input) DOUBLE PRECISION
            RCOND is used to determine the effective rank of A.
            Singular values S(i) <= RCOND*S(1) are treated as zero.
            If RCOND < 0, machine precision is used instead.

    RANK    (output) INTEGER
            The effective rank of A, i.e., the number of singular values
            which are greater than RCOND*S(1).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK. LWORK must be at least 1.
            The exact minimum amount of workspace needed depends on M,
            N and NRHS. As long as LWORK is at least
                12*N + 2*N*SMLSIZ + 8*N*NLVL + N*NRHS + (SMLSIZ+1)**2,
            if M is greater than or equal to N or
                12*M + 2*M*SMLSIZ + 8*M*NLVL + M*NRHS + (SMLSIZ+1)**2,
            if M is less than N, the code will execute correctly.
            SMLSIZ is returned by ILAENV and is equal to the maximum
            size of the subproblems at the bottom of the computation
            tree (usually about 25), and
               NLVL = MAX( 0, INT( LOG_2( MIN( M,N )/(SMLSIZ+1) ) ) + 1 )
            For good performance, LWORK should generally be larger.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    IWORK   (workspace) INTEGER array, dimension (LIWORK)
            LIWORK >= 3 * MINMN * NLVL + 11 * MINMN,
            where MINMN = MIN( M,N ).

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  the algorithm for computing the SVD failed to converge;
                  if INFO = i, i off-diagonal elements of an intermediate
                  bidiagonal form did not converge to zero.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Ren-Cang Li, Computer Science Division, University of
         California at Berkeley, USA
       Osni Marques, LBNL/NERSC, USA

    =====================================================================


       Test the input arguments.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    --s;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    minmn = min(*m,*n);
    maxmn = max(*m,*n);
    mnthr = ilaenv_(&c__6, "DGELSD", " ", m, n, nrhs, &c_n1, (ftnlen)6, (
	    ftnlen)1);
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    } else if (*ldb < max(1,maxmn)) {
	*info = -7;
    }

    smlsiz = ilaenv_(&c__9, "DGELSD", " ", &c__0, &c__0, &c__0, &c__0, (
	    ftnlen)6, (ftnlen)1);

/*
       Compute workspace.
       (Note: Comments in the code beginning "Workspace:" describe the
       minimal amount of workspace needed at that point in the code,
       as well as the preferred amount for good performance.
       NB refers to the optimal block size for the immediately
       following subroutine, as returned by ILAENV.)
*/

    minwrk = 1;
    minmn = max(1,minmn);
/* Computing MAX */
    i__1 = (integer) (log((doublereal) minmn / (doublereal) (smlsiz + 1)) /
	    log(2.)) + 1;
    nlvl = max(i__1,0);

    if (*info == 0) {
	maxwrk = 0;
	mm = *m;
	if ((*m >= *n && *m >= mnthr)) {

/*           Path 1a - overdetermined, with many more rows than columns. */

	    mm = *n;
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m,
		    n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
	    maxwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + *nrhs * ilaenv_(&c__1, "DORMQR", "LT",
		    m, nrhs, n, &c_n1, (ftnlen)6, (ftnlen)2);
	    maxwrk = max(i__1,i__2);
	}
	if (*m >= *n) {

/*
             Path 1 - overdetermined or exactly determined.

   Computing MAX
*/
	    i__1 = maxwrk, i__2 = *n * 3 + (mm + *n) * ilaenv_(&c__1, "DGEBRD"
		    , " ", &mm, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
	    maxwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * 3 + *nrhs * ilaenv_(&c__1, "DORMBR",
		    "QLT", &mm, nrhs, n, &c_n1, (ftnlen)6, (ftnlen)3);
	    maxwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * 3 + (*n - 1) * ilaenv_(&c__1, "DORMBR",
		     "PLN", n, nrhs, n, &c_n1, (ftnlen)6, (ftnlen)3);
	    maxwrk = max(i__1,i__2);
/* Computing 2nd power */
	    i__1 = smlsiz + 1;
	    wlalsd = *n * 9 + ((*n) << (1)) * smlsiz + ((*n) << (3)) * nlvl +
		    *n * *nrhs + i__1 * i__1;
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * 3 + wlalsd;
	    maxwrk = max(i__1,i__2);
/* Computing MAX */
	    i__1 = *n * 3 + mm, i__2 = *n * 3 + *nrhs, i__1 = max(i__1,i__2),
		    i__2 = *n * 3 + wlalsd;
	    minwrk = max(i__1,i__2);
	}
	if (*n > *m) {
/* Computing 2nd power */
	    i__1 = smlsiz + 1;
	    wlalsd = *m * 9 + ((*m) << (1)) * smlsiz + ((*m) << (3)) * nlvl +
		    *m * *nrhs + i__1 * i__1;
	    if (*n >= mnthr) {

/*
                Path 2a - underdetermined, with many more columns
                than rows.
*/

		maxwrk = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &c_n1,
			&c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + ((*m) << (2)) + ((*m) << (1))
			* ilaenv_(&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, (
			ftnlen)6, (ftnlen)1);
		maxwrk = max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + ((*m) << (2)) + *nrhs *
			ilaenv_(&c__1, "DORMBR", "QLT", m, nrhs, m, &c_n1, (
			ftnlen)6, (ftnlen)3);
		maxwrk = max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + ((*m) << (2)) + (*m - 1) *
			ilaenv_(&c__1, "DORMBR", "PLN", m, nrhs, m, &c_n1, (
			ftnlen)6, (ftnlen)3);
		maxwrk = max(i__1,i__2);
		if (*nrhs > 1) {
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + *m + *m * *nrhs;
		    maxwrk = max(i__1,i__2);
		} else {
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + ((*m) << (1));
		    maxwrk = max(i__1,i__2);
		}
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m + *nrhs * ilaenv_(&c__1, "DORMLQ",
			"LT", n, nrhs, m, &c_n1, (ftnlen)6, (ftnlen)2);
		maxwrk = max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + ((*m) << (2)) + wlalsd;
		maxwrk = max(i__1,i__2);
	    } else {

/*              Path 2 - remaining underdetermined cases. */

		maxwrk = *m * 3 + (*n + *m) * ilaenv_(&c__1, "DGEBRD", " ", m,
			 n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * 3 + *nrhs * ilaenv_(&c__1, "DORMBR"
			, "QLT", m, nrhs, n, &c_n1, (ftnlen)6, (ftnlen)3);
		maxwrk = max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR",
			"PLN", n, nrhs, m, &c_n1, (ftnlen)6, (ftnlen)3);
		maxwrk = max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * 3 + wlalsd;
		maxwrk = max(i__1,i__2);
	    }
/* Computing MAX */
	    i__1 = *m * 3 + *nrhs, i__2 = *m * 3 + *m, i__1 = max(i__1,i__2),
		    i__2 = *m * 3 + wlalsd;
	    minwrk = max(i__1,i__2);
	}
	minwrk = min(minwrk,maxwrk);
	work[1] = (doublereal) maxwrk;
	if ((*lwork < minwrk && ! lquery)) {
	    *info = -12;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELSD", &i__1);
	return 0;
    } else if (lquery) {
	goto L10;
    }

/*     Quick return if possible. */

    if (*m == 0 || *n == 0) {
	*rank = 0;
	return 0;
    }

/*     Get machine parameters. */

    eps = PRECISION;
    sfmin = SAFEMINIMUM;
    smlnum = sfmin / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Scale A if max entry outside range [SMLNUM,BIGNUM]. */

    anrm = dlange_("M", m, n, &a[a_offset], lda, &work[1]);
    iascl = 0;
    if ((anrm > 0. && anrm < smlnum)) {

/*        Scale matrix norm up to SMLNUM. */

	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda,
		info);
	iascl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM. */

	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda,
		info);
	iascl = 2;
    } else if (anrm == 0.) {

/*        Matrix all zero. Return zero solution. */

	i__1 = max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b29, &c_b29, &b[b_offset], ldb);
	dlaset_("F", &minmn, &c__1, &c_b29, &c_b29, &s[1], &c__1);
	*rank = 0;
	goto L10;
    }

/*     Scale B if max entry outside range [SMLNUM,BIGNUM]. */

    bnrm = dlange_("M", m, nrhs, &b[b_offset], ldb, &work[1]);
    ibscl = 0;
    if ((bnrm > 0. && bnrm < smlnum)) {

/*        Scale matrix norm up to SMLNUM. */

	dlascl_("G", &c__0, &c__0, &bnrm, &smlnum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 1;
    } else if (bnrm > bignum) {

/*        Scale matrix norm down to BIGNUM. */

	dlascl_("G", &c__0, &c__0, &bnrm, &bignum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 2;
    }

/*     If M < N make sure certain entries of B are zero. */

    if (*m < *n) {
	i__1 = *n - *m;
	dlaset_("F", &i__1, nrhs, &c_b29, &c_b29, &b[*m + 1 + b_dim1], ldb);
    }

/*     Overdetermined case. */

    if (*m >= *n) {

/*        Path 1 - overdetermined or exactly determined. */

	mm = *m;
	if (*m >= mnthr) {

/*           Path 1a - overdetermined, with many more rows than columns. */

	    mm = *n;
	    itau = 1;
	    nwork = itau + *n;

/*
             Compute A=Q*R.
             (Workspace: need 2*N, prefer N+N*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &i__1,
		     info);

/*
             Multiply B by transpose(Q).
             (Workspace: need N+NRHS, prefer N+NRHS*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dormqr_("L", "T", m, nrhs, n, &a[a_offset], lda, &work[itau], &b[
		    b_offset], ldb, &work[nwork], &i__1, info);

/*           Zero out below R. */

	    if (*n > 1) {
		i__1 = *n - 1;
		i__2 = *n - 1;
		dlaset_("L", &i__1, &i__2, &c_b29, &c_b29, &a[a_dim1 + 2],
			lda);
	    }
	}

	ie = 1;
	itauq = ie + *n;
	itaup = itauq + *n;
	nwork = itaup + *n;

/*
          Bidiagonalize R in A.
          (Workspace: need 3*N+MM, prefer 3*N+(MM+N)*NB)
*/

	i__1 = *lwork - nwork + 1;
	dgebrd_(&mm, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		work[itaup], &work[nwork], &i__1, info);

/*
          Multiply B by transpose of left bidiagonalizing vectors of R.
          (Workspace: need 3*N+NRHS, prefer 3*N+NRHS*NB)
*/

	i__1 = *lwork - nwork + 1;
	dormbr_("Q", "L", "T", &mm, nrhs, n, &a[a_offset], lda, &work[itauq],
		&b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Solve the bidiagonal least squares problem. */

	dlalsd_("U", &smlsiz, n, nrhs, &s[1], &work[ie], &b[b_offset], ldb,
		rcond, rank, &work[nwork], &iwork[1], info);
	if (*info != 0) {
	    goto L10;
	}

/*        Multiply B by right bidiagonalizing vectors of R. */

	i__1 = *lwork - nwork + 1;
	dormbr_("P", "L", "N", n, nrhs, n, &a[a_offset], lda, &work[itaup], &
		b[b_offset], ldb, &work[nwork], &i__1, info);

    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = *m, i__2 = ((*m) << (1)) - 4, i__1 = max(i__1,i__2), i__1 =
		max(i__1,*nrhs), i__2 = *n - *m * 3;
	if ((*n >= mnthr && *lwork >= ((*m) << (2)) + *m * *m + max(i__1,i__2)
		)) {

/*
          Path 2a - underdetermined, with many more columns than rows
          and sufficient workspace for an efficient algorithm.
*/

	    ldwork = *m;
/*
   Computing MAX
   Computing MAX
*/
	    i__3 = *m, i__4 = ((*m) << (1)) - 4, i__3 = max(i__3,i__4), i__3 =
		     max(i__3,*nrhs), i__4 = *n - *m * 3;
	    i__1 = ((*m) << (2)) + *m * *lda + max(i__3,i__4), i__2 = *m * *
		    lda + *m + *m * *nrhs;
	    if (*lwork >= max(i__1,i__2)) {
		ldwork = *lda;
	    }
	    itau = 1;
	    nwork = *m + 1;

/*
          Compute A=L*Q.
          (Workspace: need 2*M, prefer M+M*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &i__1,
		     info);
	    il = nwork;

/*        Copy L to WORK(IL), zeroing out above its diagonal. */

	    dlacpy_("L", m, m, &a[a_offset], lda, &work[il], &ldwork);
	    i__1 = *m - 1;
	    i__2 = *m - 1;
	    dlaset_("U", &i__1, &i__2, &c_b29, &c_b29, &work[il + ldwork], &
		    ldwork);
	    ie = il + ldwork * *m;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    nwork = itaup + *m;

/*
          Bidiagonalize L in WORK(IL).
          (Workspace: need M*M+5*M, prefer M*M+4*M+2*M*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dgebrd_(m, m, &work[il], &ldwork, &s[1], &work[ie], &work[itauq],
		    &work[itaup], &work[nwork], &i__1, info);

/*
          Multiply B by transpose of left bidiagonalizing vectors of L.
          (Workspace: need M*M+4*M+NRHS, prefer M*M+4*M+NRHS*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dormbr_("Q", "L", "T", m, nrhs, m, &work[il], &ldwork, &work[
		    itauq], &b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Solve the bidiagonal least squares problem. */

	    dlalsd_("U", &smlsiz, m, nrhs, &s[1], &work[ie], &b[b_offset],
		    ldb, rcond, rank, &work[nwork], &iwork[1], info);
	    if (*info != 0) {
		goto L10;
	    }

/*        Multiply B by right bidiagonalizing vectors of L. */

	    i__1 = *lwork - nwork + 1;
	    dormbr_("P", "L", "N", m, nrhs, m, &work[il], &ldwork, &work[
		    itaup], &b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Zero out below first M rows of B. */

	    i__1 = *n - *m;
	    dlaset_("F", &i__1, nrhs, &c_b29, &c_b29, &b[*m + 1 + b_dim1],
		    ldb);
	    nwork = itau + *m;

/*
          Multiply transpose(Q) by B.
          (Workspace: need M+NRHS, prefer M+NRHS*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dormlq_("L", "T", n, nrhs, m, &a[a_offset], lda, &work[itau], &b[
		    b_offset], ldb, &work[nwork], &i__1, info);

	} else {

/*        Path 2 - remaining underdetermined cases. */

	    ie = 1;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    nwork = itaup + *m;

/*
          Bidiagonalize A.
          (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[nwork], &i__1, info);

/*
          Multiply B by transpose of left bidiagonalizing vectors.
          (Workspace: need 3*M+NRHS, prefer 3*M+NRHS*NB)
*/

	    i__1 = *lwork - nwork + 1;
	    dormbr_("Q", "L", "T", m, nrhs, n, &a[a_offset], lda, &work[itauq]
		    , &b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Solve the bidiagonal least squares problem. */

	    dlalsd_("L", &smlsiz, m, nrhs, &s[1], &work[ie], &b[b_offset],
		    ldb, rcond, rank, &work[nwork], &iwork[1], info);
	    if (*info != 0) {
		goto L10;
	    }

/*        Multiply B by right bidiagonalizing vectors of A. */

	    i__1 = *lwork - nwork + 1;
	    dormbr_("P", "L", "N", n, nrhs, m, &a[a_offset], lda, &work[itaup]
		    , &b[b_offset], ldb, &work[nwork], &i__1, info);

	}
    }

/*     Undo scaling. */

    if (iascl == 1) {
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
		minmn, info);
    } else if (iascl == 2) {
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
		minmn, info);
    }
    if (ibscl == 1) {
	dlascl_("G", &c__0, &c__0, &smlnum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    } else if (ibscl == 2) {
	dlascl_("G", &c__0, &c__0, &bignum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    }

L10:
    work[1] = (doublereal) maxwrk;
    return 0;

/*     End of DGELSD */

} /* dgelsd_ */

/* Subroutine */ int dgeqr2_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *tau, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, k;
    static doublereal aii;
    extern /* Subroutine */ int dlarf_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *), dlarfg_(integer *, doublereal *,
	    doublereal *, integer *, doublereal *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DGEQR2 computes a QR factorization of a real m by n matrix A:
    A = Q * R.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the m by n matrix A.
            On exit, the elements on and above the diagonal of the array
            contain the min(m,n) by n upper trapezoidal matrix R (R is
            upper triangular if m >= n); the elements below the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of elementary reflectors (see Further Details).

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) DOUBLE PRECISION array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace) DOUBLE PRECISION array, dimension (N)

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(1) H(2) . . . H(k), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i),
    and tau in TAU(i).

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQR2", &i__1);
	return 0;
    }

    k = min(*m,*n);

    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Generate elementary reflector H(i) to annihilate A(i+1:m,i) */

	i__2 = *m - i__ + 1;
/* Computing MIN */
	i__3 = i__ + 1;
	dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[min(i__3,*m) + i__ * a_dim1]
		, &c__1, &tau[i__]);
	if (i__ < *n) {

/*           Apply H(i) to A(i:m,i+1:n) from the left */

	    aii = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;
	    i__2 = *m - i__ + 1;
	    i__3 = *n - i__;
	    dlarf_("Left", &i__2, &i__3, &a[i__ + i__ * a_dim1], &c__1, &tau[
		    i__], &a[i__ + (i__ + 1) * a_dim1], lda, &work[1]);
	    a[i__ + i__ * a_dim1] = aii;
	}
/* L10: */
    }
    return 0;

/*     End of DGEQR2 */

} /* dgeqr2_ */

/* Subroutine */ int dgeqrf_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *tau, doublereal *work, integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer i__, k, ib, nb, nx, iws, nbmin, iinfo;
    extern /* Subroutine */ int dgeqr2_(integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *), dlarfb_(char *,
	     char *, char *, char *, integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, integer *), dlarft_(char *, char *, integer *, integer *, doublereal
	    *, integer *, doublereal *, doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DGEQRF computes a QR factorization of a real M-by-N matrix A:
    A = Q * R.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, the elements on and above the diagonal of the array
            contain the min(M,N)-by-N upper trapezoidal matrix R (R is
            upper triangular if m >= n); the elements below the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of min(m,n) elementary reflectors (see Further
            Details).

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) DOUBLE PRECISION array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.  LWORK >= max(1,N).
            For optimum performance LWORK >= N*NB, where NB is
            the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(1) H(2) . . . H(k), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i),
    and tau in TAU(i).

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)
	    1);
    lwkopt = *n * nb;
    work[1] = (doublereal) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    } else if ((*lwork < max(1,*n) && ! lquery)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQRF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    k = min(*m,*n);
    if (k == 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *n;
    if ((nb > 1 && nb < k)) {

/*
          Determine when to cross over from blocked to unblocked code.

   Computing MAX
*/
	i__1 = 0, i__2 = ilaenv_(&c__3, "DGEQRF", " ", m, n, &c_n1, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = max(i__1,i__2);
	if (nx < k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*
                Not enough workspace to use optimal NB:  reduce NB and
                determine the minimum value of NB.
*/

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGEQRF", " ", m, n, &c_n1, &
			c_n1, (ftnlen)6, (ftnlen)1);
		nbmin = max(i__1,i__2);
	    }
	}
    }

    if (((nb >= nbmin && nb < k) && nx < k)) {

/*        Use blocked code initially */

	i__1 = k - nx;
	i__2 = nb;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = min(i__3,nb);

/*
             Compute the QR factorization of the current block
             A(i:m,i:i+ib-1)
*/

	    i__3 = *m - i__ + 1;
	    dgeqr2_(&i__3, &ib, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[
		    1], &iinfo);
	    if (i__ + ib <= *n) {

/*
                Form the triangular factor of the block reflector
                H = H(i) H(i+1) . . . H(i+ib-1)
*/

		i__3 = *m - i__ + 1;
		dlarft_("Forward", "Columnwise", &i__3, &ib, &a[i__ + i__ *
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H' to A(i:m,i+ib:n) from the left */

		i__3 = *m - i__ + 1;
		i__4 = *n - i__ - ib + 1;
		dlarfb_("Left", "Transpose", "Forward", "Columnwise", &i__3, &
			i__4, &ib, &a[i__ + i__ * a_dim1], lda, &work[1], &
			ldwork, &a[i__ + (i__ + ib) * a_dim1], lda, &work[ib
			+ 1], &ldwork);
	    }
/* L10: */
	}
    } else {
	i__ = 1;
    }

/*     Use unblocked code to factor the last or only block. */

    if (i__ <= k) {
	i__2 = *m - i__ + 1;
	i__1 = *n - i__ + 1;
	dgeqr2_(&i__2, &i__1, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[1]
		, &iinfo);
    }

    work[1] = (doublereal) iws;
    return 0;

/*     End of DGEQRF */

} /* dgeqrf_ */

/* Subroutine */ int dgesdd_(char *jobz, integer *m, integer *n, doublereal *
	a, integer *lda, doublereal *s, doublereal *u, integer *ldu,
	doublereal *vt, integer *ldvt, doublereal *work, integer *lwork,
	integer *iwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1,
	    i__2, i__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, ie, il, ir, iu, blk;
    static doublereal dum[1], eps;
    static integer ivt, iscl;
    static doublereal anrm;
    static integer idum[1], ierr, itau;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    extern logical lsame_(char *, char *);
    static integer chunk, minmn, wrkbl, itaup, itauq, mnthr;
    static logical wntqa;
    static integer nwork;
    static logical wntqn, wntqo, wntqs;
    extern /* Subroutine */ int dbdsdc_(char *, char *, integer *, doublereal
	    *, doublereal *, doublereal *, integer *, doublereal *, integer *,
	     doublereal *, integer *, doublereal *, integer *, integer *), dgebrd_(integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, integer *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *,
	    integer *, doublereal *, integer *, doublereal *);
    static integer bdspac;
    extern /* Subroutine */ int dgelqf_(integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *, integer *),
	    dlascl_(char *, integer *, integer *, doublereal *, doublereal *,
	    integer *, integer *, doublereal *, integer *, integer *),
	     dgeqrf_(integer *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *, integer *), dlacpy_(char *,
	     integer *, integer *, doublereal *, integer *, doublereal *,
	    integer *), dlaset_(char *, integer *, integer *,
	    doublereal *, doublereal *, doublereal *, integer *),
	    xerbla_(char *, integer *), dorgbr_(char *, integer *,
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static doublereal bignum;
    extern /* Subroutine */ int dormbr_(char *, char *, char *, integer *,
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, integer *), dorglq_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    integer *), dorgqr_(integer *, integer *, integer *, doublereal *,
	     integer *, doublereal *, doublereal *, integer *, integer *);
    static integer ldwrkl, ldwrkr, minwrk, ldwrku, maxwrk, ldwkvt;
    static doublereal smlnum;
    static logical wntqas, lquery;


/*
    -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DGESDD computes the singular value decomposition (SVD) of a real
    M-by-N matrix A, optionally computing the left and right singular
    vectors.  If singular vectors are desired, it uses a
    divide-and-conquer algorithm.

    The SVD is written

         A = U * SIGMA * transpose(V)

    where SIGMA is an M-by-N matrix which is zero except for its
    min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
    V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
    are the singular values of A; they are real and non-negative, and
    are returned in descending order.  The first min(m,n) columns of
    U and V are the left and right singular vectors of A.

    Note that the routine returns VT = V**T, not V.

    The divide and conquer algorithm makes very mild assumptions about
    floating point arithmetic. It will work on machines with a guard
    digit in add/subtract, or on those binary machines without guard
    digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
    Cray-2. It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.

    Arguments
    =========

    JOBZ    (input) CHARACTER*1
            Specifies options for computing all or part of the matrix U:
            = 'A':  all M columns of U and all N rows of V**T are
                    returned in the arrays U and VT;
            = 'S':  the first min(M,N) columns of U and the first
                    min(M,N) rows of V**T are returned in the arrays U
                    and VT;
            = 'O':  If M >= N, the first N columns of U are overwritten
                    on the array A and all rows of V**T are returned in
                    the array VT;
                    otherwise, all columns of U are returned in the
                    array U and the first M rows of V**T are overwritten
                    in the array VT;
            = 'N':  no columns of U or rows of V**T are computed.

    M       (input) INTEGER
            The number of rows of the input matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the input matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit,
            if JOBZ = 'O',  A is overwritten with the first N columns
                            of U (the left singular vectors, stored
                            columnwise) if M >= N;
                            A is overwritten with the first M rows
                            of V**T (the right singular vectors, stored
                            rowwise) otherwise.
            if JOBZ .ne. 'O', the contents of A are destroyed.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    S       (output) DOUBLE PRECISION array, dimension (min(M,N))
            The singular values of A, sorted so that S(i) >= S(i+1).

    U       (output) DOUBLE PRECISION array, dimension (LDU,UCOL)
            UCOL = M if JOBZ = 'A' or JOBZ = 'O' and M < N;
            UCOL = min(M,N) if JOBZ = 'S'.
            If JOBZ = 'A' or JOBZ = 'O' and M < N, U contains the M-by-M
            orthogonal matrix U;
            if JOBZ = 'S', U contains the first min(M,N) columns of U
            (the left singular vectors, stored columnwise);
            if JOBZ = 'O' and M >= N, or JOBZ = 'N', U is not referenced.

    LDU     (input) INTEGER
            The leading dimension of the array U.  LDU >= 1; if
            JOBZ = 'S' or 'A' or JOBZ = 'O' and M < N, LDU >= M.

    VT      (output) DOUBLE PRECISION array, dimension (LDVT,N)
            If JOBZ = 'A' or JOBZ = 'O' and M >= N, VT contains the
            N-by-N orthogonal matrix V**T;
            if JOBZ = 'S', VT contains the first min(M,N) rows of
            V**T (the right singular vectors, stored rowwise);
            if JOBZ = 'O' and M < N, or JOBZ = 'N', VT is not referenced.

    LDVT    (input) INTEGER
            The leading dimension of the array VT.  LDVT >= 1; if
            JOBZ = 'A' or JOBZ = 'O' and M >= N, LDVT >= N;
            if JOBZ = 'S', LDVT >= min(M,N).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK;

    LWORK   (input) INTEGER
            The dimension of the array WORK. LWORK >= 1.
            If JOBZ = 'N',
              LWORK >= 3*min(M,N) + max(max(M,N),6*min(M,N)).
            If JOBZ = 'O',
              LWORK >= 3*min(M,N)*min(M,N) +
                       max(max(M,N),5*min(M,N)*min(M,N)+4*min(M,N)).
            If JOBZ = 'S' or 'A'
              LWORK >= 3*min(M,N)*min(M,N) +
                       max(max(M,N),4*min(M,N)*min(M,N)+4*min(M,N)).
            For good performance, LWORK should generally be larger.
            If LWORK < 0 but other input arguments are legal, WORK(1)
            returns the optimal LWORK.

    IWORK   (workspace) INTEGER array, dimension (8*min(M,N))

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  DBDSDC did not converge, updating process failed.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --s;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    minmn = min(*m,*n);
    mnthr = (integer) (minmn * 11. / 6.);
    wntqa = lsame_(jobz, "A");
    wntqs = lsame_(jobz, "S");
    wntqas = wntqa || wntqs;
    wntqo = lsame_(jobz, "O");
    wntqn = lsame_(jobz, "N");
    minwrk = 1;
    maxwrk = 1;
    lquery = *lwork == -1;

    if (! (wntqa || wntqs || wntqo || wntqn)) {
	*info = -1;
    } else if (*m < 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    } else if (*ldu < 1 || (wntqas && *ldu < *m) || ((wntqo && *m < *n) && *
	    ldu < *m)) {
	*info = -8;
    } else if (*ldvt < 1 || (wntqa && *ldvt < *n) || (wntqs && *ldvt < minmn)
	    || ((wntqo && *m >= *n) && *ldvt < *n)) {
	*info = -10;
    }

/*
       Compute workspace
        (Note: Comments in the code beginning "Workspace:" describe the
         minimal amount of workspace needed at that point in the code,
         as well as the preferred amount for good performance.
         NB refers to the optimal block size for the immediately
         following subroutine, as returned by ILAENV.)
*/

    if (((*info == 0 && *m > 0) && *n > 0)) {
	if (*m >= *n) {

/*           Compute space needed for DBDSDC */

	    if (wntqn) {
		bdspac = *n * 7;
	    } else {
		bdspac = *n * 3 * *n + ((*n) << (2));
	    }
	    if (*m >= mnthr) {
		if (wntqn) {

/*                 Path 1 (M much larger than N, JOBZ='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + ((*n) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n;
		    maxwrk = max(i__1,i__2);
		    minwrk = bdspac + *n;
		} else if (wntqo) {

/*                 Path 2 (M much larger than N, JOBZ='O') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + ((*n) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "QLN", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "PRT", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + ((*n) << (1)) * *n;
		    minwrk = bdspac + ((*n) << (1)) * *n + *n * 3;
		} else if (wntqs) {

/*                 Path 3 (M much larger than N, JOBZ='S') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + ((*n) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "QLN", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "PRT", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + *n * *n;
		    minwrk = bdspac + *n * *n + *n * 3;
		} else if (wntqa) {

/*                 Path 4 (M much larger than N, JOBZ='A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n + *m * ilaenv_(&c__1, "DORGQR",
			    " ", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + ((*n) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "QLN", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "PRT", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + *n * *n;
		    minwrk = bdspac + *n * *n + *n * 3;
		}
	    } else {

/*              Path 5 (M at least N, but not much larger) */

		wrkbl = *n * 3 + (*m + *n) * ilaenv_(&c__1, "DGEBRD", " ", m,
			n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
		if (wntqn) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    maxwrk = max(i__1,i__2);
		    minwrk = *n * 3 + max(*m,bdspac);
		} else if (wntqo) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "PRT", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + *m * *n;
/* Computing MAX */
		    i__1 = *m, i__2 = *n * *n + bdspac;
		    minwrk = *n * 3 + max(i__1,i__2);
		} else if (wntqs) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "PRT", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    maxwrk = max(i__1,i__2);
		    minwrk = *n * 3 + max(*m,bdspac);
		} else if (wntqa) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, m, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
			    , "PRT", n, n, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = bdspac + *n * 3;
		    maxwrk = max(i__1,i__2);
		    minwrk = *n * 3 + max(*m,bdspac);
		}
	    }
	} else {

/*           Compute space needed for DBDSDC */

	    if (wntqn) {
		bdspac = *m * 7;
	    } else {
		bdspac = *m * 3 * *m + ((*m) << (2));
	    }
	    if (*n >= mnthr) {
		if (wntqn) {

/*                 Path 1t (N much larger than M, JOBZ='N') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + ((*m) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m;
		    maxwrk = max(i__1,i__2);
		    minwrk = bdspac + *m;
		} else if (wntqo) {

/*                 Path 2t (N much larger than M, JOBZ='O') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + ((*m) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, m, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "PRT", m, m, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + ((*m) << (1)) * *m;
		    minwrk = bdspac + ((*m) << (1)) * *m + *m * 3;
		} else if (wntqs) {

/*                 Path 3t (N much larger than M, JOBZ='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + ((*m) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, m, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "PRT", m, m, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + *m * *m;
		    minwrk = bdspac + *m * *m + *m * 3;
		} else if (wntqa) {

/*                 Path 4t (N much larger than M, JOBZ='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m + *n * ilaenv_(&c__1, "DORGLQ",
			    " ", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + ((*m) << (1)) * ilaenv_(&
			    c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
			    6, (ftnlen)1);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, m, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "PRT", m, m, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + *m * *m;
		    minwrk = bdspac + *m * *m + *m * 3;
		}
	    } else {

/*              Path 5t (N greater than M, but not much larger) */

		wrkbl = *m * 3 + (*m + *n) * ilaenv_(&c__1, "DGEBRD", " ", m,
			n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
		if (wntqn) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    maxwrk = max(i__1,i__2);
		    minwrk = *m * 3 + max(*n,bdspac);
		} else if (wntqo) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, m, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "PRT", m, n, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = max(i__1,i__2);
		    maxwrk = wrkbl + *m * *n;
/* Computing MAX */
		    i__1 = *n, i__2 = *m * *m + bdspac;
		    minwrk = *m * 3 + max(i__1,i__2);
		} else if (wntqs) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, m, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "PRT", m, n, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    maxwrk = max(i__1,i__2);
		    minwrk = *m * 3 + max(*n,bdspac);
		} else if (wntqa) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "QLN", m, m, n, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
			    , "PRT", n, n, m, &c_n1, (ftnlen)6, (ftnlen)3);
		    wrkbl = max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    maxwrk = max(i__1,i__2);
		    minwrk = *m * 3 + max(*n,bdspac);
		}
	    }
	}
	work[1] = (doublereal) maxwrk;
    }

    if ((*lwork < minwrk && ! lquery)) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGESDD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	if (*lwork >= 1) {
	    work[1] = 1.;
	}
	return 0;
    }

/*     Get machine constants */

    eps = PRECISION;
    smlnum = sqrt(SAFEMINIMUM) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &a[a_offset], lda, dum);
    iscl = 0;
    if ((anrm > 0. && anrm < smlnum)) {
	iscl = 1;
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda, &
		ierr);
    } else if (anrm > bignum) {
	iscl = 1;
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda, &
		ierr);
    }

    if (*m >= *n) {

/*
          A has at least as many rows as columns. If A has sufficiently
          more rows than columns, first reduce using the QR
          decomposition (if sufficient workspace available)
*/

	if (*m >= mnthr) {

	    if (wntqn) {

/*
                Path 1 (M much larger than N, JOBZ='N')
                No singular vectors to be computed
*/

		itau = 1;
		nwork = itau + *n;

/*
                Compute A=Q*R
                (Workspace: need 2*N, prefer N+N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Zero out below R */

		i__1 = *n - 1;
		i__2 = *n - 1;
		dlaset_("L", &i__1, &i__2, &c_b29, &c_b29, &a[a_dim1 + 2],
			lda);
		ie = 1;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*
                Bidiagonalize R in A
                (Workspace: need 4*N, prefer 3*N+2*N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);
		nwork = ie + *n;

/*
                Perform bidiagonal SVD, computing singular values only
                (Workspace: need N+BDSPAC)
*/

		dbdsdc_("U", "N", n, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);

	    } else if (wntqo) {

/*
                Path 2 (M much larger than N, JOBZ = 'O')
                N left singular vectors to be overwritten on A and
                N right singular vectors to be computed in VT
*/

		ir = 1;

/*              WORK(IR) is LDWRKR by N */

		if (*lwork >= *lda * *n + *n * *n + *n * 3 + bdspac) {
		    ldwrkr = *lda;
		} else {
		    ldwrkr = (*lwork - *n * *n - *n * 3 - bdspac) / *n;
		}
		itau = ir + ldwrkr * *n;
		nwork = itau + *n;

/*
                Compute A=Q*R
                (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Copy R to WORK(IR), zeroing out below it */

		dlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
		i__1 = *n - 1;
		i__2 = *n - 1;
		dlaset_("L", &i__1, &i__2, &c_b29, &c_b29, &work[ir + 1], &
			ldwrkr);

/*
                Generate Q in A
                (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__1, &ierr);
		ie = itau;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*
                Bidiagonalize R in VT, copying result to WORK(IR)
                (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);

/*              WORK(IU) is N by N */

		iu = nwork;
		nwork = iu + *n * *n;

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in WORK(IU) and computing right
                singular vectors of bidiagonal matrix in VT
                (Workspace: need N+N*N+BDSPAC)
*/

		dbdsdc_("U", "I", n, &s[1], &work[ie], &work[iu], n, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*
                Overwrite WORK(IU) by left singular vectors of R
                and VT by right singular vectors of R
                (Workspace: need 2*N*N+3*N, prefer 2*N*N+2*N+N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", n, n, n, &work[ir], &ldwrkr, &work[
			itauq], &work[iu], n, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &work[ir], &ldwrkr, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);

/*
                Multiply Q in A by left singular vectors of R in
                WORK(IU), storing result in WORK(IR) and copying to A
                (Workspace: need 2*N*N, prefer N*N+M*N)
*/

		i__1 = *m;
		i__2 = ldwrkr;
		for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ +=
			i__2) {
/* Computing MIN */
		    i__3 = *m - i__ + 1;
		    chunk = min(i__3,ldwrkr);
		    dgemm_("N", "N", &chunk, n, n, &c_b15, &a[i__ + a_dim1],
			    lda, &work[iu], n, &c_b29, &work[ir], &ldwrkr);
		    dlacpy_("F", &chunk, n, &work[ir], &ldwrkr, &a[i__ +
			    a_dim1], lda);
/* L10: */
		}

	    } else if (wntqs) {

/*
                Path 3 (M much larger than N, JOBZ='S')
                N left singular vectors to be computed in U and
                N right singular vectors to be computed in VT
*/

		ir = 1;

/*              WORK(IR) is N by N */

		ldwrkr = *n;
		itau = ir + ldwrkr * *n;
		nwork = itau + *n;

/*
                Compute A=Q*R
                (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);

/*              Copy R to WORK(IR), zeroing out below it */

		dlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
		i__2 = *n - 1;
		i__1 = *n - 1;
		dlaset_("L", &i__2, &i__1, &c_b29, &c_b29, &work[ir + 1], &
			ldwrkr);

/*
                Generate Q in A
                (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__2, &ierr);
		ie = itau;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*
                Bidiagonalize R in WORK(IR)
                (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagoal matrix in U and computing right singular
                vectors of bidiagonal matrix in VT
                (Workspace: need N+BDSPAC)
*/

		dbdsdc_("U", "I", n, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*
                Overwrite U by left singular vectors of R and VT
                by right singular vectors of R
                (Workspace: need N*N+3*N, prefer N*N+2*N+N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", n, n, n, &work[ir], &ldwrkr, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);

		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &work[ir], &ldwrkr, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

/*
                Multiply Q in A by left singular vectors of R in
                WORK(IR), storing result in U
                (Workspace: need N*N)
*/

		dlacpy_("F", n, n, &u[u_offset], ldu, &work[ir], &ldwrkr);
		dgemm_("N", "N", m, n, n, &c_b15, &a[a_offset], lda, &work[ir]
			, &ldwrkr, &c_b29, &u[u_offset], ldu);

	    } else if (wntqa) {

/*
                Path 4 (M much larger than N, JOBZ='A')
                M left singular vectors to be computed in U and
                N right singular vectors to be computed in VT
*/

		iu = 1;

/*              WORK(IU) is N by N */

		ldwrku = *n;
		itau = iu + ldwrku * *n;
		nwork = itau + *n;

/*
                Compute A=Q*R, copying result to U
                (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);
		dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

/*
                Generate Q in U
                (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/
		i__2 = *lwork - nwork + 1;
		dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &work[nwork],
			 &i__2, &ierr);

/*              Produce R in A, zeroing out other entries */

		i__2 = *n - 1;
		i__1 = *n - 1;
		dlaset_("L", &i__2, &i__1, &c_b29, &c_b29, &a[a_dim1 + 2],
			lda);
		ie = itau;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*
                Bidiagonalize R in A
                (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in WORK(IU) and computing right
                singular vectors of bidiagonal matrix in VT
                (Workspace: need N+N*N+BDSPAC)
*/

		dbdsdc_("U", "I", n, &s[1], &work[ie], &work[iu], n, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*
                Overwrite WORK(IU) by left singular vectors of R and VT
                by right singular vectors of R
                (Workspace: need N*N+3*N, prefer N*N+2*N+N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", n, n, n, &a[a_offset], lda, &work[
			itauq], &work[iu], &ldwrku, &work[nwork], &i__2, &
			ierr);
		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

/*
                Multiply Q in U by left singular vectors of R in
                WORK(IU), storing result in A
                (Workspace: need N*N)
*/

		dgemm_("N", "N", m, n, n, &c_b15, &u[u_offset], ldu, &work[iu]
			, &ldwrku, &c_b29, &a[a_offset], lda);

/*              Copy left singular vectors of A from A to U */

		dlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], ldu);

	    }

	} else {

/*
             M .LT. MNTHR

             Path 5 (M at least N, but not much larger)
             Reduce to bidiagonal form without QR decomposition
*/

	    ie = 1;
	    itauq = ie + *n;
	    itaup = itauq + *n;
	    nwork = itaup + *n;

/*
             Bidiagonalize A
             (Workspace: need 3*N+M, prefer 3*N+(M+N)*NB)
*/

	    i__2 = *lwork - nwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[nwork], &i__2, &ierr);
	    if (wntqn) {

/*
                Perform bidiagonal SVD, only computing singular values
                (Workspace: need N+BDSPAC)
*/

		dbdsdc_("U", "N", n, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);
	    } else if (wntqo) {
		iu = nwork;
		if (*lwork >= *m * *n + *n * 3 + bdspac) {

/*                 WORK( IU ) is M by N */

		    ldwrku = *m;
		    nwork = iu + ldwrku * *n;
		    dlaset_("F", m, n, &c_b29, &c_b29, &work[iu], &ldwrku);
		} else {

/*                 WORK( IU ) is N by N */

		    ldwrku = *n;
		    nwork = iu + ldwrku * *n;

/*                 WORK(IR) is LDWRKR by N */

		    ir = nwork;
		    ldwrkr = (*lwork - *n * *n - *n * 3) / *n;
		}
		nwork = iu + ldwrku * *n;

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in WORK(IU) and computing right
                singular vectors of bidiagonal matrix in VT
                (Workspace: need N+N*N+BDSPAC)
*/

		dbdsdc_("U", "I", n, &s[1], &work[ie], &work[iu], &ldwrku, &
			vt[vt_offset], ldvt, dum, idum, &work[nwork], &iwork[
			1], info);

/*
                Overwrite VT by right singular vectors of A
                (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

		if (*lwork >= *m * *n + *n * 3 + bdspac) {

/*
                   Overwrite WORK(IU) by left singular vectors of A
                   (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		    i__2 = *lwork - nwork + 1;
		    dormbr_("Q", "L", "N", m, n, n, &a[a_offset], lda, &work[
			    itauq], &work[iu], &ldwrku, &work[nwork], &i__2, &
			    ierr);

/*                 Copy left singular vectors of A from WORK(IU) to A */

		    dlacpy_("F", m, n, &work[iu], &ldwrku, &a[a_offset], lda);
		} else {

/*
                   Generate Q in A
                   (Workspace: need N*N+2*N, prefer N*N+N+N*NB)
*/

		    i__2 = *lwork - nwork + 1;
		    dorgbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &
			    work[nwork], &i__2, &ierr);

/*
                   Multiply Q in A by left singular vectors of
                   bidiagonal matrix in WORK(IU), storing result in
                   WORK(IR) and copying to A
                   (Workspace: need 2*N*N, prefer N*N+M*N)
*/

		    i__2 = *m;
		    i__1 = ldwrkr;
		    for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__1) {
/* Computing MIN */
			i__3 = *m - i__ + 1;
			chunk = min(i__3,ldwrkr);
			dgemm_("N", "N", &chunk, n, n, &c_b15, &a[i__ +
				a_dim1], lda, &work[iu], &ldwrku, &c_b29, &
				work[ir], &ldwrkr);
			dlacpy_("F", &chunk, n, &work[ir], &ldwrkr, &a[i__ +
				a_dim1], lda);
/* L20: */
		    }
		}

	    } else if (wntqs) {

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U and computing right singular
                vectors of bidiagonal matrix in VT
                (Workspace: need N+BDSPAC)
*/

		dlaset_("F", m, n, &c_b29, &c_b29, &u[u_offset], ldu);
		dbdsdc_("U", "I", n, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*
                Overwrite U by left singular vectors of A and VT
                by right singular vectors of A
                (Workspace: need 3*N, prefer 2*N+N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, n, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    } else if (wntqa) {

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U and computing right singular
                vectors of bidiagonal matrix in VT
                (Workspace: need N+BDSPAC)
*/

		dlaset_("F", m, m, &c_b29, &c_b29, &u[u_offset], ldu);
		dbdsdc_("U", "I", n, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Set the right corner of U to identity matrix */

		i__1 = *m - *n;
		i__2 = *m - *n;
		dlaset_("F", &i__1, &i__2, &c_b29, &c_b15, &u[*n + 1 + (*n +
			1) * u_dim1], ldu);

/*
                Overwrite U by left singular vectors of A and VT
                by right singular vectors of A
                (Workspace: need N*N+2*N+M, prefer N*N+2*N+M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, m, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    }

	}

    } else {

/*
          A has more columns than rows. If A has sufficiently more
          columns than rows, first reduce using the LQ decomposition (if
          sufficient workspace available)
*/

	if (*n >= mnthr) {

	    if (wntqn) {

/*
                Path 1t (N much larger than M, JOBZ='N')
                No singular vectors to be computed
*/

		itau = 1;
		nwork = itau + *m;

/*
                Compute A=L*Q
                (Workspace: need 2*M, prefer M+M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Zero out above L */

		i__1 = *m - 1;
		i__2 = *m - 1;
		dlaset_("U", &i__1, &i__2, &c_b29, &c_b29, &a[((a_dim1) << (1)
			) + 1], lda);
		ie = 1;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*
                Bidiagonalize L in A
                (Workspace: need 4*M, prefer 3*M+2*M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);
		nwork = ie + *m;

/*
                Perform bidiagonal SVD, computing singular values only
                (Workspace: need M+BDSPAC)
*/

		dbdsdc_("U", "N", m, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);

	    } else if (wntqo) {

/*
                Path 2t (N much larger than M, JOBZ='O')
                M right singular vectors to be overwritten on A and
                M left singular vectors to be computed in U
*/

		ivt = 1;

/*              IVT is M by M */

		il = ivt + *m * *m;
		if (*lwork >= *m * *n + *m * *m + *m * 3 + bdspac) {

/*                 WORK(IL) is M by N */

		    ldwrkl = *m;
		    chunk = *n;
		} else {
		    ldwrkl = *m;
		    chunk = (*lwork - *m * *m) / *m;
		}
		itau = il + ldwrkl * *m;
		nwork = itau + *m;

/*
                Compute A=L*Q
                (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Copy L to WORK(IL), zeroing about above it */

		dlacpy_("L", m, m, &a[a_offset], lda, &work[il], &ldwrkl);
		i__1 = *m - 1;
		i__2 = *m - 1;
		dlaset_("U", &i__1, &i__2, &c_b29, &c_b29, &work[il + ldwrkl],
			 &ldwrkl);

/*
                Generate Q in A
                (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__1, &ierr);
		ie = itau;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*
                Bidiagonalize L in WORK(IL)
                (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dgebrd_(m, m, &work[il], &ldwrkl, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U, and computing right singular
                vectors of bidiagonal matrix in WORK(IVT)
                (Workspace: need M+M*M+BDSPAC)
*/

		dbdsdc_("U", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &
			work[ivt], m, dum, idum, &work[nwork], &iwork[1],
			info);

/*
                Overwrite U by left singular vectors of L and WORK(IVT)
                by right singular vectors of L
                (Workspace: need 2*M*M+3*M, prefer 2*M*M+2*M+M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, m, &work[il], &ldwrkl, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, m, m, &work[il], &ldwrkl, &work[
			itaup], &work[ivt], m, &work[nwork], &i__1, &ierr);

/*
                Multiply right singular vectors of L in WORK(IVT) by Q
                in A, storing result in WORK(IL) and copying to A
                (Workspace: need 2*M*M, prefer M*M+M*N)
*/

		i__1 = *n;
		i__2 = chunk;
		for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ +=
			i__2) {
/* Computing MIN */
		    i__3 = *n - i__ + 1;
		    blk = min(i__3,chunk);
		    dgemm_("N", "N", m, &blk, m, &c_b15, &work[ivt], m, &a[
			    i__ * a_dim1 + 1], lda, &c_b29, &work[il], &
			    ldwrkl);
		    dlacpy_("F", m, &blk, &work[il], &ldwrkl, &a[i__ * a_dim1
			    + 1], lda);
/* L30: */
		}

	    } else if (wntqs) {

/*
                Path 3t (N much larger than M, JOBZ='S')
                M right singular vectors to be computed in VT and
                M left singular vectors to be computed in U
*/

		il = 1;

/*              WORK(IL) is M by M */

		ldwrkl = *m;
		itau = il + ldwrkl * *m;
		nwork = itau + *m;

/*
                Compute A=L*Q
                (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);

/*              Copy L to WORK(IL), zeroing out above it */

		dlacpy_("L", m, m, &a[a_offset], lda, &work[il], &ldwrkl);
		i__2 = *m - 1;
		i__1 = *m - 1;
		dlaset_("U", &i__2, &i__1, &c_b29, &c_b29, &work[il + ldwrkl],
			 &ldwrkl);

/*
                Generate Q in A
                (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__2, &ierr);
		ie = itau;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*
                Bidiagonalize L in WORK(IU), copying result to U
                (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgebrd_(m, m, &work[il], &ldwrkl, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U and computing right singular
                vectors of bidiagonal matrix in VT
                (Workspace: need M+BDSPAC)
*/

		dbdsdc_("U", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*
                Overwrite U by left singular vectors of L and VT
                by right singular vectors of L
                (Workspace: need M*M+3*M, prefer M*M+2*M+M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, m, &work[il], &ldwrkl, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);
		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, m, m, &work[il], &ldwrkl, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

/*
                Multiply right singular vectors of L in WORK(IL) by
                Q in A, storing result in VT
                (Workspace: need M*M)
*/

		dlacpy_("F", m, m, &vt[vt_offset], ldvt, &work[il], &ldwrkl);
		dgemm_("N", "N", m, n, m, &c_b15, &work[il], &ldwrkl, &a[
			a_offset], lda, &c_b29, &vt[vt_offset], ldvt);

	    } else if (wntqa) {

/*
                Path 4t (N much larger than M, JOBZ='A')
                N right singular vectors to be computed in VT and
                M left singular vectors to be computed in U
*/

		ivt = 1;

/*              WORK(IVT) is M by M */

		ldwkvt = *m;
		itau = ivt + ldwkvt * *m;
		nwork = itau + *m;

/*
                Compute A=L*Q, copying result to VT
                (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);
		dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

/*
                Generate Q in VT
                (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[
			nwork], &i__2, &ierr);

/*              Produce L in A, zeroing out other entries */

		i__2 = *m - 1;
		i__1 = *m - 1;
		dlaset_("U", &i__2, &i__1, &c_b29, &c_b29, &a[((a_dim1) << (1)
			) + 1], lda);
		ie = itau;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*
                Bidiagonalize L in A
                (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U and computing right singular
                vectors of bidiagonal matrix in WORK(IVT)
                (Workspace: need M+M*M+BDSPAC)
*/

		dbdsdc_("U", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &
			work[ivt], &ldwkvt, dum, idum, &work[nwork], &iwork[1]
			, info);

/*
                Overwrite U by left singular vectors of L and WORK(IVT)
                by right singular vectors of L
                (Workspace: need M*M+3*M, prefer M*M+2*M+M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, m, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);
		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, m, m, &a[a_offset], lda, &work[
			itaup], &work[ivt], &ldwkvt, &work[nwork], &i__2, &
			ierr);

/*
                Multiply right singular vectors of L in WORK(IVT) by
                Q in VT, storing result in A
                (Workspace: need M*M)
*/

		dgemm_("N", "N", m, n, m, &c_b15, &work[ivt], &ldwkvt, &vt[
			vt_offset], ldvt, &c_b29, &a[a_offset], lda);

/*              Copy right singular vectors of A from A to VT */

		dlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

	    }

	} else {

/*
             N .LT. MNTHR

             Path 5t (N greater than M, but not much larger)
             Reduce to bidiagonal form without LQ decomposition
*/

	    ie = 1;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    nwork = itaup + *m;

/*
             Bidiagonalize A
             (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB)
*/

	    i__2 = *lwork - nwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[nwork], &i__2, &ierr);
	    if (wntqn) {

/*
                Perform bidiagonal SVD, only computing singular values
                (Workspace: need M+BDSPAC)
*/

		dbdsdc_("L", "N", m, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);
	    } else if (wntqo) {
		ldwkvt = *m;
		ivt = nwork;
		if (*lwork >= *m * *n + *m * 3 + bdspac) {

/*                 WORK( IVT ) is M by N */

		    dlaset_("F", m, n, &c_b29, &c_b29, &work[ivt], &ldwkvt);
		    nwork = ivt + ldwkvt * *n;
		} else {

/*                 WORK( IVT ) is M by M */

		    nwork = ivt + ldwkvt * *m;
		    il = nwork;

/*                 WORK(IL) is M by CHUNK */

		    chunk = (*lwork - *m * *m - *m * 3) / *m;
		}

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U and computing right singular
                vectors of bidiagonal matrix in WORK(IVT)
                (Workspace: need M*M+BDSPAC)
*/

		dbdsdc_("L", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &
			work[ivt], &ldwkvt, dum, idum, &work[nwork], &iwork[1]
			, info);

/*
                Overwrite U by left singular vectors of A
                (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);

		if (*lwork >= *m * *n + *m * 3 + bdspac) {

/*
                   Overwrite WORK(IVT) by left singular vectors of A
                   (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		    i__2 = *lwork - nwork + 1;
		    dormbr_("P", "R", "T", m, n, m, &a[a_offset], lda, &work[
			    itaup], &work[ivt], &ldwkvt, &work[nwork], &i__2,
			    &ierr);

/*                 Copy right singular vectors of A from WORK(IVT) to A */

		    dlacpy_("F", m, n, &work[ivt], &ldwkvt, &a[a_offset], lda);
		} else {

/*
                   Generate P**T in A
                   (Workspace: need M*M+2*M, prefer M*M+M+M*NB)
*/

		    i__2 = *lwork - nwork + 1;
		    dorgbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &
			    work[nwork], &i__2, &ierr);

/*
                   Multiply Q in A by right singular vectors of
                   bidiagonal matrix in WORK(IVT), storing result in
                   WORK(IL) and copying to A
                   (Workspace: need 2*M*M, prefer M*M+M*N)
*/

		    i__2 = *n;
		    i__1 = chunk;
		    for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__1) {
/* Computing MIN */
			i__3 = *n - i__ + 1;
			blk = min(i__3,chunk);
			dgemm_("N", "N", m, &blk, m, &c_b15, &work[ivt], &
				ldwkvt, &a[i__ * a_dim1 + 1], lda, &c_b29, &
				work[il], m);
			dlacpy_("F", m, &blk, &work[il], m, &a[i__ * a_dim1 +
				1], lda);
/* L40: */
		    }
		}
	    } else if (wntqs) {

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U and computing right singular
                vectors of bidiagonal matrix in VT
                (Workspace: need M+BDSPAC)
*/

		dlaset_("F", m, n, &c_b29, &c_b29, &vt[vt_offset], ldvt);
		dbdsdc_("L", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*
                Overwrite U by left singular vectors of A and VT
                by right singular vectors of A
                (Workspace: need 3*M, prefer 2*M+M*NB)
*/

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, n, m, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    } else if (wntqa) {

/*
                Perform bidiagonal SVD, computing left singular vectors
                of bidiagonal matrix in U and computing right singular
                vectors of bidiagonal matrix in VT
                (Workspace: need M+BDSPAC)
*/

		dlaset_("F", n, n, &c_b29, &c_b29, &vt[vt_offset], ldvt);
		dbdsdc_("L", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Set the right corner of VT to identity matrix */

		i__1 = *n - *m;
		i__2 = *n - *m;
		dlaset_("F", &i__1, &i__2, &c_b29, &c_b15, &vt[*m + 1 + (*m +
			1) * vt_dim1], ldvt);

/*
                Overwrite U by left singular vectors of A and VT
                by right singular vectors of A
                (Workspace: need 2*M+N, prefer 2*M+N*NB)
*/

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, m, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    }

	}

    }

/*     Undo scaling if necessary */

    if (iscl == 1) {
	if (anrm > bignum) {
	    dlascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
	if (anrm < smlnum) {
	    dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
    }

/*     Return optimal workspace in WORK(1) */

    work[1] = (doublereal) maxwrk;

    return 0;

/*     End of DGESDD */

} /* dgesdd_ */

/* Subroutine */ int dgesv_(integer *n, integer *nrhs, doublereal *a, integer
	*lda, integer *ipiv, doublereal *b, integer *ldb, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    extern /* Subroutine */ int dgetrf_(integer *, integer *, doublereal *,
	    integer *, integer *, integer *), xerbla_(char *, integer *), dgetrs_(char *, integer *, integer *, doublereal *,
	    integer *, integer *, doublereal *, integer *, integer *);


/*
    -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       March 31, 1993


    Purpose
    =======

    DGESV computes the solution to a real system of linear equations
       A * X = B,
    where A is an N-by-N matrix and X and B are N-by-NRHS matrices.

    The LU decomposition with partial pivoting and row interchanges is
    used to factor A as
       A = P * L * U,
    where P is a permutation matrix, L is unit lower triangular, and U is
    upper triangular.  The factored form of A is then used to solve the
    system of equations A * X = B.

    Arguments
    =========

    N       (input) INTEGER
            The number of linear equations, i.e., the order of the
            matrix A.  N >= 0.

    NRHS    (input) INTEGER
            The number of right hand sides, i.e., the number of columns
            of the matrix B.  NRHS >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the N-by-N coefficient matrix A.
            On exit, the factors L and U from the factorization
            A = P*L*U; the unit diagonal elements of L are not stored.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    IPIV    (output) INTEGER array, dimension (N)
            The pivot indices that define the permutation matrix P;
            row i of the matrix was interchanged with row IPIV(i).

    B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
            On entry, the N-by-NRHS matrix of right hand side matrix B.
            On exit, if INFO = 0, the N-by-NRHS solution matrix X.

    LDB     (input) INTEGER
            The leading dimension of the array B.  LDB >= max(1,N).

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, U(i,i) is exactly zero.  The factorization
                  has been completed, but the factor U is exactly
                  singular, so the solution could not be computed.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*nrhs < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    } else if (*ldb < max(1,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGESV ", &i__1);
	return 0;
    }

/*     Compute the LU factorization of A. */

    dgetrf_(n, n, &a[a_offset], lda, &ipiv[1], info);
    if (*info == 0) {

/*        Solve the system A*X = B, overwriting B with X. */

	dgetrs_("No transpose", n, nrhs, &a[a_offset], lda, &ipiv[1], &b[
		b_offset], ldb, info);
    }
    return 0;

/*     End of DGESV */

} /* dgesv_ */

/* Subroutine */ int dgetf2_(integer *m, integer *n, doublereal *a, integer *
	lda, integer *ipiv, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    doublereal d__1;

    /* Local variables */
    static integer j, jp;
    extern /* Subroutine */ int dger_(integer *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *), dscal_(integer *, doublereal *, doublereal *, integer
	    *), dswap_(integer *, doublereal *, integer *, doublereal *,
	    integer *);
    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1992


    Purpose
    =======

    DGETF2 computes an LU factorization of a general m-by-n matrix A
    using partial pivoting with row interchanges.

    The factorization has the form
       A = P * L * U
    where P is a permutation matrix, L is lower triangular with unit
    diagonal elements (lower trapezoidal if m > n), and U is upper
    triangular (upper trapezoidal if m < n).

    This is the right-looking Level 2 BLAS version of the algorithm.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the m by n matrix to be factored.
            On exit, the factors L and U from the factorization
            A = P*L*U; the unit diagonal elements of L are not stored.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    IPIV    (output) INTEGER array, dimension (min(M,N))
            The pivot indices; for 1 <= i <= min(M,N), row i of the
            matrix was interchanged with row IPIV(i).

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -k, the k-th argument had an illegal value
            > 0: if INFO = k, U(k,k) is exactly zero. The factorization
                 has been completed, but the factor U is exactly
                 singular, and division by zero will occur if it is used
                 to solve a system of equations.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --ipiv;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGETF2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

    i__1 = min(*m,*n);
    for (j = 1; j <= i__1; ++j) {

/*        Find pivot and test for singularity. */

	i__2 = *m - j + 1;
	jp = j - 1 + idamax_(&i__2, &a[j + j * a_dim1], &c__1);
	ipiv[j] = jp;
	if (a[jp + j * a_dim1] != 0.) {

/*           Apply the interchange to columns 1:N. */

	    if (jp != j) {
		dswap_(n, &a[j + a_dim1], lda, &a[jp + a_dim1], lda);
	    }

/*           Compute elements J+1:M of J-th column. */

	    if (j < *m) {
		i__2 = *m - j;
		d__1 = 1. / a[j + j * a_dim1];
		dscal_(&i__2, &d__1, &a[j + 1 + j * a_dim1], &c__1);
	    }

	} else if (*info == 0) {

	    *info = j;
	}

	if (j < min(*m,*n)) {

/*           Update trailing submatrix. */

	    i__2 = *m - j;
	    i__3 = *n - j;
	    dger_(&i__2, &i__3, &c_b151, &a[j + 1 + j * a_dim1], &c__1, &a[j
		    + (j + 1) * a_dim1], lda, &a[j + 1 + (j + 1) * a_dim1],
		    lda);
	}
/* L10: */
    }
    return 0;

/*     End of DGETF2 */

} /* dgetf2_ */

/* Subroutine */ int dgetrf_(integer *m, integer *n, doublereal *a, integer *
	lda, integer *ipiv, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;

    /* Local variables */
    static integer i__, j, jb, nb;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    static integer iinfo;
    extern /* Subroutine */ int dtrsm_(char *, char *, char *, char *,
	    integer *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *), dgetf2_(
	    integer *, integer *, doublereal *, integer *, integer *, integer
	    *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int dlaswp_(integer *, doublereal *, integer *,
	    integer *, integer *, integer *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       March 31, 1993


    Purpose
    =======

    DGETRF computes an LU factorization of a general M-by-N matrix A
    using partial pivoting with row interchanges.

    The factorization has the form
       A = P * L * U
    where P is a permutation matrix, L is lower triangular with unit
    diagonal elements (lower trapezoidal if m > n), and U is upper
    triangular (upper trapezoidal if m < n).

    This is the right-looking Level 3 BLAS version of the algorithm.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the M-by-N matrix to be factored.
            On exit, the factors L and U from the factorization
            A = P*L*U; the unit diagonal elements of L are not stored.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    IPIV    (output) INTEGER array, dimension (min(M,N))
            The pivot indices; for 1 <= i <= min(M,N), row i of the
            matrix was interchanged with row IPIV(i).

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, U(i,i) is exactly zero. The factorization
                  has been completed, but the factor U is exactly
                  singular, and division by zero will occur if it is used
                  to solve a system of equations.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --ipiv;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGETRF", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Determine the block size for this environment. */

    nb = ilaenv_(&c__1, "DGETRF", " ", m, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)
	    1);
    if (nb <= 1 || nb >= min(*m,*n)) {

/*        Use unblocked code. */

	dgetf2_(m, n, &a[a_offset], lda, &ipiv[1], info);
    } else {

/*        Use blocked code. */

	i__1 = min(*m,*n);
	i__2 = nb;
	for (j = 1; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
/* Computing MIN */
	    i__3 = min(*m,*n) - j + 1;
	    jb = min(i__3,nb);

/*
             Factor diagonal and subdiagonal blocks and test for exact
             singularity.
*/

	    i__3 = *m - j + 1;
	    dgetf2_(&i__3, &jb, &a[j + j * a_dim1], lda, &ipiv[j], &iinfo);

/*           Adjust INFO and the pivot indices. */

	    if ((*info == 0 && iinfo > 0)) {
		*info = iinfo + j - 1;
	    }
/* Computing MIN */
	    i__4 = *m, i__5 = j + jb - 1;
	    i__3 = min(i__4,i__5);
	    for (i__ = j; i__ <= i__3; ++i__) {
		ipiv[i__] = j - 1 + ipiv[i__];
/* L10: */
	    }

/*           Apply interchanges to columns 1:J-1. */

	    i__3 = j - 1;
	    i__4 = j + jb - 1;
	    dlaswp_(&i__3, &a[a_offset], lda, &j, &i__4, &ipiv[1], &c__1);

	    if (j + jb <= *n) {

/*              Apply interchanges to columns J+JB:N. */

		i__3 = *n - j - jb + 1;
		i__4 = j + jb - 1;
		dlaswp_(&i__3, &a[(j + jb) * a_dim1 + 1], lda, &j, &i__4, &
			ipiv[1], &c__1);

/*              Compute block row of U. */

		i__3 = *n - j - jb + 1;
		dtrsm_("Left", "Lower", "No transpose", "Unit", &jb, &i__3, &
			c_b15, &a[j + j * a_dim1], lda, &a[j + (j + jb) *
			a_dim1], lda);
		if (j + jb <= *m) {

/*                 Update trailing submatrix. */

		    i__3 = *m - j - jb + 1;
		    i__4 = *n - j - jb + 1;
		    dgemm_("No transpose", "No transpose", &i__3, &i__4, &jb,
			    &c_b151, &a[j + jb + j * a_dim1], lda, &a[j + (j
			    + jb) * a_dim1], lda, &c_b15, &a[j + jb + (j + jb)
			     * a_dim1], lda);
		}
	    }
/* L20: */
	}
    }
    return 0;

/*     End of DGETRF */

} /* dgetrf_ */

/* Subroutine */ int dgetrs_(char *trans, integer *n, integer *nrhs,
	doublereal *a, integer *lda, integer *ipiv, doublereal *b, integer *
	ldb, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dtrsm_(char *, char *, char *, char *,
	    integer *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *), xerbla_(
	    char *, integer *), dlaswp_(integer *, doublereal *,
	    integer *, integer *, integer *, integer *, integer *);
    static logical notran;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       March 31, 1993


    Purpose
    =======

    DGETRS solves a system of linear equations
       A * X = B  or  A' * X = B
    with a general N-by-N matrix A using the LU factorization computed
    by DGETRF.

    Arguments
    =========

    TRANS   (input) CHARACTER*1
            Specifies the form of the system of equations:
            = 'N':  A * X = B  (No transpose)
            = 'T':  A'* X = B  (Transpose)
            = 'C':  A'* X = B  (Conjugate transpose = Transpose)

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    NRHS    (input) INTEGER
            The number of right hand sides, i.e., the number of columns
            of the matrix B.  NRHS >= 0.

    A       (input) DOUBLE PRECISION array, dimension (LDA,N)
            The factors L and U from the factorization A = P*L*U
            as computed by DGETRF.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    IPIV    (input) INTEGER array, dimension (N)
            The pivot indices from DGETRF; for 1<=i<=N, row i of the
            matrix was interchanged with row IPIV(i).

    B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
            On entry, the right hand side matrix B.
            On exit, the solution matrix X.

    LDB     (input) INTEGER
            The leading dimension of the array B.  LDB >= max(1,N).

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    notran = lsame_(trans, "N");
    if (((! notran && ! lsame_(trans, "T")) && ! lsame_(
	    trans, "C"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < max(1,*n)) {
	*info = -5;
    } else if (*ldb < max(1,*n)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGETRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	return 0;
    }

    if (notran) {

/*
          Solve A * X = B.

          Apply row interchanges to the right hand sides.
*/

	dlaswp_(nrhs, &b[b_offset], ldb, &c__1, n, &ipiv[1], &c__1);

/*        Solve L*X = B, overwriting B with X. */

	dtrsm_("Left", "Lower", "No transpose", "Unit", n, nrhs, &c_b15, &a[
		a_offset], lda, &b[b_offset], ldb);

/*        Solve U*X = B, overwriting B with X. */

	dtrsm_("Left", "Upper", "No transpose", "Non-unit", n, nrhs, &c_b15, &
		a[a_offset], lda, &b[b_offset], ldb);
    } else {

/*
          Solve A' * X = B.

          Solve U'*X = B, overwriting B with X.
*/

	dtrsm_("Left", "Upper", "Transpose", "Non-unit", n, nrhs, &c_b15, &a[
		a_offset], lda, &b[b_offset], ldb);

/*        Solve L'*X = B, overwriting B with X. */

	dtrsm_("Left", "Lower", "Transpose", "Unit", n, nrhs, &c_b15, &a[
		a_offset], lda, &b[b_offset], ldb);

/*        Apply row interchanges to the solution vectors. */

	dlaswp_(nrhs, &b[b_offset], ldb, &c__1, n, &ipiv[1], &c_n1);
    }

    return 0;

/*     End of DGETRS */

} /* dgetrs_ */

/* Subroutine */ int dhseqr_(char *job, char *compz, integer *n, integer *ilo,
	 integer *ihi, doublereal *h__, integer *ldh, doublereal *wr,
	doublereal *wi, doublereal *z__, integer *ldz, doublereal *work,
	integer *lwork, integer *info)
{
    /* System generated locals */
    address a__1[2];
    integer h_dim1, h_offset, z_dim1, z_offset, i__1, i__2, i__3[2], i__4,
	    i__5;
    doublereal d__1, d__2;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__, j, k, l;
    static doublereal s[225]	/* was [15][15] */, v[16];
    static integer i1, i2, ii, nh, nr, ns, nv;
    static doublereal vv[16];
    static integer itn;
    static doublereal tau;
    static integer its;
    static doublereal ulp, tst1;
    static integer maxb;
    static doublereal absw;
    static integer ierr;
    static doublereal unfl, temp, ovfl;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *);
    static integer itemp;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static logical initz, wantt, wantz;
    extern doublereal dlapy2_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);

    extern /* Subroutine */ int dlarfg_(integer *, doublereal *, doublereal *,
	     integer *, doublereal *);
    extern integer idamax_(integer *, doublereal *, integer *);
    extern doublereal dlanhs_(char *, integer *, doublereal *, integer *,
	    doublereal *);
    extern /* Subroutine */ int dlahqr_(logical *, logical *, integer *,
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *, integer *, doublereal *, integer *,
	    integer *), dlacpy_(char *, integer *, integer *, doublereal *,
	    integer *, doublereal *, integer *), dlaset_(char *,
	    integer *, integer *, doublereal *, doublereal *, doublereal *,
	    integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int xerbla_(char *, integer *), dlarfx_(
	    char *, integer *, integer *, doublereal *, doublereal *,
	    doublereal *, integer *, doublereal *);
    static doublereal smlnum;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DHSEQR computes the eigenvalues of a real upper Hessenberg matrix H
    and, optionally, the matrices T and Z from the Schur decomposition
    H = Z T Z**T, where T is an upper quasi-triangular matrix (the Schur
    form), and Z is the orthogonal matrix of Schur vectors.

    Optionally Z may be postmultiplied into an input orthogonal matrix Q,
    so that this routine can give the Schur factorization of a matrix A
    which has been reduced to the Hessenberg form H by the orthogonal
    matrix Q:  A = Q*H*Q**T = (QZ)*T*(QZ)**T.

    Arguments
    =========

    JOB     (input) CHARACTER*1
            = 'E':  compute eigenvalues only;
            = 'S':  compute eigenvalues and the Schur form T.

    COMPZ   (input) CHARACTER*1
            = 'N':  no Schur vectors are computed;
            = 'I':  Z is initialized to the unit matrix and the matrix Z
                    of Schur vectors of H is returned;
            = 'V':  Z must contain an orthogonal matrix Q on entry, and
                    the product Q*Z is returned.

    N       (input) INTEGER
            The order of the matrix H.  N >= 0.

    ILO     (input) INTEGER
    IHI     (input) INTEGER
            It is assumed that H is already upper triangular in rows
            and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally
            set by a previous call to DGEBAL, and then passed to SGEHRD
            when the matrix output by DGEBAL is reduced to Hessenberg
            form. Otherwise ILO and IHI should be set to 1 and N
            respectively.
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    H       (input/output) DOUBLE PRECISION array, dimension (LDH,N)
            On entry, the upper Hessenberg matrix H.
            On exit, if JOB = 'S', H contains the upper quasi-triangular
            matrix T from the Schur decomposition (the Schur form);
            2-by-2 diagonal blocks (corresponding to complex conjugate
            pairs of eigenvalues) are returned in standard form, with
            H(i,i) = H(i+1,i+1) and H(i+1,i)*H(i,i+1) < 0. If JOB = 'E',
            the contents of H are unspecified on exit.

    LDH     (input) INTEGER
            The leading dimension of the array H. LDH >= max(1,N).

    WR      (output) DOUBLE PRECISION array, dimension (N)
    WI      (output) DOUBLE PRECISION array, dimension (N)
            The real and imaginary parts, respectively, of the computed
            eigenvalues. If two eigenvalues are computed as a complex
            conjugate pair, they are stored in consecutive elements of
            WR and WI, say the i-th and (i+1)th, with WI(i) > 0 and
            WI(i+1) < 0. If JOB = 'S', the eigenvalues are stored in the
            same order as on the diagonal of the Schur form returned in
            H, with WR(i) = H(i,i) and, if H(i:i+1,i:i+1) is a 2-by-2
            diagonal block, WI(i) = sqrt(H(i+1,i)*H(i,i+1)) and
            WI(i+1) = -WI(i).

    Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N)
            If COMPZ = 'N': Z is not referenced.
            If COMPZ = 'I': on entry, Z need not be set, and on exit, Z
            contains the orthogonal matrix Z of the Schur vectors of H.
            If COMPZ = 'V': on entry Z must contain an N-by-N matrix Q,
            which is assumed to be equal to the unit matrix except for
            the submatrix Z(ILO:IHI,ILO:IHI); on exit Z contains Q*Z.
            Normally Q is the orthogonal matrix generated by DORGHR after
            the call to DGEHRD which formed the Hessenberg matrix H.

    LDZ     (input) INTEGER
            The leading dimension of the array Z.
            LDZ >= max(1,N) if COMPZ = 'I' or 'V'; LDZ >= 1 otherwise.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.  LWORK >= max(1,N).

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, DHSEQR failed to compute all of the
                  eigenvalues in a total of 30*(IHI-ILO+1) iterations;
                  elements 1:ilo-1 and i+1:n of WR and WI contain those
                  eigenvalues which have been successfully computed.

    =====================================================================


       Decode and test the input parameters
*/

    /* Parameter adjustments */
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1 * 1;
    h__ -= h_offset;
    --wr;
    --wi;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    wantt = lsame_(job, "S");
    initz = lsame_(compz, "I");
    wantz = initz || lsame_(compz, "V");

    *info = 0;
    work[1] = (doublereal) max(1,*n);
    lquery = *lwork == -1;
    if ((! lsame_(job, "E") && ! wantt)) {
	*info = -1;
    } else if ((! lsame_(compz, "N") && ! wantz)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ilo < 1 || *ilo > max(1,*n)) {
	*info = -4;
    } else if (*ihi < min(*ilo,*n) || *ihi > *n) {
	*info = -5;
    } else if (*ldh < max(1,*n)) {
	*info = -7;
    } else if (*ldz < 1 || (wantz && *ldz < max(1,*n))) {
	*info = -11;
    } else if ((*lwork < max(1,*n) && ! lquery)) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DHSEQR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Initialize Z, if necessary */

    if (initz) {
	dlaset_("Full", n, n, &c_b29, &c_b15, &z__[z_offset], ldz);
    }

/*     Store the eigenvalues isolated by DGEBAL. */

    i__1 = *ilo - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	wr[i__] = h__[i__ + i__ * h_dim1];
	wi[i__] = 0.;
/* L10: */
    }
    i__1 = *n;
    for (i__ = *ihi + 1; i__ <= i__1; ++i__) {
	wr[i__] = h__[i__ + i__ * h_dim1];
	wi[i__] = 0.;
/* L20: */
    }

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    }
    if (*ilo == *ihi) {
	wr[*ilo] = h__[*ilo + *ilo * h_dim1];
	wi[*ilo] = 0.;
	return 0;
    }

/*
       Set rows and columns ILO to IHI to zero below the first
       subdiagonal.
*/

    i__1 = *ihi - 2;
    for (j = *ilo; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = j + 2; i__ <= i__2; ++i__) {
	    h__[i__ + j * h_dim1] = 0.;
/* L30: */
	}
/* L40: */
    }
    nh = *ihi - *ilo + 1;

/*
       Determine the order of the multi-shift QR algorithm to be used.

   Writing concatenation
*/
    i__3[0] = 1, a__1[0] = job;
    i__3[1] = 1, a__1[1] = compz;
    s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
    ns = ilaenv_(&c__4, "DHSEQR", ch__1, n, ilo, ihi, &c_n1, (ftnlen)6, (
	    ftnlen)2);
/* Writing concatenation */
    i__3[0] = 1, a__1[0] = job;
    i__3[1] = 1, a__1[1] = compz;
    s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
    maxb = ilaenv_(&c__8, "DHSEQR", ch__1, n, ilo, ihi, &c_n1, (ftnlen)6, (
	    ftnlen)2);
    if (ns <= 2 || ns > nh || maxb >= nh) {

/*        Use the standard double-shift algorithm */

	dlahqr_(&wantt, &wantz, n, ilo, ihi, &h__[h_offset], ldh, &wr[1], &wi[
		1], ilo, ihi, &z__[z_offset], ldz, info);
	return 0;
    }
    maxb = max(3,maxb);
/* Computing MIN */
    i__1 = min(ns,maxb);
    ns = min(i__1,15);

/*
       Now 2 < NS <= MAXB < NH.

       Set machine-dependent constants for the stopping criterion.
       If norm(H) <= sqrt(OVFL), overflow should not occur.
*/

    unfl = SAFEMINIMUM;
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    ulp = PRECISION;
    smlnum = unfl * (nh / ulp);

/*
       I1 and I2 are the indices of the first row and last column of H
       to which transformations must be applied. If eigenvalues only are
       being computed, I1 and I2 are set inside the main loop.
*/

    if (wantt) {
	i1 = 1;
	i2 = *n;
    }

/*     ITN is the total number of multiple-shift QR iterations allowed. */

    itn = nh * 30;

/*
       The main loop begins here. I is the loop index and decreases from
       IHI to ILO in steps of at most MAXB. Each iteration of the loop
       works with the active submatrix in rows and columns L to I.
       Eigenvalues I+1 to IHI have already converged. Either L = ILO or
       H(L,L-1) is negligible so that the matrix splits.
*/

    i__ = *ihi;
L50:
    l = *ilo;
    if (i__ < *ilo) {
	goto L170;
    }

/*
       Perform multiple-shift QR iterations on rows and columns ILO to I
       until a submatrix of order at most MAXB splits off at the bottom
       because a subdiagonal element has become negligible.
*/

    i__1 = itn;
    for (its = 0; its <= i__1; ++its) {

/*        Look for a single small subdiagonal element. */

	i__2 = l + 1;
	for (k = i__; k >= i__2; --k) {
	    tst1 = (d__1 = h__[k - 1 + (k - 1) * h_dim1], abs(d__1)) + (d__2 =
		     h__[k + k * h_dim1], abs(d__2));
	    if (tst1 == 0.) {
		i__4 = i__ - l + 1;
		tst1 = dlanhs_("1", &i__4, &h__[l + l * h_dim1], ldh, &work[1]
			);
	    }
/* Computing MAX */
	    d__2 = ulp * tst1;
	    if ((d__1 = h__[k + (k - 1) * h_dim1], abs(d__1)) <= max(d__2,
		    smlnum)) {
		goto L70;
	    }
/* L60: */
	}
L70:
	l = k;
	if (l > *ilo) {

/*           H(L,L-1) is negligible. */

	    h__[l + (l - 1) * h_dim1] = 0.;
	}

/*        Exit from loop if a submatrix of order <= MAXB has split off. */

	if (l >= i__ - maxb + 1) {
	    goto L160;
	}

/*
          Now the active submatrix is in rows and columns L to I. If
          eigenvalues only are being computed, only the active submatrix
          need be transformed.
*/

	if (! wantt) {
	    i1 = l;
	    i2 = i__;
	}

	if (its == 20 || its == 30) {

/*           Exceptional shifts. */

	    i__2 = i__;
	    for (ii = i__ - ns + 1; ii <= i__2; ++ii) {
		wr[ii] = ((d__1 = h__[ii + (ii - 1) * h_dim1], abs(d__1)) + (
			d__2 = h__[ii + ii * h_dim1], abs(d__2))) * 1.5;
		wi[ii] = 0.;
/* L80: */
	    }
	} else {

/*           Use eigenvalues of trailing submatrix of order NS as shifts. */

	    dlacpy_("Full", &ns, &ns, &h__[i__ - ns + 1 + (i__ - ns + 1) *
		    h_dim1], ldh, s, &c__15);
	    dlahqr_(&c_false, &c_false, &ns, &c__1, &ns, s, &c__15, &wr[i__ -
		    ns + 1], &wi[i__ - ns + 1], &c__1, &ns, &z__[z_offset],
		    ldz, &ierr);
	    if (ierr > 0) {

/*
                If DLAHQR failed to compute all NS eigenvalues, use the
                unconverged diagonal elements as the remaining shifts.
*/

		i__2 = ierr;
		for (ii = 1; ii <= i__2; ++ii) {
		    wr[i__ - ns + ii] = s[ii + ii * 15 - 16];
		    wi[i__ - ns + ii] = 0.;
/* L90: */
		}
	    }
	}

/*
          Form the first column of (G-w(1)) (G-w(2)) . . . (G-w(ns))
          where G is the Hessenberg submatrix H(L:I,L:I) and w is
          the vector of shifts (stored in WR and WI). The result is
          stored in the local array V.
*/

	v[0] = 1.;
	i__2 = ns + 1;
	for (ii = 2; ii <= i__2; ++ii) {
	    v[ii - 1] = 0.;
/* L100: */
	}
	nv = 1;
	i__2 = i__;
	for (j = i__ - ns + 1; j <= i__2; ++j) {
	    if (wi[j] >= 0.) {
		if (wi[j] == 0.) {

/*                 real shift */

		    i__4 = nv + 1;
		    dcopy_(&i__4, v, &c__1, vv, &c__1);
		    i__4 = nv + 1;
		    d__1 = -wr[j];
		    dgemv_("No transpose", &i__4, &nv, &c_b15, &h__[l + l *
			    h_dim1], ldh, vv, &c__1, &d__1, v, &c__1);
		    ++nv;
		} else if (wi[j] > 0.) {

/*                 complex conjugate pair of shifts */

		    i__4 = nv + 1;
		    dcopy_(&i__4, v, &c__1, vv, &c__1);
		    i__4 = nv + 1;
		    d__1 = wr[j] * -2.;
		    dgemv_("No transpose", &i__4, &nv, &c_b15, &h__[l + l *
			    h_dim1], ldh, v, &c__1, &d__1, vv, &c__1);
		    i__4 = nv + 1;
		    itemp = idamax_(&i__4, vv, &c__1);
/* Computing MAX */
		    d__2 = (d__1 = vv[itemp - 1], abs(d__1));
		    temp = 1. / max(d__2,smlnum);
		    i__4 = nv + 1;
		    dscal_(&i__4, &temp, vv, &c__1);
		    absw = dlapy2_(&wr[j], &wi[j]);
		    temp = temp * absw * absw;
		    i__4 = nv + 2;
		    i__5 = nv + 1;
		    dgemv_("No transpose", &i__4, &i__5, &c_b15, &h__[l + l *
			    h_dim1], ldh, vv, &c__1, &temp, v, &c__1);
		    nv += 2;
		}

/*
                Scale V(1:NV) so that max(abs(V(i))) = 1. If V is zero,
                reset it to the unit vector.
*/

		itemp = idamax_(&nv, v, &c__1);
		temp = (d__1 = v[itemp - 1], abs(d__1));
		if (temp == 0.) {
		    v[0] = 1.;
		    i__4 = nv;
		    for (ii = 2; ii <= i__4; ++ii) {
			v[ii - 1] = 0.;
/* L110: */
		    }
		} else {
		    temp = max(temp,smlnum);
		    d__1 = 1. / temp;
		    dscal_(&nv, &d__1, v, &c__1);
		}
	    }
/* L120: */
	}

/*        Multiple-shift QR step */

	i__2 = i__ - 1;
	for (k = l; k <= i__2; ++k) {

/*
             The first iteration of this loop determines a reflection G
             from the vector V and applies it from left and right to H,
             thus creating a nonzero bulge below the subdiagonal.

             Each subsequent iteration determines a reflection G to
             restore the Hessenberg form in the (K-1)th column, and thus
             chases the bulge one step toward the bottom of the active
             submatrix. NR is the order of G.

   Computing MIN
*/
	    i__4 = ns + 1, i__5 = i__ - k + 1;
	    nr = min(i__4,i__5);
	    if (k > l) {
		dcopy_(&nr, &h__[k + (k - 1) * h_dim1], &c__1, v, &c__1);
	    }
	    dlarfg_(&nr, v, &v[1], &c__1, &tau);
	    if (k > l) {
		h__[k + (k - 1) * h_dim1] = v[0];
		i__4 = i__;
		for (ii = k + 1; ii <= i__4; ++ii) {
		    h__[ii + (k - 1) * h_dim1] = 0.;
/* L130: */
		}
	    }
	    v[0] = 1.;

/*
             Apply G from the left to transform the rows of the matrix in
             columns K to I2.
*/

	    i__4 = i2 - k + 1;
	    dlarfx_("Left", &nr, &i__4, v, &tau, &h__[k + k * h_dim1], ldh, &
		    work[1]);

/*
             Apply G from the right to transform the columns of the
             matrix in rows I1 to min(K+NR,I).

   Computing MIN
*/
	    i__5 = k + nr;
	    i__4 = min(i__5,i__) - i1 + 1;
	    dlarfx_("Right", &i__4, &nr, v, &tau, &h__[i1 + k * h_dim1], ldh,
		    &work[1]);

	    if (wantz) {

/*              Accumulate transformations in the matrix Z */

		dlarfx_("Right", &nh, &nr, v, &tau, &z__[*ilo + k * z_dim1],
			ldz, &work[1]);
	    }
/* L140: */
	}

/* L150: */
    }

/*     Failure to converge in remaining number of iterations */

    *info = i__;
    return 0;

L160:

/*
       A submatrix of order <= MAXB in rows and columns L to I has split
       off. Use the double-shift QR algorithm to handle it.
*/

    dlahqr_(&wantt, &wantz, n, &l, &i__, &h__[h_offset], ldh, &wr[1], &wi[1],
	    ilo, ihi, &z__[z_offset], ldz, info);
    if (*info > 0) {
	return 0;
    }

/*
       Decrement number of remaining iterations, and return to start of
       the main loop with a new value of I.
*/

    itn -= its;
    i__ = l - 1;
    goto L50;

L170:
    work[1] = (doublereal) max(1,*n);
    return 0;

/*     End of DHSEQR */

} /* dhseqr_ */

/* Subroutine */ int dlabad_(doublereal *small, doublereal *large)
{
    /* Builtin functions */
    double d_lg10(doublereal *), sqrt(doublereal);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLABAD takes as input the values computed by DLAMCH for underflow and
    overflow, and returns the square root of each of these values if the
    log of LARGE is sufficiently large.  This subroutine is intended to
    identify machines with a large exponent range, such as the Crays, and
    redefine the underflow and overflow limits to be the square roots of
    the values computed by DLAMCH.  This subroutine is needed because
    DLAMCH does not compensate for poor arithmetic in the upper half of
    the exponent range, as is found on a Cray.

    Arguments
    =========

    SMALL   (input/output) DOUBLE PRECISION
            On entry, the underflow threshold as computed by DLAMCH.
            On exit, if LOG10(LARGE) is sufficiently large, the square
            root of SMALL, otherwise unchanged.

    LARGE   (input/output) DOUBLE PRECISION
            On entry, the overflow threshold as computed by DLAMCH.
            On exit, if LOG10(LARGE) is sufficiently large, the square
            root of LARGE, otherwise unchanged.

    =====================================================================


       If it looks like we're on a Cray, take the square root of
       SMALL and LARGE to avoid overflow and underflow problems.
*/

    if (d_lg10(large) > 2e3) {
	*small = sqrt(*small);
	*large = sqrt(*large);
    }

    return 0;

/*     End of DLABAD */

} /* dlabad_ */

/* Subroutine */ int dlabrd_(integer *m, integer *n, integer *nb, doublereal *
	a, integer *lda, doublereal *d__, doublereal *e, doublereal *tauq,
	doublereal *taup, doublereal *x, integer *ldx, doublereal *y, integer
	*ldy)
{
    /* System generated locals */
    integer a_dim1, a_offset, x_dim1, x_offset, y_dim1, y_offset, i__1, i__2,
	    i__3;

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *), dgemv_(char *, integer *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *), dlarfg_(integer *, doublereal *,
	     doublereal *, integer *, doublereal *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLABRD reduces the first NB rows and columns of a real general
    m by n matrix A to upper or lower bidiagonal form by an orthogonal
    transformation Q' * A * P, and returns the matrices X and Y which
    are needed to apply the transformation to the unreduced part of A.

    If m >= n, A is reduced to upper bidiagonal form; if m < n, to lower
    bidiagonal form.

    This is an auxiliary routine called by DGEBRD

    Arguments
    =========

    M       (input) INTEGER
            The number of rows in the matrix A.

    N       (input) INTEGER
            The number of columns in the matrix A.

    NB      (input) INTEGER
            The number of leading rows and columns of A to be reduced.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the m by n general matrix to be reduced.
            On exit, the first NB rows and columns of the matrix are
            overwritten; the rest of the array is unchanged.
            If m >= n, elements on and below the diagonal in the first NB
              columns, with the array TAUQ, represent the orthogonal
              matrix Q as a product of elementary reflectors; and
              elements above the diagonal in the first NB rows, with the
              array TAUP, represent the orthogonal matrix P as a product
              of elementary reflectors.
            If m < n, elements below the diagonal in the first NB
              columns, with the array TAUQ, represent the orthogonal
              matrix Q as a product of elementary reflectors, and
              elements on and above the diagonal in the first NB rows,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors.
            See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    D       (output) DOUBLE PRECISION array, dimension (NB)
            The diagonal elements of the first NB rows and columns of
            the reduced matrix.  D(i) = A(i,i).

    E       (output) DOUBLE PRECISION array, dimension (NB)
            The off-diagonal elements of the first NB rows and columns of
            the reduced matrix.

    TAUQ    (output) DOUBLE PRECISION array dimension (NB)
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix Q. See Further Details.

    TAUP    (output) DOUBLE PRECISION array, dimension (NB)
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix P. See Further Details.

    X       (output) DOUBLE PRECISION array, dimension (LDX,NB)
            The m-by-nb matrix X required to update the unreduced part
            of A.

    LDX     (input) INTEGER
            The leading dimension of the array X. LDX >= M.

    Y       (output) DOUBLE PRECISION array, dimension (LDY,NB)
            The n-by-nb matrix Y required to update the unreduced part
            of A.

    LDY     (output) INTEGER
            The leading dimension of the array Y. LDY >= N.

    Further Details
    ===============

    The matrices Q and P are represented as products of elementary
    reflectors:

       Q = H(1) H(2) . . . H(nb)  and  P = G(1) G(2) . . . G(nb)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors.

    If m >= n, v(1:i-1) = 0, v(i) = 1, and v(i:m) is stored on exit in
    A(i:m,i); u(1:i) = 0, u(i+1) = 1, and u(i+1:n) is stored on exit in
    A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i).

    If m < n, v(1:i) = 0, v(i+1) = 1, and v(i+1:m) is stored on exit in
    A(i+2:m,i); u(1:i-1) = 0, u(i) = 1, and u(i:n) is stored on exit in
    A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i).

    The elements of the vectors v and u together form the m-by-nb matrix
    V and the nb-by-n matrix U' which are needed, with X and Y, to apply
    the transformation to the unreduced part of the matrix, using a block
    update of the form:  A := A - V*Y' - X*U'.

    The contents of A on exit are illustrated by the following examples
    with nb = 2:

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):

      (  1   1   u1  u1  u1 )           (  1   u1  u1  u1  u1  u1 )
      (  v1  1   1   u2  u2 )           (  1   1   u2  u2  u2  u2 )
      (  v1  v2  a   a   a  )           (  v1  1   a   a   a   a  )
      (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  )
      (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  )
      (  v1  v2  a   a   a  )

    where a denotes an element of the original matrix which is unchanged,
    vi denotes an element of the vector defining H(i), and ui an element
    of the vector defining G(i).

    =====================================================================


       Quick return if possible
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1 * 1;
    x -= x_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1 * 1;
    y -= y_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    if (*m >= *n) {

/*        Reduce to upper bidiagonal form */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i:m,i) */

	    i__2 = *m - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[i__ + a_dim1],
		    lda, &y[i__ + y_dim1], ldy, &c_b15, &a[i__ + i__ * a_dim1]
		    , &c__1);
	    i__2 = *m - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b151, &x[i__ + x_dim1],
		    ldx, &a[i__ * a_dim1 + 1], &c__1, &c_b15, &a[i__ + i__ *
		    a_dim1], &c__1);

/*           Generate reflection Q(i) to annihilate A(i+1:m,i) */

	    i__2 = *m - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[min(i__3,*m) + i__ *
		    a_dim1], &c__1, &tauq[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    if (i__ < *n) {
		a[i__ + i__ * a_dim1] = 1.;

/*              Compute Y(i+1:n,i) */

		i__2 = *m - i__ + 1;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[i__ + (i__ + 1) *
			 a_dim1], lda, &a[i__ + i__ * a_dim1], &c__1, &c_b29,
			&y[i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__ + 1;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[i__ + a_dim1],
			lda, &a[i__ + i__ * a_dim1], &c__1, &c_b29, &y[i__ *
			y_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &y[i__ + 1 +
			y_dim1], ldy, &y[i__ * y_dim1 + 1], &c__1, &c_b15, &y[
			i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__ + 1;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &x[i__ + x_dim1],
			ldx, &a[i__ + i__ * a_dim1], &c__1, &c_b29, &y[i__ *
			y_dim1 + 1], &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b151, &a[(i__ + 1) *
			a_dim1 + 1], lda, &y[i__ * y_dim1 + 1], &c__1, &c_b15,
			 &y[i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *n - i__;
		dscal_(&i__2, &tauq[i__], &y[i__ + 1 + i__ * y_dim1], &c__1);

/*              Update A(i,i+1:n) */

		i__2 = *n - i__;
		dgemv_("No transpose", &i__2, &i__, &c_b151, &y[i__ + 1 +
			y_dim1], ldy, &a[i__ + a_dim1], lda, &c_b15, &a[i__ +
			(i__ + 1) * a_dim1], lda);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b151, &a[(i__ + 1) *
			a_dim1 + 1], lda, &x[i__ + x_dim1], ldx, &c_b15, &a[
			i__ + (i__ + 1) * a_dim1], lda);

/*              Generate reflection P(i) to annihilate A(i,i+2:n) */

		i__2 = *n - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + (i__ + 1) * a_dim1], &a[i__ + min(
			i__3,*n) * a_dim1], lda, &taup[i__]);
		e[i__] = a[i__ + (i__ + 1) * a_dim1];
		a[i__ + (i__ + 1) * a_dim1] = 1.;

/*              Compute X(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dgemv_("No transpose", &i__2, &i__3, &c_b15, &a[i__ + 1 + (
			i__ + 1) * a_dim1], lda, &a[i__ + (i__ + 1) * a_dim1],
			 lda, &c_b29, &x[i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *n - i__;
		dgemv_("Transpose", &i__2, &i__, &c_b15, &y[i__ + 1 + y_dim1],
			 ldy, &a[i__ + (i__ + 1) * a_dim1], lda, &c_b29, &x[
			i__ * x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		dgemv_("No transpose", &i__2, &i__, &c_b151, &a[i__ + 1 +
			a_dim1], lda, &x[i__ * x_dim1 + 1], &c__1, &c_b15, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		dgemv_("No transpose", &i__2, &i__3, &c_b15, &a[(i__ + 1) *
			a_dim1 + 1], lda, &a[i__ + (i__ + 1) * a_dim1], lda, &
			c_b29, &x[i__ * x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &x[i__ + 1 +
			x_dim1], ldx, &x[i__ * x_dim1 + 1], &c__1, &c_b15, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *m - i__;
		dscal_(&i__2, &taup[i__], &x[i__ + 1 + i__ * x_dim1], &c__1);
	    }
/* L10: */
	}
    } else {

/*        Reduce to lower bidiagonal form */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i,i:n) */

	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b151, &y[i__ + y_dim1],
		    ldy, &a[i__ + a_dim1], lda, &c_b15, &a[i__ + i__ * a_dim1]
		    , lda);
	    i__2 = i__ - 1;
	    i__3 = *n - i__ + 1;
	    dgemv_("Transpose", &i__2, &i__3, &c_b151, &a[i__ * a_dim1 + 1],
		    lda, &x[i__ + x_dim1], ldx, &c_b15, &a[i__ + i__ * a_dim1]
		    , lda);

/*           Generate reflection P(i) to annihilate A(i,i+1:n) */

	    i__2 = *n - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[i__ + min(i__3,*n) *
		    a_dim1], lda, &taup[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    if (i__ < *m) {
		a[i__ + i__ * a_dim1] = 1.;

/*              Compute X(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = *n - i__ + 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b15, &a[i__ + 1 + i__
			* a_dim1], lda, &a[i__ + i__ * a_dim1], lda, &c_b29, &
			x[i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *n - i__ + 1;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &y[i__ + y_dim1],
			ldy, &a[i__ + i__ * a_dim1], lda, &c_b29, &x[i__ *
			x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[i__ + 1 +
			a_dim1], lda, &x[i__ * x_dim1 + 1], &c__1, &c_b15, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__ + 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b15, &a[i__ * a_dim1
			+ 1], lda, &a[i__ + i__ * a_dim1], lda, &c_b29, &x[
			i__ * x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &x[i__ + 1 +
			x_dim1], ldx, &x[i__ * x_dim1 + 1], &c__1, &c_b15, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *m - i__;
		dscal_(&i__2, &taup[i__], &x[i__ + 1 + i__ * x_dim1], &c__1);

/*              Update A(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[i__ + 1 +
			a_dim1], lda, &y[i__ + y_dim1], ldy, &c_b15, &a[i__ +
			1 + i__ * a_dim1], &c__1);
		i__2 = *m - i__;
		dgemv_("No transpose", &i__2, &i__, &c_b151, &x[i__ + 1 +
			x_dim1], ldx, &a[i__ * a_dim1 + 1], &c__1, &c_b15, &a[
			i__ + 1 + i__ * a_dim1], &c__1);

/*              Generate reflection Q(i) to annihilate A(i+2:m,i) */

		i__2 = *m - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[min(i__3,*m) +
			i__ * a_dim1], &c__1, &tauq[i__]);
		e[i__] = a[i__ + 1 + i__ * a_dim1];
		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Compute Y(i+1:n,i) */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[i__ + 1 + (i__ +
			1) * a_dim1], lda, &a[i__ + 1 + i__ * a_dim1], &c__1,
			&c_b29, &y[i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[i__ + 1 + a_dim1]
			, lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b29, &y[
			i__ * y_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &y[i__ + 1 +
			y_dim1], ldy, &y[i__ * y_dim1 + 1], &c__1, &c_b15, &y[
			i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__;
		dgemv_("Transpose", &i__2, &i__, &c_b15, &x[i__ + 1 + x_dim1],
			 ldx, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b29, &y[
			i__ * y_dim1 + 1], &c__1);
		i__2 = *n - i__;
		dgemv_("Transpose", &i__, &i__2, &c_b151, &a[(i__ + 1) *
			a_dim1 + 1], lda, &y[i__ * y_dim1 + 1], &c__1, &c_b15,
			 &y[i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *n - i__;
		dscal_(&i__2, &tauq[i__], &y[i__ + 1 + i__ * y_dim1], &c__1);
	    }
/* L20: */
	}
    }
    return 0;

/*     End of DLABRD */

} /* dlabrd_ */

/* Subroutine */ int dlacpy_(char *uplo, integer *m, integer *n, doublereal *
	a, integer *lda, doublereal *b, integer *ldb)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;
    extern logical lsame_(char *, char *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLACPY copies all or part of a two-dimensional matrix A to another
    matrix B.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            Specifies the part of the matrix A to be copied to B.
            = 'U':      Upper triangular part
            = 'L':      Lower triangular part
            Otherwise:  All of the matrix A

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input) DOUBLE PRECISION array, dimension (LDA,N)
            The m by n matrix A.  If UPLO = 'U', only the upper triangle
            or trapezoid is accessed; if UPLO = 'L', only the lower
            triangle or trapezoid is accessed.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    B       (output) DOUBLE PRECISION array, dimension (LDB,N)
            On exit, B = A in the locations specified by UPLO.

    LDB     (input) INTEGER
            The leading dimension of the array B.  LDB >= max(1,M).

    =====================================================================
*/


    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;

    /* Function Body */
    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = min(j,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = a[i__ + j * a_dim1];
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(uplo, "L")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = a[i__ + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = a[i__ + j * a_dim1];
/* L50: */
	    }
/* L60: */
	}
    }
    return 0;

/*     End of DLACPY */

} /* dlacpy_ */

/* Subroutine */ int dladiv_(doublereal *a, doublereal *b, doublereal *c__,
	doublereal *d__, doublereal *p, doublereal *q)
{
    static doublereal e, f;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLADIV performs complex division in  real arithmetic

                          a + i*b
               p + i*q = ---------
                          c + i*d

    The algorithm is due to Robert L. Smith and can be found
    in D. Knuth, The art of Computer Programming, Vol.2, p.195

    Arguments
    =========

    A       (input) DOUBLE PRECISION
    B       (input) DOUBLE PRECISION
    C       (input) DOUBLE PRECISION
    D       (input) DOUBLE PRECISION
            The scalars a, b, c, and d in the above expression.

    P       (output) DOUBLE PRECISION
    Q       (output) DOUBLE PRECISION
            The scalars p and q in the above expression.

    =====================================================================
*/


    if (abs(*d__) < abs(*c__)) {
	e = *d__ / *c__;
	f = *c__ + *d__ * e;
	*p = (*a + *b * e) / f;
	*q = (*b - *a * e) / f;
    } else {
	e = *c__ / *d__;
	f = *d__ + *c__ * e;
	*p = (*b + *a * e) / f;
	*q = (-(*a) + *b * e) / f;
    }

    return 0;

/*     End of DLADIV */

} /* dladiv_ */

/* Subroutine */ int dlae2_(doublereal *a, doublereal *b, doublereal *c__,
	doublereal *rt1, doublereal *rt2)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal ab, df, tb, sm, rt, adf, acmn, acmx;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLAE2  computes the eigenvalues of a 2-by-2 symmetric matrix
       [  A   B  ]
       [  B   C  ].
    On return, RT1 is the eigenvalue of larger absolute value, and RT2
    is the eigenvalue of smaller absolute value.

    Arguments
    =========

    A       (input) DOUBLE PRECISION
            The (1,1) element of the 2-by-2 matrix.

    B       (input) DOUBLE PRECISION
            The (1,2) and (2,1) elements of the 2-by-2 matrix.

    C       (input) DOUBLE PRECISION
            The (2,2) element of the 2-by-2 matrix.

    RT1     (output) DOUBLE PRECISION
            The eigenvalue of larger absolute value.

    RT2     (output) DOUBLE PRECISION
            The eigenvalue of smaller absolute value.

    Further Details
    ===============

    RT1 is accurate to a few ulps barring over/underflow.

    RT2 may be inaccurate if there is massive cancellation in the
    determinant A*C-B*B; higher precision or correctly rounded or
    correctly truncated arithmetic would be needed to compute RT2
    accurately in all cases.

    Overflow is possible only if RT1 is within a factor of 5 of overflow.
    Underflow is harmless if the input data is 0 or exceeds
       underflow_threshold / macheps.

   =====================================================================


       Compute the eigenvalues
*/

    sm = *a + *c__;
    df = *a - *c__;
    adf = abs(df);
    tb = *b + *b;
    ab = abs(tb);
    if (abs(*a) > abs(*c__)) {
	acmx = *a;
	acmn = *c__;
    } else {
	acmx = *c__;
	acmn = *a;
    }
    if (adf > ab) {
/* Computing 2nd power */
	d__1 = ab / adf;
	rt = adf * sqrt(d__1 * d__1 + 1.);
    } else if (adf < ab) {
/* Computing 2nd power */
	d__1 = adf / ab;
	rt = ab * sqrt(d__1 * d__1 + 1.);
    } else {

/*        Includes case AB=ADF=0 */

	rt = ab * sqrt(2.);
    }
    if (sm < 0.) {
	*rt1 = (sm - rt) * .5;

/*
          Order of execution important.
          To get fully accurate smaller eigenvalue,
          next line needs to be executed in higher precision.
*/

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else if (sm > 0.) {
	*rt1 = (sm + rt) * .5;

/*
          Order of execution important.
          To get fully accurate smaller eigenvalue,
          next line needs to be executed in higher precision.
*/

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else {

/*        Includes case RT1 = RT2 = 0 */

	*rt1 = rt * .5;
	*rt2 = rt * -.5;
    }
    return 0;

/*     End of DLAE2 */

} /* dlae2_ */

/* Subroutine */ int dlaed0_(integer *icompq, integer *qsiz, integer *n,
	doublereal *d__, doublereal *e, doublereal *q, integer *ldq,
	doublereal *qstore, integer *ldqs, doublereal *work, integer *iwork,
	integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, qstore_dim1, qstore_offset, i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double log(doublereal);
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__, j, k, iq, lgn, msd2, smm1, spm1, spm2;
    static doublereal temp;
    static integer curr;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    static integer iperm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer indxq, iwrem;
    extern /* Subroutine */ int dlaed1_(integer *, doublereal *, doublereal *,
	     integer *, integer *, doublereal *, integer *, doublereal *,
	    integer *, integer *);
    static integer iqptr;
    extern /* Subroutine */ int dlaed7_(integer *, integer *, integer *,
	    integer *, integer *, integer *, doublereal *, doublereal *,
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    integer *, integer *, integer *, integer *, integer *, doublereal
	    *, doublereal *, integer *, integer *);
    static integer tlvls;
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *);
    static integer igivcl;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer igivnm, submat, curprb, subpbs, igivpt;
    extern /* Subroutine */ int dsteqr_(char *, integer *, doublereal *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *);
    static integer curlvl, matsiz, iprmpt, smlsiz;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLAED0 computes all eigenvalues and corresponding eigenvectors of a
    symmetric tridiagonal matrix using the divide and conquer method.

    Arguments
    =========

    ICOMPQ  (input) INTEGER
            = 0:  Compute eigenvalues only.
            = 1:  Compute eigenvectors of original dense symmetric matrix
                  also.  On entry, Q contains the orthogonal matrix used
                  to reduce the original matrix to tridiagonal form.
            = 2:  Compute eigenvalues and eigenvectors of tridiagonal
                  matrix.

    QSIZ   (input) INTEGER
           The dimension of the orthogonal matrix used to reduce
           the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1.

    N      (input) INTEGER
           The dimension of the symmetric tridiagonal matrix.  N >= 0.

    D      (input/output) DOUBLE PRECISION array, dimension (N)
           On entry, the main diagonal of the tridiagonal matrix.
           On exit, its eigenvalues.

    E      (input) DOUBLE PRECISION array, dimension (N-1)
           The off-diagonal elements of the tridiagonal matrix.
           On exit, E has been destroyed.

    Q      (input/output) DOUBLE PRECISION array, dimension (LDQ, N)
           On entry, Q must contain an N-by-N orthogonal matrix.
           If ICOMPQ = 0    Q is not referenced.
           If ICOMPQ = 1    On entry, Q is a subset of the columns of the
                            orthogonal matrix used to reduce the full
                            matrix to tridiagonal form corresponding to
                            the subset of the full matrix which is being
                            decomposed at this time.
           If ICOMPQ = 2    On entry, Q will be the identity matrix.
                            On exit, Q contains the eigenvectors of the
                            tridiagonal matrix.

    LDQ    (input) INTEGER
           The leading dimension of the array Q.  If eigenvectors are
           desired, then  LDQ >= max(1,N).  In any case,  LDQ >= 1.

    QSTORE (workspace) DOUBLE PRECISION array, dimension (LDQS, N)
           Referenced only when ICOMPQ = 1.  Used to store parts of
           the eigenvector matrix when the updating matrix multiplies
           take place.

    LDQS   (input) INTEGER
           The leading dimension of the array QSTORE.  If ICOMPQ = 1,
           then  LDQS >= max(1,N).  In any case,  LDQS >= 1.

    WORK   (workspace) DOUBLE PRECISION array,
           If ICOMPQ = 0 or 1, the dimension of WORK must be at least
                       1 + 3*N + 2*N*lg N + 2*N**2
                       ( lg( N ) = smallest integer k
                                   such that 2^k >= N )
           If ICOMPQ = 2, the dimension of WORK must be at least
                       4*N + N**2.

    IWORK  (workspace) INTEGER array,
           If ICOMPQ = 0 or 1, the dimension of IWORK must be at least
                          6 + 6*N + 5*N*lg N.
                          ( lg( N ) = smallest integer k
                                      such that 2^k >= N )
           If ICOMPQ = 2, the dimension of IWORK must be at least
                          3 + 5*N.

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  The algorithm failed to compute an eigenvalue while
                  working on the submatrix lying in rows and columns
                  INFO/(N+1) through mod(INFO,N+1).

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    qstore_dim1 = *ldqs;
    qstore_offset = 1 + qstore_dim1 * 1;
    qstore -= qstore_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 2) {
	*info = -1;
    } else if ((*icompq == 1 && *qsiz < max(0,*n))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ldq < max(1,*n)) {
	*info = -7;
    } else if (*ldqs < max(1,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED0", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    smlsiz = ilaenv_(&c__9, "DLAED0", " ", &c__0, &c__0, &c__0, &c__0, (
	    ftnlen)6, (ftnlen)1);

/*
       Determine the size and placement of the submatrices, and save in
       the leading elements of IWORK.
*/

    iwork[1] = *n;
    subpbs = 1;
    tlvls = 0;
L10:
    if (iwork[subpbs] > smlsiz) {
	for (j = subpbs; j >= 1; --j) {
	    iwork[j * 2] = (iwork[j] + 1) / 2;
	    iwork[((j) << (1)) - 1] = iwork[j] / 2;
/* L20: */
	}
	++tlvls;
	subpbs <<= 1;
	goto L10;
    }
    i__1 = subpbs;
    for (j = 2; j <= i__1; ++j) {
	iwork[j] += iwork[j - 1];
/* L30: */
    }

/*
       Divide the matrix into SUBPBS submatrices of size at most SMLSIZ+1
       using rank-1 modifications (cuts).
*/

    spm1 = subpbs - 1;
    i__1 = spm1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	submat = iwork[i__] + 1;
	smm1 = submat - 1;
	d__[smm1] -= (d__1 = e[smm1], abs(d__1));
	d__[submat] -= (d__1 = e[smm1], abs(d__1));
/* L40: */
    }

    indxq = ((*n) << (2)) + 3;
    if (*icompq != 2) {

/*
          Set up workspaces for eigenvalues only/accumulate new vectors
          routine
*/

	temp = log((doublereal) (*n)) / log(2.);
	lgn = (integer) temp;
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	iprmpt = indxq + *n + 1;
	iperm = iprmpt + *n * lgn;
	iqptr = iperm + *n * lgn;
	igivpt = iqptr + *n + 2;
	igivcl = igivpt + *n * lgn;

	igivnm = 1;
	iq = igivnm + ((*n) << (1)) * lgn;
/* Computing 2nd power */
	i__1 = *n;
	iwrem = iq + i__1 * i__1 + 1;

/*        Initialize pointers */

	i__1 = subpbs;
	for (i__ = 0; i__ <= i__1; ++i__) {
	    iwork[iprmpt + i__] = 1;
	    iwork[igivpt + i__] = 1;
/* L50: */
	}
	iwork[iqptr] = 1;
    }

/*
       Solve each submatrix eigenproblem at the bottom of the divide and
       conquer tree.
*/

    curr = 0;
    i__1 = spm1;
    for (i__ = 0; i__ <= i__1; ++i__) {
	if (i__ == 0) {
	    submat = 1;
	    matsiz = iwork[1];
	} else {
	    submat = iwork[i__] + 1;
	    matsiz = iwork[i__ + 1] - iwork[i__];
	}
	if (*icompq == 2) {
	    dsteqr_("I", &matsiz, &d__[submat], &e[submat], &q[submat +
		    submat * q_dim1], ldq, &work[1], info);
	    if (*info != 0) {
		goto L130;
	    }
	} else {
	    dsteqr_("I", &matsiz, &d__[submat], &e[submat], &work[iq - 1 +
		    iwork[iqptr + curr]], &matsiz, &work[1], info);
	    if (*info != 0) {
		goto L130;
	    }
	    if (*icompq == 1) {
		dgemm_("N", "N", qsiz, &matsiz, &matsiz, &c_b15, &q[submat *
			q_dim1 + 1], ldq, &work[iq - 1 + iwork[iqptr + curr]],
			 &matsiz, &c_b29, &qstore[submat * qstore_dim1 + 1],
			ldqs);
	    }
/* Computing 2nd power */
	    i__2 = matsiz;
	    iwork[iqptr + curr + 1] = iwork[iqptr + curr] + i__2 * i__2;
	    ++curr;
	}
	k = 1;
	i__2 = iwork[i__ + 1];
	for (j = submat; j <= i__2; ++j) {
	    iwork[indxq + j] = k;
	    ++k;
/* L60: */
	}
/* L70: */
    }

/*
       Successively merge eigensystems of adjacent submatrices
       into eigensystem for the corresponding larger matrix.

       while ( SUBPBS > 1 )
*/

    curlvl = 1;
L80:
    if (subpbs > 1) {
	spm2 = subpbs - 2;
	i__1 = spm2;
	for (i__ = 0; i__ <= i__1; i__ += 2) {
	    if (i__ == 0) {
		submat = 1;
		matsiz = iwork[2];
		msd2 = iwork[1];
		curprb = 0;
	    } else {
		submat = iwork[i__] + 1;
		matsiz = iwork[i__ + 2] - iwork[i__];
		msd2 = matsiz / 2;
		++curprb;
	    }

/*
       Merge lower order eigensystems (of size MSD2 and MATSIZ - MSD2)
       into an eigensystem of size MATSIZ.
       DLAED1 is used only for the full eigensystem of a tridiagonal
       matrix.
       DLAED7 handles the cases in which eigenvalues only or eigenvalues
       and eigenvectors of a full symmetric matrix (which was reduced to
       tridiagonal form) are desired.
*/

	    if (*icompq == 2) {
		dlaed1_(&matsiz, &d__[submat], &q[submat + submat * q_dim1],
			ldq, &iwork[indxq + submat], &e[submat + msd2 - 1], &
			msd2, &work[1], &iwork[subpbs + 1], info);
	    } else {
		dlaed7_(icompq, &matsiz, qsiz, &tlvls, &curlvl, &curprb, &d__[
			submat], &qstore[submat * qstore_dim1 + 1], ldqs, &
			iwork[indxq + submat], &e[submat + msd2 - 1], &msd2, &
			work[iq], &iwork[iqptr], &iwork[iprmpt], &iwork[iperm]
			, &iwork[igivpt], &iwork[igivcl], &work[igivnm], &
			work[iwrem], &iwork[subpbs + 1], info);
	    }
	    if (*info != 0) {
		goto L130;
	    }
	    iwork[i__ / 2 + 1] = iwork[i__ + 2];
/* L90: */
	}
	subpbs /= 2;
	++curlvl;
	goto L80;
    }

/*
       end while

       Re-merge the eigenvalues/vectors which were deflated at the final
       merge step.
*/

    if (*icompq == 1) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
	    dcopy_(qsiz, &qstore[j * qstore_dim1 + 1], &c__1, &q[i__ * q_dim1
		    + 1], &c__1);
/* L100: */
	}
	dcopy_(n, &work[1], &c__1, &d__[1], &c__1);
    } else if (*icompq == 2) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
	    dcopy_(n, &q[j * q_dim1 + 1], &c__1, &work[*n * i__ + 1], &c__1);
/* L110: */
	}
	dcopy_(n, &work[1], &c__1, &d__[1], &c__1);
	dlacpy_("A", n, n, &work[*n + 1], n, &q[q_offset], ldq);
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
/* L120: */
	}
	dcopy_(n, &work[1], &c__1, &d__[1], &c__1);
    }
    goto L140;

L130:
    *info = submat * (*n + 1) + submat + matsiz - 1;

L140:
    return 0;

/*     End of DLAED0 */

} /* dlaed0_ */

/* Subroutine */ int dlaed1_(integer *n, doublereal *d__, doublereal *q,
	integer *ldq, integer *indxq, doublereal *rho, integer *cutpnt,
	doublereal *work, integer *iwork, integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;

    /* Local variables */
    static integer i__, k, n1, n2, is, iw, iz, iq2, zpp1, indx, indxc;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer indxp;
    extern /* Subroutine */ int dlaed2_(integer *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *, integer *, integer *, integer *), dlaed3_(integer *,
	    integer *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, doublereal *, doublereal *, integer *, integer *,
	    doublereal *, doublereal *, integer *);
    static integer idlmda;
    extern /* Subroutine */ int dlamrg_(integer *, integer *, doublereal *,
	    integer *, integer *, integer *), xerbla_(char *, integer *);
    static integer coltyp;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLAED1 computes the updated eigensystem of a diagonal
    matrix after modification by a rank-one symmetric matrix.  This
    routine is used only for the eigenproblem which requires all
    eigenvalues and eigenvectors of a tridiagonal matrix.  DLAED7 handles
    the case in which eigenvalues only or eigenvalues and eigenvectors
    of a full symmetric matrix (which was reduced to tridiagonal form)
    are desired.

      T = Q(in) ( D(in) + RHO * Z*Z' ) Q'(in) = Q(out) * D(out) * Q'(out)

       where Z = Q'u, u is a vector of length N with ones in the
       CUTPNT and CUTPNT + 1 th elements and zeros elsewhere.

       The eigenvectors of the original matrix are stored in Q, and the
       eigenvalues are in D.  The algorithm consists of three stages:

          The first stage consists of deflating the size of the problem
          when there are multiple eigenvalues or if there is a zero in
          the Z vector.  For each such occurence the dimension of the
          secular equation problem is reduced by one.  This stage is
          performed by the routine DLAED2.

          The second stage consists of calculating the updated
          eigenvalues. This is done by finding the roots of the secular
          equation via the routine DLAED4 (as called by DLAED3).
          This routine also calculates the eigenvectors of the current
          problem.

          The final stage consists of computing the updated eigenvectors
          directly using the updated eigenvalues.  The eigenvectors for
          the current problem are multiplied with the eigenvectors from
          the overall problem.

    Arguments
    =========

    N      (input) INTEGER
           The dimension of the symmetric tridiagonal matrix.  N >= 0.

    D      (input/output) DOUBLE PRECISION array, dimension (N)
           On entry, the eigenvalues of the rank-1-perturbed matrix.
           On exit, the eigenvalues of the repaired matrix.

    Q      (input/output) DOUBLE PRECISION array, dimension (LDQ,N)
           On entry, the eigenvectors of the rank-1-perturbed matrix.
           On exit, the eigenvectors of the repaired tridiagonal matrix.

    LDQ    (input) INTEGER
           The leading dimension of the array Q.  LDQ >= max(1,N).

    INDXQ  (input/output) INTEGER array, dimension (N)
           On entry, the permutation which separately sorts the two
           subproblems in D into ascending order.
           On exit, the permutation which will reintegrate the
           subproblems back into sorted order,
           i.e. D( INDXQ( I = 1, N ) ) will be in ascending order.

    RHO    (input) DOUBLE PRECISION
           The subdiagonal entry used to create the rank-1 modification.

    CUTPNT (input) INTEGER
           The location of the last eigenvalue in the leading sub-matrix.
           min(1,N) <= CUTPNT <= N/2.

    WORK   (workspace) DOUBLE PRECISION array, dimension (4*N + N**2)

    IWORK  (workspace) INTEGER array, dimension (4*N)

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an eigenvalue did not converge

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA
    Modified by Francoise Tisseur, University of Tennessee.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    } else if (*ldq < max(1,*n)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MIN */
	i__1 = 1, i__2 = *n / 2;
	if (min(i__1,i__2) > *cutpnt || *n / 2 < *cutpnt) {
	    *info = -7;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED1", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*
       The following values are integer pointers which indicate
       the portion of the workspace
       used by a particular array in DLAED2 and DLAED3.
*/

    iz = 1;
    idlmda = iz + *n;
    iw = idlmda + *n;
    iq2 = iw + *n;

    indx = 1;
    indxc = indx + *n;
    coltyp = indxc + *n;
    indxp = coltyp + *n;


/*
       Form the z-vector which consists of the last row of Q_1 and the
       first row of Q_2.
*/

    dcopy_(cutpnt, &q[*cutpnt + q_dim1], ldq, &work[iz], &c__1);
    zpp1 = *cutpnt + 1;
    i__1 = *n - *cutpnt;
    dcopy_(&i__1, &q[zpp1 + zpp1 * q_dim1], ldq, &work[iz + *cutpnt], &c__1);

/*     Deflate eigenvalues. */

    dlaed2_(&k, n, cutpnt, &d__[1], &q[q_offset], ldq, &indxq[1], rho, &work[
	    iz], &work[idlmda], &work[iw], &work[iq2], &iwork[indx], &iwork[
	    indxc], &iwork[indxp], &iwork[coltyp], info);

    if (*info != 0) {
	goto L20;
    }

/*     Solve Secular Equation. */

    if (k != 0) {
	is = (iwork[coltyp] + iwork[coltyp + 1]) * *cutpnt + (iwork[coltyp +
		1] + iwork[coltyp + 2]) * (*n - *cutpnt) + iq2;
	dlaed3_(&k, n, cutpnt, &d__[1], &q[q_offset], ldq, rho, &work[idlmda],
		 &work[iq2], &iwork[indxc], &iwork[coltyp], &work[iw], &work[
		is], info);
	if (*info != 0) {
	    goto L20;
	}

/*     Prepare the INDXQ sorting permutation. */

	n1 = k;
	n2 = *n - k;
	dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &indxq[1]);
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    indxq[i__] = i__;
/* L10: */
	}
    }

L20:
    return 0;

/*     End of DLAED1 */

} /* dlaed1_ */

/* Subroutine */ int dlaed2_(integer *k, integer *n, integer *n1, doublereal *
	d__, doublereal *q, integer *ldq, integer *indxq, doublereal *rho,
	doublereal *z__, doublereal *dlamda, doublereal *w, doublereal *q2,
	integer *indx, integer *indxc, integer *indxp, integer *coltyp,
	integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal c__;
    static integer i__, j;
    static doublereal s, t;
    static integer k2, n2, ct, nj, pj, js, iq1, iq2, n1p1;
    static doublereal eps, tau, tol;
    static integer psm[4], imax, jmax;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    static integer ctot[4];
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *), dcopy_(integer *, doublereal *, integer *, doublereal
	    *, integer *);

    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dlamrg_(integer *, integer *, doublereal *,
	    integer *, integer *, integer *), dlacpy_(char *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DLAED2 merges the two sets of eigenvalues together into a single
    sorted set.  Then it tries to deflate the size of the problem.
    There are two ways in which deflation can occur:  when two or more
    eigenvalues are close together or if there is a tiny entry in the
    Z vector.  For each such occurrence the order of the related secular
    equation problem is reduced by one.

    Arguments
    =========

    K      (output) INTEGER
           The number of non-deflated eigenvalues, and the order of the
           related secular equation. 0 <= K <=N.

    N      (input) INTEGER
           The dimension of the symmetric tridiagonal matrix.  N >= 0.

    N1     (input) INTEGER
           The location of the last eigenvalue in the leading sub-matrix.
           min(1,N) <= N1 <= N/2.

    D      (input/output) DOUBLE PRECISION array, dimension (N)
           On entry, D contains the eigenvalues of the two submatrices to
           be combined.
           On exit, D contains the trailing (N-K) updated eigenvalues
           (those which were deflated) sorted into increasing order.

    Q      (input/output) DOUBLE PRECISION array, dimension (LDQ, N)
           On entry, Q contains the eigenvectors of two submatrices in
           the two square blocks with corners at (1,1), (N1,N1)
           and (N1+1, N1+1), (N,N).
           On exit, Q contains the trailing (N-K) updated eigenvectors
           (those which were deflated) in its last N-K columns.

    LDQ    (input) INTEGER
           The leading dimension of the array Q.  LDQ >= max(1,N).

    INDXQ  (input/output) INTEGER array, dimension (N)
           The permutation which separately sorts the two sub-problems
           in D into ascending order.  Note that elements in the second
           half of this permutation must first have N1 added to their
           values. Destroyed on exit.

    RHO    (input/output) DOUBLE PRECISION
           On entry, the off-diagonal element associated with the rank-1
           cut which originally split the two submatrices which are now
           being recombined.
           On exit, RHO has been modified to the value required by
           DLAED3.

    Z      (input) DOUBLE PRECISION array, dimension (N)
           On entry, Z contains the updating vector (the last
           row of the first sub-eigenvector matrix and the first row of
           the second sub-eigenvector matrix).
           On exit, the contents of Z have been destroyed by the updating
           process.

    DLAMDA (output) DOUBLE PRECISION array, dimension (N)
           A copy of the first K eigenvalues which will be used by
           DLAED3 to form the secular equation.

    W      (output) DOUBLE PRECISION array, dimension (N)
           The first k values of the final deflation-altered z-vector
           which will be passed to DLAED3.

    Q2     (output) DOUBLE PRECISION array, dimension (N1**2+(N-N1)**2)
           A copy of the first K eigenvectors which will be used by
           DLAED3 in a matrix multiply (DGEMM) to solve for the new
           eigenvectors.

    INDX   (workspace) INTEGER array, dimension (N)
           The permutation used to sort the contents of DLAMDA into
           ascending order.

    INDXC  (output) INTEGER array, dimension (N)
           The permutation used to arrange the columns of the deflated
           Q matrix into three groups:  the first group contains non-zero
           elements only at and above N1, the second contains
           non-zero elements only below N1, and the third is dense.

    INDXP  (workspace) INTEGER array, dimension (N)
           The permutation used to place deflated values of D at the end
           of the array.  INDXP(1:K) points to the nondeflated D-values
           and INDXP(K+1:N) points to the deflated eigenvalues.

    COLTYP (workspace/output) INTEGER array, dimension (N)
           During execution, a label which will indicate which of the
           following types a column in the Q2 matrix is:
           1 : non-zero in the upper half only;
           2 : dense;
           3 : non-zero in the lower half only;
           4 : deflated.
           On exit, COLTYP(i) is the number of columns of type i,
           for i=1 to 4 only.

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA
    Modified by Francoise Tisseur, University of Tennessee.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --z__;
    --dlamda;
    --w;
    --q2;
    --indx;
    --indxc;
    --indxp;
    --coltyp;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -2;
    } else if (*ldq < max(1,*n)) {
	*info = -6;
    } else /* if(complicated condition) */ {
/* Computing MIN */
	i__1 = 1, i__2 = *n / 2;
	if (min(i__1,i__2) > *n1 || *n / 2 < *n1) {
	    *info = -3;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    n2 = *n - *n1;
    n1p1 = *n1 + 1;

    if (*rho < 0.) {
	dscal_(&n2, &c_b151, &z__[n1p1], &c__1);
    }

/*
       Normalize z so that norm(z) = 1.  Since z is the concatenation of
       two normalized vectors, norm2(z) = sqrt(2).
*/

    t = 1. / sqrt(2.);
    dscal_(n, &t, &z__[1], &c__1);

/*     RHO = ABS( norm(z)**2 * RHO ) */

    *rho = (d__1 = *rho * 2., abs(d__1));

/*     Sort the eigenvalues into increasing order */

    i__1 = *n;
    for (i__ = n1p1; i__ <= i__1; ++i__) {
	indxq[i__] += *n1;
/* L10: */
    }

/*     re-integrate the deflated parts from the last pass */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = d__[indxq[i__]];
/* L20: */
    }
    dlamrg_(n1, &n2, &dlamda[1], &c__1, &c__1, &indxc[1]);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	indx[i__] = indxq[indxc[i__]];
/* L30: */
    }

/*     Calculate the allowable deflation tolerance */

    imax = idamax_(n, &z__[1], &c__1);
    jmax = idamax_(n, &d__[1], &c__1);
    eps = EPSILON;
/* Computing MAX */
    d__3 = (d__1 = d__[jmax], abs(d__1)), d__4 = (d__2 = z__[imax], abs(d__2))
	    ;
    tol = eps * 8. * max(d__3,d__4);

/*
       If the rank-1 modifier is small enough, no more needs to be done
       except to reorganize Q so that its columns correspond with the
       elements in D.
*/

    if (*rho * (d__1 = z__[imax], abs(d__1)) <= tol) {
	*k = 0;
	iq2 = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__ = indx[j];
	    dcopy_(n, &q[i__ * q_dim1 + 1], &c__1, &q2[iq2], &c__1);
	    dlamda[j] = d__[i__];
	    iq2 += *n;
/* L40: */
	}
	dlacpy_("A", n, n, &q2[1], n, &q[q_offset], ldq);
	dcopy_(n, &dlamda[1], &c__1, &d__[1], &c__1);
	goto L190;
    }

/*
       If there are multiple eigenvalues then the problem deflates.  Here
       the number of equal eigenvalues are found.  As each equal
       eigenvalue is found, an elementary reflector is computed to rotate
       the corresponding eigensubspace so that the corresponding
       components of Z are zero in this new basis.
*/

    i__1 = *n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	coltyp[i__] = 1;
/* L50: */
    }
    i__1 = *n;
    for (i__ = n1p1; i__ <= i__1; ++i__) {
	coltyp[i__] = 3;
/* L60: */
    }


    *k = 0;
    k2 = *n + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	nj = indx[j];
	if (*rho * (d__1 = z__[nj], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    coltyp[nj] = 4;
	    indxp[k2] = nj;
	    if (j == *n) {
		goto L100;
	    }
	} else {
	    pj = nj;
	    goto L80;
	}
/* L70: */
    }
L80:
    ++j;
    nj = indx[j];
    if (j > *n) {
	goto L100;
    }
    if (*rho * (d__1 = z__[nj], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	coltyp[nj] = 4;
	indxp[k2] = nj;
    } else {

/*        Check if eigenvalues are close enough to allow deflation. */

	s = z__[pj];
	c__ = z__[nj];

/*
          Find sqrt(a**2+b**2) without overflow or
          destructive underflow.
*/

	tau = dlapy2_(&c__, &s);
	t = d__[nj] - d__[pj];
	c__ /= tau;
	s = -s / tau;
	if ((d__1 = t * c__ * s, abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    z__[nj] = tau;
	    z__[pj] = 0.;
	    if (coltyp[nj] != coltyp[pj]) {
		coltyp[nj] = 2;
	    }
	    coltyp[pj] = 4;
	    drot_(n, &q[pj * q_dim1 + 1], &c__1, &q[nj * q_dim1 + 1], &c__1, &
		    c__, &s);
/* Computing 2nd power */
	    d__1 = c__;
/* Computing 2nd power */
	    d__2 = s;
	    t = d__[pj] * (d__1 * d__1) + d__[nj] * (d__2 * d__2);
/* Computing 2nd power */
	    d__1 = s;
/* Computing 2nd power */
	    d__2 = c__;
	    d__[nj] = d__[pj] * (d__1 * d__1) + d__[nj] * (d__2 * d__2);
	    d__[pj] = t;
	    --k2;
	    i__ = 1;
L90:
	    if (k2 + i__ <= *n) {
		if (d__[pj] < d__[indxp[k2 + i__]]) {
		    indxp[k2 + i__ - 1] = indxp[k2 + i__];
		    indxp[k2 + i__] = pj;
		    ++i__;
		    goto L90;
		} else {
		    indxp[k2 + i__ - 1] = pj;
		}
	    } else {
		indxp[k2 + i__ - 1] = pj;
	    }
	    pj = nj;
	} else {
	    ++(*k);
	    dlamda[*k] = d__[pj];
	    w[*k] = z__[pj];
	    indxp[*k] = pj;
	    pj = nj;
	}
    }
    goto L80;
L100:

/*     Record the last eigenvalue. */

    ++(*k);
    dlamda[*k] = d__[pj];
    w[*k] = z__[pj];
    indxp[*k] = pj;

/*
       Count up the total number of the various types of columns, then
       form a permutation which positions the four column types into
       four uniform groups (although one or more of these groups may be
       empty).
*/

    for (j = 1; j <= 4; ++j) {
	ctot[j - 1] = 0;
/* L110: */
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	ct = coltyp[j];
	++ctot[ct - 1];
/* L120: */
    }

/*     PSM(*) = Position in SubMatrix (of types 1 through 4) */

    psm[0] = 1;
    psm[1] = ctot[0] + 1;
    psm[2] = psm[1] + ctot[1];
    psm[3] = psm[2] + ctot[2];
    *k = *n - ctot[3];

/*
       Fill out the INDXC array so that the permutation which it induces
       will place all type-1 columns first, all type-2 columns next,
       then all type-3's, and finally all type-4's.
*/

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	js = indxp[j];
	ct = coltyp[js];
	indx[psm[ct - 1]] = js;
	indxc[psm[ct - 1]] = j;
	++psm[ct - 1];
/* L130: */
    }

/*
       Sort the eigenvalues and corresponding eigenvectors into DLAMDA
       and Q2 respectively.  The eigenvalues/vectors which were not
       deflated go into the first K slots of DLAMDA and Q2 respectively,
       while those which were deflated go into the last N - K slots.
*/

    i__ = 1;
    iq1 = 1;
    iq2 = (ctot[0] + ctot[1]) * *n1 + 1;
    i__1 = ctot[0];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(n1, &q[js * q_dim1 + 1], &c__1, &q2[iq1], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq1 += *n1;
/* L140: */
    }

    i__1 = ctot[1];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(n1, &q[js * q_dim1 + 1], &c__1, &q2[iq1], &c__1);
	dcopy_(&n2, &q[*n1 + 1 + js * q_dim1], &c__1, &q2[iq2], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq1 += *n1;
	iq2 += n2;
/* L150: */
    }

    i__1 = ctot[2];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(&n2, &q[*n1 + 1 + js * q_dim1], &c__1, &q2[iq2], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq2 += n2;
/* L160: */
    }

    iq1 = iq2;
    i__1 = ctot[3];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(n, &q[js * q_dim1 + 1], &c__1, &q2[iq2], &c__1);
	iq2 += *n;
	z__[i__] = d__[js];
	++i__;
/* L170: */
    }

/*
       The deflated eigenvalues and their corresponding vectors go back
       into the last N - K slots of D and Q respectively.
*/

    dlacpy_("A", n, &ctot[3], &q2[iq1], n, &q[(*k + 1) * q_dim1 + 1], ldq);
    i__1 = *n - *k;
    dcopy_(&i__1, &z__[*k + 1], &c__1, &d__[*k + 1], &c__1);

/*     Copy CTOT into COLTYP for referencing in DLAED3. */

    for (j = 1; j <= 4; ++j) {
	coltyp[j] = ctot[j - 1];
/* L180: */
    }

L190:
    return 0;

/*     End of DLAED2 */

} /* dlaed2_ */

/* Subroutine */ int dlaed3_(integer *k, integer *n, integer *n1, doublereal *
	d__, doublereal *q, integer *ldq, doublereal *rho, doublereal *dlamda,
	 doublereal *q2, integer *indx, integer *ctot, doublereal *w,
	doublereal *s, integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer i__, j, n2, n12, ii, n23, iq2;
    static doublereal temp;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer
	    *), dlaed4_(integer *, integer *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, integer *);
    extern doublereal dlamc3_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *),
	    dlaset_(char *, integer *, integer *, doublereal *, doublereal *,
	    doublereal *, integer *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       June 30, 1999


    Purpose
    =======

    DLAED3 finds the roots of the secular equation, as defined by the
    values in D, W, and RHO, between 1 and K.  It makes the
    appropriate calls to DLAED4 and then updates the eigenvectors by
    multiplying the matrix of eigenvectors of the pair of eigensystems
    being combined by the matrix of eigenvectors of the K-by-K system
    which is solved here.

    This code makes very mild assumptions about floating point
    arithmetic. It will work on machines with a guard digit in
    add/subtract, or on those binary machines without guard digits
    which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or Cray-2.
    It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.

    Arguments
    =========

    K       (input) INTEGER
            The number of terms in the rational function to be solved by
            DLAED4.  K >= 0.

    N       (input) INTEGER
            The number of rows and columns in the Q matrix.
            N >= K (deflation may result in N>K).

    N1      (input) INTEGER
            The location of the last eigenvalue in the leading submatrix.
            min(1,N) <= N1 <= N/2.

    D       (output) DOUBLE PRECISION array, dimension (N)
            D(I) contains the updated eigenvalues for
            1 <= I <= K.

    Q       (output) DOUBLE PRECISION array, dimension (LDQ,N)
            Initially the first K columns are used as workspace.
            On output the columns 1 to K contain
            the updated eigenvectors.

    LDQ     (input) INTEGER
            The leading dimension of the array Q.  LDQ >= max(1,N).

    RHO     (input) DOUBLE PRECISION
            The value of the parameter in the rank one update equation.
            RHO >= 0 required.

    DLAMDA  (input/output) DOUBLE PRECISION array, dimension (K)
            The first K elements of this array contain the old roots
            of the deflated updating problem.  These are the poles
            of the secular equation. May be changed on output by
            having lowest order bit set to zero on Cray X-MP, Cray Y-MP,
            Cray-2, or Cray C-90, as described above.

    Q2      (input) DOUBLE PRECISION array, dimension (LDQ2, N)
            The first K columns of this matrix contain the non-deflated
            eigenvectors for the split problem.

    INDX    (input) INTEGER array, dimension (N)
            The permutation used to arrange the columns of the deflated
            Q matrix into three groups (see DLAED2).
            The rows of the eigenvectors found by DLAED4 must be likewise
            permuted before the matrix multiply can take place.

    CTOT    (input) INTEGER array, dimension (4)
            A count of the total number of the various types of columns
            in Q, as described in INDX.  The fourth column type is any
            column which has been deflated.

    W       (input/output) DOUBLE PRECISION array, dimension (K)
            The first K elements of this array contain the components
            of the deflation-adjusted updating vector. Destroyed on
            output.

    S       (workspace) DOUBLE PRECISION array, dimension (N1 + 1)*K
            Will contain the eigenvectors of the repaired matrix which
            will be multiplied by the previously accumulated eigenvectors
            to update the system.

    LDS     (input) INTEGER
            The leading dimension of S.  LDS >= max(1,K).

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an eigenvalue did not converge

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA
    Modified by Francoise Tisseur, University of Tennessee.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --dlamda;
    --q2;
    --indx;
    --ctot;
    --w;
    --s;

    /* Function Body */
    *info = 0;

    if (*k < 0) {
	*info = -1;
    } else if (*n < *k) {
	*info = -2;
    } else if (*ldq < max(1,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED3", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 0) {
	return 0;
    }

/*
       Modify values DLAMDA(i) to make sure all DLAMDA(i)-DLAMDA(j) can
       be computed with high relative accuracy (barring over/underflow).
       This is a problem on machines without a guard digit in
       add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2).
       The following code replaces DLAMDA(I) by 2*DLAMDA(I)-DLAMDA(I),
       which on any of these machines zeros out the bottommost
       bit of DLAMDA(I) if it is 1; this makes the subsequent
       subtractions DLAMDA(I)-DLAMDA(J) unproblematic when cancellation
       occurs. On binary machines with a guard digit (almost all
       machines) it does not change DLAMDA(I) at all. On hexadecimal
       and decimal machines with a guard digit, it slightly
       changes the bottommost bits of DLAMDA(I). It does not account
       for hexadecimal or decimal machines without guard digits
       (we know of none). We use a subroutine call to compute
       2*DLAMBDA(I) to prevent optimizing compilers from eliminating
       this code.
*/

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = dlamc3_(&dlamda[i__], &dlamda[i__]) - dlamda[i__];
/* L10: */
    }

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	dlaed4_(k, &j, &dlamda[1], &w[1], &q[j * q_dim1 + 1], rho, &d__[j],
		info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    goto L120;
	}
/* L20: */
    }

    if (*k == 1) {
	goto L110;
    }
    if (*k == 2) {
	i__1 = *k;
	for (j = 1; j <= i__1; ++j) {
	    w[1] = q[j * q_dim1 + 1];
	    w[2] = q[j * q_dim1 + 2];
	    ii = indx[1];
	    q[j * q_dim1 + 1] = w[ii];
	    ii = indx[2];
	    q[j * q_dim1 + 2] = w[ii];
/* L30: */
	}
	goto L110;
    }

/*     Compute updated W. */

    dcopy_(k, &w[1], &c__1, &s[1], &c__1);

/*     Initialize W(I) = Q(I,I) */

    i__1 = *ldq + 1;
    dcopy_(k, &q[q_offset], &i__1, &w[1], &c__1);
    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L40: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L50: */
	}
/* L60: */
    }
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = sqrt(-w[i__]);
	w[i__] = d_sign(&d__1, &s[i__]);
/* L70: */
    }

/*     Compute eigenvectors of the modified rank-1 modification. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    s[i__] = w[i__] / q[i__ + j * q_dim1];
/* L80: */
	}
	temp = dnrm2_(k, &s[1], &c__1);
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    ii = indx[i__];
	    q[i__ + j * q_dim1] = s[ii] / temp;
/* L90: */
	}
/* L100: */
    }

/*     Compute the updated eigenvectors. */

L110:

    n2 = *n - *n1;
    n12 = ctot[1] + ctot[2];
    n23 = ctot[2] + ctot[3];

    dlacpy_("A", &n23, k, &q[ctot[1] + 1 + q_dim1], ldq, &s[1], &n23);
    iq2 = *n1 * n12 + 1;
    if (n23 != 0) {
	dgemm_("N", "N", &n2, k, &n23, &c_b15, &q2[iq2], &n2, &s[1], &n23, &
		c_b29, &q[*n1 + 1 + q_dim1], ldq);
    } else {
	dlaset_("A", &n2, k, &c_b29, &c_b29, &q[*n1 + 1 + q_dim1], ldq);
    }

    dlacpy_("A", &n12, k, &q[q_offset], ldq, &s[1], &n12);
    if (n12 != 0) {
	dgemm_("N", "N", n1, k, &n12, &c_b15, &q2[1], n1, &s[1], &n12, &c_b29,
		 &q[q_offset], ldq);
    } else {
	dlaset_("A", n1, k, &c_b29, &c_b29, &q[q_dim1 + 1], ldq);
    }


L120:
    return 0;

/*     End of DLAED3 */

} /* dlaed3_ */

/* Subroutine */ int dlaed4_(integer *n, integer *i__, doublereal *d__,
	doublereal *z__, doublereal *delta, doublereal *rho, doublereal *dlam,
	 integer *info)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal a, b, c__;
    static integer j;
    static doublereal w;
    static integer ii;
    static doublereal dw, zz[3];
    static integer ip1;
    static doublereal del, eta, phi, eps, tau, psi;
    static integer iim1, iip1;
    static doublereal dphi, dpsi;
    static integer iter;
    static doublereal temp, prew, temp1, dltlb, dltub, midpt;
    static integer niter;
    static logical swtch;
    extern /* Subroutine */ int dlaed5_(integer *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *), dlaed6_(integer *,
	    logical *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, integer *);
    static logical swtch3;

    static logical orgati;
    static doublereal erretm, rhoinv;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       December 23, 1999


    Purpose
    =======

    This subroutine computes the I-th updated eigenvalue of a symmetric
    rank-one modification to a diagonal matrix whose elements are
    given in the array d, and that

               D(i) < D(j)  for  i < j

    and that RHO > 0.  This is arranged by the calling routine, and is
    no loss in generality.  The rank-one modified system is thus

               diag( D )  +  RHO *  Z * Z_transpose.

    where we assume the Euclidean norm of Z is 1.

    The method consists of approximating the rational functions in the
    secular equation by simpler interpolating rational functions.

    Arguments
    =========

    N      (input) INTEGER
           The length of all arrays.

    I      (input) INTEGER
           The index of the eigenvalue to be computed.  1 <= I <= N.

    D      (input) DOUBLE PRECISION array, dimension (N)
           The original eigenvalues.  It is assumed that they are in
           order, D(I) < D(J)  for I < J.

    Z      (input) DOUBLE PRECISION array, dimension (N)
           The components of the updating vector.

    DELTA  (output) DOUBLE PRECISION array, dimension (N)
           If N .ne. 1, DELTA contains (D(j) - lambda_I) in its  j-th
           component.  If N = 1, then DELTA(1) = 1.  The vector DELTA
           contains the information necessary to construct the
           eigenvectors.

    RHO    (input) DOUBLE PRECISION
           The scalar in the symmetric updating formula.

    DLAM   (output) DOUBLE PRECISION
           The computed lambda_I, the I-th updated eigenvalue.

    INFO   (output) INTEGER
           = 0:  successful exit
           > 0:  if INFO = 1, the updating process failed.

    Internal Parameters
    ===================

    Logical variable ORGATI (origin-at-i?) is used for distinguishing
    whether D(i) or D(i+1) is treated as the origin.

              ORGATI = .true.    origin at i
              ORGATI = .false.   origin at i+1

     Logical variable SWTCH3 (switch-for-3-poles?) is for noting
     if we are working with THREE poles!

     MAXIT is the maximum number of iterations allowed for each
     eigenvalue.

    Further Details
    ===============

    Based on contributions by
       Ren-Cang Li, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================


       Since this routine is called in an inner loop, we do no argument
       checking.

       Quick return for N=1 and 2.
*/

    /* Parameter adjustments */
    --delta;
    --z__;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n == 1) {

/*         Presumably, I=1 upon entry */

	*dlam = d__[1] + *rho * z__[1] * z__[1];
	delta[1] = 1.;
	return 0;
    }
    if (*n == 2) {
	dlaed5_(i__, &d__[1], &z__[1], &delta[1], rho, dlam);
	return 0;
    }

/*     Compute machine epsilon */

    eps = EPSILON;
    rhoinv = 1. / *rho;

/*     The case I = N */

    if (*i__ == *n) {

/*        Initialize some basic variables */

	ii = *n - 1;
	niter = 1;

/*        Calculate initial guess */

	midpt = *rho / 2.;

/*
          If ||Z||_2 is not one, then TEMP should be set to
          RHO * ||Z||_2^2 / TWO
*/

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - midpt;
/* L10: */
	}

	psi = 0.;
	i__1 = *n - 2;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / delta[j];
/* L20: */
	}

	c__ = rhoinv + psi;
	w = c__ + z__[ii] * z__[ii] / delta[ii] + z__[*n] * z__[*n] / delta[*
		n];

	if (w <= 0.) {
	    temp = z__[*n - 1] * z__[*n - 1] / (d__[*n] - d__[*n - 1] + *rho)
		    + z__[*n] * z__[*n] / *rho;
	    if (c__ <= temp) {
		tau = *rho;
	    } else {
		del = d__[*n] - d__[*n - 1];
		a = -c__ * del + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n]
			;
		b = z__[*n] * z__[*n] * del;
		if (a < 0.) {
		    tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
		} else {
		    tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
		}
	    }

/*
             It can be proved that
                 D(N)+RHO/2 <= LAMBDA(N) < D(N)+TAU <= D(N)+RHO
*/

	    dltlb = midpt;
	    dltub = *rho;
	} else {
	    del = d__[*n] - d__[*n - 1];
	    a = -c__ * del + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n];
	    b = z__[*n] * z__[*n] * del;
	    if (a < 0.) {
		tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
	    } else {
		tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
	    }

/*
             It can be proved that
                 D(N) < D(N)+TAU < LAMBDA(N) < D(N)+RHO/2
*/

	    dltlb = 0.;
	    dltub = midpt;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - tau;
/* L30: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L40: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / delta[*n];
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    *dlam = d__[*i__] + tau;
	    goto L250;
	}

	if (w <= 0.) {
	    dltlb = max(dltlb,tau);
	} else {
	    dltub = min(dltub,tau);
	}

/*        Calculate the new step */

	++niter;
	c__ = w - delta[*n - 1] * dpsi - delta[*n] * dphi;
	a = (delta[*n - 1] + delta[*n]) * w - delta[*n - 1] * delta[*n] * (
		dpsi + dphi);
	b = delta[*n - 1] * delta[*n] * w;
	if (c__ < 0.) {
	    c__ = abs(c__);
	}
	if (c__ == 0.) {
/*
            ETA = B/A
             ETA = RHO - TAU
*/
	    eta = dltub - tau;
	} else if (a >= 0.) {
	    eta = (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (c__
		    * 2.);
	} else {
	    eta = b * 2. / (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))
		    );
	}

/*
          Note, eta should be positive if w is negative, and
          eta should be negative otherwise. However,
          if for some reason caused by roundoff, eta*w > 0,
          we simply use one Newton step instead. This way
          will guarantee eta*w < 0.
*/

	if (w * eta > 0.) {
	    eta = -w / (dpsi + dphi);
	}
	temp = tau + eta;
	if (temp > dltub || temp < dltlb) {
	    if (w < 0.) {
		eta = (dltub - tau) / 2.;
	    } else {
		eta = (dltlb - tau) / 2.;
	    }
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
/* L50: */
	}

	tau += eta;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L60: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / delta[*n];
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 30; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		*dlam = d__[*i__] + tau;
		goto L250;
	    }

	    if (w <= 0.) {
		dltlb = max(dltlb,tau);
	    } else {
		dltub = min(dltub,tau);
	    }

/*           Calculate the new step */

	    c__ = w - delta[*n - 1] * dpsi - delta[*n] * dphi;
	    a = (delta[*n - 1] + delta[*n]) * w - delta[*n - 1] * delta[*n] *
		    (dpsi + dphi);
	    b = delta[*n - 1] * delta[*n] * w;
	    if (a >= 0.) {
		eta = (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    } else {
		eta = b * 2. / (a - sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    }

/*
             Note, eta should be positive if w is negative, and
             eta should be negative otherwise. However,
             if for some reason caused by roundoff, eta*w > 0,
             we simply use one Newton step instead. This way
             will guarantee eta*w < 0.
*/

	    if (w * eta > 0.) {
		eta = -w / (dpsi + dphi);
	    }
	    temp = tau + eta;
	    if (temp > dltub || temp < dltlb) {
		if (w < 0.) {
		    eta = (dltub - tau) / 2.;
		} else {
		    eta = (dltlb - tau) / 2.;
		}
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
/* L70: */
	    }

	    tau += eta;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = ii;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / delta[j];
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L80: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    temp = z__[*n] / delta[*n];
	    phi = z__[*n] * temp;
	    dphi = temp * temp;
	    erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (
		    dpsi + dphi);

	    w = rhoinv + phi + psi;
/* L90: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	*dlam = d__[*i__] + tau;
	goto L250;

/*        End for the case I = N */

    } else {

/*        The case for I < N */

	niter = 1;
	ip1 = *i__ + 1;

/*        Calculate initial guess */

	del = d__[ip1] - d__[*i__];
	midpt = del / 2.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - midpt;
/* L100: */
	}

	psi = 0.;
	i__1 = *i__ - 1;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / delta[j];
/* L110: */
	}

	phi = 0.;
	i__1 = *i__ + 2;
	for (j = *n; j >= i__1; --j) {
	    phi += z__[j] * z__[j] / delta[j];
/* L120: */
	}
	c__ = rhoinv + psi + phi;
	w = c__ + z__[*i__] * z__[*i__] / delta[*i__] + z__[ip1] * z__[ip1] /
		delta[ip1];

	if (w > 0.) {

/*
             d(i)< the ith eigenvalue < (d(i)+d(i+1))/2

             We choose d(i) as origin.
*/

	    orgati = TRUE_;
	    a = c__ * del + z__[*i__] * z__[*i__] + z__[ip1] * z__[ip1];
	    b = z__[*i__] * z__[*i__] * del;
	    if (a > 0.) {
		tau = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    }
	    dltlb = 0.;
	    dltub = midpt;
	} else {

/*
             (d(i)+d(i+1))/2 <= the ith eigenvalue < d(i+1)

             We choose d(i+1) as origin.
*/

	    orgati = FALSE_;
	    a = c__ * del - z__[*i__] * z__[*i__] - z__[ip1] * z__[ip1];
	    b = z__[ip1] * z__[ip1] * del;
	    if (a < 0.) {
		tau = b * 2. / (a - sqrt((d__1 = a * a + b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = -(a + sqrt((d__1 = a * a + b * 4. * c__, abs(d__1)))) /
			(c__ * 2.);
	    }
	    dltlb = -midpt;
	    dltub = 0.;
	}

	if (orgati) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] = d__[j] - d__[*i__] - tau;
/* L130: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] = d__[j] - d__[ip1] - tau;
/* L140: */
	    }
	}
	if (orgati) {
	    ii = *i__;
	} else {
	    ii = *i__ + 1;
	}
	iim1 = ii - 1;
	iip1 = ii + 1;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L150: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.;
	phi = 0.;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / delta[j];
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L160: */
	}

	w = rhoinv + phi + psi;

/*
          W is the value of the secular function with
          its ii-th element removed.
*/

	swtch3 = FALSE_;
	if (orgati) {
	    if (w < 0.) {
		swtch3 = TRUE_;
	    }
	} else {
	    if (w > 0.) {
		swtch3 = TRUE_;
	    }
	}
	if (ii == 1 || ii == *n) {
	    swtch3 = FALSE_;
	}

	temp = z__[ii] / delta[ii];
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w += temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. +
		abs(tau) * dw;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    if (orgati) {
		*dlam = d__[*i__] + tau;
	    } else {
		*dlam = d__[ip1] + tau;
	    }
	    goto L250;
	}

	if (w <= 0.) {
	    dltlb = max(dltlb,tau);
	} else {
	    dltub = min(dltub,tau);
	}

/*        Calculate the new step */

	++niter;
	if (! swtch3) {
	    if (orgati) {
/* Computing 2nd power */
		d__1 = z__[*i__] / delta[*i__];
		c__ = w - delta[ip1] * dw - (d__[*i__] - d__[ip1]) * (d__1 *
			d__1);
	    } else {
/* Computing 2nd power */
		d__1 = z__[ip1] / delta[ip1];
		c__ = w - delta[*i__] * dw - (d__[ip1] - d__[*i__]) * (d__1 *
			d__1);
	    }
	    a = (delta[*i__] + delta[ip1]) * w - delta[*i__] * delta[ip1] *
		    dw;
	    b = delta[*i__] * delta[ip1] * w;
	    if (c__ == 0.) {
		if (a == 0.) {
		    if (orgati) {
			a = z__[*i__] * z__[*i__] + delta[ip1] * delta[ip1] *
				(dpsi + dphi);
		    } else {
			a = z__[ip1] * z__[ip1] + delta[*i__] * delta[*i__] *
				(dpsi + dphi);
		    }
		}
		eta = b / a;
	    } else if (a <= 0.) {
		eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    } else {
		eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    }
	} else {

/*           Interpolation using THREE most relevant poles */

	    temp = rhoinv + psi + phi;
	    if (orgati) {
		temp1 = z__[iim1] / delta[iim1];
		temp1 *= temp1;
		c__ = temp - delta[iip1] * (dpsi + dphi) - (d__[iim1] - d__[
			iip1]) * temp1;
		zz[0] = z__[iim1] * z__[iim1];
		zz[2] = delta[iip1] * delta[iip1] * (dpsi - temp1 + dphi);
	    } else {
		temp1 = z__[iip1] / delta[iip1];
		temp1 *= temp1;
		c__ = temp - delta[iim1] * (dpsi + dphi) - (d__[iip1] - d__[
			iim1]) * temp1;
		zz[0] = delta[iim1] * delta[iim1] * (dpsi + (dphi - temp1));
		zz[2] = z__[iip1] * z__[iip1];
	    }
	    zz[1] = z__[ii] * z__[ii];
	    dlaed6_(&niter, &orgati, &c__, &delta[iim1], zz, &w, &eta, info);
	    if (*info != 0) {
		goto L250;
	    }
	}

/*
          Note, eta should be positive if w is negative, and
          eta should be negative otherwise. However,
          if for some reason caused by roundoff, eta*w > 0,
          we simply use one Newton step instead. This way
          will guarantee eta*w < 0.
*/

	if (w * eta >= 0.) {
	    eta = -w / dw;
	}
	temp = tau + eta;
	if (temp > dltub || temp < dltlb) {
	    if (w < 0.) {
		eta = (dltub - tau) / 2.;
	    } else {
		eta = (dltlb - tau) / 2.;
	    }
	}

	prew = w;

/* L170: */
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
/* L180: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L190: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.;
	phi = 0.;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / delta[j];
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L200: */
	}

	temp = z__[ii] / delta[ii];
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w = rhoinv + phi + psi + temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. + (
		d__1 = tau + eta, abs(d__1)) * dw;

	swtch = FALSE_;
	if (orgati) {
	    if (-w > abs(prew) / 10.) {
		swtch = TRUE_;
	    }
	} else {
	    if (w > abs(prew) / 10.) {
		swtch = TRUE_;
	    }
	}

	tau += eta;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 30; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		if (orgati) {
		    *dlam = d__[*i__] + tau;
		} else {
		    *dlam = d__[ip1] + tau;
		}
		goto L250;
	    }

	    if (w <= 0.) {
		dltlb = max(dltlb,tau);
	    } else {
		dltub = min(dltub,tau);
	    }

/*           Calculate the new step */

	    if (! swtch3) {
		if (! swtch) {
		    if (orgati) {
/* Computing 2nd power */
			d__1 = z__[*i__] / delta[*i__];
			c__ = w - delta[ip1] * dw - (d__[*i__] - d__[ip1]) * (
				d__1 * d__1);
		    } else {
/* Computing 2nd power */
			d__1 = z__[ip1] / delta[ip1];
			c__ = w - delta[*i__] * dw - (d__[ip1] - d__[*i__]) *
				(d__1 * d__1);
		    }
		} else {
		    temp = z__[ii] / delta[ii];
		    if (orgati) {
			dpsi += temp * temp;
		    } else {
			dphi += temp * temp;
		    }
		    c__ = w - delta[*i__] * dpsi - delta[ip1] * dphi;
		}
		a = (delta[*i__] + delta[ip1]) * w - delta[*i__] * delta[ip1]
			* dw;
		b = delta[*i__] * delta[ip1] * w;
		if (c__ == 0.) {
		    if (a == 0.) {
			if (! swtch) {
			    if (orgati) {
				a = z__[*i__] * z__[*i__] + delta[ip1] *
					delta[ip1] * (dpsi + dphi);
			    } else {
				a = z__[ip1] * z__[ip1] + delta[*i__] * delta[
					*i__] * (dpsi + dphi);
			    }
			} else {
			    a = delta[*i__] * delta[*i__] * dpsi + delta[ip1]
				    * delta[ip1] * dphi;
			}
		    }
		    eta = b / a;
		} else if (a <= 0.) {
		    eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1))))
			     / (c__ * 2.);
		} else {
		    eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__,
			    abs(d__1))));
		}
	    } else {

/*              Interpolation using THREE most relevant poles */

		temp = rhoinv + psi + phi;
		if (swtch) {
		    c__ = temp - delta[iim1] * dpsi - delta[iip1] * dphi;
		    zz[0] = delta[iim1] * delta[iim1] * dpsi;
		    zz[2] = delta[iip1] * delta[iip1] * dphi;
		} else {
		    if (orgati) {
			temp1 = z__[iim1] / delta[iim1];
			temp1 *= temp1;
			c__ = temp - delta[iip1] * (dpsi + dphi) - (d__[iim1]
				- d__[iip1]) * temp1;
			zz[0] = z__[iim1] * z__[iim1];
			zz[2] = delta[iip1] * delta[iip1] * (dpsi - temp1 +
				dphi);
		    } else {
			temp1 = z__[iip1] / delta[iip1];
			temp1 *= temp1;
			c__ = temp - delta[iim1] * (dpsi + dphi) - (d__[iip1]
				- d__[iim1]) * temp1;
			zz[0] = delta[iim1] * delta[iim1] * (dpsi + (dphi -
				temp1));
			zz[2] = z__[iip1] * z__[iip1];
		    }
		}
		dlaed6_(&niter, &orgati, &c__, &delta[iim1], zz, &w, &eta,
			info);
		if (*info != 0) {
		    goto L250;
		}
	    }

/*
             Note, eta should be positive if w is negative, and
             eta should be negative otherwise. However,
             if for some reason caused by roundoff, eta*w > 0,
             we simply use one Newton step instead. This way
             will guarantee eta*w < 0.
*/

	    if (w * eta >= 0.) {
		eta = -w / dw;
	    }
	    temp = tau + eta;
	    if (temp > dltub || temp < dltlb) {
		if (w < 0.) {
		    eta = (dltub - tau) / 2.;
		} else {
		    eta = (dltlb - tau) / 2.;
		}
	    }

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
/* L210: */
	    }

	    tau += eta;
	    prew = w;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = iim1;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / delta[j];
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L220: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    dphi = 0.;
	    phi = 0.;
	    i__1 = iip1;
	    for (j = *n; j >= i__1; --j) {
		temp = z__[j] / delta[j];
		phi += z__[j] * temp;
		dphi += temp * temp;
		erretm += phi;
/* L230: */
	    }

	    temp = z__[ii] / delta[ii];
	    dw = dpsi + dphi + temp * temp;
	    temp = z__[ii] * temp;
	    w = rhoinv + phi + psi + temp;
	    erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3.
		    + abs(tau) * dw;
	    if ((w * prew > 0. && abs(w) > abs(prew) / 10.)) {
		swtch = ! swtch;
	    }

/* L240: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	if (orgati) {
	    *dlam = d__[*i__] + tau;
	} else {
	    *dlam = d__[ip1] + tau;
	}

    }

L250:

    return 0;

/*     End of DLAED4 */

} /* dlaed4_ */

/* Subroutine */ int dlaed5_(integer *i__, doublereal *d__, doublereal *z__,
	doublereal *delta, doublereal *rho, doublereal *dlam)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal b, c__, w, del, tau, temp;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       September 30, 1994


    Purpose
    =======

    This subroutine computes the I-th eigenvalue of a symmetric rank-one
    modification of a 2-by-2 diagonal matrix

               diag( D )  +  RHO *  Z * transpose(Z) .

    The diagonal elements in the array D are assumed to satisfy

               D(i) < D(j)  for  i < j .

    We also assume RHO > 0 and that the Euclidean norm of the vector
    Z is one.

    Arguments
    =========

    I      (input) INTEGER
           The index of the eigenvalue to be computed.  I = 1 or I = 2.

    D      (input) DOUBLE PRECISION array, dimension (2)
           The original eigenvalues.  We assume D(1) < D(2).

    Z      (input) DOUBLE PRECISION array, dimension (2)
           The components of the updating vector.

    DELTA  (output) DOUBLE PRECISION array, dimension (2)
           The vector DELTA contains the information necessary
           to construct the eigenvectors.

    RHO    (input) DOUBLE PRECISION
           The scalar in the symmetric updating formula.

    DLAM   (output) DOUBLE PRECISION
           The computed lambda_I, the I-th updated eigenvalue.

    Further Details
    ===============

    Based on contributions by
       Ren-Cang Li, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================
*/


    /* Parameter adjustments */
    --delta;
    --z__;
    --d__;

    /* Function Body */
    del = d__[2] - d__[1];
    if (*i__ == 1) {
	w = *rho * 2. * (z__[2] * z__[2] - z__[1] * z__[1]) / del + 1.;
	if (w > 0.) {
	    b = del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[1] * z__[1] * del;

/*           B > ZERO, always */

	    tau = c__ * 2. / (b + sqrt((d__1 = b * b - c__ * 4., abs(d__1))));
	    *dlam = d__[1] + tau;
	    delta[1] = -z__[1] / tau;
	    delta[2] = z__[2] / (del - tau);
	} else {
	    b = -del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[2] * z__[2] * del;
	    if (b > 0.) {
		tau = c__ * -2. / (b + sqrt(b * b + c__ * 4.));
	    } else {
		tau = (b - sqrt(b * b + c__ * 4.)) / 2.;
	    }
	    *dlam = d__[2] + tau;
	    delta[1] = -z__[1] / (del + tau);
	    delta[2] = -z__[2] / tau;
	}
	temp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	delta[1] /= temp;
	delta[2] /= temp;
    } else {

/*     Now I=2 */

	b = -del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	c__ = *rho * z__[2] * z__[2] * del;
	if (b > 0.) {
	    tau = (b + sqrt(b * b + c__ * 4.)) / 2.;
	} else {
	    tau = c__ * 2. / (-b + sqrt(b * b + c__ * 4.));
	}
	*dlam = d__[2] + tau;
	delta[1] = -z__[1] / (del + tau);
	delta[2] = -z__[2] / tau;
	temp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	delta[1] /= temp;
	delta[2] /= temp;
    }
    return 0;

/*     End OF DLAED5 */

} /* dlaed5_ */

/* Subroutine */ int dlaed6_(integer *kniter, logical *orgati, doublereal *
	rho, doublereal *d__, doublereal *z__, doublereal *finit, doublereal *
	tau, integer *info)
{
    /* Initialized data */

    static logical first = TRUE_;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double sqrt(doublereal), log(doublereal), pow_di(doublereal *, integer *);

    /* Local variables */
    static doublereal a, b, c__, f;
    static integer i__;
    static doublereal fc, df, ddf, eta, eps, base;
    static integer iter;
    static doublereal temp, temp1, temp2, temp3, temp4;
    static logical scale;
    static integer niter;
    static doublereal small1, small2, sminv1, sminv2;

    static doublereal dscale[3], sclfac, zscale[3], erretm, sclinv;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       June 30, 1999


    Purpose
    =======

    DLAED6 computes the positive or negative root (closest to the origin)
    of
                     z(1)        z(2)        z(3)
    f(x) =   rho + --------- + ---------- + ---------
                    d(1)-x      d(2)-x      d(3)-x

    It is assumed that

          if ORGATI = .true. the root is between d(2) and d(3);
          otherwise it is between d(1) and d(2)

    This routine will be called by DLAED4 when necessary. In most cases,
    the root sought is the smallest in magnitude, though it might not be
    in some extremely rare situations.

    Arguments
    =========

    KNITER       (input) INTEGER
                 Refer to DLAED4 for its significance.

    ORGATI       (input) LOGICAL
                 If ORGATI is true, the needed root is between d(2) and
                 d(3); otherwise it is between d(1) and d(2).  See
                 DLAED4 for further details.

    RHO          (input) DOUBLE PRECISION
                 Refer to the equation f(x) above.

    D            (input) DOUBLE PRECISION array, dimension (3)
                 D satisfies d(1) < d(2) < d(3).

    Z            (input) DOUBLE PRECISION array, dimension (3)
                 Each of the elements in z must be positive.

    FINIT        (input) DOUBLE PRECISION
                 The value of f at 0. It is more accurate than the one
                 evaluated inside this routine (if someone wants to do
                 so).

    TAU          (output) DOUBLE PRECISION
                 The root of the equation f(x).

    INFO         (output) INTEGER
                 = 0: successful exit
                 > 0: if INFO = 1, failure to converge

    Further Details
    ===============

    Based on contributions by
       Ren-Cang Li, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================
*/

    /* Parameter adjustments */
    --z__;
    --d__;

    /* Function Body */

    *info = 0;

    niter = 1;
    *tau = 0.;
    if (*kniter == 2) {
	if (*orgati) {
	    temp = (d__[3] - d__[2]) / 2.;
	    c__ = *rho + z__[1] / (d__[1] - d__[2] - temp);
	    a = c__ * (d__[2] + d__[3]) + z__[2] + z__[3];
	    b = c__ * d__[2] * d__[3] + z__[2] * d__[3] + z__[3] * d__[2];
	} else {
	    temp = (d__[1] - d__[2]) / 2.;
	    c__ = *rho + z__[3] / (d__[3] - d__[2] - temp);
	    a = c__ * (d__[1] + d__[2]) + z__[1] + z__[2];
	    b = c__ * d__[1] * d__[2] + z__[1] * d__[2] + z__[2] * d__[1];
	}
/* Computing MAX */
	d__1 = abs(a), d__2 = abs(b), d__1 = max(d__1,d__2), d__2 = abs(c__);
	temp = max(d__1,d__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.) {
	    *tau = b / a;
	} else if (a <= 0.) {
	    *tau = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
		    c__ * 2.);
	} else {
	    *tau = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1))
		    ));
	}
	temp = *rho + z__[1] / (d__[1] - *tau) + z__[2] / (d__[2] - *tau) +
		z__[3] / (d__[3] - *tau);
	if (abs(*finit) <= abs(temp)) {
	    *tau = 0.;
	}
    }

/*
       On first call to routine, get machine parameters for
       possible scaling to avoid overflow
*/

    if (first) {
	eps = EPSILON;
	base = BASE;
	i__1 = (integer) (log(SAFEMINIMUM) / log(base) / 3.);
	small1 = pow_di(&base, &i__1);
	sminv1 = 1. / small1;
	small2 = small1 * small1;
	sminv2 = sminv1 * sminv1;
	first = FALSE_;
    }

/*
       Determine if scaling of inputs necessary to avoid overflow
       when computing 1/TEMP**3
*/

    if (*orgati) {
/* Computing MIN */
	d__3 = (d__1 = d__[2] - *tau, abs(d__1)), d__4 = (d__2 = d__[3] - *
		tau, abs(d__2));
	temp = min(d__3,d__4);
    } else {
/* Computing MIN */
	d__3 = (d__1 = d__[1] - *tau, abs(d__1)), d__4 = (d__2 = d__[2] - *
		tau, abs(d__2));
	temp = min(d__3,d__4);
    }
    scale = FALSE_;
    if (temp <= small1) {
	scale = TRUE_;
	if (temp <= small2) {

/*        Scale up by power of radix nearest 1/SAFMIN**(2/3) */

	    sclfac = sminv2;
	    sclinv = small2;
	} else {

/*        Scale up by power of radix nearest 1/SAFMIN**(1/3) */

	    sclfac = sminv1;
	    sclinv = small1;
	}

/*        Scaling up safe because D, Z, TAU scaled elsewhere to be O(1) */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__] * sclfac;
	    zscale[i__ - 1] = z__[i__] * sclfac;
/* L10: */
	}
	*tau *= sclfac;
    } else {

/*        Copy D and Z to DSCALE and ZSCALE */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__];
	    zscale[i__ - 1] = z__[i__];
/* L20: */
	}
    }

    fc = 0.;
    df = 0.;
    ddf = 0.;
    for (i__ = 1; i__ <= 3; ++i__) {
	temp = 1. / (dscale[i__ - 1] - *tau);
	temp1 = zscale[i__ - 1] * temp;
	temp2 = temp1 * temp;
	temp3 = temp2 * temp;
	fc += temp1 / dscale[i__ - 1];
	df += temp2;
	ddf += temp3;
/* L30: */
    }
    f = *finit + *tau * fc;

    if (abs(f) <= 0.) {
	goto L60;
    }

/*
          Iteration begins

       It is not hard to see that

             1) Iterations will go up monotonically
                if FINIT < 0;

             2) Iterations will go down monotonically
                if FINIT > 0.
*/

    iter = niter + 1;

    for (niter = iter; niter <= 20; ++niter) {

	if (*orgati) {
	    temp1 = dscale[1] - *tau;
	    temp2 = dscale[2] - *tau;
	} else {
	    temp1 = dscale[0] - *tau;
	    temp2 = dscale[1] - *tau;
	}
	a = (temp1 + temp2) * f - temp1 * temp2 * df;
	b = temp1 * temp2 * f;
	c__ = f - (temp1 + temp2) * df + temp1 * temp2 * ddf;
/* Computing MAX */
	d__1 = abs(a), d__2 = abs(b), d__1 = max(d__1,d__2), d__2 = abs(c__);
	temp = max(d__1,d__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.) {
	    eta = b / a;
	} else if (a <= 0.) {
	    eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (c__
		    * 2.);
	} else {
	    eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))
		    );
	}
	if (f * eta >= 0.) {
	    eta = -f / df;
	}

	temp = eta + *tau;
	if (*orgati) {
	    if ((eta > 0. && temp >= dscale[2])) {
		eta = (dscale[2] - *tau) / 2.;
	    }
	    if ((eta < 0. && temp <= dscale[1])) {
		eta = (dscale[1] - *tau) / 2.;
	    }
	} else {
	    if ((eta > 0. && temp >= dscale[1])) {
		eta = (dscale[1] - *tau) / 2.;
	    }
	    if ((eta < 0. && temp <= dscale[0])) {
		eta = (dscale[0] - *tau) / 2.;
	    }
	}
	*tau += eta;

	fc = 0.;
	erretm = 0.;
	df = 0.;
	ddf = 0.;
	for (i__ = 1; i__ <= 3; ++i__) {
	    temp = 1. / (dscale[i__ - 1] - *tau);
	    temp1 = zscale[i__ - 1] * temp;
	    temp2 = temp1 * temp;
	    temp3 = temp2 * temp;
	    temp4 = temp1 / dscale[i__ - 1];
	    fc += temp4;
	    erretm += abs(temp4);
	    df += temp2;
	    ddf += temp3;
/* L40: */
	}
	f = *finit + *tau * fc;
	erretm = (abs(*finit) + abs(*tau) * erretm) * 8. + abs(*tau) * df;
	if (abs(f) <= eps * erretm) {
	    goto L60;
	}
/* L50: */
    }
    *info = 1;
L60:

/*     Undo scaling */

    if (scale) {
	*tau *= sclinv;
    }
    return 0;

/*     End of DLAED6 */

} /* dlaed6_ */

/* Subroutine */ int dlaed7_(integer *icompq, integer *n, integer *qsiz,
	integer *tlvls, integer *curlvl, integer *curpbm, doublereal *d__,
	doublereal *q, integer *ldq, integer *indxq, doublereal *rho, integer
	*cutpnt, doublereal *qstore, integer *qptr, integer *prmptr, integer *
	perm, integer *givptr, integer *givcol, doublereal *givnum,
	doublereal *work, integer *iwork, integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__, k, n1, n2, is, iw, iz, iq2, ptr, ldq2, indx, curr;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    static integer indxc, indxp;
    extern /* Subroutine */ int dlaed8_(integer *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *, doublereal *, integer *, integer *, integer *,
	    doublereal *, integer *, integer *, integer *), dlaed9_(integer *,
	     integer *, integer *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     integer *, integer *), dlaeda_(integer *, integer *, integer *,
	    integer *, integer *, integer *, integer *, integer *, doublereal
	    *, doublereal *, integer *, doublereal *, doublereal *, integer *)
	    ;
    static integer idlmda;
    extern /* Subroutine */ int dlamrg_(integer *, integer *, doublereal *,
	    integer *, integer *, integer *), xerbla_(char *, integer *);
    static integer coltyp;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DLAED7 computes the updated eigensystem of a diagonal
    matrix after modification by a rank-one symmetric matrix. This
    routine is used only for the eigenproblem which requires all
    eigenvalues and optionally eigenvectors of a dense symmetric matrix
    that has been reduced to tridiagonal form.  DLAED1 handles
    the case in which all eigenvalues and eigenvectors of a symmetric
    tridiagonal matrix are desired.

      T = Q(in) ( D(in) + RHO * Z*Z' ) Q'(in) = Q(out) * D(out) * Q'(out)

       where Z = Q'u, u is a vector of length N with ones in the
       CUTPNT and CUTPNT + 1 th elements and zeros elsewhere.

       The eigenvectors of the original matrix are stored in Q, and the
       eigenvalues are in D.  The algorithm consists of three stages:

          The first stage consists of deflating the size of the problem
          when there are multiple eigenvalues or if there is a zero in
          the Z vector.  For each such occurence the dimension of the
          secular equation problem is reduced by one.  This stage is
          performed by the routine DLAED8.

          The second stage consists of calculating the updated
          eigenvalues. This is done by finding the roots of the secular
          equation via the routine DLAED4 (as called by DLAED9).
          This routine also calculates the eigenvectors of the current
          problem.

          The final stage consists of computing the updated eigenvectors
          directly using the updated eigenvalues.  The eigenvectors for
          the current problem are multiplied with the eigenvectors from
          the overall problem.

    Arguments
    =========

    ICOMPQ  (input) INTEGER
            = 0:  Compute eigenvalues only.
            = 1:  Compute eigenvectors of original dense symmetric matrix
                  also.  On entry, Q contains the orthogonal matrix used
                  to reduce the original matrix to tridiagonal form.

    N      (input) INTEGER
           The dimension of the symmetric tridiagonal matrix.  N >= 0.

    QSIZ   (input) INTEGER
           The dimension of the orthogonal matrix used to reduce
           the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1.

    TLVLS  (input) INTEGER
           The total number of merging levels in the overall divide and
           conquer tree.

    CURLVL (input) INTEGER
           The current level in the overall merge routine,
           0 <= CURLVL <= TLVLS.

    CURPBM (input) INTEGER
           The current problem in the current level in the overall
           merge routine (counting from upper left to lower right).

    D      (input/output) DOUBLE PRECISION array, dimension (N)
           On entry, the eigenvalues of the rank-1-perturbed matrix.
           On exit, the eigenvalues of the repaired matrix.

    Q      (input/output) DOUBLE PRECISION array, dimension (LDQ, N)
           On entry, the eigenvectors of the rank-1-perturbed matrix.
           On exit, the eigenvectors of the repaired tridiagonal matrix.

    LDQ    (input) INTEGER
           The leading dimension of the array Q.  LDQ >= max(1,N).

    INDXQ  (output) INTEGER array, dimension (N)
           The permutation which will reintegrate the subproblem just
           solved back into sorted order, i.e., D( INDXQ( I = 1, N ) )
           will be in ascending order.

    RHO    (input) DOUBLE PRECISION
           The subdiagonal element used to create the rank-1
           modification.

    CUTPNT (input) INTEGER
           Contains the location of the last eigenvalue in the leading
           sub-matrix.  min(1,N) <= CUTPNT <= N.

    QSTORE (input/output) DOUBLE PRECISION array, dimension (N**2+1)
           Stores eigenvectors of submatrices encountered during
           divide and conquer, packed together. QPTR points to
           beginning of the submatrices.

    QPTR   (input/output) INTEGER array, dimension (N+2)
           List of indices pointing to beginning of submatrices stored
           in QSTORE. The submatrices are numbered starting at the
           bottom left of the divide and conquer tree, from left to
           right and bottom to top.

    PRMPTR (input) INTEGER array, dimension (N lg N)
           Contains a list of pointers which indicate where in PERM a
           level's permutation is stored.  PRMPTR(i+1) - PRMPTR(i)
           indicates the size of the permutation and also the size of
           the full, non-deflated problem.

    PERM   (input) INTEGER array, dimension (N lg N)
           Contains the permutations (from deflation and sorting) to be
           applied to each eigenblock.

    GIVPTR (input) INTEGER array, dimension (N lg N)
           Contains a list of pointers which indicate where in GIVCOL a
           level's Givens rotations are stored.  GIVPTR(i+1) - GIVPTR(i)
           indicates the number of Givens rotations.

    GIVCOL (input) INTEGER array, dimension (2, N lg N)
           Each pair of numbers indicates a pair of columns to take place
           in a Givens rotation.

    GIVNUM (input) DOUBLE PRECISION array, dimension (2, N lg N)
           Each number indicates the S value to be used in the
           corresponding Givens rotation.

    WORK   (workspace) DOUBLE PRECISION array, dimension (3*N+QSIZ*N)

    IWORK  (workspace) INTEGER array, dimension (4*N)

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an eigenvalue did not converge

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --qstore;
    --qptr;
    --prmptr;
    --perm;
    --givptr;
    givcol -= 3;
    givnum -= 3;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if ((*icompq == 1 && *qsiz < *n)) {
	*info = -4;
    } else if (*ldq < max(1,*n)) {
	*info = -9;
    } else if (min(1,*n) > *cutpnt || *n < *cutpnt) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED7", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*
       The following values are for bookkeeping purposes only.  They are
       integer pointers which indicate the portion of the workspace
       used by a particular array in DLAED8 and DLAED9.
*/

    if (*icompq == 1) {
	ldq2 = *qsiz;
    } else {
	ldq2 = *n;
    }

    iz = 1;
    idlmda = iz + *n;
    iw = idlmda + *n;
    iq2 = iw + *n;
    is = iq2 + *n * ldq2;

    indx = 1;
    indxc = indx + *n;
    coltyp = indxc + *n;
    indxp = coltyp + *n;

/*
       Form the z-vector which consists of the last row of Q_1 and the
       first row of Q_2.
*/

    ptr = pow_ii(&c__2, tlvls) + 1;
    i__1 = *curlvl - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *tlvls - i__;
	ptr += pow_ii(&c__2, &i__2);
/* L10: */
    }
    curr = ptr + *curpbm;
    dlaeda_(n, tlvls, curlvl, curpbm, &prmptr[1], &perm[1], &givptr[1], &
	    givcol[3], &givnum[3], &qstore[1], &qptr[1], &work[iz], &work[iz
	    + *n], info);

/*
       When solving the final problem, we no longer need the stored data,
       so we will overwrite the data from this level onto the previously
       used storage space.
*/

    if (*curlvl == *tlvls) {
	qptr[curr] = 1;
	prmptr[curr] = 1;
	givptr[curr] = 1;
    }

/*     Sort and Deflate eigenvalues. */

    dlaed8_(icompq, &k, n, qsiz, &d__[1], &q[q_offset], ldq, &indxq[1], rho,
	    cutpnt, &work[iz], &work[idlmda], &work[iq2], &ldq2, &work[iw], &
	    perm[prmptr[curr]], &givptr[curr + 1], &givcol[((givptr[curr]) <<
	    (1)) + 1], &givnum[((givptr[curr]) << (1)) + 1], &iwork[indxp], &
	    iwork[indx], info);
    prmptr[curr + 1] = prmptr[curr] + *n;
    givptr[curr + 1] += givptr[curr];

/*     Solve Secular Equation. */

    if (k != 0) {
	dlaed9_(&k, &c__1, &k, n, &d__[1], &work[is], &k, rho, &work[idlmda],
		&work[iw], &qstore[qptr[curr]], &k, info);
	if (*info != 0) {
	    goto L30;
	}
	if (*icompq == 1) {
	    dgemm_("N", "N", qsiz, &k, &k, &c_b15, &work[iq2], &ldq2, &qstore[
		    qptr[curr]], &k, &c_b29, &q[q_offset], ldq);
	}
/* Computing 2nd power */
	i__1 = k;
	qptr[curr + 1] = qptr[curr] + i__1 * i__1;

/*     Prepare the INDXQ sorting permutation. */

	n1 = k;
	n2 = *n - k;
	dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &indxq[1]);
    } else {
	qptr[curr + 1] = qptr[curr];
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    indxq[i__] = i__;
/* L20: */
	}
    }

L30:
    return 0;

/*     End of DLAED7 */

} /* dlaed7_ */

/* Subroutine */ int dlaed8_(integer *icompq, integer *k, integer *n, integer
	*qsiz, doublereal *d__, doublereal *q, integer *ldq, integer *indxq,
	doublereal *rho, integer *cutpnt, doublereal *z__, doublereal *dlamda,
	 doublereal *q2, integer *ldq2, doublereal *w, integer *perm, integer
	*givptr, integer *givcol, doublereal *givnum, integer *indxp, integer
	*indx, integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, q2_dim1, q2_offset, i__1;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal c__;
    static integer i__, j;
    static doublereal s, t;
    static integer k2, n1, n2, jp, n1p1;
    static doublereal eps, tau, tol;
    static integer jlam, imax, jmax;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *), dscal_(
	    integer *, doublereal *, doublereal *, integer *), dcopy_(integer
	    *, doublereal *, integer *, doublereal *, integer *);

    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dlamrg_(integer *, integer *, doublereal *,
	    integer *, integer *, integer *), dlacpy_(char *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       September 30, 1994


    Purpose
    =======

    DLAED8 merges the two sets of eigenvalues together into a single
    sorted set.  Then it tries to deflate the size of the problem.
    There are two ways in which deflation can occur:  when two or more
    eigenvalues are close together or if there is a tiny element in the
    Z vector.  For each such occurrence the order of the related secular
    equation problem is reduced by one.

    Arguments
    =========

    ICOMPQ  (input) INTEGER
            = 0:  Compute eigenvalues only.
            = 1:  Compute eigenvectors of original dense symmetric matrix
                  also.  On entry, Q contains the orthogonal matrix used
                  to reduce the original matrix to tridiagonal form.

    K      (output) INTEGER
           The number of non-deflated eigenvalues, and the order of the
           related secular equation.

    N      (input) INTEGER
           The dimension of the symmetric tridiagonal matrix.  N >= 0.

    QSIZ   (input) INTEGER
           The dimension of the orthogonal matrix used to reduce
           the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1.

    D      (input/output) DOUBLE PRECISION array, dimension (N)
           On entry, the eigenvalues of the two submatrices to be
           combined.  On exit, the trailing (N-K) updated eigenvalues
           (those which were deflated) sorted into increasing order.

    Q      (input/output) DOUBLE PRECISION array, dimension (LDQ,N)
           If ICOMPQ = 0, Q is not referenced.  Otherwise,
           on entry, Q contains the eigenvectors of the partially solved
           system which has been previously updated in matrix
           multiplies with other partially solved eigensystems.
           On exit, Q contains the trailing (N-K) updated eigenvectors
           (those which were deflated) in its last N-K columns.

    LDQ    (input) INTEGER
           The leading dimension of the array Q.  LDQ >= max(1,N).

    INDXQ  (input) INTEGER array, dimension (N)
           The permutation which separately sorts the two sub-problems
           in D into ascending order.  Note that elements in the second
           half of this permutation must first have CUTPNT added to
           their values in order to be accurate.

    RHO    (input/output) DOUBLE PRECISION
           On entry, the off-diagonal element associated with the rank-1
           cut which originally split the two submatrices which are now
           being recombined.
           On exit, RHO has been modified to the value required by
           DLAED3.

    CUTPNT (input) INTEGER
           The location of the last eigenvalue in the leading
           sub-matrix.  min(1,N) <= CUTPNT <= N.

    Z      (input) DOUBLE PRECISION array, dimension (N)
           On entry, Z contains the updating vector (the last row of
           the first sub-eigenvector matrix and the first row of the
           second sub-eigenvector matrix).
           On exit, the contents of Z are destroyed by the updating
           process.

    DLAMDA (output) DOUBLE PRECISION array, dimension (N)
           A copy of the first K eigenvalues which will be used by
           DLAED3 to form the secular equation.

    Q2     (output) DOUBLE PRECISION array, dimension (LDQ2,N)
           If ICOMPQ = 0, Q2 is not referenced.  Otherwise,
           a copy of the first K eigenvectors which will be used by
           DLAED7 in a matrix multiply (DGEMM) to update the new
           eigenvectors.

    LDQ2   (input) INTEGER
           The leading dimension of the array Q2.  LDQ2 >= max(1,N).

    W      (output) DOUBLE PRECISION array, dimension (N)
           The first k values of the final deflation-altered z-vector and
           will be passed to DLAED3.

    PERM   (output) INTEGER array, dimension (N)
           The permutations (from deflation and sorting) to be applied
           to each eigenblock.

    GIVPTR (output) INTEGER
           The number of Givens rotations which took place in this
           subproblem.

    GIVCOL (output) INTEGER array, dimension (2, N)
           Each pair of numbers indicates a pair of columns to take place
           in a Givens rotation.

    GIVNUM (output) DOUBLE PRECISION array, dimension (2, N)
           Each number indicates the S value to be used in the
           corresponding Givens rotation.

    INDXP  (workspace) INTEGER array, dimension (N)
           The permutation used to place deflated values of D at the end
           of the array.  INDXP(1:K) points to the nondeflated D-values
           and INDXP(K+1:N) points to the deflated eigenvalues.

    INDX   (workspace) INTEGER array, dimension (N)
           The permutation used to sort the contents of D into ascending
           order.

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --z__;
    --dlamda;
    q2_dim1 = *ldq2;
    q2_offset = 1 + q2_dim1 * 1;
    q2 -= q2_offset;
    --w;
    --perm;
    givcol -= 3;
    givnum -= 3;
    --indxp;
    --indx;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -3;
    } else if ((*icompq == 1 && *qsiz < *n)) {
	*info = -4;
    } else if (*ldq < max(1,*n)) {
	*info = -7;
    } else if (*cutpnt < min(1,*n) || *cutpnt > *n) {
	*info = -10;
    } else if (*ldq2 < max(1,*n)) {
	*info = -14;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED8", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    n1 = *cutpnt;
    n2 = *n - n1;
    n1p1 = n1 + 1;

    if (*rho < 0.) {
	dscal_(&n2, &c_b151, &z__[n1p1], &c__1);
    }

/*     Normalize z so that norm(z) = 1 */

    t = 1. / sqrt(2.);
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	indx[j] = j;
/* L10: */
    }
    dscal_(n, &t, &z__[1], &c__1);
    *rho = (d__1 = *rho * 2., abs(d__1));

/*     Sort the eigenvalues into increasing order */

    i__1 = *n;
    for (i__ = *cutpnt + 1; i__ <= i__1; ++i__) {
	indxq[i__] += *cutpnt;
/* L20: */
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = d__[indxq[i__]];
	w[i__] = z__[indxq[i__]];
/* L30: */
    }
    i__ = 1;
    j = *cutpnt + 1;
    dlamrg_(&n1, &n2, &dlamda[1], &c__1, &c__1, &indx[1]);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__[i__] = dlamda[indx[i__]];
	z__[i__] = w[indx[i__]];
/* L40: */
    }

/*     Calculate the allowable deflation tolerence */

    imax = idamax_(n, &z__[1], &c__1);
    jmax = idamax_(n, &d__[1], &c__1);
    eps = EPSILON;
    tol = eps * 8. * (d__1 = d__[jmax], abs(d__1));

/*
       If the rank-1 modifier is small enough, no more needs to be done
       except to reorganize Q so that its columns correspond with the
       elements in D.
*/

    if (*rho * (d__1 = z__[imax], abs(d__1)) <= tol) {
	*k = 0;
	if (*icompq == 0) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		perm[j] = indxq[indx[j]];
/* L50: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		perm[j] = indxq[indx[j]];
		dcopy_(qsiz, &q[perm[j] * q_dim1 + 1], &c__1, &q2[j * q2_dim1
			+ 1], &c__1);
/* L60: */
	    }
	    dlacpy_("A", qsiz, n, &q2[q2_dim1 + 1], ldq2, &q[q_dim1 + 1], ldq);
	}
	return 0;
    }

/*
       If there are multiple eigenvalues then the problem deflates.  Here
       the number of equal eigenvalues are found.  As each equal
       eigenvalue is found, an elementary reflector is computed to rotate
       the corresponding eigensubspace so that the corresponding
       components of Z are zero in this new basis.
*/

    *k = 0;
    *givptr = 0;
    k2 = *n + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (*rho * (d__1 = z__[j], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    indxp[k2] = j;
	    if (j == *n) {
		goto L110;
	    }
	} else {
	    jlam = j;
	    goto L80;
	}
/* L70: */
    }
L80:
    ++j;
    if (j > *n) {
	goto L100;
    }
    if (*rho * (d__1 = z__[j], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	indxp[k2] = j;
    } else {

/*        Check if eigenvalues are close enough to allow deflation. */

	s = z__[jlam];
	c__ = z__[j];

/*
          Find sqrt(a**2+b**2) without overflow or
          destructive underflow.
*/

	tau = dlapy2_(&c__, &s);
	t = d__[j] - d__[jlam];
	c__ /= tau;
	s = -s / tau;
	if ((d__1 = t * c__ * s, abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    z__[j] = tau;
	    z__[jlam] = 0.;

/*           Record the appropriate Givens rotation */

	    ++(*givptr);
	    givcol[((*givptr) << (1)) + 1] = indxq[indx[jlam]];
	    givcol[((*givptr) << (1)) + 2] = indxq[indx[j]];
	    givnum[((*givptr) << (1)) + 1] = c__;
	    givnum[((*givptr) << (1)) + 2] = s;
	    if (*icompq == 1) {
		drot_(qsiz, &q[indxq[indx[jlam]] * q_dim1 + 1], &c__1, &q[
			indxq[indx[j]] * q_dim1 + 1], &c__1, &c__, &s);
	    }
	    t = d__[jlam] * c__ * c__ + d__[j] * s * s;
	    d__[j] = d__[jlam] * s * s + d__[j] * c__ * c__;
	    d__[jlam] = t;
	    --k2;
	    i__ = 1;
L90:
	    if (k2 + i__ <= *n) {
		if (d__[jlam] < d__[indxp[k2 + i__]]) {
		    indxp[k2 + i__ - 1] = indxp[k2 + i__];
		    indxp[k2 + i__] = jlam;
		    ++i__;
		    goto L90;
		} else {
		    indxp[k2 + i__ - 1] = jlam;
		}
	    } else {
		indxp[k2 + i__ - 1] = jlam;
	    }
	    jlam = j;
	} else {
	    ++(*k);
	    w[*k] = z__[jlam];
	    dlamda[*k] = d__[jlam];
	    indxp[*k] = jlam;
	    jlam = j;
	}
    }
    goto L80;
L100:

/*     Record the last eigenvalue. */

    ++(*k);
    w[*k] = z__[jlam];
    dlamda[*k] = d__[jlam];
    indxp[*k] = jlam;

L110:

/*
       Sort the eigenvalues and corresponding eigenvectors into DLAMDA
       and Q2 respectively.  The eigenvalues/vectors which were not
       deflated go into the first K slots of DLAMDA and Q2 respectively,
       while those which were deflated go into the last N - K slots.
*/

    if (*icompq == 0) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    jp = indxp[j];
	    dlamda[j] = d__[jp];
	    perm[j] = indxq[indx[jp]];
/* L120: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    jp = indxp[j];
	    dlamda[j] = d__[jp];
	    perm[j] = indxq[indx[jp]];
	    dcopy_(qsiz, &q[perm[j] * q_dim1 + 1], &c__1, &q2[j * q2_dim1 + 1]
		    , &c__1);
/* L130: */
	}
    }

/*
       The deflated eigenvalues and their corresponding vectors go back
       into the last N - K slots of D and Q respectively.
*/

    if (*k < *n) {
	if (*icompq == 0) {
	    i__1 = *n - *k;
	    dcopy_(&i__1, &dlamda[*k + 1], &c__1, &d__[*k + 1], &c__1);
	} else {
	    i__1 = *n - *k;
	    dcopy_(&i__1, &dlamda[*k + 1], &c__1, &d__[*k + 1], &c__1);
	    i__1 = *n - *k;
	    dlacpy_("A", qsiz, &i__1, &q2[(*k + 1) * q2_dim1 + 1], ldq2, &q[(*
		    k + 1) * q_dim1 + 1], ldq);
	}
    }

    return 0;

/*     End of DLAED8 */

} /* dlaed8_ */

/* Subroutine */ int dlaed9_(integer *k, integer *kstart, integer *kstop,
	integer *n, doublereal *d__, doublereal *q, integer *ldq, doublereal *
	rho, doublereal *dlamda, doublereal *w, doublereal *s, integer *lds,
	integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, s_dim1, s_offset, i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer i__, j;
    static doublereal temp;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *), dlaed4_(integer *, integer *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, integer *);
    extern doublereal dlamc3_(doublereal *, doublereal *);
    extern /* Subroutine */ int xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       September 30, 1994


    Purpose
    =======

    DLAED9 finds the roots of the secular equation, as defined by the
    values in D, Z, and RHO, between KSTART and KSTOP.  It makes the
    appropriate calls to DLAED4 and then stores the new matrix of
    eigenvectors for use in calculating the next level of Z vectors.

    Arguments
    =========

    K       (input) INTEGER
            The number of terms in the rational function to be solved by
            DLAED4.  K >= 0.

    KSTART  (input) INTEGER
    KSTOP   (input) INTEGER
            The updated eigenvalues Lambda(I), KSTART <= I <= KSTOP
            are to be computed.  1 <= KSTART <= KSTOP <= K.

    N       (input) INTEGER
            The number of rows and columns in the Q matrix.
            N >= K (delation may result in N > K).

    D       (output) DOUBLE PRECISION array, dimension (N)
            D(I) contains the updated eigenvalues
            for KSTART <= I <= KSTOP.

    Q       (workspace) DOUBLE PRECISION array, dimension (LDQ,N)

    LDQ     (input) INTEGER
            The leading dimension of the array Q.  LDQ >= max( 1, N ).

    RHO     (input) DOUBLE PRECISION
            The value of the parameter in the rank one update equation.
            RHO >= 0 required.

    DLAMDA  (input) DOUBLE PRECISION array, dimension (K)
            The first K elements of this array contain the old roots
            of the deflated updating problem.  These are the poles
            of the secular equation.

    W       (input) DOUBLE PRECISION array, dimension (K)
            The first K elements of this array contain the components
            of the deflation-adjusted updating vector.

    S       (output) DOUBLE PRECISION array, dimension (LDS, K)
            Will contain the eigenvectors of the repaired matrix which
            will be stored for subsequent Z vector calculation and
            multiplied by the previously accumulated eigenvectors
            to update the system.

    LDS     (input) INTEGER
            The leading dimension of S.  LDS >= max( 1, K ).

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an eigenvalue did not converge

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --dlamda;
    --w;
    s_dim1 = *lds;
    s_offset = 1 + s_dim1 * 1;
    s -= s_offset;

    /* Function Body */
    *info = 0;

    if (*k < 0) {
	*info = -1;
    } else if (*kstart < 1 || *kstart > max(1,*k)) {
	*info = -2;
    } else if (max(1,*kstop) < *kstart || *kstop > max(1,*k)) {
	*info = -3;
    } else if (*n < *k) {
	*info = -4;
    } else if (*ldq < max(1,*k)) {
	*info = -7;
    } else if (*lds < max(1,*k)) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED9", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 0) {
	return 0;
    }

/*
       Modify values DLAMDA(i) to make sure all DLAMDA(i)-DLAMDA(j) can
       be computed with high relative accuracy (barring over/underflow).
       This is a problem on machines without a guard digit in
       add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2).
       The following code replaces DLAMDA(I) by 2*DLAMDA(I)-DLAMDA(I),
       which on any of these machines zeros out the bottommost
       bit of DLAMDA(I) if it is 1; this makes the subsequent
       subtractions DLAMDA(I)-DLAMDA(J) unproblematic when cancellation
       occurs. On binary machines with a guard digit (almost all
       machines) it does not change DLAMDA(I) at all. On hexadecimal
       and decimal machines with a guard digit, it slightly
       changes the bottommost bits of DLAMDA(I). It does not account
       for hexadecimal or decimal machines without guard digits
       (we know of none). We use a subroutine call to compute
       2*DLAMBDA(I) to prevent optimizing compilers from eliminating
       this code.
*/

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = dlamc3_(&dlamda[i__], &dlamda[i__]) - dlamda[i__];
/* L10: */
    }

    i__1 = *kstop;
    for (j = *kstart; j <= i__1; ++j) {
	dlaed4_(k, &j, &dlamda[1], &w[1], &q[j * q_dim1 + 1], rho, &d__[j],
		info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    goto L120;
	}
/* L20: */
    }

    if (*k == 1 || *k == 2) {
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *k;
	    for (j = 1; j <= i__2; ++j) {
		s[j + i__ * s_dim1] = q[j + i__ * q_dim1];
/* L30: */
	    }
/* L40: */
	}
	goto L120;
    }

/*     Compute updated W. */

    dcopy_(k, &w[1], &c__1, &s[s_offset], &c__1);

/*     Initialize W(I) = Q(I,I) */

    i__1 = *ldq + 1;
    dcopy_(k, &q[q_offset], &i__1, &w[1], &c__1);
    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L50: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L60: */
	}
/* L70: */
    }
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = sqrt(-w[i__]);
	w[i__] = d_sign(&d__1, &s[i__ + s_dim1]);
/* L80: */
    }

/*     Compute eigenvectors of the modified rank-1 modification. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    q[i__ + j * q_dim1] = w[i__] / q[i__ + j * q_dim1];
/* L90: */
	}
	temp = dnrm2_(k, &q[j * q_dim1 + 1], &c__1);
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    s[i__ + j * s_dim1] = q[i__ + j * q_dim1] / temp;
/* L100: */
	}
/* L110: */
    }

L120:
    return 0;

/*     End of DLAED9 */

} /* dlaed9_ */

/* Subroutine */ int dlaeda_(integer *n, integer *tlvls, integer *curlvl,
	integer *curpbm, integer *prmptr, integer *perm, integer *givptr,
	integer *givcol, doublereal *givnum, doublereal *q, integer *qptr,
	doublereal *z__, doublereal *ztemp, integer *info)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, k, mid, ptr;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    static integer curr, bsiz1, bsiz2, psiz1, psiz2, zptr1;
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *), dcopy_(integer *,
	    doublereal *, integer *, doublereal *, integer *), xerbla_(char *,
	     integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DLAEDA computes the Z vector corresponding to the merge step in the
    CURLVLth step of the merge process with TLVLS steps for the CURPBMth
    problem.

    Arguments
    =========

    N      (input) INTEGER
           The dimension of the symmetric tridiagonal matrix.  N >= 0.

    TLVLS  (input) INTEGER
           The total number of merging levels in the overall divide and
           conquer tree.

    CURLVL (input) INTEGER
           The current level in the overall merge routine,
           0 <= curlvl <= tlvls.

    CURPBM (input) INTEGER
           The current problem in the current level in the overall
           merge routine (counting from upper left to lower right).

    PRMPTR (input) INTEGER array, dimension (N lg N)
           Contains a list of pointers which indicate where in PERM a
           level's permutation is stored.  PRMPTR(i+1) - PRMPTR(i)
           indicates the size of the permutation and incidentally the
           size of the full, non-deflated problem.

    PERM   (input) INTEGER array, dimension (N lg N)
           Contains the permutations (from deflation and sorting) to be
           applied to each eigenblock.

    GIVPTR (input) INTEGER array, dimension (N lg N)
           Contains a list of pointers which indicate where in GIVCOL a
           level's Givens rotations are stored.  GIVPTR(i+1) - GIVPTR(i)
           indicates the number of Givens rotations.

    GIVCOL (input) INTEGER array, dimension (2, N lg N)
           Each pair of numbers indicates a pair of columns to take place
           in a Givens rotation.

    GIVNUM (input) DOUBLE PRECISION array, dimension (2, N lg N)
           Each number indicates the S value to be used in the
           corresponding Givens rotation.

    Q      (input) DOUBLE PRECISION array, dimension (N**2)
           Contains the square eigenblocks from previous levels, the
           starting positions for blocks are given by QPTR.

    QPTR   (input) INTEGER array, dimension (N+2)
           Contains a list of pointers which indicate where in Q an
           eigenblock is stored.  SQRT( QPTR(i+1) - QPTR(i) ) indicates
           the size of the block.

    Z      (output) DOUBLE PRECISION array, dimension (N)
           On output this vector contains the updating vector (the last
           row of the first sub-eigenvector matrix and the first row of
           the second sub-eigenvector matrix).

    ZTEMP  (workspace) DOUBLE PRECISION array, dimension (N)

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --ztemp;
    --z__;
    --qptr;
    --q;
    givnum -= 3;
    givcol -= 3;
    --givptr;
    --perm;
    --prmptr;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAEDA", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine location of first number in second half. */

    mid = *n / 2 + 1;

/*     Gather last/first rows of appropriate eigenblocks into center of Z */

    ptr = 1;

/*
       Determine location of lowest level subproblem in the full storage
       scheme
*/

    i__1 = *curlvl - 1;
    curr = ptr + *curpbm * pow_ii(&c__2, curlvl) + pow_ii(&c__2, &i__1) - 1;

/*
       Determine size of these matrices.  We add HALF to the value of
       the SQRT in case the machine underestimates one of these square
       roots.
*/

    bsiz1 = (integer) (sqrt((doublereal) (qptr[curr + 1] - qptr[curr])) + .5);
    bsiz2 = (integer) (sqrt((doublereal) (qptr[curr + 2] - qptr[curr + 1])) +
	    .5);
    i__1 = mid - bsiz1 - 1;
    for (k = 1; k <= i__1; ++k) {
	z__[k] = 0.;
/* L10: */
    }
    dcopy_(&bsiz1, &q[qptr[curr] + bsiz1 - 1], &bsiz1, &z__[mid - bsiz1], &
	    c__1);
    dcopy_(&bsiz2, &q[qptr[curr + 1]], &bsiz2, &z__[mid], &c__1);
    i__1 = *n;
    for (k = mid + bsiz2; k <= i__1; ++k) {
	z__[k] = 0.;
/* L20: */
    }

/*
       Loop thru remaining levels 1 -> CURLVL applying the Givens
       rotations and permutation and then multiplying the center matrices
       against the current Z.
*/

    ptr = pow_ii(&c__2, tlvls) + 1;
    i__1 = *curlvl - 1;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *curlvl - k;
	i__3 = *curlvl - k - 1;
	curr = ptr + *curpbm * pow_ii(&c__2, &i__2) + pow_ii(&c__2, &i__3) -
		1;
	psiz1 = prmptr[curr + 1] - prmptr[curr];
	psiz2 = prmptr[curr + 2] - prmptr[curr + 1];
	zptr1 = mid - psiz1;

/*       Apply Givens at CURR and CURR+1 */

	i__2 = givptr[curr + 1] - 1;
	for (i__ = givptr[curr]; i__ <= i__2; ++i__) {
	    drot_(&c__1, &z__[zptr1 + givcol[((i__) << (1)) + 1] - 1], &c__1,
		    &z__[zptr1 + givcol[((i__) << (1)) + 2] - 1], &c__1, &
		    givnum[((i__) << (1)) + 1], &givnum[((i__) << (1)) + 2]);
/* L30: */
	}
	i__2 = givptr[curr + 2] - 1;
	for (i__ = givptr[curr + 1]; i__ <= i__2; ++i__) {
	    drot_(&c__1, &z__[mid - 1 + givcol[((i__) << (1)) + 1]], &c__1, &
		    z__[mid - 1 + givcol[((i__) << (1)) + 2]], &c__1, &givnum[
		    ((i__) << (1)) + 1], &givnum[((i__) << (1)) + 2]);
/* L40: */
	}
	psiz1 = prmptr[curr + 1] - prmptr[curr];
	psiz2 = prmptr[curr + 2] - prmptr[curr + 1];
	i__2 = psiz1 - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    ztemp[i__ + 1] = z__[zptr1 + perm[prmptr[curr] + i__] - 1];
/* L50: */
	}
	i__2 = psiz2 - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    ztemp[psiz1 + i__ + 1] = z__[mid + perm[prmptr[curr + 1] + i__] -
		    1];
/* L60: */
	}

/*
          Multiply Blocks at CURR and CURR+1

          Determine size of these matrices.  We add HALF to the value of
          the SQRT in case the machine underestimates one of these
          square roots.
*/

	bsiz1 = (integer) (sqrt((doublereal) (qptr[curr + 1] - qptr[curr])) +
		.5);
	bsiz2 = (integer) (sqrt((doublereal) (qptr[curr + 2] - qptr[curr + 1])
		) + .5);
	if (bsiz1 > 0) {
	    dgemv_("T", &bsiz1, &bsiz1, &c_b15, &q[qptr[curr]], &bsiz1, &
		    ztemp[1], &c__1, &c_b29, &z__[zptr1], &c__1);
	}
	i__2 = psiz1 - bsiz1;
	dcopy_(&i__2, &ztemp[bsiz1 + 1], &c__1, &z__[zptr1 + bsiz1], &c__1);
	if (bsiz2 > 0) {
	    dgemv_("T", &bsiz2, &bsiz2, &c_b15, &q[qptr[curr + 1]], &bsiz2, &
		    ztemp[psiz1 + 1], &c__1, &c_b29, &z__[mid], &c__1);
	}
	i__2 = psiz2 - bsiz2;
	dcopy_(&i__2, &ztemp[psiz1 + bsiz2 + 1], &c__1, &z__[mid + bsiz2], &
		c__1);

	i__2 = *tlvls - k;
	ptr += pow_ii(&c__2, &i__2);
/* L70: */
    }

    return 0;

/*     End of DLAEDA */

} /* dlaeda_ */

/* Subroutine */ int dlaev2_(doublereal *a, doublereal *b, doublereal *c__,
	doublereal *rt1, doublereal *rt2, doublereal *cs1, doublereal *sn1)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal ab, df, cs, ct, tb, sm, tn, rt, adf, acs;
    static integer sgn1, sgn2;
    static doublereal acmn, acmx;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLAEV2 computes the eigendecomposition of a 2-by-2 symmetric matrix
       [  A   B  ]
       [  B   C  ].
    On return, RT1 is the eigenvalue of larger absolute value, RT2 is the
    eigenvalue of smaller absolute value, and (CS1,SN1) is the unit right
    eigenvector for RT1, giving the decomposition

       [ CS1  SN1 ] [  A   B  ] [ CS1 -SN1 ]  =  [ RT1  0  ]
       [-SN1  CS1 ] [  B   C  ] [ SN1  CS1 ]     [  0  RT2 ].

    Arguments
    =========

    A       (input) DOUBLE PRECISION
            The (1,1) element of the 2-by-2 matrix.

    B       (input) DOUBLE PRECISION
            The (1,2) element and the conjugate of the (2,1) element of
            the 2-by-2 matrix.

    C       (input) DOUBLE PRECISION
            The (2,2) element of the 2-by-2 matrix.

    RT1     (output) DOUBLE PRECISION
            The eigenvalue of larger absolute value.

    RT2     (output) DOUBLE PRECISION
            The eigenvalue of smaller absolute value.

    CS1     (output) DOUBLE PRECISION
    SN1     (output) DOUBLE PRECISION
            The vector (CS1, SN1) is a unit right eigenvector for RT1.

    Further Details
    ===============

    RT1 is accurate to a few ulps barring over/underflow.

    RT2 may be inaccurate if there is massive cancellation in the
    determinant A*C-B*B; higher precision or correctly rounded or
    correctly truncated arithmetic would be needed to compute RT2
    accurately in all cases.

    CS1 and SN1 are accurate to a few ulps barring over/underflow.

    Overflow is possible only if RT1 is within a factor of 5 of overflow.
    Underflow is harmless if the input data is 0 or exceeds
       underflow_threshold / macheps.

   =====================================================================


       Compute the eigenvalues
*/

    sm = *a + *c__;
    df = *a - *c__;
    adf = abs(df);
    tb = *b + *b;
    ab = abs(tb);
    if (abs(*a) > abs(*c__)) {
	acmx = *a;
	acmn = *c__;
    } else {
	acmx = *c__;
	acmn = *a;
    }
    if (adf > ab) {
/* Computing 2nd power */
	d__1 = ab / adf;
	rt = adf * sqrt(d__1 * d__1 + 1.);
    } else if (adf < ab) {
/* Computing 2nd power */
	d__1 = adf / ab;
	rt = ab * sqrt(d__1 * d__1 + 1.);
    } else {

/*        Includes case AB=ADF=0 */

	rt = ab * sqrt(2.);
    }
    if (sm < 0.) {
	*rt1 = (sm - rt) * .5;
	sgn1 = -1;

/*
          Order of execution important.
          To get fully accurate smaller eigenvalue,
          next line needs to be executed in higher precision.
*/

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else if (sm > 0.) {
	*rt1 = (sm + rt) * .5;
	sgn1 = 1;

/*
          Order of execution important.
          To get fully accurate smaller eigenvalue,
          next line needs to be executed in higher precision.
*/

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else {

/*        Includes case RT1 = RT2 = 0 */

	*rt1 = rt * .5;
	*rt2 = rt * -.5;
	sgn1 = 1;
    }

/*     Compute the eigenvector */

    if (df >= 0.) {
	cs = df + rt;
	sgn2 = 1;
    } else {
	cs = df - rt;
	sgn2 = -1;
    }
    acs = abs(cs);
    if (acs > ab) {
	ct = -tb / cs;
	*sn1 = 1. / sqrt(ct * ct + 1.);
	*cs1 = ct * *sn1;
    } else {
	if (ab == 0.) {
	    *cs1 = 1.;
	    *sn1 = 0.;
	} else {
	    tn = -cs / tb;
	    *cs1 = 1. / sqrt(tn * tn + 1.);
	    *sn1 = tn * *cs1;
	}
    }
    if (sgn1 == sgn2) {
	tn = *cs1;
	*cs1 = -(*sn1);
	*sn1 = tn;
    }
    return 0;

/*     End of DLAEV2 */

} /* dlaev2_ */

/* Subroutine */ int dlahqr_(logical *wantt, logical *wantz, integer *n,
	integer *ilo, integer *ihi, doublereal *h__, integer *ldh, doublereal
	*wr, doublereal *wi, integer *iloz, integer *ihiz, doublereal *z__,
	integer *ldz, integer *info)
{
    /* System generated locals */
    integer h_dim1, h_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer i__, j, k, l, m;
    static doublereal s, v[3];
    static integer i1, i2;
    static doublereal t1, t2, t3, v1, v2, v3, h00, h10, h11, h12, h21, h22,
	    h33, h44;
    static integer nh;
    static doublereal cs;
    static integer nr;
    static doublereal sn;
    static integer nz;
    static doublereal ave, h33s, h44s;
    static integer itn, its;
    static doublereal ulp, sum, tst1, h43h34, disc, unfl, ovfl;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    static doublereal work[1];
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *), dlanv2_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *), dlabad_(
	    doublereal *, doublereal *);

    extern /* Subroutine */ int dlarfg_(integer *, doublereal *, doublereal *,
	     integer *, doublereal *);
    extern doublereal dlanhs_(char *, integer *, doublereal *, integer *,
	    doublereal *);
    static doublereal smlnum;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLAHQR is an auxiliary routine called by DHSEQR to update the
    eigenvalues and Schur decomposition already computed by DHSEQR, by
    dealing with the Hessenberg submatrix in rows and columns ILO to IHI.

    Arguments
    =========

    WANTT   (input) LOGICAL
            = .TRUE. : the full Schur form T is required;
            = .FALSE.: only eigenvalues are required.

    WANTZ   (input) LOGICAL
            = .TRUE. : the matrix of Schur vectors Z is required;
            = .FALSE.: Schur vectors are not required.

    N       (input) INTEGER
            The order of the matrix H.  N >= 0.

    ILO     (input) INTEGER
    IHI     (input) INTEGER
            It is assumed that H is already upper quasi-triangular in
            rows and columns IHI+1:N, and that H(ILO,ILO-1) = 0 (unless
            ILO = 1). DLAHQR works primarily with the Hessenberg
            submatrix in rows and columns ILO to IHI, but applies
            transformations to all of H if WANTT is .TRUE..
            1 <= ILO <= max(1,IHI); IHI <= N.

    H       (input/output) DOUBLE PRECISION array, dimension (LDH,N)
            On entry, the upper Hessenberg matrix H.
            On exit, if WANTT is .TRUE., H is upper quasi-triangular in
            rows and columns ILO:IHI, with any 2-by-2 diagonal blocks in
            standard form. If WANTT is .FALSE., the contents of H are
            unspecified on exit.

    LDH     (input) INTEGER
            The leading dimension of the array H. LDH >= max(1,N).

    WR      (output) DOUBLE PRECISION array, dimension (N)
    WI      (output) DOUBLE PRECISION array, dimension (N)
            The real and imaginary parts, respectively, of the computed
            eigenvalues ILO to IHI are stored in the corresponding
            elements of WR and WI. If two eigenvalues are computed as a
            complex conjugate pair, they are stored in consecutive
            elements of WR and WI, say the i-th and (i+1)th, with
            WI(i) > 0 and WI(i+1) < 0. If WANTT is .TRUE., the
            eigenvalues are stored in the same order as on the diagonal
            of the Schur form returned in H, with WR(i) = H(i,i), and, if
            H(i:i+1,i:i+1) is a 2-by-2 diagonal block,
            WI(i) = sqrt(H(i+1,i)*H(i,i+1)) and WI(i+1) = -WI(i).

    ILOZ    (input) INTEGER
    IHIZ    (input) INTEGER
            Specify the rows of Z to which transformations must be
            applied if WANTZ is .TRUE..
            1 <= ILOZ <= ILO; IHI <= IHIZ <= N.

    Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N)
            If WANTZ is .TRUE., on entry Z must contain the current
            matrix Z of transformations accumulated by DHSEQR, and on
            exit Z has been updated; transformations are applied only to
            the submatrix Z(ILOZ:IHIZ,ILO:IHI).
            If WANTZ is .FALSE., Z is not referenced.

    LDZ     (input) INTEGER
            The leading dimension of the array Z. LDZ >= max(1,N).

    INFO    (output) INTEGER
            = 0: successful exit
            > 0: DLAHQR failed to compute all the eigenvalues ILO to IHI
                 in a total of 30*(IHI-ILO+1) iterations; if INFO = i,
                 elements i+1:ihi of WR and WI contain those eigenvalues
                 which have been successfully computed.

    Further Details
    ===============

    2-96 Based on modifications by
       David Day, Sandia National Laboratory, USA

    =====================================================================
*/


    /* Parameter adjustments */
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1 * 1;
    h__ -= h_offset;
    --wr;
    --wi;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*ilo == *ihi) {
	wr[*ilo] = h__[*ilo + *ilo * h_dim1];
	wi[*ilo] = 0.;
	return 0;
    }

    nh = *ihi - *ilo + 1;
    nz = *ihiz - *iloz + 1;

/*
       Set machine-dependent constants for the stopping criterion.
       If norm(H) <= sqrt(OVFL), overflow should not occur.
*/

    unfl = SAFEMINIMUM;
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    ulp = PRECISION;
    smlnum = unfl * (nh / ulp);

/*
       I1 and I2 are the indices of the first row and last column of H
       to which transformations must be applied. If eigenvalues only are
       being computed, I1 and I2 are set inside the main loop.
*/

    if (*wantt) {
	i1 = 1;
	i2 = *n;
    }

/*     ITN is the total number of QR iterations allowed. */

    itn = nh * 30;

/*
       The main loop begins here. I is the loop index and decreases from
       IHI to ILO in steps of 1 or 2. Each iteration of the loop works
       with the active submatrix in rows and columns L to I.
       Eigenvalues I+1 to IHI have already converged. Either L = ILO or
       H(L,L-1) is negligible so that the matrix splits.
*/

    i__ = *ihi;
L10:
    l = *ilo;
    if (i__ < *ilo) {
	goto L150;
    }

/*
       Perform QR iterations on rows and columns ILO to I until a
       submatrix of order 1 or 2 splits off at the bottom because a
       subdiagonal element has become negligible.
*/

    i__1 = itn;
    for (its = 0; its <= i__1; ++its) {

/*        Look for a single small subdiagonal element. */

	i__2 = l + 1;
	for (k = i__; k >= i__2; --k) {
	    tst1 = (d__1 = h__[k - 1 + (k - 1) * h_dim1], abs(d__1)) + (d__2 =
		     h__[k + k * h_dim1], abs(d__2));
	    if (tst1 == 0.) {
		i__3 = i__ - l + 1;
		tst1 = dlanhs_("1", &i__3, &h__[l + l * h_dim1], ldh, work);
	    }
/* Computing MAX */
	    d__2 = ulp * tst1;
	    if ((d__1 = h__[k + (k - 1) * h_dim1], abs(d__1)) <= max(d__2,
		    smlnum)) {
		goto L30;
	    }
/* L20: */
	}
L30:
	l = k;
	if (l > *ilo) {

/*           H(L,L-1) is negligible */

	    h__[l + (l - 1) * h_dim1] = 0.;
	}

/*        Exit from loop if a submatrix of order 1 or 2 has split off. */

	if (l >= i__ - 1) {
	    goto L140;
	}

/*
          Now the active submatrix is in rows and columns L to I. If
          eigenvalues only are being computed, only the active submatrix
          need be transformed.
*/

	if (! (*wantt)) {
	    i1 = l;
	    i2 = i__;
	}

	if (its == 10 || its == 20) {

/*           Exceptional shift. */

	    s = (d__1 = h__[i__ + (i__ - 1) * h_dim1], abs(d__1)) + (d__2 =
		    h__[i__ - 1 + (i__ - 2) * h_dim1], abs(d__2));
	    h44 = s * .75 + h__[i__ + i__ * h_dim1];
	    h33 = h44;
	    h43h34 = s * -.4375 * s;
	} else {

/*
             Prepare to use Francis' double shift
             (i.e. 2nd degree generalized Rayleigh quotient)
*/

	    h44 = h__[i__ + i__ * h_dim1];
	    h33 = h__[i__ - 1 + (i__ - 1) * h_dim1];
	    h43h34 = h__[i__ + (i__ - 1) * h_dim1] * h__[i__ - 1 + i__ *
		    h_dim1];
	    s = h__[i__ - 1 + (i__ - 2) * h_dim1] * h__[i__ - 1 + (i__ - 2) *
		    h_dim1];
	    disc = (h33 - h44) * .5;
	    disc = disc * disc + h43h34;
	    if (disc > 0.) {

/*              Real roots: use Wilkinson's shift twice */

		disc = sqrt(disc);
		ave = (h33 + h44) * .5;
		if (abs(h33) - abs(h44) > 0.) {
		    h33 = h33 * h44 - h43h34;
		    h44 = h33 / (d_sign(&disc, &ave) + ave);
		} else {
		    h44 = d_sign(&disc, &ave) + ave;
		}
		h33 = h44;
		h43h34 = 0.;
	    }
	}

/*        Look for two consecutive small subdiagonal elements. */

	i__2 = l;
	for (m = i__ - 2; m >= i__2; --m) {
/*
             Determine the effect of starting the double-shift QR
             iteration at row M, and see if this would make H(M,M-1)
             negligible.
*/

	    h11 = h__[m + m * h_dim1];
	    h22 = h__[m + 1 + (m + 1) * h_dim1];
	    h21 = h__[m + 1 + m * h_dim1];
	    h12 = h__[m + (m + 1) * h_dim1];
	    h44s = h44 - h11;
	    h33s = h33 - h11;
	    v1 = (h33s * h44s - h43h34) / h21 + h12;
	    v2 = h22 - h11 - h33s - h44s;
	    v3 = h__[m + 2 + (m + 1) * h_dim1];
	    s = abs(v1) + abs(v2) + abs(v3);
	    v1 /= s;
	    v2 /= s;
	    v3 /= s;
	    v[0] = v1;
	    v[1] = v2;
	    v[2] = v3;
	    if (m == l) {
		goto L50;
	    }
	    h00 = h__[m - 1 + (m - 1) * h_dim1];
	    h10 = h__[m + (m - 1) * h_dim1];
	    tst1 = abs(v1) * (abs(h00) + abs(h11) + abs(h22));
	    if (abs(h10) * (abs(v2) + abs(v3)) <= ulp * tst1) {
		goto L50;
	    }
/* L40: */
	}
L50:

/*        Double-shift QR step */

	i__2 = i__ - 1;
	for (k = m; k <= i__2; ++k) {

/*
             The first iteration of this loop determines a reflection G
             from the vector V and applies it from left and right to H,
             thus creating a nonzero bulge below the subdiagonal.

             Each subsequent iteration determines a reflection G to
             restore the Hessenberg form in the (K-1)th column, and thus
             chases the bulge one step toward the bottom of the active
             submatrix. NR is the order of G.

   Computing MIN
*/
	    i__3 = 3, i__4 = i__ - k + 1;
	    nr = min(i__3,i__4);
	    if (k > m) {
		dcopy_(&nr, &h__[k + (k - 1) * h_dim1], &c__1, v, &c__1);
	    }
	    dlarfg_(&nr, v, &v[1], &c__1, &t1);
	    if (k > m) {
		h__[k + (k - 1) * h_dim1] = v[0];
		h__[k + 1 + (k - 1) * h_dim1] = 0.;
		if (k < i__ - 1) {
		    h__[k + 2 + (k - 1) * h_dim1] = 0.;
		}
	    } else if (m > l) {
		h__[k + (k - 1) * h_dim1] = -h__[k + (k - 1) * h_dim1];
	    }
	    v2 = v[1];
	    t2 = t1 * v2;
	    if (nr == 3) {
		v3 = v[2];
		t3 = t1 * v3;

/*
                Apply G from the left to transform the rows of the matrix
                in columns K to I2.
*/

		i__3 = i2;
		for (j = k; j <= i__3; ++j) {
		    sum = h__[k + j * h_dim1] + v2 * h__[k + 1 + j * h_dim1]
			    + v3 * h__[k + 2 + j * h_dim1];
		    h__[k + j * h_dim1] -= sum * t1;
		    h__[k + 1 + j * h_dim1] -= sum * t2;
		    h__[k + 2 + j * h_dim1] -= sum * t3;
/* L60: */
		}

/*
                Apply G from the right to transform the columns of the
                matrix in rows I1 to min(K+3,I).

   Computing MIN
*/
		i__4 = k + 3;
		i__3 = min(i__4,i__);
		for (j = i1; j <= i__3; ++j) {
		    sum = h__[j + k * h_dim1] + v2 * h__[j + (k + 1) * h_dim1]
			     + v3 * h__[j + (k + 2) * h_dim1];
		    h__[j + k * h_dim1] -= sum * t1;
		    h__[j + (k + 1) * h_dim1] -= sum * t2;
		    h__[j + (k + 2) * h_dim1] -= sum * t3;
/* L70: */
		}

		if (*wantz) {

/*                 Accumulate transformations in the matrix Z */

		    i__3 = *ihiz;
		    for (j = *iloz; j <= i__3; ++j) {
			sum = z__[j + k * z_dim1] + v2 * z__[j + (k + 1) *
				z_dim1] + v3 * z__[j + (k + 2) * z_dim1];
			z__[j + k * z_dim1] -= sum * t1;
			z__[j + (k + 1) * z_dim1] -= sum * t2;
			z__[j + (k + 2) * z_dim1] -= sum * t3;
/* L80: */
		    }
		}
	    } else if (nr == 2) {

/*
                Apply G from the left to transform the rows of the matrix
                in columns K to I2.
*/

		i__3 = i2;
		for (j = k; j <= i__3; ++j) {
		    sum = h__[k + j * h_dim1] + v2 * h__[k + 1 + j * h_dim1];
		    h__[k + j * h_dim1] -= sum * t1;
		    h__[k + 1 + j * h_dim1] -= sum * t2;
/* L90: */
		}

/*
                Apply G from the right to transform the columns of the
                matrix in rows I1 to min(K+3,I).
*/

		i__3 = i__;
		for (j = i1; j <= i__3; ++j) {
		    sum = h__[j + k * h_dim1] + v2 * h__[j + (k + 1) * h_dim1]
			    ;
		    h__[j + k * h_dim1] -= sum * t1;
		    h__[j + (k + 1) * h_dim1] -= sum * t2;
/* L100: */
		}

		if (*wantz) {

/*                 Accumulate transformations in the matrix Z */

		    i__3 = *ihiz;
		    for (j = *iloz; j <= i__3; ++j) {
			sum = z__[j + k * z_dim1] + v2 * z__[j + (k + 1) *
				z_dim1];
			z__[j + k * z_dim1] -= sum * t1;
			z__[j + (k + 1) * z_dim1] -= sum * t2;
/* L110: */
		    }
		}
	    }
/* L120: */
	}

/* L130: */
    }

/*     Failure to converge in remaining number of iterations */

    *info = i__;
    return 0;

L140:

    if (l == i__) {

/*        H(I,I-1) is negligible: one eigenvalue has converged. */

	wr[i__] = h__[i__ + i__ * h_dim1];
	wi[i__] = 0.;
    } else if (l == i__ - 1) {

/*
          H(I-1,I-2) is negligible: a pair of eigenvalues have converged.

          Transform the 2-by-2 submatrix to standard Schur form,
          and compute and store the eigenvalues.
*/

	dlanv2_(&h__[i__ - 1 + (i__ - 1) * h_dim1], &h__[i__ - 1 + i__ *
		h_dim1], &h__[i__ + (i__ - 1) * h_dim1], &h__[i__ + i__ *
		h_dim1], &wr[i__ - 1], &wi[i__ - 1], &wr[i__], &wi[i__], &cs,
		&sn);

	if (*wantt) {

/*           Apply the transformation to the rest of H. */

	    if (i2 > i__) {
		i__1 = i2 - i__;
		drot_(&i__1, &h__[i__ - 1 + (i__ + 1) * h_dim1], ldh, &h__[
			i__ + (i__ + 1) * h_dim1], ldh, &cs, &sn);
	    }
	    i__1 = i__ - i1 - 1;
	    drot_(&i__1, &h__[i1 + (i__ - 1) * h_dim1], &c__1, &h__[i1 + i__ *
		     h_dim1], &c__1, &cs, &sn);
	}
	if (*wantz) {

/*           Apply the transformation to Z. */

	    drot_(&nz, &z__[*iloz + (i__ - 1) * z_dim1], &c__1, &z__[*iloz +
		    i__ * z_dim1], &c__1, &cs, &sn);
	}
    }

/*
       Decrement number of remaining iterations, and return to start of
       the main loop with new value of I.
*/

    itn -= its;
    i__ = l - 1;
    goto L10;

L150:
    return 0;

/*     End of DLAHQR */

} /* dlahqr_ */

/* Subroutine */ int dlahrd_(integer *n, integer *k, integer *nb, doublereal *
	a, integer *lda, doublereal *tau, doublereal *t, integer *ldt,
	doublereal *y, integer *ldy)
{
    /* System generated locals */
    integer a_dim1, a_offset, t_dim1, t_offset, y_dim1, y_offset, i__1, i__2,
	    i__3;
    doublereal d__1;

    /* Local variables */
    static integer i__;
    static doublereal ei;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *), dgemv_(char *, integer *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *), dcopy_(integer *, doublereal *,
	    integer *, doublereal *, integer *), daxpy_(integer *, doublereal
	    *, doublereal *, integer *, doublereal *, integer *), dtrmv_(char
	    *, char *, char *, integer *, doublereal *, integer *, doublereal
	    *, integer *), dlarfg_(integer *,
	    doublereal *, doublereal *, integer *, doublereal *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLAHRD reduces the first NB columns of a real general n-by-(n-k+1)
    matrix A so that elements below the k-th subdiagonal are zero. The
    reduction is performed by an orthogonal similarity transformation
    Q' * A * Q. The routine returns the matrices V and T which determine
    Q as a block reflector I - V*T*V', and also the matrix Y = A * V * T.

    This is an auxiliary routine called by DGEHRD.

    Arguments
    =========

    N       (input) INTEGER
            The order of the matrix A.

    K       (input) INTEGER
            The offset for the reduction. Elements below the k-th
            subdiagonal in the first NB columns are reduced to zero.

    NB      (input) INTEGER
            The number of columns to be reduced.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N-K+1)
            On entry, the n-by-(n-k+1) general matrix A.
            On exit, the elements on and above the k-th subdiagonal in
            the first NB columns are overwritten with the corresponding
            elements of the reduced matrix; the elements below the k-th
            subdiagonal, with the array TAU, represent the matrix Q as a
            product of elementary reflectors. The other columns of A are
            unchanged. See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    TAU     (output) DOUBLE PRECISION array, dimension (NB)
            The scalar factors of the elementary reflectors. See Further
            Details.

    T       (output) DOUBLE PRECISION array, dimension (LDT,NB)
            The upper triangular matrix T.

    LDT     (input) INTEGER
            The leading dimension of the array T.  LDT >= NB.

    Y       (output) DOUBLE PRECISION array, dimension (LDY,NB)
            The n-by-nb matrix Y.

    LDY     (input) INTEGER
            The leading dimension of the array Y. LDY >= N.

    Further Details
    ===============

    The matrix Q is represented as a product of nb elementary reflectors

       Q = H(1) H(2) . . . H(nb).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i+k-1) = 0, v(i+k) = 1; v(i+k+1:n) is stored on exit in
    A(i+k+1:n,i), and tau in TAU(i).

    The elements of the vectors v together form the (n-k+1)-by-nb matrix
    V which is needed, with T and Y, to apply the transformation to the
    unreduced part of the matrix, using an update of the form:
    A := (I - V*T*V') * (A - Y*V').

    The contents of A on exit are illustrated by the following example
    with n = 7, k = 3 and nb = 2:

       ( a   h   a   a   a )
       ( a   h   a   a   a )
       ( a   h   a   a   a )
       ( h   h   a   a   a )
       ( v1  h   a   a   a )
       ( v1  v2  a   a   a )
       ( v1  v2  a   a   a )

    where a denotes an element of the original matrix A, h denotes a
    modified element of the upper Hessenberg matrix H, and vi denotes an
    element of the vector defining H(i).

    =====================================================================


       Quick return if possible
*/

    /* Parameter adjustments */
    --tau;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1 * 1;
    t -= t_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1 * 1;
    y -= y_offset;

    /* Function Body */
    if (*n <= 1) {
	return 0;
    }

    i__1 = *nb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (i__ > 1) {

/*
             Update A(1:n,i)

             Compute i-th column of A - Y * V'
*/

	    i__2 = i__ - 1;
	    dgemv_("No transpose", n, &i__2, &c_b151, &y[y_offset], ldy, &a[*
		    k + i__ - 1 + a_dim1], lda, &c_b15, &a[i__ * a_dim1 + 1],
		    &c__1);

/*
             Apply I - V * T' * V' to this column (call it b) from the
             left, using the last column of T as workspace

             Let  V = ( V1 )   and   b = ( b1 )   (first I-1 rows)
                      ( V2 )             ( b2 )

             where V1 is unit lower triangular

             w := V1' * b1
*/

	    i__2 = i__ - 1;
	    dcopy_(&i__2, &a[*k + 1 + i__ * a_dim1], &c__1, &t[*nb * t_dim1 +
		    1], &c__1);
	    i__2 = i__ - 1;
	    dtrmv_("Lower", "Transpose", "Unit", &i__2, &a[*k + 1 + a_dim1],
		    lda, &t[*nb * t_dim1 + 1], &c__1);

/*           w := w + V2'*b2 */

	    i__2 = *n - *k - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[*k + i__ + a_dim1],
		    lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b15, &t[*nb *
		    t_dim1 + 1], &c__1);

/*           w := T'*w */

	    i__2 = i__ - 1;
	    dtrmv_("Upper", "Transpose", "Non-unit", &i__2, &t[t_offset], ldt,
		     &t[*nb * t_dim1 + 1], &c__1);

/*           b2 := b2 - V2*w */

	    i__2 = *n - *k - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[*k + i__ +
		    a_dim1], lda, &t[*nb * t_dim1 + 1], &c__1, &c_b15, &a[*k
		    + i__ + i__ * a_dim1], &c__1);

/*           b1 := b1 - V1*w */

	    i__2 = i__ - 1;
	    dtrmv_("Lower", "No transpose", "Unit", &i__2, &a[*k + 1 + a_dim1]
		    , lda, &t[*nb * t_dim1 + 1], &c__1);
	    i__2 = i__ - 1;
	    daxpy_(&i__2, &c_b151, &t[*nb * t_dim1 + 1], &c__1, &a[*k + 1 +
		    i__ * a_dim1], &c__1);

	    a[*k + i__ - 1 + (i__ - 1) * a_dim1] = ei;
	}

/*
          Generate the elementary reflector H(i) to annihilate
          A(k+i+1:n,i)
*/

	i__2 = *n - *k - i__ + 1;
/* Computing MIN */
	i__3 = *k + i__ + 1;
	dlarfg_(&i__2, &a[*k + i__ + i__ * a_dim1], &a[min(i__3,*n) + i__ *
		a_dim1], &c__1, &tau[i__]);
	ei = a[*k + i__ + i__ * a_dim1];
	a[*k + i__ + i__ * a_dim1] = 1.;

/*        Compute  Y(1:n,i) */

	i__2 = *n - *k - i__ + 1;
	dgemv_("No transpose", n, &i__2, &c_b15, &a[(i__ + 1) * a_dim1 + 1],
		lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b29, &y[i__ *
		y_dim1 + 1], &c__1);
	i__2 = *n - *k - i__ + 1;
	i__3 = i__ - 1;
	dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[*k + i__ + a_dim1], lda,
		&a[*k + i__ + i__ * a_dim1], &c__1, &c_b29, &t[i__ * t_dim1 +
		1], &c__1);
	i__2 = i__ - 1;
	dgemv_("No transpose", n, &i__2, &c_b151, &y[y_offset], ldy, &t[i__ *
		t_dim1 + 1], &c__1, &c_b15, &y[i__ * y_dim1 + 1], &c__1);
	dscal_(n, &tau[i__], &y[i__ * y_dim1 + 1], &c__1);

/*        Compute T(1:i,i) */

	i__2 = i__ - 1;
	d__1 = -tau[i__];
	dscal_(&i__2, &d__1, &t[i__ * t_dim1 + 1], &c__1);
	i__2 = i__ - 1;
	dtrmv_("Upper", "No transpose", "Non-unit", &i__2, &t[t_offset], ldt,
		&t[i__ * t_dim1 + 1], &c__1)
		;
	t[i__ + i__ * t_dim1] = tau[i__];

/* L10: */
    }
    a[*k + *nb + *nb * a_dim1] = ei;

    return 0;

/*     End of DLAHRD */

} /* dlahrd_ */

/* Subroutine */ int dlaln2_(logical *ltrans, integer *na, integer *nw,
	doublereal *smin, doublereal *ca, doublereal *a, integer *lda,
	doublereal *d1, doublereal *d2, doublereal *b, integer *ldb,
	doublereal *wr, doublereal *wi, doublereal *x, integer *ldx,
	doublereal *scale, doublereal *xnorm, integer *info)
{
    /* Initialized data */

    static logical zswap[4] = { FALSE_,FALSE_,TRUE_,TRUE_ };
    static logical rswap[4] = { FALSE_,TRUE_,FALSE_,TRUE_ };
    static integer ipivot[16]	/* was [4][4] */ = { 1,2,3,4,2,1,4,3,3,4,1,2,
	    4,3,2,1 };

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6;
    static doublereal equiv_0[4], equiv_1[4];

    /* Local variables */
    static integer j;
#define ci (equiv_0)
#define cr (equiv_1)
    static doublereal bi1, bi2, br1, br2, xi1, xi2, xr1, xr2, ci21, ci22,
	    cr21, cr22, li21, csi, ui11, lr21, ui12, ui22;
#define civ (equiv_0)
    static doublereal csr, ur11, ur12, ur22;
#define crv (equiv_1)
    static doublereal bbnd, cmax, ui11r, ui12s, temp, ur11r, ur12s, u22abs;
    static integer icmax;
    static doublereal bnorm, cnorm, smini;

    extern /* Subroutine */ int dladiv_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *);
    static doublereal bignum, smlnum;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLALN2 solves a system of the form  (ca A - w D ) X = s B
    or (ca A' - w D) X = s B   with possible scaling ("s") and
    perturbation of A.  (A' means A-transpose.)

    A is an NA x NA real matrix, ca is a real scalar, D is an NA x NA
    real diagonal matrix, w is a real or complex value, and X and B are
    NA x 1 matrices -- real if w is real, complex if w is complex.  NA
    may be 1 or 2.

    If w is complex, X and B are represented as NA x 2 matrices,
    the first column of each being the real part and the second
    being the imaginary part.

    "s" is a scaling factor (.LE. 1), computed by DLALN2, which is
    so chosen that X can be computed without overflow.  X is further
    scaled if necessary to assure that norm(ca A - w D)*norm(X) is less
    than overflow.

    If both singular values of (ca A - w D) are less than SMIN,
    SMIN*identity will be used instead of (ca A - w D).  If only one
    singular value is less than SMIN, one element of (ca A - w D) will be
    perturbed enough to make the smallest singular value roughly SMIN.
    If both singular values are at least SMIN, (ca A - w D) will not be
    perturbed.  In any case, the perturbation will be at most some small
    multiple of max( SMIN, ulp*norm(ca A - w D) ).  The singular values
    are computed by infinity-norm approximations, and thus will only be
    correct to a factor of 2 or so.

    Note: all input quantities are assumed to be smaller than overflow
    by a reasonable factor.  (See BIGNUM.)

    Arguments
    ==========

    LTRANS  (input) LOGICAL
            =.TRUE.:  A-transpose will be used.
            =.FALSE.: A will be used (not transposed.)

    NA      (input) INTEGER
            The size of the matrix A.  It may (only) be 1 or 2.

    NW      (input) INTEGER
            1 if "w" is real, 2 if "w" is complex.  It may only be 1
            or 2.

    SMIN    (input) DOUBLE PRECISION
            The desired lower bound on the singular values of A.  This
            should be a safe distance away from underflow or overflow,
            say, between (underflow/machine precision) and  (machine
            precision * overflow ).  (See BIGNUM and ULP.)

    CA      (input) DOUBLE PRECISION
            The coefficient c, which A is multiplied by.

    A       (input) DOUBLE PRECISION array, dimension (LDA,NA)
            The NA x NA matrix A.

    LDA     (input) INTEGER
            The leading dimension of A.  It must be at least NA.

    D1      (input) DOUBLE PRECISION
            The 1,1 element in the diagonal matrix D.

    D2      (input) DOUBLE PRECISION
            The 2,2 element in the diagonal matrix D.  Not used if NW=1.

    B       (input) DOUBLE PRECISION array, dimension (LDB,NW)
            The NA x NW matrix B (right-hand side).  If NW=2 ("w" is
            complex), column 1 contains the real part of B and column 2
            contains the imaginary part.

    LDB     (input) INTEGER
            The leading dimension of B.  It must be at least NA.

    WR      (input) DOUBLE PRECISION
            The real part of the scalar "w".

    WI      (input) DOUBLE PRECISION
            The imaginary part of the scalar "w".  Not used if NW=1.

    X       (output) DOUBLE PRECISION array, dimension (LDX,NW)
            The NA x NW matrix X (unknowns), as computed by DLALN2.
            If NW=2 ("w" is complex), on exit, column 1 will contain
            the real part of X and column 2 will contain the imaginary
            part.

    LDX     (input) INTEGER
            The leading dimension of X.  It must be at least NA.

    SCALE   (output) DOUBLE PRECISION
            The scale factor that B must be multiplied by to insure
            that overflow does not occur when computing X.  Thus,
            (ca A - w D) X  will be SCALE*B, not B (ignoring
            perturbations of A.)  It will be at most 1.

    XNORM   (output) DOUBLE PRECISION
            The infinity-norm of X, when X is regarded as an NA x NW
            real matrix.

    INFO    (output) INTEGER
            An error flag.  It will be set to zero if no error occurs,
            a negative number if an argument is in error, or a positive
            number if  ca A - w D  had to be perturbed.
            The possible values are:
            = 0: No error occurred, and (ca A - w D) did not have to be
                   perturbed.
            = 1: (ca A - w D) had to be perturbed to make its smallest
                 (or only) singular value greater than SMIN.
            NOTE: In the interests of speed, this routine does not
                  check the inputs for errors.

   =====================================================================
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1 * 1;
    x -= x_offset;

    /* Function Body */

/*     Compute BIGNUM */

    smlnum = 2. * SAFEMINIMUM;
    bignum = 1. / smlnum;
    smini = max(*smin,smlnum);

/*     Don't check for input errors */

    *info = 0;

/*     Standard Initializations */

    *scale = 1.;

    if (*na == 1) {

/*        1 x 1  (i.e., scalar) system   C X = B */

	if (*nw == 1) {

/*
             Real 1x1 system.

             C = ca A - w D
*/

	    csr = *ca * a[a_dim1 + 1] - *wr * *d1;
	    cnorm = abs(csr);

/*           If | C | < SMINI, use C = SMINI */

	    if (cnorm < smini) {
		csr = smini;
		cnorm = smini;
		*info = 1;
	    }

/*           Check scaling for  X = B / C */

	    bnorm = (d__1 = b[b_dim1 + 1], abs(d__1));
	    if ((cnorm < 1. && bnorm > 1.)) {
		if (bnorm > bignum * cnorm) {
		    *scale = 1. / bnorm;
		}
	    }

/*           Compute X */

	    x[x_dim1 + 1] = b[b_dim1 + 1] * *scale / csr;
	    *xnorm = (d__1 = x[x_dim1 + 1], abs(d__1));
	} else {

/*
             Complex 1x1 system (w is complex)

             C = ca A - w D
*/

	    csr = *ca * a[a_dim1 + 1] - *wr * *d1;
	    csi = -(*wi) * *d1;
	    cnorm = abs(csr) + abs(csi);

/*           If | C | < SMINI, use C = SMINI */

	    if (cnorm < smini) {
		csr = smini;
		csi = 0.;
		cnorm = smini;
		*info = 1;
	    }

/*           Check scaling for  X = B / C */

	    bnorm = (d__1 = b[b_dim1 + 1], abs(d__1)) + (d__2 = b[((b_dim1) <<
		     (1)) + 1], abs(d__2));
	    if ((cnorm < 1. && bnorm > 1.)) {
		if (bnorm > bignum * cnorm) {
		    *scale = 1. / bnorm;
		}
	    }

/*           Compute X */

	    d__1 = *scale * b[b_dim1 + 1];
	    d__2 = *scale * b[((b_dim1) << (1)) + 1];
	    dladiv_(&d__1, &d__2, &csr, &csi, &x[x_dim1 + 1], &x[((x_dim1) <<
		    (1)) + 1]);
	    *xnorm = (d__1 = x[x_dim1 + 1], abs(d__1)) + (d__2 = x[((x_dim1)
		    << (1)) + 1], abs(d__2));
	}

    } else {

/*
          2x2 System

          Compute the real part of  C = ca A - w D  (or  ca A' - w D )
*/

	cr[0] = *ca * a[a_dim1 + 1] - *wr * *d1;
	cr[3] = *ca * a[((a_dim1) << (1)) + 2] - *wr * *d2;
	if (*ltrans) {
	    cr[2] = *ca * a[a_dim1 + 2];
	    cr[1] = *ca * a[((a_dim1) << (1)) + 1];
	} else {
	    cr[1] = *ca * a[a_dim1 + 2];
	    cr[2] = *ca * a[((a_dim1) << (1)) + 1];
	}

	if (*nw == 1) {

/*
             Real 2x2 system  (w is real)

             Find the largest element in C
*/

	    cmax = 0.;
	    icmax = 0;

	    for (j = 1; j <= 4; ++j) {
		if ((d__1 = crv[j - 1], abs(d__1)) > cmax) {
		    cmax = (d__1 = crv[j - 1], abs(d__1));
		    icmax = j;
		}
/* L10: */
	    }

/*           If norm(C) < SMINI, use SMINI*identity. */

	    if (cmax < smini) {
/* Computing MAX */
		d__3 = (d__1 = b[b_dim1 + 1], abs(d__1)), d__4 = (d__2 = b[
			b_dim1 + 2], abs(d__2));
		bnorm = max(d__3,d__4);
		if ((smini < 1. && bnorm > 1.)) {
		    if (bnorm > bignum * smini) {
			*scale = 1. / bnorm;
		    }
		}
		temp = *scale / smini;
		x[x_dim1 + 1] = temp * b[b_dim1 + 1];
		x[x_dim1 + 2] = temp * b[b_dim1 + 2];
		*xnorm = temp * bnorm;
		*info = 1;
		return 0;
	    }

/*           Gaussian elimination with complete pivoting. */

	    ur11 = crv[icmax - 1];
	    cr21 = crv[ipivot[((icmax) << (2)) - 3] - 1];
	    ur12 = crv[ipivot[((icmax) << (2)) - 2] - 1];
	    cr22 = crv[ipivot[((icmax) << (2)) - 1] - 1];
	    ur11r = 1. / ur11;
	    lr21 = ur11r * cr21;
	    ur22 = cr22 - ur12 * lr21;

/*           If smaller pivot < SMINI, use SMINI */

	    if (abs(ur22) < smini) {
		ur22 = smini;
		*info = 1;
	    }
	    if (rswap[icmax - 1]) {
		br1 = b[b_dim1 + 2];
		br2 = b[b_dim1 + 1];
	    } else {
		br1 = b[b_dim1 + 1];
		br2 = b[b_dim1 + 2];
	    }
	    br2 -= lr21 * br1;
/* Computing MAX */
	    d__2 = (d__1 = br1 * (ur22 * ur11r), abs(d__1)), d__3 = abs(br2);
	    bbnd = max(d__2,d__3);
	    if ((bbnd > 1. && abs(ur22) < 1.)) {
		if (bbnd >= bignum * abs(ur22)) {
		    *scale = 1. / bbnd;
		}
	    }

	    xr2 = br2 * *scale / ur22;
	    xr1 = *scale * br1 * ur11r - xr2 * (ur11r * ur12);
	    if (zswap[icmax - 1]) {
		x[x_dim1 + 1] = xr2;
		x[x_dim1 + 2] = xr1;
	    } else {
		x[x_dim1 + 1] = xr1;
		x[x_dim1 + 2] = xr2;
	    }
/* Computing MAX */
	    d__1 = abs(xr1), d__2 = abs(xr2);
	    *xnorm = max(d__1,d__2);

/*           Further scaling if  norm(A) norm(X) > overflow */

	    if ((*xnorm > 1. && cmax > 1.)) {
		if (*xnorm > bignum / cmax) {
		    temp = cmax / bignum;
		    x[x_dim1 + 1] = temp * x[x_dim1 + 1];
		    x[x_dim1 + 2] = temp * x[x_dim1 + 2];
		    *xnorm = temp * *xnorm;
		    *scale = temp * *scale;
		}
	    }
	} else {

/*
             Complex 2x2 system  (w is complex)

             Find the largest element in C
*/

	    ci[0] = -(*wi) * *d1;
	    ci[1] = 0.;
	    ci[2] = 0.;
	    ci[3] = -(*wi) * *d2;
	    cmax = 0.;
	    icmax = 0;

	    for (j = 1; j <= 4; ++j) {
		if ((d__1 = crv[j - 1], abs(d__1)) + (d__2 = civ[j - 1], abs(
			d__2)) > cmax) {
		    cmax = (d__1 = crv[j - 1], abs(d__1)) + (d__2 = civ[j - 1]
			    , abs(d__2));
		    icmax = j;
		}
/* L20: */
	    }

/*           If norm(C) < SMINI, use SMINI*identity. */

	    if (cmax < smini) {
/* Computing MAX */
		d__5 = (d__1 = b[b_dim1 + 1], abs(d__1)) + (d__2 = b[((b_dim1)
			 << (1)) + 1], abs(d__2)), d__6 = (d__3 = b[b_dim1 +
			2], abs(d__3)) + (d__4 = b[((b_dim1) << (1)) + 2],
			abs(d__4));
		bnorm = max(d__5,d__6);
		if ((smini < 1. && bnorm > 1.)) {
		    if (bnorm > bignum * smini) {
			*scale = 1. / bnorm;
		    }
		}
		temp = *scale / smini;
		x[x_dim1 + 1] = temp * b[b_dim1 + 1];
		x[x_dim1 + 2] = temp * b[b_dim1 + 2];
		x[((x_dim1) << (1)) + 1] = temp * b[((b_dim1) << (1)) + 1];
		x[((x_dim1) << (1)) + 2] = temp * b[((b_dim1) << (1)) + 2];
		*xnorm = temp * bnorm;
		*info = 1;
		return 0;
	    }

/*           Gaussian elimination with complete pivoting. */

	    ur11 = crv[icmax - 1];
	    ui11 = civ[icmax - 1];
	    cr21 = crv[ipivot[((icmax) << (2)) - 3] - 1];
	    ci21 = civ[ipivot[((icmax) << (2)) - 3] - 1];
	    ur12 = crv[ipivot[((icmax) << (2)) - 2] - 1];
	    ui12 = civ[ipivot[((icmax) << (2)) - 2] - 1];
	    cr22 = crv[ipivot[((icmax) << (2)) - 1] - 1];
	    ci22 = civ[ipivot[((icmax) << (2)) - 1] - 1];
	    if (icmax == 1 || icmax == 4) {

/*              Code when off-diagonals of pivoted C are real */

		if (abs(ur11) > abs(ui11)) {
		    temp = ui11 / ur11;
/* Computing 2nd power */
		    d__1 = temp;
		    ur11r = 1. / (ur11 * (d__1 * d__1 + 1.));
		    ui11r = -temp * ur11r;
		} else {
		    temp = ur11 / ui11;
/* Computing 2nd power */
		    d__1 = temp;
		    ui11r = -1. / (ui11 * (d__1 * d__1 + 1.));
		    ur11r = -temp * ui11r;
		}
		lr21 = cr21 * ur11r;
		li21 = cr21 * ui11r;
		ur12s = ur12 * ur11r;
		ui12s = ur12 * ui11r;
		ur22 = cr22 - ur12 * lr21;
		ui22 = ci22 - ur12 * li21;
	    } else {

/*              Code when diagonals of pivoted C are real */

		ur11r = 1. / ur11;
		ui11r = 0.;
		lr21 = cr21 * ur11r;
		li21 = ci21 * ur11r;
		ur12s = ur12 * ur11r;
		ui12s = ui12 * ur11r;
		ur22 = cr22 - ur12 * lr21 + ui12 * li21;
		ui22 = -ur12 * li21 - ui12 * lr21;
	    }
	    u22abs = abs(ur22) + abs(ui22);

/*           If smaller pivot < SMINI, use SMINI */

	    if (u22abs < smini) {
		ur22 = smini;
		ui22 = 0.;
		*info = 1;
	    }
	    if (rswap[icmax - 1]) {
		br2 = b[b_dim1 + 1];
		br1 = b[b_dim1 + 2];
		bi2 = b[((b_dim1) << (1)) + 1];
		bi1 = b[((b_dim1) << (1)) + 2];
	    } else {
		br1 = b[b_dim1 + 1];
		br2 = b[b_dim1 + 2];
		bi1 = b[((b_dim1) << (1)) + 1];
		bi2 = b[((b_dim1) << (1)) + 2];
	    }
	    br2 = br2 - lr21 * br1 + li21 * bi1;
	    bi2 = bi2 - li21 * br1 - lr21 * bi1;
/* Computing MAX */
	    d__1 = (abs(br1) + abs(bi1)) * (u22abs * (abs(ur11r) + abs(ui11r))
		    ), d__2 = abs(br2) + abs(bi2);
	    bbnd = max(d__1,d__2);
	    if ((bbnd > 1. && u22abs < 1.)) {
		if (bbnd >= bignum * u22abs) {
		    *scale = 1. / bbnd;
		    br1 = *scale * br1;
		    bi1 = *scale * bi1;
		    br2 = *scale * br2;
		    bi2 = *scale * bi2;
		}
	    }

	    dladiv_(&br2, &bi2, &ur22, &ui22, &xr2, &xi2);
	    xr1 = ur11r * br1 - ui11r * bi1 - ur12s * xr2 + ui12s * xi2;
	    xi1 = ui11r * br1 + ur11r * bi1 - ui12s * xr2 - ur12s * xi2;
	    if (zswap[icmax - 1]) {
		x[x_dim1 + 1] = xr2;
		x[x_dim1 + 2] = xr1;
		x[((x_dim1) << (1)) + 1] = xi2;
		x[((x_dim1) << (1)) + 2] = xi1;
	    } else {
		x[x_dim1 + 1] = xr1;
		x[x_dim1 + 2] = xr2;
		x[((x_dim1) << (1)) + 1] = xi1;
		x[((x_dim1) << (1)) + 2] = xi2;
	    }
/* Computing MAX */
	    d__1 = abs(xr1) + abs(xi1), d__2 = abs(xr2) + abs(xi2);
	    *xnorm = max(d__1,d__2);

/*           Further scaling if  norm(A) norm(X) > overflow */

	    if ((*xnorm > 1. && cmax > 1.)) {
		if (*xnorm > bignum / cmax) {
		    temp = cmax / bignum;
		    x[x_dim1 + 1] = temp * x[x_dim1 + 1];
		    x[x_dim1 + 2] = temp * x[x_dim1 + 2];
		    x[((x_dim1) << (1)) + 1] = temp * x[((x_dim1) << (1)) + 1]
			    ;
		    x[((x_dim1) << (1)) + 2] = temp * x[((x_dim1) << (1)) + 2]
			    ;
		    *xnorm = temp * *xnorm;
		    *scale = temp * *scale;
		}
	    }
	}
    }

    return 0;

/*     End of DLALN2 */

} /* dlaln2_ */

#undef crv
#undef civ
#undef cr
#undef ci


/* Subroutine */ int dlals0_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, integer *nrhs, doublereal *b, integer *ldb, doublereal
	*bx, integer *ldbx, integer *perm, integer *givptr, integer *givcol,
	integer *ldgcol, doublereal *givnum, integer *ldgnum, doublereal *
	poles, doublereal *difl, doublereal *difr, doublereal *z__, integer *
	k, doublereal *c__, doublereal *s, doublereal *work, integer *info)
{
    /* System generated locals */
    integer givcol_dim1, givcol_offset, b_dim1, b_offset, bx_dim1, bx_offset,
	    difr_dim1, difr_offset, givnum_dim1, givnum_offset, poles_dim1,
	    poles_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer i__, j, m, n;
    static doublereal dj;
    static integer nlp1;
    static doublereal temp;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    static doublereal diflj, difrj, dsigj;
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *), dcopy_(integer *,
	    doublereal *, integer *, doublereal *, integer *);
    extern doublereal dlamc3_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *), dlacpy_(char *, integer *, integer
	    *, doublereal *, integer *, doublereal *, integer *),
	    xerbla_(char *, integer *);
    static doublereal dsigjp;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       December 1, 1999


    Purpose
    =======

    DLALS0 applies back the multiplying factors of either the left or the
    right singular vector matrix of a diagonal matrix appended by a row
    to the right hand side matrix B in solving the least squares problem
    using the divide-and-conquer SVD approach.

    For the left singular vector matrix, three types of orthogonal
    matrices are involved:

    (1L) Givens rotations: the number of such rotations is GIVPTR; the
         pairs of columns/rows they were applied to are stored in GIVCOL;
         and the C- and S-values of these rotations are stored in GIVNUM.

    (2L) Permutation. The (NL+1)-st row of B is to be moved to the first
         row, and for J=2:N, PERM(J)-th row of B is to be moved to the
         J-th row.

    (3L) The left singular vector matrix of the remaining matrix.

    For the right singular vector matrix, four types of orthogonal
    matrices are involved:

    (1R) The right singular vector matrix of the remaining matrix.

    (2R) If SQRE = 1, one extra Givens rotation to generate the right
         null space.

    (3R) The inverse transformation of (2L).

    (4R) The inverse transformation of (1L).

    Arguments
    =========

    ICOMPQ (input) INTEGER
           Specifies whether singular vectors are to be computed in
           factored form:
           = 0: Left singular vector matrix.
           = 1: Right singular vector matrix.

    NL     (input) INTEGER
           The row dimension of the upper block. NL >= 1.

    NR     (input) INTEGER
           The row dimension of the lower block. NR >= 1.

    SQRE   (input) INTEGER
           = 0: the lower block is an NR-by-NR square matrix.
           = 1: the lower block is an NR-by-(NR+1) rectangular matrix.

           The bidiagonal matrix has row dimension N = NL + NR + 1,
           and column dimension M = N + SQRE.

    NRHS   (input) INTEGER
           The number of columns of B and BX. NRHS must be at least 1.

    B      (input/output) DOUBLE PRECISION array, dimension ( LDB, NRHS )
           On input, B contains the right hand sides of the least
           squares problem in rows 1 through M. On output, B contains
           the solution X in rows 1 through N.

    LDB    (input) INTEGER
           The leading dimension of B. LDB must be at least
           max(1,MAX( M, N ) ).

    BX     (workspace) DOUBLE PRECISION array, dimension ( LDBX, NRHS )

    LDBX   (input) INTEGER
           The leading dimension of BX.

    PERM   (input) INTEGER array, dimension ( N )
           The permutations (from deflation and sorting) applied
           to the two blocks.

    GIVPTR (input) INTEGER
           The number of Givens rotations which took place in this
           subproblem.

    GIVCOL (input) INTEGER array, dimension ( LDGCOL, 2 )
           Each pair of numbers indicates a pair of rows/columns
           involved in a Givens rotation.

    LDGCOL (input) INTEGER
           The leading dimension of GIVCOL, must be at least N.

    GIVNUM (input) DOUBLE PRECISION array, dimension ( LDGNUM, 2 )
           Each number indicates the C or S value used in the
           corresponding Givens rotation.

    LDGNUM (input) INTEGER
           The leading dimension of arrays DIFR, POLES and
           GIVNUM, must be at least K.

    POLES  (input) DOUBLE PRECISION array, dimension ( LDGNUM, 2 )
           On entry, POLES(1:K, 1) contains the new singular
           values obtained from solving the secular equation, and
           POLES(1:K, 2) is an array containing the poles in the secular
           equation.

    DIFL   (input) DOUBLE PRECISION array, dimension ( K ).
           On entry, DIFL(I) is the distance between I-th updated
           (undeflated) singular value and the I-th (undeflated) old
           singular value.

    DIFR   (input) DOUBLE PRECISION array, dimension ( LDGNUM, 2 ).
           On entry, DIFR(I, 1) contains the distances between I-th
           updated (undeflated) singular value and the I+1-th
           (undeflated) old singular value. And DIFR(I, 2) is the
           normalizing factor for the I-th right singular vector.

    Z      (input) DOUBLE PRECISION array, dimension ( K )
           Contain the components of the deflation-adjusted updating row
           vector.

    K      (input) INTEGER
           Contains the dimension of the non-deflated matrix,
           This is the order of the related secular equation. 1 <= K <=N.

    C      (input) DOUBLE PRECISION
           C contains garbage if SQRE =0 and the C-value of a Givens
           rotation related to the right null space if SQRE = 1.

    S      (input) DOUBLE PRECISION
           S contains garbage if SQRE =0 and the S-value of a Givens
           rotation related to the right null space if SQRE = 1.

    WORK   (workspace) DOUBLE PRECISION array, dimension ( K )

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Ren-Cang Li, Computer Science Division, University of
         California at Berkeley, USA
       Osni Marques, LBNL/NERSC, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    bx_dim1 = *ldbx;
    bx_offset = 1 + bx_dim1 * 1;
    bx -= bx_offset;
    --perm;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1 * 1;
    givcol -= givcol_offset;
    difr_dim1 = *ldgnum;
    difr_offset = 1 + difr_dim1 * 1;
    difr -= difr_offset;
    poles_dim1 = *ldgnum;
    poles_offset = 1 + poles_dim1 * 1;
    poles -= poles_offset;
    givnum_dim1 = *ldgnum;
    givnum_offset = 1 + givnum_dim1 * 1;
    givnum -= givnum_offset;
    --difl;
    --z__;
    --work;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*nl < 1) {
	*info = -2;
    } else if (*nr < 1) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    }

    n = *nl + *nr + 1;

    if (*nrhs < 1) {
	*info = -5;
    } else if (*ldb < n) {
	*info = -7;
    } else if (*ldbx < n) {
	*info = -9;
    } else if (*givptr < 0) {
	*info = -11;
    } else if (*ldgcol < n) {
	*info = -13;
    } else if (*ldgnum < n) {
	*info = -15;
    } else if (*k < 1) {
	*info = -20;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLALS0", &i__1);
	return 0;
    }

    m = n + *sqre;
    nlp1 = *nl + 1;

    if (*icompq == 0) {

/*
          Apply back orthogonal transformations from the left.

          Step (1L): apply back the Givens rotations performed.
*/

	i__1 = *givptr;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    drot_(nrhs, &b[givcol[i__ + ((givcol_dim1) << (1))] + b_dim1],
		    ldb, &b[givcol[i__ + givcol_dim1] + b_dim1], ldb, &givnum[
		    i__ + ((givnum_dim1) << (1))], &givnum[i__ + givnum_dim1])
		    ;
/* L10: */
	}

/*        Step (2L): permute rows of B. */

	dcopy_(nrhs, &b[nlp1 + b_dim1], ldb, &bx[bx_dim1 + 1], ldbx);
	i__1 = n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    dcopy_(nrhs, &b[perm[i__] + b_dim1], ldb, &bx[i__ + bx_dim1],
		    ldbx);
/* L20: */
	}

/*
          Step (3L): apply the inverse of the left singular vector
          matrix to BX.
*/

	if (*k == 1) {
	    dcopy_(nrhs, &bx[bx_offset], ldbx, &b[b_offset], ldb);
	    if (z__[1] < 0.) {
		dscal_(nrhs, &c_b151, &b[b_offset], ldb);
	    }
	} else {
	    i__1 = *k;
	    for (j = 1; j <= i__1; ++j) {
		diflj = difl[j];
		dj = poles[j + poles_dim1];
		dsigj = -poles[j + ((poles_dim1) << (1))];
		if (j < *k) {
		    difrj = -difr[j + difr_dim1];
		    dsigjp = -poles[j + 1 + ((poles_dim1) << (1))];
		}
		if (z__[j] == 0. || poles[j + ((poles_dim1) << (1))] == 0.) {
		    work[j] = 0.;
		} else {
		    work[j] = -poles[j + ((poles_dim1) << (1))] * z__[j] /
			    diflj / (poles[j + ((poles_dim1) << (1))] + dj);
		}
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    if (z__[i__] == 0. || poles[i__ + ((poles_dim1) << (1))]
			    == 0.) {
			work[i__] = 0.;
		    } else {
			work[i__] = poles[i__ + ((poles_dim1) << (1))] * z__[
				i__] / (dlamc3_(&poles[i__ + ((poles_dim1) <<
				(1))], &dsigj) - diflj) / (poles[i__ + ((
				poles_dim1) << (1))] + dj);
		    }
/* L30: */
		}
		i__2 = *k;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    if (z__[i__] == 0. || poles[i__ + ((poles_dim1) << (1))]
			    == 0.) {
			work[i__] = 0.;
		    } else {
			work[i__] = poles[i__ + ((poles_dim1) << (1))] * z__[
				i__] / (dlamc3_(&poles[i__ + ((poles_dim1) <<
				(1))], &dsigjp) + difrj) / (poles[i__ + ((
				poles_dim1) << (1))] + dj);
		    }
/* L40: */
		}
		work[1] = -1.;
		temp = dnrm2_(k, &work[1], &c__1);
		dgemv_("T", k, nrhs, &c_b15, &bx[bx_offset], ldbx, &work[1], &
			c__1, &c_b29, &b[j + b_dim1], ldb);
		dlascl_("G", &c__0, &c__0, &temp, &c_b15, &c__1, nrhs, &b[j +
			b_dim1], ldb, info);
/* L50: */
	    }
	}

/*        Move the deflated rows of BX to B also. */

	if (*k < max(m,n)) {
	    i__1 = n - *k;
	    dlacpy_("A", &i__1, nrhs, &bx[*k + 1 + bx_dim1], ldbx, &b[*k + 1
		    + b_dim1], ldb);
	}
    } else {

/*
          Apply back the right orthogonal transformations.

          Step (1R): apply back the new right singular vector matrix
          to B.
*/

	if (*k == 1) {
	    dcopy_(nrhs, &b[b_offset], ldb, &bx[bx_offset], ldbx);
	} else {
	    i__1 = *k;
	    for (j = 1; j <= i__1; ++j) {
		dsigj = poles[j + ((poles_dim1) << (1))];
		if (z__[j] == 0.) {
		    work[j] = 0.;
		} else {
		    work[j] = -z__[j] / difl[j] / (dsigj + poles[j +
			    poles_dim1]) / difr[j + ((difr_dim1) << (1))];
		}
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    if (z__[j] == 0.) {
			work[i__] = 0.;
		    } else {
			d__1 = -poles[i__ + 1 + ((poles_dim1) << (1))];
			work[i__] = z__[j] / (dlamc3_(&dsigj, &d__1) - difr[
				i__ + difr_dim1]) / (dsigj + poles[i__ +
				poles_dim1]) / difr[i__ + ((difr_dim1) << (1))
				];
		    }
/* L60: */
		}
		i__2 = *k;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    if (z__[j] == 0.) {
			work[i__] = 0.;
		    } else {
			d__1 = -poles[i__ + ((poles_dim1) << (1))];
			work[i__] = z__[j] / (dlamc3_(&dsigj, &d__1) - difl[
				i__]) / (dsigj + poles[i__ + poles_dim1]) /
				difr[i__ + ((difr_dim1) << (1))];
		    }
/* L70: */
		}
		dgemv_("T", k, nrhs, &c_b15, &b[b_offset], ldb, &work[1], &
			c__1, &c_b29, &bx[j + bx_dim1], ldbx);
/* L80: */
	    }
	}

/*
          Step (2R): if SQRE = 1, apply back the rotation that is
          related to the right null space of the subproblem.
*/

	if (*sqre == 1) {
	    dcopy_(nrhs, &b[m + b_dim1], ldb, &bx[m + bx_dim1], ldbx);
	    drot_(nrhs, &bx[bx_dim1 + 1], ldbx, &bx[m + bx_dim1], ldbx, c__,
		    s);
	}
	if (*k < max(m,n)) {
	    i__1 = n - *k;
	    dlacpy_("A", &i__1, nrhs, &b[*k + 1 + b_dim1], ldb, &bx[*k + 1 +
		    bx_dim1], ldbx);
	}

/*        Step (3R): permute rows of B. */

	dcopy_(nrhs, &bx[bx_dim1 + 1], ldbx, &b[nlp1 + b_dim1], ldb);
	if (*sqre == 1) {
	    dcopy_(nrhs, &bx[m + bx_dim1], ldbx, &b[m + b_dim1], ldb);
	}
	i__1 = n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    dcopy_(nrhs, &bx[i__ + bx_dim1], ldbx, &b[perm[i__] + b_dim1],
		    ldb);
/* L90: */
	}

/*        Step (4R): apply back the Givens rotations performed. */

	for (i__ = *givptr; i__ >= 1; --i__) {
	    d__1 = -givnum[i__ + givnum_dim1];
	    drot_(nrhs, &b[givcol[i__ + ((givcol_dim1) << (1))] + b_dim1],
		    ldb, &b[givcol[i__ + givcol_dim1] + b_dim1], ldb, &givnum[
		    i__ + ((givnum_dim1) << (1))], &d__1);
/* L100: */
	}
    }

    return 0;

/*     End of DLALS0 */

} /* dlals0_ */

/* Subroutine */ int dlalsa_(integer *icompq, integer *smlsiz, integer *n,
	integer *nrhs, doublereal *b, integer *ldb, doublereal *bx, integer *
	ldbx, doublereal *u, integer *ldu, doublereal *vt, integer *k,
	doublereal *difl, doublereal *difr, doublereal *z__, doublereal *
	poles, integer *givptr, integer *givcol, integer *ldgcol, integer *
	perm, doublereal *givnum, doublereal *c__, doublereal *s, doublereal *
	work, integer *iwork, integer *info)
{
    /* System generated locals */
    integer givcol_dim1, givcol_offset, perm_dim1, perm_offset, b_dim1,
	    b_offset, bx_dim1, bx_offset, difl_dim1, difl_offset, difr_dim1,
	    difr_offset, givnum_dim1, givnum_offset, poles_dim1, poles_offset,
	     u_dim1, u_offset, vt_dim1, vt_offset, z_dim1, z_offset, i__1,
	    i__2;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__, j, i1, ic, lf, nd, ll, nl, nr, im1, nlf, nrf, lvl,
	    ndb1, nlp1, lvl2, nrp1, nlvl, sqre;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    static integer inode, ndiml, ndimr;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *), dlals0_(integer *, integer *, integer *,
	     integer *, integer *, doublereal *, integer *, doublereal *,
	    integer *, integer *, integer *, integer *, integer *, doublereal
	    *, integer *, doublereal *, doublereal *, doublereal *,
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *), dlasdt_(integer *, integer *, integer *, integer *,
	    integer *, integer *, integer *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLALSA is an itermediate step in solving the least squares problem
    by computing the SVD of the coefficient matrix in compact form (The
    singular vectors are computed as products of simple orthorgonal
    matrices.).

    If ICOMPQ = 0, DLALSA applies the inverse of the left singular vector
    matrix of an upper bidiagonal matrix to the right hand side; and if
    ICOMPQ = 1, DLALSA applies the right singular vector matrix to the
    right hand side. The singular vector matrices were generated in
    compact form by DLALSA.

    Arguments
    =========


    ICOMPQ (input) INTEGER
           Specifies whether the left or the right singular vector
           matrix is involved.
           = 0: Left singular vector matrix
           = 1: Right singular vector matrix

    SMLSIZ (input) INTEGER
           The maximum size of the subproblems at the bottom of the
           computation tree.

    N      (input) INTEGER
           The row and column dimensions of the upper bidiagonal matrix.

    NRHS   (input) INTEGER
           The number of columns of B and BX. NRHS must be at least 1.

    B      (input) DOUBLE PRECISION array, dimension ( LDB, NRHS )
           On input, B contains the right hand sides of the least
           squares problem in rows 1 through M. On output, B contains
           the solution X in rows 1 through N.

    LDB    (input) INTEGER
           The leading dimension of B in the calling subprogram.
           LDB must be at least max(1,MAX( M, N ) ).

    BX     (output) DOUBLE PRECISION array, dimension ( LDBX, NRHS )
           On exit, the result of applying the left or right singular
           vector matrix to B.

    LDBX   (input) INTEGER
           The leading dimension of BX.

    U      (input) DOUBLE PRECISION array, dimension ( LDU, SMLSIZ ).
           On entry, U contains the left singular vector matrices of all
           subproblems at the bottom level.

    LDU    (input) INTEGER, LDU = > N.
           The leading dimension of arrays U, VT, DIFL, DIFR,
           POLES, GIVNUM, and Z.

    VT     (input) DOUBLE PRECISION array, dimension ( LDU, SMLSIZ+1 ).
           On entry, VT' contains the right singular vector matrices of
           all subproblems at the bottom level.

    K      (input) INTEGER array, dimension ( N ).

    DIFL   (input) DOUBLE PRECISION array, dimension ( LDU, NLVL ).
           where NLVL = INT(log_2 (N/(SMLSIZ+1))) + 1.

    DIFR   (input) DOUBLE PRECISION array, dimension ( LDU, 2 * NLVL ).
           On entry, DIFL(*, I) and DIFR(*, 2 * I -1) record
           distances between singular values on the I-th level and
           singular values on the (I -1)-th level, and DIFR(*, 2 * I)
           record the normalizing factors of the right singular vectors
           matrices of subproblems on I-th level.

    Z      (input) DOUBLE PRECISION array, dimension ( LDU, NLVL ).
           On entry, Z(1, I) contains the components of the deflation-
           adjusted updating row vector for subproblems on the I-th
           level.

    POLES  (input) DOUBLE PRECISION array, dimension ( LDU, 2 * NLVL ).
           On entry, POLES(*, 2 * I -1: 2 * I) contains the new and old
           singular values involved in the secular equations on the I-th
           level.

    GIVPTR (input) INTEGER array, dimension ( N ).
           On entry, GIVPTR( I ) records the number of Givens
           rotations performed on the I-th problem on the computation
           tree.

    GIVCOL (input) INTEGER array, dimension ( LDGCOL, 2 * NLVL ).
           On entry, for each I, GIVCOL(*, 2 * I - 1: 2 * I) records the
           locations of Givens rotations performed on the I-th level on
           the computation tree.

    LDGCOL (input) INTEGER, LDGCOL = > N.
           The leading dimension of arrays GIVCOL and PERM.

    PERM   (input) INTEGER array, dimension ( LDGCOL, NLVL ).
           On entry, PERM(*, I) records permutations done on the I-th
           level of the computation tree.

    GIVNUM (input) DOUBLE PRECISION array, dimension ( LDU, 2 * NLVL ).
           On entry, GIVNUM(*, 2 *I -1 : 2 * I) records the C- and S-
           values of Givens rotations performed on the I-th level on the
           computation tree.

    C      (input) DOUBLE PRECISION array, dimension ( N ).
           On entry, if the I-th subproblem is not square,
           C( I ) contains the C-value of a Givens rotation related to
           the right null space of the I-th subproblem.

    S      (input) DOUBLE PRECISION array, dimension ( N ).
           On entry, if the I-th subproblem is not square,
           S( I ) contains the S-value of a Givens rotation related to
           the right null space of the I-th subproblem.

    WORK   (workspace) DOUBLE PRECISION array.
           The dimension must be at least N.

    IWORK  (workspace) INTEGER array.
           The dimension must be at least 3 * N

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Ren-Cang Li, Computer Science Division, University of
         California at Berkeley, USA
       Osni Marques, LBNL/NERSC, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    bx_dim1 = *ldbx;
    bx_offset = 1 + bx_dim1 * 1;
    bx -= bx_offset;
    givnum_dim1 = *ldu;
    givnum_offset = 1 + givnum_dim1 * 1;
    givnum -= givnum_offset;
    poles_dim1 = *ldu;
    poles_offset = 1 + poles_dim1 * 1;
    poles -= poles_offset;
    z_dim1 = *ldu;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    difr_dim1 = *ldu;
    difr_offset = 1 + difr_dim1 * 1;
    difr -= difr_offset;
    difl_dim1 = *ldu;
    difl_offset = 1 + difl_dim1 * 1;
    difl -= difl_offset;
    vt_dim1 = *ldu;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    --k;
    --givptr;
    perm_dim1 = *ldgcol;
    perm_offset = 1 + perm_dim1 * 1;
    perm -= perm_offset;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1 * 1;
    givcol -= givcol_offset;
    --c__;
    --s;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*smlsiz < 3) {
	*info = -2;
    } else if (*n < *smlsiz) {
	*info = -3;
    } else if (*nrhs < 1) {
	*info = -4;
    } else if (*ldb < *n) {
	*info = -6;
    } else if (*ldbx < *n) {
	*info = -8;
    } else if (*ldu < *n) {
	*info = -10;
    } else if (*ldgcol < *n) {
	*info = -19;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLALSA", &i__1);
	return 0;
    }

/*     Book-keeping and  setting up the computation tree. */

    inode = 1;
    ndiml = inode + *n;
    ndimr = ndiml + *n;

    dlasdt_(n, &nlvl, &nd, &iwork[inode], &iwork[ndiml], &iwork[ndimr],
	    smlsiz);

/*
       The following code applies back the left singular vector factors.
       For applying back the right singular vector factors, go to 50.
*/

    if (*icompq == 1) {
	goto L50;
    }

/*
       The nodes on the bottom level of the tree were solved
       by DLASDQ. The corresponding left and right singular vector
       matrices are in explicit form. First apply back the left
       singular vector matrices.
*/

    ndb1 = (nd + 1) / 2;
    i__1 = nd;
    for (i__ = ndb1; i__ <= i__1; ++i__) {

/*
          IC : center row of each node
          NL : number of rows of left  subproblem
          NR : number of rows of right subproblem
          NLF: starting row of the left   subproblem
          NRF: starting row of the right  subproblem
*/

	i1 = i__ - 1;
	ic = iwork[inode + i1];
	nl = iwork[ndiml + i1];
	nr = iwork[ndimr + i1];
	nlf = ic - nl;
	nrf = ic + 1;
	dgemm_("T", "N", &nl, nrhs, &nl, &c_b15, &u[nlf + u_dim1], ldu, &b[
		nlf + b_dim1], ldb, &c_b29, &bx[nlf + bx_dim1], ldbx);
	dgemm_("T", "N", &nr, nrhs, &nr, &c_b15, &u[nrf + u_dim1], ldu, &b[
		nrf + b_dim1], ldb, &c_b29, &bx[nrf + bx_dim1], ldbx);
/* L10: */
    }

/*
       Next copy the rows of B that correspond to unchanged rows
       in the bidiagonal matrix to BX.
*/

    i__1 = nd;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ic = iwork[inode + i__ - 1];
	dcopy_(nrhs, &b[ic + b_dim1], ldb, &bx[ic + bx_dim1], ldbx);
/* L20: */
    }

/*
       Finally go through the left singular vector matrices of all
       the other subproblems bottom-up on the tree.
*/

    j = pow_ii(&c__2, &nlvl);
    sqre = 0;

    for (lvl = nlvl; lvl >= 1; --lvl) {
	lvl2 = ((lvl) << (1)) - 1;

/*
          find the first node LF and last node LL on
          the current level LVL
*/

	if (lvl == 1) {
	    lf = 1;
	    ll = 1;
	} else {
	    i__1 = lvl - 1;
	    lf = pow_ii(&c__2, &i__1);
	    ll = ((lf) << (1)) - 1;
	}
	i__1 = ll;
	for (i__ = lf; i__ <= i__1; ++i__) {
	    im1 = i__ - 1;
	    ic = iwork[inode + im1];
	    nl = iwork[ndiml + im1];
	    nr = iwork[ndimr + im1];
	    nlf = ic - nl;
	    nrf = ic + 1;
	    --j;
	    dlals0_(icompq, &nl, &nr, &sqre, nrhs, &bx[nlf + bx_dim1], ldbx, &
		    b[nlf + b_dim1], ldb, &perm[nlf + lvl * perm_dim1], &
		    givptr[j], &givcol[nlf + lvl2 * givcol_dim1], ldgcol, &
		    givnum[nlf + lvl2 * givnum_dim1], ldu, &poles[nlf + lvl2 *
		     poles_dim1], &difl[nlf + lvl * difl_dim1], &difr[nlf +
		    lvl2 * difr_dim1], &z__[nlf + lvl * z_dim1], &k[j], &c__[
		    j], &s[j], &work[1], info);
/* L30: */
	}
/* L40: */
    }
    goto L90;

/*     ICOMPQ = 1: applying back the right singular vector factors. */

L50:

/*
       First now go through the right singular vector matrices of all
       the tree nodes top-down.
*/

    j = 0;
    i__1 = nlvl;
    for (lvl = 1; lvl <= i__1; ++lvl) {
	lvl2 = ((lvl) << (1)) - 1;

/*
          Find the first node LF and last node LL on
          the current level LVL.
*/

	if (lvl == 1) {
	    lf = 1;
	    ll = 1;
	} else {
	    i__2 = lvl - 1;
	    lf = pow_ii(&c__2, &i__2);
	    ll = ((lf) << (1)) - 1;
	}
	i__2 = lf;
	for (i__ = ll; i__ >= i__2; --i__) {
	    im1 = i__ - 1;
	    ic = iwork[inode + im1];
	    nl = iwork[ndiml + im1];
	    nr = iwork[ndimr + im1];
	    nlf = ic - nl;
	    nrf = ic + 1;
	    if (i__ == ll) {
		sqre = 0;
	    } else {
		sqre = 1;
	    }
	    ++j;
	    dlals0_(icompq, &nl, &nr, &sqre, nrhs, &b[nlf + b_dim1], ldb, &bx[
		    nlf + bx_dim1], ldbx, &perm[nlf + lvl * perm_dim1], &
		    givptr[j], &givcol[nlf + lvl2 * givcol_dim1], ldgcol, &
		    givnum[nlf + lvl2 * givnum_dim1], ldu, &poles[nlf + lvl2 *
		     poles_dim1], &difl[nlf + lvl * difl_dim1], &difr[nlf +
		    lvl2 * difr_dim1], &z__[nlf + lvl * z_dim1], &k[j], &c__[
		    j], &s[j], &work[1], info);
/* L60: */
	}
/* L70: */
    }

/*
       The nodes on the bottom level of the tree were solved
       by DLASDQ. The corresponding right singular vector
       matrices are in explicit form. Apply them back.
*/

    ndb1 = (nd + 1) / 2;
    i__1 = nd;
    for (i__ = ndb1; i__ <= i__1; ++i__) {
	i1 = i__ - 1;
	ic = iwork[inode + i1];
	nl = iwork[ndiml + i1];
	nr = iwork[ndimr + i1];
	nlp1 = nl + 1;
	if (i__ == nd) {
	    nrp1 = nr;
	} else {
	    nrp1 = nr + 1;
	}
	nlf = ic - nl;
	nrf = ic + 1;
	dgemm_("T", "N", &nlp1, nrhs, &nlp1, &c_b15, &vt[nlf + vt_dim1], ldu,
		&b[nlf + b_dim1], ldb, &c_b29, &bx[nlf + bx_dim1], ldbx);
	dgemm_("T", "N", &nrp1, nrhs, &nrp1, &c_b15, &vt[nrf + vt_dim1], ldu,
		&b[nrf + b_dim1], ldb, &c_b29, &bx[nrf + bx_dim1], ldbx);
/* L80: */
    }

L90:

    return 0;

/*     End of DLALSA */

} /* dlalsa_ */

/* Subroutine */ int dlalsd_(char *uplo, integer *smlsiz, integer *n, integer
	*nrhs, doublereal *d__, doublereal *e, doublereal *b, integer *ldb,
	doublereal *rcond, integer *rank, doublereal *work, integer *iwork,
	integer *info)
{
    /* System generated locals */
    integer b_dim1, b_offset, i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double log(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer c__, i__, j, k;
    static doublereal r__;
    static integer s, u, z__;
    static doublereal cs;
    static integer bx;
    static doublereal sn;
    static integer st, vt, nm1, st1;
    static doublereal eps;
    static integer iwk;
    static doublereal tol;
    static integer difl, difr, perm, nsub;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    static integer nlvl, sqre, bxst;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *),
	     dcopy_(integer *, doublereal *, integer *, doublereal *, integer
	    *);
    static integer poles, sizei, nsize, nwork, icmpq1, icmpq2;

    extern /* Subroutine */ int dlasda_(integer *, integer *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     doublereal *, integer *, integer *, integer *, integer *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *), dlalsa_(integer *, integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    doublereal *, doublereal *, integer *, integer *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     integer *, integer *), dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *);
    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dlasdq_(char *, integer *, integer *, integer
	    *, integer *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *), dlacpy_(char *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *), dlartg_(doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *), dlaset_(char *, integer *, integer *,
	     doublereal *, doublereal *, doublereal *, integer *),
	    xerbla_(char *, integer *);
    static integer givcol;
    extern doublereal dlanst_(char *, integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int dlasrt_(char *, integer *, doublereal *,
	    integer *);
    static doublereal orgnrm;
    static integer givnum, givptr, smlszp;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DLALSD uses the singular value decomposition of A to solve the least
    squares problem of finding X to minimize the Euclidean norm of each
    column of A*X-B, where A is N-by-N upper bidiagonal, and X and B
    are N-by-NRHS. The solution X overwrites B.

    The singular values of A smaller than RCOND times the largest
    singular value are treated as zero in solving the least squares
    problem; in this case a minimum norm solution is returned.
    The actual singular values are returned in D in ascending order.

    This code makes very mild assumptions about floating point
    arithmetic. It will work on machines with a guard digit in
    add/subtract, or on those binary machines without guard digits
    which subtract like the Cray XMP, Cray YMP, Cray C 90, or Cray 2.
    It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.

    Arguments
    =========

    UPLO   (input) CHARACTER*1
           = 'U': D and E define an upper bidiagonal matrix.
           = 'L': D and E define a  lower bidiagonal matrix.

    SMLSIZ (input) INTEGER
           The maximum size of the subproblems at the bottom of the
           computation tree.

    N      (input) INTEGER
           The dimension of the  bidiagonal matrix.  N >= 0.

    NRHS   (input) INTEGER
           The number of columns of B. NRHS must be at least 1.

    D      (input/output) DOUBLE PRECISION array, dimension (N)
           On entry D contains the main diagonal of the bidiagonal
           matrix. On exit, if INFO = 0, D contains its singular values.

    E      (input) DOUBLE PRECISION array, dimension (N-1)
           Contains the super-diagonal entries of the bidiagonal matrix.
           On exit, E has been destroyed.

    B      (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
           On input, B contains the right hand sides of the least
           squares problem. On output, B contains the solution X.

    LDB    (input) INTEGER
           The leading dimension of B in the calling subprogram.
           LDB must be at least max(1,N).

    RCOND  (input) DOUBLE PRECISION
           The singular values of A less than or equal to RCOND times
           the largest singular value are treated as zero in solving
           the least squares problem. If RCOND is negative,
           machine precision is used instead.
           For example, if diag(S)*X=B were the least squares problem,
           where diag(S) is a diagonal matrix of singular values, the
           solution would be X(i) = B(i) / S(i) if S(i) is greater than
           RCOND*max(S), and X(i) = 0 if S(i) is less than or equal to
           RCOND*max(S).

    RANK   (output) INTEGER
           The number of singular values of A greater than RCOND times
           the largest singular value.

    WORK   (workspace) DOUBLE PRECISION array, dimension at least
           (9*N + 2*N*SMLSIZ + 8*N*NLVL + N*NRHS + (SMLSIZ+1)**2),
           where NLVL = max(0, INT(log_2 (N/(SMLSIZ+1))) + 1).

    IWORK  (workspace) INTEGER array, dimension at least
           (3*N*NLVL + 11*N)

    INFO   (output) INTEGER
           = 0:  successful exit.
           < 0:  if INFO = -i, the i-th argument had an illegal value.
           > 0:  The algorithm failed to compute an singular value while
                 working on the submatrix lying in rows and columns
                 INFO/(N+1) through MOD(INFO,N+1).

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Ren-Cang Li, Computer Science Division, University of
         California at Berkeley, USA
       Osni Marques, LBNL/NERSC, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 1) {
	*info = -4;
    } else if (*ldb < 1 || *ldb < *n) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLALSD", &i__1);
	return 0;
    }

    eps = EPSILON;

/*     Set up the tolerance. */

    if (*rcond <= 0. || *rcond >= 1.) {
	*rcond = eps;
    }

    *rank = 0;

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    } else if (*n == 1) {
	if (d__[1] == 0.) {
	    dlaset_("A", &c__1, nrhs, &c_b29, &c_b29, &b[b_offset], ldb);
	} else {
	    *rank = 1;
	    dlascl_("G", &c__0, &c__0, &d__[1], &c_b15, &c__1, nrhs, &b[
		    b_offset], ldb, info);
	    d__[1] = abs(d__[1]);
	}
	return 0;
    }

/*     Rotate the matrix if it is lower bidiagonal. */

    if (*(unsigned char *)uplo == 'L') {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    if (*nrhs == 1) {
		drot_(&c__1, &b[i__ + b_dim1], &c__1, &b[i__ + 1 + b_dim1], &
			c__1, &cs, &sn);
	    } else {
		work[((i__) << (1)) - 1] = cs;
		work[i__ * 2] = sn;
	    }
/* L10: */
	}
	if (*nrhs > 1) {
	    i__1 = *nrhs;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = *n - 1;
		for (j = 1; j <= i__2; ++j) {
		    cs = work[((j) << (1)) - 1];
		    sn = work[j * 2];
		    drot_(&c__1, &b[j + i__ * b_dim1], &c__1, &b[j + 1 + i__ *
			     b_dim1], &c__1, &cs, &sn);
/* L20: */
		}
/* L30: */
	    }
	}
    }

/*     Scale. */

    nm1 = *n - 1;
    orgnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (orgnrm == 0.) {
	dlaset_("A", n, nrhs, &c_b29, &c_b29, &b[b_offset], ldb);
	return 0;
    }

    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, n, &c__1, &d__[1], n, info);
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, &nm1, &c__1, &e[1], &nm1,
	    info);

/*
       If N is smaller than the minimum divide size SMLSIZ, then solve
       the problem with another solver.
*/

    if (*n <= *smlsiz) {
	nwork = *n * *n + 1;
	dlaset_("A", n, n, &c_b29, &c_b15, &work[1], n);
	dlasdq_("U", &c__0, n, n, &c__0, nrhs, &d__[1], &e[1], &work[1], n, &
		work[1], n, &b[b_offset], ldb, &work[nwork], info);
	if (*info != 0) {
	    return 0;
	}
	tol = *rcond * (d__1 = d__[idamax_(n, &d__[1], &c__1)], abs(d__1));
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (d__[i__] <= tol) {
		dlaset_("A", &c__1, nrhs, &c_b29, &c_b29, &b[i__ + b_dim1],
			ldb);
	    } else {
		dlascl_("G", &c__0, &c__0, &d__[i__], &c_b15, &c__1, nrhs, &b[
			i__ + b_dim1], ldb, info);
		++(*rank);
	    }
/* L40: */
	}
	dgemm_("T", "N", n, nrhs, n, &c_b15, &work[1], n, &b[b_offset], ldb, &
		c_b29, &work[nwork], n);
	dlacpy_("A", n, nrhs, &work[nwork], n, &b[b_offset], ldb);

/*        Unscale. */

	dlascl_("G", &c__0, &c__0, &c_b15, &orgnrm, n, &c__1, &d__[1], n,
		info);
	dlasrt_("D", n, &d__[1], info);
	dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, n, nrhs, &b[b_offset],
		ldb, info);

	return 0;
    }

/*     Book-keeping and setting up some constants. */

    nlvl = (integer) (log((doublereal) (*n) / (doublereal) (*smlsiz + 1)) /
	    log(2.)) + 1;

    smlszp = *smlsiz + 1;

    u = 1;
    vt = *smlsiz * *n + 1;
    difl = vt + smlszp * *n;
    difr = difl + nlvl * *n;
    z__ = difr + ((nlvl * *n) << (1));
    c__ = z__ + nlvl * *n;
    s = c__ + *n;
    poles = s + *n;
    givnum = poles + ((nlvl) << (1)) * *n;
    bx = givnum + ((nlvl) << (1)) * *n;
    nwork = bx + *n * *nrhs;

    sizei = *n + 1;
    k = sizei + *n;
    givptr = k + *n;
    perm = givptr + *n;
    givcol = perm + nlvl * *n;
    iwk = givcol + ((nlvl * *n) << (1));

    st = 1;
    sqre = 0;
    icmpq1 = 1;
    icmpq2 = 0;
    nsub = 0;

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) < eps) {
	    d__[i__] = d_sign(&eps, &d__[i__]);
	}
/* L50: */
    }

    i__1 = nm1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = e[i__], abs(d__1)) < eps || i__ == nm1) {
	    ++nsub;
	    iwork[nsub] = st;

/*
             Subproblem found. First determine its size and then
             apply divide and conquer on it.
*/

	    if (i__ < nm1) {

/*              A subproblem with E(I) small for I < NM1. */

		nsize = i__ - st + 1;
		iwork[sizei + nsub - 1] = nsize;
	    } else if ((d__1 = e[i__], abs(d__1)) >= eps) {

/*              A subproblem with E(NM1) not too small but I = NM1. */

		nsize = *n - st + 1;
		iwork[sizei + nsub - 1] = nsize;
	    } else {

/*
                A subproblem with E(NM1) small. This implies an
                1-by-1 subproblem at D(N), which is not solved
                explicitly.
*/

		nsize = i__ - st + 1;
		iwork[sizei + nsub - 1] = nsize;
		++nsub;
		iwork[nsub] = *n;
		iwork[sizei + nsub - 1] = 1;
		dcopy_(nrhs, &b[*n + b_dim1], ldb, &work[bx + nm1], n);
	    }
	    st1 = st - 1;
	    if (nsize == 1) {

/*
                This is a 1-by-1 subproblem and is not solved
                explicitly.
*/

		dcopy_(nrhs, &b[st + b_dim1], ldb, &work[bx + st1], n);
	    } else if (nsize <= *smlsiz) {

/*              This is a small subproblem and is solved by DLASDQ. */

		dlaset_("A", &nsize, &nsize, &c_b29, &c_b15, &work[vt + st1],
			n);
		dlasdq_("U", &c__0, &nsize, &nsize, &c__0, nrhs, &d__[st], &e[
			st], &work[vt + st1], n, &work[nwork], n, &b[st +
			b_dim1], ldb, &work[nwork], info);
		if (*info != 0) {
		    return 0;
		}
		dlacpy_("A", &nsize, nrhs, &b[st + b_dim1], ldb, &work[bx +
			st1], n);
	    } else {

/*              A large problem. Solve it using divide and conquer. */

		dlasda_(&icmpq1, smlsiz, &nsize, &sqre, &d__[st], &e[st], &
			work[u + st1], n, &work[vt + st1], &iwork[k + st1], &
			work[difl + st1], &work[difr + st1], &work[z__ + st1],
			 &work[poles + st1], &iwork[givptr + st1], &iwork[
			givcol + st1], n, &iwork[perm + st1], &work[givnum +
			st1], &work[c__ + st1], &work[s + st1], &work[nwork],
			&iwork[iwk], info);
		if (*info != 0) {
		    return 0;
		}
		bxst = bx + st1;
		dlalsa_(&icmpq2, smlsiz, &nsize, nrhs, &b[st + b_dim1], ldb, &
			work[bxst], n, &work[u + st1], n, &work[vt + st1], &
			iwork[k + st1], &work[difl + st1], &work[difr + st1],
			&work[z__ + st1], &work[poles + st1], &iwork[givptr +
			st1], &iwork[givcol + st1], n, &iwork[perm + st1], &
			work[givnum + st1], &work[c__ + st1], &work[s + st1],
			&work[nwork], &iwork[iwk], info);
		if (*info != 0) {
		    return 0;
		}
	    }
	    st = i__ + 1;
	}
/* L60: */
    }

/*     Apply the singular values and treat the tiny ones as zero. */

    tol = *rcond * (d__1 = d__[idamax_(n, &d__[1], &c__1)], abs(d__1));

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*
          Some of the elements in D can be negative because 1-by-1
          subproblems were not solved explicitly.
*/

	if ((d__1 = d__[i__], abs(d__1)) <= tol) {
	    dlaset_("A", &c__1, nrhs, &c_b29, &c_b29, &work[bx + i__ - 1], n);
	} else {
	    ++(*rank);
	    dlascl_("G", &c__0, &c__0, &d__[i__], &c_b15, &c__1, nrhs, &work[
		    bx + i__ - 1], n, info);
	}
	d__[i__] = (d__1 = d__[i__], abs(d__1));
/* L70: */
    }

/*     Now apply back the right singular vectors. */

    icmpq2 = 1;
    i__1 = nsub;
    for (i__ = 1; i__ <= i__1; ++i__) {
	st = iwork[i__];
	st1 = st - 1;
	nsize = iwork[sizei + i__ - 1];
	bxst = bx + st1;
	if (nsize == 1) {
	    dcopy_(nrhs, &work[bxst], n, &b[st + b_dim1], ldb);
	} else if (nsize <= *smlsiz) {
	    dgemm_("T", "N", &nsize, nrhs, &nsize, &c_b15, &work[vt + st1], n,
		     &work[bxst], n, &c_b29, &b[st + b_dim1], ldb);
	} else {
	    dlalsa_(&icmpq2, smlsiz, &nsize, nrhs, &work[bxst], n, &b[st +
		    b_dim1], ldb, &work[u + st1], n, &work[vt + st1], &iwork[
		    k + st1], &work[difl + st1], &work[difr + st1], &work[z__
		    + st1], &work[poles + st1], &iwork[givptr + st1], &iwork[
		    givcol + st1], n, &iwork[perm + st1], &work[givnum + st1],
		     &work[c__ + st1], &work[s + st1], &work[nwork], &iwork[
		    iwk], info);
	    if (*info != 0) {
		return 0;
	    }
	}
/* L80: */
    }

/*     Unscale and sort the singular values. */

    dlascl_("G", &c__0, &c__0, &c_b15, &orgnrm, n, &c__1, &d__[1], n, info);
    dlasrt_("D", n, &d__[1], info);
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, n, nrhs, &b[b_offset], ldb,
	    info);

    return 0;

/*     End of DLALSD */

} /* dlalsd_ */

/* Subroutine */ int dlamrg_(integer *n1, integer *n2, doublereal *a, integer
	*dtrd1, integer *dtrd2, integer *index)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, ind1, ind2, n1sv, n2sv;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DLAMRG will create a permutation list which will merge the elements
    of A (which is composed of two independently sorted sets) into a
    single set which is sorted in ascending order.

    Arguments
    =========

    N1     (input) INTEGER
    N2     (input) INTEGER
           These arguements contain the respective lengths of the two
           sorted lists to be merged.

    A      (input) DOUBLE PRECISION array, dimension (N1+N2)
           The first N1 elements of A contain a list of numbers which
           are sorted in either ascending or descending order.  Likewise
           for the final N2 elements.

    DTRD1  (input) INTEGER
    DTRD2  (input) INTEGER
           These are the strides to be taken through the array A.
           Allowable strides are 1 and -1.  They indicate whether a
           subset of A is sorted in ascending (DTRDx = 1) or descending
           (DTRDx = -1) order.

    INDEX  (output) INTEGER array, dimension (N1+N2)
           On exit this array will contain a permutation such that
           if B( I ) = A( INDEX( I ) ) for I=1,N1+N2, then B will be
           sorted in ascending order.

    =====================================================================
*/


    /* Parameter adjustments */
    --index;
    --a;

    /* Function Body */
    n1sv = *n1;
    n2sv = *n2;
    if (*dtrd1 > 0) {
	ind1 = 1;
    } else {
	ind1 = *n1;
    }
    if (*dtrd2 > 0) {
	ind2 = *n1 + 1;
    } else {
	ind2 = *n1 + *n2;
    }
    i__ = 1;
/*     while ( (N1SV > 0) & (N2SV > 0) ) */
L10:
    if ((n1sv > 0 && n2sv > 0)) {
	if (a[ind1] <= a[ind2]) {
	    index[i__] = ind1;
	    ++i__;
	    ind1 += *dtrd1;
	    --n1sv;
	} else {
	    index[i__] = ind2;
	    ++i__;
	    ind2 += *dtrd2;
	    --n2sv;
	}
	goto L10;
    }
/*     end while */
    if (n1sv == 0) {
	i__1 = n2sv;
	for (n1sv = 1; n1sv <= i__1; ++n1sv) {
	    index[i__] = ind2;
	    ++i__;
	    ind2 += *dtrd2;
/* L20: */
	}
    } else {
/*     N2SV .EQ. 0 */
	i__1 = n1sv;
	for (n2sv = 1; n2sv <= i__1; ++n2sv) {
	    index[i__] = ind1;
	    ++i__;
	    ind1 += *dtrd1;
/* L30: */
	}
    }

    return 0;

/*     End of DLAMRG */

} /* dlamrg_ */

doublereal dlange_(char *norm, integer *m, integer *n, doublereal *a, integer
	*lda, doublereal *work)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    doublereal ret_val, d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j;
    static doublereal sum, scale;
    extern logical lsame_(char *, char *);
    static doublereal value;
    extern /* Subroutine */ int dlassq_(integer *, doublereal *, integer *,
	    doublereal *, doublereal *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLANGE  returns the value of the one norm,  or the Frobenius norm, or
    the  infinity norm,  or the  element of  largest absolute value  of a
    real matrix A.

    Description
    ===========

    DLANGE returns the value

       DLANGE = ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) CHARACTER*1
            Specifies the value to be returned in DLANGE as described
            above.

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.  When M = 0,
            DLANGE is set to zero.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.  When N = 0,
            DLANGE is set to zero.

    A       (input) DOUBLE PRECISION array, dimension (LDA,N)
            The m by n matrix A.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(M,1).

    WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK),
            where LWORK >= M when NORM = 'I'; otherwise, WORK is not
            referenced.

   =====================================================================
*/


    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (min(*m,*n) == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		value = max(d__2,d__3);
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = 0.;
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L30: */
	    }
	    value = max(value,sum);
/* L40: */
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.;
/* L50: */
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L60: */
	    }
/* L70: */
	}
	value = 0.;
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = max(d__1,d__2);
/* L80: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    dlassq_(m, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L90: */
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANGE */

} /* dlange_ */

doublereal dlanhs_(char *norm, integer *n, doublereal *a, integer *lda,
	doublereal *work)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    doublereal ret_val, d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j;
    static doublereal sum, scale;
    extern logical lsame_(char *, char *);
    static doublereal value;
    extern /* Subroutine */ int dlassq_(integer *, doublereal *, integer *,
	    doublereal *, doublereal *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLANHS  returns the value of the one norm,  or the Frobenius norm, or
    the  infinity norm,  or the  element of  largest absolute value  of a
    Hessenberg matrix A.

    Description
    ===========

    DLANHS returns the value

       DLANHS = ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) CHARACTER*1
            Specifies the value to be returned in DLANHS as described
            above.

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.  When N = 0, DLANHS is
            set to zero.

    A       (input) DOUBLE PRECISION array, dimension (LDA,N)
            The n by n upper Hessenberg matrix A; the part of A below the
            first sub-diagonal is not referenced.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(N,1).

    WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK),
            where LWORK >= N when NORM = 'I'; otherwise, WORK is not
            referenced.

   =====================================================================
*/


    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		value = max(d__2,d__3);
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = 0.;
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L30: */
	    }
	    value = max(value,sum);
/* L40: */
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.;
/* L50: */
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L60: */
	    }
/* L70: */
	}
	value = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = max(d__1,d__2);
/* L80: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = min(i__3,i__4);
	    dlassq_(&i__2, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L90: */
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANHS */

} /* dlanhs_ */

doublereal dlanst_(char *norm, integer *n, doublereal *d__, doublereal *e)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1, d__2, d__3, d__4, d__5;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static doublereal sum, scale;
    extern logical lsame_(char *, char *);
    static doublereal anorm;
    extern /* Subroutine */ int dlassq_(integer *, doublereal *, integer *,
	    doublereal *, doublereal *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLANST  returns the value of the one norm,  or the Frobenius norm, or
    the  infinity norm,  or the  element of  largest absolute value  of a
    real symmetric tridiagonal matrix A.

    Description
    ===========

    DLANST returns the value

       DLANST = ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) CHARACTER*1
            Specifies the value to be returned in DLANST as described
            above.

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.  When N = 0, DLANST is
            set to zero.

    D       (input) DOUBLE PRECISION array, dimension (N)
            The diagonal elements of A.

    E       (input) DOUBLE PRECISION array, dimension (N-1)
            The (n-1) sub-diagonal or super-diagonal elements of A.

    =====================================================================
*/


    /* Parameter adjustments */
    --e;
    --d__;

    /* Function Body */
    if (*n <= 0) {
	anorm = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	anorm = (d__1 = d__[*n], abs(d__1));
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__2 = anorm, d__3 = (d__1 = d__[i__], abs(d__1));
	    anorm = max(d__2,d__3);
/* Computing MAX */
	    d__2 = anorm, d__3 = (d__1 = e[i__], abs(d__1));
	    anorm = max(d__2,d__3);
/* L10: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1' || lsame_(norm, "I")) {

/*        Find norm1(A). */

	if (*n == 1) {
	    anorm = abs(d__[1]);
	} else {
/* Computing MAX */
	    d__3 = abs(d__[1]) + abs(e[1]), d__4 = (d__1 = e[*n - 1], abs(
		    d__1)) + (d__2 = d__[*n], abs(d__2));
	    anorm = max(d__3,d__4);
	    i__1 = *n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__4 = anorm, d__5 = (d__1 = d__[i__], abs(d__1)) + (d__2 = e[
			i__], abs(d__2)) + (d__3 = e[i__ - 1], abs(d__3));
		anorm = max(d__4,d__5);
/* L20: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	if (*n > 1) {
	    i__1 = *n - 1;
	    dlassq_(&i__1, &e[1], &c__1, &scale, &sum);
	    sum *= 2;
	}
	dlassq_(n, &d__[1], &c__1, &scale, &sum);
	anorm = scale * sqrt(sum);
    }

    ret_val = anorm;
    return ret_val;

/*     End of DLANST */

} /* dlanst_ */

doublereal dlansy_(char *norm, char *uplo, integer *n, doublereal *a, integer
	*lda, doublereal *work)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    doublereal ret_val, d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j;
    static doublereal sum, absa, scale;
    extern logical lsame_(char *, char *);
    static doublereal value;
    extern /* Subroutine */ int dlassq_(integer *, doublereal *, integer *,
	    doublereal *, doublereal *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLANSY  returns the value of the one norm,  or the Frobenius norm, or
    the  infinity norm,  or the  element of  largest absolute value  of a
    real symmetric matrix A.

    Description
    ===========

    DLANSY returns the value

       DLANSY = ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) CHARACTER*1
            Specifies the value to be returned in DLANSY as described
            above.

    UPLO    (input) CHARACTER*1
            Specifies whether the upper or lower triangular part of the
            symmetric matrix A is to be referenced.
            = 'U':  Upper triangular part of A is referenced
            = 'L':  Lower triangular part of A is referenced

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.  When N = 0, DLANSY is
            set to zero.

    A       (input) DOUBLE PRECISION array, dimension (LDA,N)
            The symmetric matrix A.  If UPLO = 'U', the leading n by n
            upper triangular part of A contains the upper triangular part
            of the matrix A, and the strictly lower triangular part of A
            is not referenced.  If UPLO = 'L', the leading n by n lower
            triangular part of A contains the lower triangular part of
            the matrix A, and the strictly upper triangular part of A is
            not referenced.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(N,1).

    WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK),
            where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise,
            WORK is not referenced.

   =====================================================================
*/


    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
			    d__1));
		    value = max(d__2,d__3);
/* L10: */
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
			    d__1));
		    value = max(d__2,d__3);
/* L30: */
		}
/* L40: */
	    }
	}
    } else if (lsame_(norm, "I") || lsame_(norm, "O") || *(unsigned char *)norm == '1') {

/*        Find normI(A) ( = norm1(A), since A is symmetric). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = 0.;
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    absa = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
/* L50: */
		}
		work[j] = sum + (d__1 = a[j + j * a_dim1], abs(d__1));
/* L60: */
	    }
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__1 = value, d__2 = work[i__];
		value = max(d__1,d__2);
/* L70: */
	    }
	} else {
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] = 0.;
/* L80: */
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = work[j] + (d__1 = a[j + j * a_dim1], abs(d__1));
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    absa = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
/* L90: */
		}
		value = max(value,sum);
/* L100: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j - 1;
		dlassq_(&i__2, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L110: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		dlassq_(&i__2, &a[j + 1 + j * a_dim1], &c__1, &scale, &sum);
/* L120: */
	    }
	}
	sum *= 2;
	i__1 = *lda + 1;
	dlassq_(n, &a[a_offset], &i__1, &scale, &sum);
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANSY */

} /* dlansy_ */

/* Subroutine */ int dlanv2_(doublereal *a, doublereal *b, doublereal *c__,
	doublereal *d__, doublereal *rt1r, doublereal *rt1i, doublereal *rt2r,
	 doublereal *rt2i, doublereal *cs, doublereal *sn)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *), sqrt(doublereal);

    /* Local variables */
    static doublereal p, z__, aa, bb, cc, dd, cs1, sn1, sab, sac, eps, tau,
	    temp, scale, bcmax, bcmis, sigma;



/*
    -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLANV2 computes the Schur factorization of a real 2-by-2 nonsymmetric
    matrix in standard form:

         [ A  B ] = [ CS -SN ] [ AA  BB ] [ CS  SN ]
         [ C  D ]   [ SN  CS ] [ CC  DD ] [-SN  CS ]

    where either
    1) CC = 0 so that AA and DD are real eigenvalues of the matrix, or
    2) AA = DD and BB*CC < 0, so that AA + or - sqrt(BB*CC) are complex
    conjugate eigenvalues.

    Arguments
    =========

    A       (input/output) DOUBLE PRECISION
    B       (input/output) DOUBLE PRECISION
    C       (input/output) DOUBLE PRECISION
    D       (input/output) DOUBLE PRECISION
            On entry, the elements of the input matrix.
            On exit, they are overwritten by the elements of the
            standardised Schur form.

    RT1R    (output) DOUBLE PRECISION
    RT1I    (output) DOUBLE PRECISION
    RT2R    (output) DOUBLE PRECISION
    RT2I    (output) DOUBLE PRECISION
            The real and imaginary parts of the eigenvalues. If the
            eigenvalues are a complex conjugate pair, RT1I > 0.

    CS      (output) DOUBLE PRECISION
    SN      (output) DOUBLE PRECISION
            Parameters of the rotation matrix.

    Further Details
    ===============

    Modified by V. Sima, Research Institute for Informatics, Bucharest,
    Romania, to reduce the risk of cancellation errors,
    when computing real eigenvalues, and to ensure, if possible, that
    abs(RT1R) >= abs(RT2R).

    =====================================================================
*/


    eps = PRECISION;
    if (*c__ == 0.) {
	*cs = 1.;
	*sn = 0.;
	goto L10;

    } else if (*b == 0.) {

/*        Swap rows and columns */

	*cs = 0.;
	*sn = 1.;
	temp = *d__;
	*d__ = *a;
	*a = temp;
	*b = -(*c__);
	*c__ = 0.;
	goto L10;
    } else if ((*a - *d__ == 0. && d_sign(&c_b15, b) != d_sign(&c_b15, c__)))
	    {
	*cs = 1.;
	*sn = 0.;
	goto L10;
    } else {

	temp = *a - *d__;
	p = temp * .5;
/* Computing MAX */
	d__1 = abs(*b), d__2 = abs(*c__);
	bcmax = max(d__1,d__2);
/* Computing MIN */
	d__1 = abs(*b), d__2 = abs(*c__);
	bcmis = min(d__1,d__2) * d_sign(&c_b15, b) * d_sign(&c_b15, c__);
/* Computing MAX */
	d__1 = abs(p);
	scale = max(d__1,bcmax);
	z__ = p / scale * p + bcmax / scale * bcmis;

/*
          If Z is of the order of the machine accuracy, postpone the
          decision on the nature of eigenvalues
*/

	if (z__ >= eps * 4.) {

/*           Real eigenvalues. Compute A and D. */

	    d__1 = sqrt(scale) * sqrt(z__);
	    z__ = p + d_sign(&d__1, &p);
	    *a = *d__ + z__;
	    *d__ -= bcmax / z__ * bcmis;

/*           Compute B and the rotation matrix */

	    tau = dlapy2_(c__, &z__);
	    *cs = z__ / tau;
	    *sn = *c__ / tau;
	    *b -= *c__;
	    *c__ = 0.;
	} else {

/*
             Complex eigenvalues, or real (almost) equal eigenvalues.
             Make diagonal elements equal.
*/

	    sigma = *b + *c__;
	    tau = dlapy2_(&sigma, &temp);
	    *cs = sqrt((abs(sigma) / tau + 1.) * .5);
	    *sn = -(p / (tau * *cs)) * d_sign(&c_b15, &sigma);

/*
             Compute [ AA  BB ] = [ A  B ] [ CS -SN ]
                     [ CC  DD ]   [ C  D ] [ SN  CS ]
*/

	    aa = *a * *cs + *b * *sn;
	    bb = -(*a) * *sn + *b * *cs;
	    cc = *c__ * *cs + *d__ * *sn;
	    dd = -(*c__) * *sn + *d__ * *cs;

/*
             Compute [ A  B ] = [ CS  SN ] [ AA  BB ]
                     [ C  D ]   [-SN  CS ] [ CC  DD ]
*/

	    *a = aa * *cs + cc * *sn;
	    *b = bb * *cs + dd * *sn;
	    *c__ = -aa * *sn + cc * *cs;
	    *d__ = -bb * *sn + dd * *cs;

	    temp = (*a + *d__) * .5;
	    *a = temp;
	    *d__ = temp;

	    if (*c__ != 0.) {
		if (*b != 0.) {
		    if (d_sign(&c_b15, b) == d_sign(&c_b15, c__)) {

/*                    Real eigenvalues: reduce to upper triangular form */

			sab = sqrt((abs(*b)));
			sac = sqrt((abs(*c__)));
			d__1 = sab * sac;
			p = d_sign(&d__1, c__);
			tau = 1. / sqrt((d__1 = *b + *c__, abs(d__1)));
			*a = temp + p;
			*d__ = temp - p;
			*b -= *c__;
			*c__ = 0.;
			cs1 = sab * tau;
			sn1 = sac * tau;
			temp = *cs * cs1 - *sn * sn1;
			*sn = *cs * sn1 + *sn * cs1;
			*cs = temp;
		    }
		} else {
		    *b = -(*c__);
		    *c__ = 0.;
		    temp = *cs;
		    *cs = -(*sn);
		    *sn = temp;
		}
	    }
	}

    }

L10:

/*     Store eigenvalues in (RT1R,RT1I) and (RT2R,RT2I). */

    *rt1r = *a;
    *rt2r = *d__;
    if (*c__ == 0.) {
	*rt1i = 0.;
	*rt2i = 0.;
    } else {
	*rt1i = sqrt((abs(*b))) * sqrt((abs(*c__)));
	*rt2i = -(*rt1i);
    }
    return 0;

/*     End of DLANV2 */

} /* dlanv2_ */

doublereal dlapy2_(doublereal *x, doublereal *y)
{
    /* System generated locals */
    doublereal ret_val, d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal w, z__, xabs, yabs;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLAPY2 returns sqrt(x**2+y**2), taking care not to cause unnecessary
    overflow.

    Arguments
    =========

    X       (input) DOUBLE PRECISION
    Y       (input) DOUBLE PRECISION
            X and Y specify the values x and y.

    =====================================================================
*/


    xabs = abs(*x);
    yabs = abs(*y);
    w = max(xabs,yabs);
    z__ = min(xabs,yabs);
    if (z__ == 0.) {
	ret_val = w;
    } else {
/* Computing 2nd power */
	d__1 = z__ / w;
	ret_val = w * sqrt(d__1 * d__1 + 1.);
    }
    return ret_val;

/*     End of DLAPY2 */

} /* dlapy2_ */

doublereal dlapy3_(doublereal *x, doublereal *y, doublereal *z__)
{
    /* System generated locals */
    doublereal ret_val, d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal w, xabs, yabs, zabs;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLAPY3 returns sqrt(x**2+y**2+z**2), taking care not to cause
    unnecessary overflow.

    Arguments
    =========

    X       (input) DOUBLE PRECISION
    Y       (input) DOUBLE PRECISION
    Z       (input) DOUBLE PRECISION
            X, Y and Z specify the values x, y and z.

    =====================================================================
*/


    xabs = abs(*x);
    yabs = abs(*y);
    zabs = abs(*z__);
/* Computing MAX */
    d__1 = max(xabs,yabs);
    w = max(d__1,zabs);
    if (w == 0.) {
	ret_val = 0.;
    } else {
/* Computing 2nd power */
	d__1 = xabs / w;
/* Computing 2nd power */
	d__2 = yabs / w;
/* Computing 2nd power */
	d__3 = zabs / w;
	ret_val = w * sqrt(d__1 * d__1 + d__2 * d__2 + d__3 * d__3);
    }
    return ret_val;

/*     End of DLAPY3 */

} /* dlapy3_ */

/* Subroutine */ int dlarf_(char *side, integer *m, integer *n, doublereal *v,
	 integer *incv, doublereal *tau, doublereal *c__, integer *ldc,
	doublereal *work)
{
    /* System generated locals */
    integer c_dim1, c_offset;
    doublereal d__1;

    /* Local variables */
    extern /* Subroutine */ int dger_(integer *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLARF applies a real elementary reflector H to a real m by n matrix
    C, from either the left or the right. H is represented in the form

          H = I - tau * v * v'

    where tau is a real scalar and v is a real vector.

    If tau = 0, then H is taken to be the unit matrix.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': form  H * C
            = 'R': form  C * H

    M       (input) INTEGER
            The number of rows of the matrix C.

    N       (input) INTEGER
            The number of columns of the matrix C.

    V       (input) DOUBLE PRECISION array, dimension
                       (1 + (M-1)*abs(INCV)) if SIDE = 'L'
                    or (1 + (N-1)*abs(INCV)) if SIDE = 'R'
            The vector v in the representation of H. V is not used if
            TAU = 0.

    INCV    (input) INTEGER
            The increment between elements of v. INCV <> 0.

    TAU     (input) DOUBLE PRECISION
            The value tau in the representation of H.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by the matrix H * C if SIDE = 'L',
            or C * H if SIDE = 'R'.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) DOUBLE PRECISION array, dimension
                           (N) if SIDE = 'L'
                        or (M) if SIDE = 'R'

    =====================================================================
*/


    /* Parameter adjustments */
    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    if (lsame_(side, "L")) {

/*        Form  H * C */

	if (*tau != 0.) {

/*           w := C' * v */

	    dgemv_("Transpose", m, n, &c_b15, &c__[c_offset], ldc, &v[1],
		    incv, &c_b29, &work[1], &c__1);

/*           C := C - v * w' */

	    d__1 = -(*tau);
	    dger_(m, n, &d__1, &v[1], incv, &work[1], &c__1, &c__[c_offset],
		    ldc);
	}
    } else {

/*        Form  C * H */

	if (*tau != 0.) {

/*           w := C * v */

	    dgemv_("No transpose", m, n, &c_b15, &c__[c_offset], ldc, &v[1],
		    incv, &c_b29, &work[1], &c__1);

/*           C := C - w * v' */

	    d__1 = -(*tau);
	    dger_(m, n, &d__1, &work[1], &c__1, &v[1], incv, &c__[c_offset],
		    ldc);
	}
    }
    return 0;

/*     End of DLARF */

} /* dlarf_ */

/* Subroutine */ int dlarfb_(char *side, char *trans, char *direct, char *
	storev, integer *m, integer *n, integer *k, doublereal *v, integer *
	ldv, doublereal *t, integer *ldt, doublereal *c__, integer *ldc,
	doublereal *work, integer *ldwork)
{
    /* System generated locals */
    integer c_dim1, c_offset, t_dim1, t_offset, v_dim1, v_offset, work_dim1,
	    work_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *), dtrmm_(char *, char *, char *, char *,
	    integer *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *);
    static char transt[1];


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLARFB applies a real block reflector H or its transpose H' to a
    real m by n matrix C, from either the left or the right.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply H or H' from the Left
            = 'R': apply H or H' from the Right

    TRANS   (input) CHARACTER*1
            = 'N': apply H (No transpose)
            = 'T': apply H' (Transpose)

    DIRECT  (input) CHARACTER*1
            Indicates how H is formed from a product of elementary
            reflectors
            = 'F': H = H(1) H(2) . . . H(k) (Forward)
            = 'B': H = H(k) . . . H(2) H(1) (Backward)

    STOREV  (input) CHARACTER*1
            Indicates how the vectors which define the elementary
            reflectors are stored:
            = 'C': Columnwise
            = 'R': Rowwise

    M       (input) INTEGER
            The number of rows of the matrix C.

    N       (input) INTEGER
            The number of columns of the matrix C.

    K       (input) INTEGER
            The order of the matrix T (= the number of elementary
            reflectors whose product defines the block reflector).

    V       (input) DOUBLE PRECISION array, dimension
                                  (LDV,K) if STOREV = 'C'
                                  (LDV,M) if STOREV = 'R' and SIDE = 'L'
                                  (LDV,N) if STOREV = 'R' and SIDE = 'R'
            The matrix V. See further details.

    LDV     (input) INTEGER
            The leading dimension of the array V.
            If STOREV = 'C' and SIDE = 'L', LDV >= max(1,M);
            if STOREV = 'C' and SIDE = 'R', LDV >= max(1,N);
            if STOREV = 'R', LDV >= K.

    T       (input) DOUBLE PRECISION array, dimension (LDT,K)
            The triangular k by k matrix T in the representation of the
            block reflector.

    LDT     (input) INTEGER
            The leading dimension of the array T. LDT >= K.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by H*C or H'*C or C*H or C*H'.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDA >= max(1,M).

    WORK    (workspace) DOUBLE PRECISION array, dimension (LDWORK,K)

    LDWORK  (input) INTEGER
            The leading dimension of the array WORK.
            If SIDE = 'L', LDWORK >= max(1,N);
            if SIDE = 'R', LDWORK >= max(1,M).

    =====================================================================


       Quick return if possible
*/

    /* Parameter adjustments */
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1 * 1;
    v -= v_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1 * 1;
    t -= t_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1 * 1;
    work -= work_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    if (lsame_(trans, "N")) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

    if (lsame_(storev, "C")) {

	if (lsame_(direct, "F")) {

/*
             Let  V =  ( V1 )    (first K rows)
                       ( V2 )
             where  V1  is unit lower triangular.
*/

	    if (lsame_(side, "L")) {

/*
                Form  H * C  or  H' * C  where  C = ( C1 )
                                                    ( C2 )

                W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in WORK)

                W := C1'
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(n, &c__[j + c_dim1], ldc, &work[j * work_dim1 + 1],
			     &c__1);
/* L10: */
		}

/*              W := W * V1 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", n, k, &c_b15,
			 &v[v_offset], ldv, &work[work_offset], ldwork);
		if (*m > *k) {

/*                 W := W + C2'*V2 */

		    i__1 = *m - *k;
		    dgemm_("Transpose", "No transpose", n, k, &i__1, &c_b15, &
			    c__[*k + 1 + c_dim1], ldc, &v[*k + 1 + v_dim1],
			    ldv, &c_b15, &work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Upper", transt, "Non-unit", n, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V * W' */

		if (*m > *k) {

/*                 C2 := C2 - V2 * W' */

		    i__1 = *m - *k;
		    dgemm_("No transpose", "Transpose", &i__1, n, k, &c_b151,
			    &v[*k + 1 + v_dim1], ldv, &work[work_offset],
			    ldwork, &c_b15, &c__[*k + 1 + c_dim1], ldc);
		}

/*              W := W * V1' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", n, k, &c_b15, &
			v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[j + i__ * c_dim1] -= work[i__ + j * work_dim1];
/* L20: */
		    }
/* L30: */
		}

	    } else if (lsame_(side, "R")) {

/*
                Form  C * H  or  C * H'  where  C = ( C1  C2 )

                W := C * V  =  (C1*V1 + C2*V2)  (stored in WORK)

                W := C1
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(m, &c__[j * c_dim1 + 1], &c__1, &work[j *
			    work_dim1 + 1], &c__1);
/* L40: */
		}

/*              W := W * V1 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", m, k, &c_b15,
			 &v[v_offset], ldv, &work[work_offset], ldwork);
		if (*n > *k) {

/*                 W := W + C2 * V2 */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "No transpose", m, k, &i__1, &
			    c_b15, &c__[(*k + 1) * c_dim1 + 1], ldc, &v[*k +
			    1 + v_dim1], ldv, &c_b15, &work[work_offset],
			    ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Upper", trans, "Non-unit", m, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V' */

		if (*n > *k) {

/*                 C2 := C2 - W * V2' */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "Transpose", m, &i__1, k, &c_b151,
			    &work[work_offset], ldwork, &v[*k + 1 + v_dim1],
			    ldv, &c_b15, &c__[(*k + 1) * c_dim1 + 1], ldc);
		}

/*              W := W * V1' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", m, k, &c_b15, &
			v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + j * c_dim1] -= work[i__ + j * work_dim1];
/* L50: */
		    }
/* L60: */
		}
	    }

	} else {

/*
             Let  V =  ( V1 )
                       ( V2 )    (last K rows)
             where  V2  is unit upper triangular.
*/

	    if (lsame_(side, "L")) {

/*
                Form  H * C  or  H' * C  where  C = ( C1 )
                                                    ( C2 )

                W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in WORK)

                W := C2'
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(n, &c__[*m - *k + j + c_dim1], ldc, &work[j *
			    work_dim1 + 1], &c__1);
/* L70: */
		}

/*              W := W * V2 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", n, k, &c_b15,
			 &v[*m - *k + 1 + v_dim1], ldv, &work[work_offset],
			ldwork);
		if (*m > *k) {

/*                 W := W + C1'*V1 */

		    i__1 = *m - *k;
		    dgemm_("Transpose", "No transpose", n, k, &i__1, &c_b15, &
			    c__[c_offset], ldc, &v[v_offset], ldv, &c_b15, &
			    work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Lower", transt, "Non-unit", n, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V * W' */

		if (*m > *k) {

/*                 C1 := C1 - V1 * W' */

		    i__1 = *m - *k;
		    dgemm_("No transpose", "Transpose", &i__1, n, k, &c_b151,
			    &v[v_offset], ldv, &work[work_offset], ldwork, &
			    c_b15, &c__[c_offset], ldc)
			    ;
		}

/*              W := W * V2' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", n, k, &c_b15, &
			v[*m - *k + 1 + v_dim1], ldv, &work[work_offset],
			ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[*m - *k + j + i__ * c_dim1] -= work[i__ + j *
				work_dim1];
/* L80: */
		    }
/* L90: */
		}

	    } else if (lsame_(side, "R")) {

/*
                Form  C * H  or  C * H'  where  C = ( C1  C2 )

                W := C * V  =  (C1*V1 + C2*V2)  (stored in WORK)

                W := C2
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(m, &c__[(*n - *k + j) * c_dim1 + 1], &c__1, &work[
			    j * work_dim1 + 1], &c__1);
/* L100: */
		}

/*              W := W * V2 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", m, k, &c_b15,
			 &v[*n - *k + 1 + v_dim1], ldv, &work[work_offset],
			ldwork);
		if (*n > *k) {

/*                 W := W + C1 * V1 */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "No transpose", m, k, &i__1, &
			    c_b15, &c__[c_offset], ldc, &v[v_offset], ldv, &
			    c_b15, &work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Lower", trans, "Non-unit", m, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V' */

		if (*n > *k) {

/*                 C1 := C1 - W * V1' */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "Transpose", m, &i__1, k, &c_b151,
			    &work[work_offset], ldwork, &v[v_offset], ldv, &
			    c_b15, &c__[c_offset], ldc)
			    ;
		}

/*              W := W * V2' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", m, k, &c_b15, &
			v[*n - *k + 1 + v_dim1], ldv, &work[work_offset],
			ldwork);

/*              C2 := C2 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + (*n - *k + j) * c_dim1] -= work[i__ + j *
				work_dim1];
/* L110: */
		    }
/* L120: */
		}
	    }
	}

    } else if (lsame_(storev, "R")) {

	if (lsame_(direct, "F")) {

/*
             Let  V =  ( V1  V2 )    (V1: first K columns)
             where  V1  is unit upper triangular.
*/

	    if (lsame_(side, "L")) {

/*
                Form  H * C  or  H' * C  where  C = ( C1 )
                                                    ( C2 )

                W := C' * V'  =  (C1'*V1' + C2'*V2') (stored in WORK)

                W := C1'
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(n, &c__[j + c_dim1], ldc, &work[j * work_dim1 + 1],
			     &c__1);
/* L130: */
		}

/*              W := W * V1' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", n, k, &c_b15, &
			v[v_offset], ldv, &work[work_offset], ldwork);
		if (*m > *k) {

/*                 W := W + C2'*V2' */

		    i__1 = *m - *k;
		    dgemm_("Transpose", "Transpose", n, k, &i__1, &c_b15, &
			    c__[*k + 1 + c_dim1], ldc, &v[(*k + 1) * v_dim1 +
			    1], ldv, &c_b15, &work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Upper", transt, "Non-unit", n, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V' * W' */

		if (*m > *k) {

/*                 C2 := C2 - V2' * W' */

		    i__1 = *m - *k;
		    dgemm_("Transpose", "Transpose", &i__1, n, k, &c_b151, &v[
			    (*k + 1) * v_dim1 + 1], ldv, &work[work_offset],
			    ldwork, &c_b15, &c__[*k + 1 + c_dim1], ldc);
		}

/*              W := W * V1 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", n, k, &c_b15,
			 &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[j + i__ * c_dim1] -= work[i__ + j * work_dim1];
/* L140: */
		    }
/* L150: */
		}

	    } else if (lsame_(side, "R")) {

/*
                Form  C * H  or  C * H'  where  C = ( C1  C2 )

                W := C * V'  =  (C1*V1' + C2*V2')  (stored in WORK)

                W := C1
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(m, &c__[j * c_dim1 + 1], &c__1, &work[j *
			    work_dim1 + 1], &c__1);
/* L160: */
		}

/*              W := W * V1' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", m, k, &c_b15, &
			v[v_offset], ldv, &work[work_offset], ldwork);
		if (*n > *k) {

/*                 W := W + C2 * V2' */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "Transpose", m, k, &i__1, &c_b15, &
			    c__[(*k + 1) * c_dim1 + 1], ldc, &v[(*k + 1) *
			    v_dim1 + 1], ldv, &c_b15, &work[work_offset],
			    ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Upper", trans, "Non-unit", m, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V */

		if (*n > *k) {

/*                 C2 := C2 - W * V2 */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "No transpose", m, &i__1, k, &
			    c_b151, &work[work_offset], ldwork, &v[(*k + 1) *
			    v_dim1 + 1], ldv, &c_b15, &c__[(*k + 1) * c_dim1
			    + 1], ldc);
		}

/*              W := W * V1 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", m, k, &c_b15,
			 &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + j * c_dim1] -= work[i__ + j * work_dim1];
/* L170: */
		    }
/* L180: */
		}

	    }

	} else {

/*
             Let  V =  ( V1  V2 )    (V2: last K columns)
             where  V2  is unit lower triangular.
*/

	    if (lsame_(side, "L")) {

/*
                Form  H * C  or  H' * C  where  C = ( C1 )
                                                    ( C2 )

                W := C' * V'  =  (C1'*V1' + C2'*V2') (stored in WORK)

                W := C2'
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(n, &c__[*m - *k + j + c_dim1], ldc, &work[j *
			    work_dim1 + 1], &c__1);
/* L190: */
		}

/*              W := W * V2' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", n, k, &c_b15, &
			v[(*m - *k + 1) * v_dim1 + 1], ldv, &work[work_offset]
			, ldwork);
		if (*m > *k) {

/*                 W := W + C1'*V1' */

		    i__1 = *m - *k;
		    dgemm_("Transpose", "Transpose", n, k, &i__1, &c_b15, &
			    c__[c_offset], ldc, &v[v_offset], ldv, &c_b15, &
			    work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Lower", transt, "Non-unit", n, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V' * W' */

		if (*m > *k) {

/*                 C1 := C1 - V1' * W' */

		    i__1 = *m - *k;
		    dgemm_("Transpose", "Transpose", &i__1, n, k, &c_b151, &v[
			    v_offset], ldv, &work[work_offset], ldwork, &
			    c_b15, &c__[c_offset], ldc);
		}

/*              W := W * V2 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", n, k, &c_b15,
			 &v[(*m - *k + 1) * v_dim1 + 1], ldv, &work[
			work_offset], ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[*m - *k + j + i__ * c_dim1] -= work[i__ + j *
				work_dim1];
/* L200: */
		    }
/* L210: */
		}

	    } else if (lsame_(side, "R")) {

/*
                Form  C * H  or  C * H'  where  C = ( C1  C2 )

                W := C * V'  =  (C1*V1' + C2*V2')  (stored in WORK)

                W := C2
*/

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(m, &c__[(*n - *k + j) * c_dim1 + 1], &c__1, &work[
			    j * work_dim1 + 1], &c__1);
/* L220: */
		}

/*              W := W * V2' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", m, k, &c_b15, &
			v[(*n - *k + 1) * v_dim1 + 1], ldv, &work[work_offset]
			, ldwork);
		if (*n > *k) {

/*                 W := W + C1 * V1' */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "Transpose", m, k, &i__1, &c_b15, &
			    c__[c_offset], ldc, &v[v_offset], ldv, &c_b15, &
			    work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Lower", trans, "Non-unit", m, k, &c_b15, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V */

		if (*n > *k) {

/*                 C1 := C1 - W * V1 */

		    i__1 = *n - *k;
		    dgemm_("No transpose", "No transpose", m, &i__1, k, &
			    c_b151, &work[work_offset], ldwork, &v[v_offset],
			    ldv, &c_b15, &c__[c_offset], ldc);
		}

/*              W := W * V2 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", m, k, &c_b15,
			 &v[(*n - *k + 1) * v_dim1 + 1], ldv, &work[
			work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + (*n - *k + j) * c_dim1] -= work[i__ + j *
				work_dim1];
/* L230: */
		    }
/* L240: */
		}

	    }

	}
    }

    return 0;

/*     End of DLARFB */

} /* dlarfb_ */

/* Subroutine */ int dlarfg_(integer *n, doublereal *alpha, doublereal *x,
	integer *incx, doublereal *tau)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer j, knt;
    static doublereal beta;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    static doublereal xnorm;

    static doublereal safmin, rsafmn;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DLARFG generates a real elementary reflector H of order n, such
    that

          H * ( alpha ) = ( beta ),   H' * H = I.
              (   x   )   (   0  )

    where alpha and beta are scalars, and x is an (n-1)-element real
    vector. H is represented in the form

          H = I - tau * ( 1 ) * ( 1 v' ) ,
                        ( v )

    where tau is a real scalar and v is a real (n-1)-element
    vector.

    If the elements of x are all zero, then tau = 0 and H is taken to be
    the unit matrix.

    Otherwise  1 <= tau <= 2.

    Arguments
    =========

    N       (input) INTEGER
            The order of the elementary reflector.

    ALPHA   (input/output) DOUBLE PRECISION
            On entry, the value alpha.
            On exit, it is overwritten with the value beta.

    X       (input/output) DOUBLE PRECISION array, dimension
                           (1+(N-2)*abs(INCX))
            On entry, the vector x.
            On exit, it is overwritten with the vector v.

    INCX    (input) INTEGER
            The increment between elements of X. INCX > 0.

    TAU     (output) DOUBLE PRECISION
            The value tau.

    =====================================================================
*/


    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n <= 1) {
	*tau = 0.;
	return 0;
    }

    i__1 = *n - 1;
    xnorm = dnrm2_(&i__1, &x[1], incx);

    if (xnorm == 0.) {

/*        H  =  I */

	*tau = 0.;
    } else {

/*        general case */

	d__1 = dlapy2_(alpha, &xnorm);
	beta = -d_sign(&d__1, alpha);
	safmin = SAFEMINIMUM / EPSILON;
	if (abs(beta) < safmin) {

/*           XNORM, BETA may be inaccurate; scale X and recompute them */

	    rsafmn = 1. / safmin;
	    knt = 0;
L10:
	    ++knt;
	    i__1 = *n - 1;
	    dscal_(&i__1, &rsafmn, &x[1], incx);
	    beta *= rsafmn;
	    *alpha *= rsafmn;
	    if (abs(beta) < safmin) {
		goto L10;
	    }

/*           New BETA is at most 1, at least SAFMIN */

	    i__1 = *n - 1;
	    xnorm = dnrm2_(&i__1, &x[1], incx);
	    d__1 = dlapy2_(alpha, &xnorm);
	    beta = -d_sign(&d__1, alpha);
	    *tau = (beta - *alpha) / beta;
	    i__1 = *n - 1;
	    d__1 = 1. / (*alpha - beta);
	    dscal_(&i__1, &d__1, &x[1], incx);

/*           If ALPHA is subnormal, it may lose relative accuracy */

	    *alpha = beta;
	    i__1 = knt;
	    for (j = 1; j <= i__1; ++j) {
		*alpha *= safmin;
/* L20: */
	    }
	} else {
	    *tau = (beta - *alpha) / beta;
	    i__1 = *n - 1;
	    d__1 = 1. / (*alpha - beta);
	    dscal_(&i__1, &d__1, &x[1], incx);
	    *alpha = beta;
	}
    }

    return 0;

/*     End of DLARFG */

} /* dlarfg_ */

/* Subroutine */ int dlarft_(char *direct, char *storev, integer *n, integer *
	k, doublereal *v, integer *ldv, doublereal *tau, doublereal *t,
	integer *ldt)
{
    /* System generated locals */
    integer t_dim1, t_offset, v_dim1, v_offset, i__1, i__2, i__3;
    doublereal d__1;

    /* Local variables */
    static integer i__, j;
    static doublereal vii;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *), dtrmv_(char *,
	    char *, char *, integer *, doublereal *, integer *, doublereal *,
	    integer *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLARFT forms the triangular factor T of a real block reflector H
    of order n, which is defined as a product of k elementary reflectors.

    If DIRECT = 'F', H = H(1) H(2) . . . H(k) and T is upper triangular;

    If DIRECT = 'B', H = H(k) . . . H(2) H(1) and T is lower triangular.

    If STOREV = 'C', the vector which defines the elementary reflector
    H(i) is stored in the i-th column of the array V, and

       H  =  I - V * T * V'

    If STOREV = 'R', the vector which defines the elementary reflector
    H(i) is stored in the i-th row of the array V, and

       H  =  I - V' * T * V

    Arguments
    =========

    DIRECT  (input) CHARACTER*1
            Specifies the order in which the elementary reflectors are
            multiplied to form the block reflector:
            = 'F': H = H(1) H(2) . . . H(k) (Forward)
            = 'B': H = H(k) . . . H(2) H(1) (Backward)

    STOREV  (input) CHARACTER*1
            Specifies how the vectors which define the elementary
            reflectors are stored (see also Further Details):
            = 'C': columnwise
            = 'R': rowwise

    N       (input) INTEGER
            The order of the block reflector H. N >= 0.

    K       (input) INTEGER
            The order of the triangular factor T (= the number of
            elementary reflectors). K >= 1.

    V       (input/output) DOUBLE PRECISION array, dimension
                                 (LDV,K) if STOREV = 'C'
                                 (LDV,N) if STOREV = 'R'
            The matrix V. See further details.

    LDV     (input) INTEGER
            The leading dimension of the array V.
            If STOREV = 'C', LDV >= max(1,N); if STOREV = 'R', LDV >= K.

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i).

    T       (output) DOUBLE PRECISION array, dimension (LDT,K)
            The k by k triangular factor T of the block reflector.
            If DIRECT = 'F', T is upper triangular; if DIRECT = 'B', T is
            lower triangular. The rest of the array is not used.

    LDT     (input) INTEGER
            The leading dimension of the array T. LDT >= K.

    Further Details
    ===============

    The shape of the matrix V and the storage of the vectors which define
    the H(i) is best illustrated by the following example with n = 5 and
    k = 3. The elements equal to 1 are not stored; the corresponding
    array elements are modified but restored on exit. The rest of the
    array is not used.

    DIRECT = 'F' and STOREV = 'C':         DIRECT = 'F' and STOREV = 'R':

                 V = (  1       )                 V = (  1 v1 v1 v1 v1 )
                     ( v1  1    )                     (     1 v2 v2 v2 )
                     ( v1 v2  1 )                     (        1 v3 v3 )
                     ( v1 v2 v3 )
                     ( v1 v2 v3 )

    DIRECT = 'B' and STOREV = 'C':         DIRECT = 'B' and STOREV = 'R':

                 V = ( v1 v2 v3 )                 V = ( v1 v1  1       )
                     ( v1 v2 v3 )                     ( v2 v2 v2  1    )
                     (  1 v2 v3 )                     ( v3 v3 v3 v3  1 )
                     (     1 v3 )
                     (        1 )

    =====================================================================


       Quick return if possible
*/

    /* Parameter adjustments */
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1 * 1;
    v -= v_offset;
    --tau;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1 * 1;
    t -= t_offset;

    /* Function Body */
    if (*n == 0) {
	return 0;
    }

    if (lsame_(direct, "F")) {
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (tau[i__] == 0.) {

/*              H(i)  =  I */

		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    t[j + i__ * t_dim1] = 0.;
/* L10: */
		}
	    } else {

/*              general case */

		vii = v[i__ + i__ * v_dim1];
		v[i__ + i__ * v_dim1] = 1.;
		if (lsame_(storev, "C")) {

/*                 T(1:i-1,i) := - tau(i) * V(i:n,1:i-1)' * V(i:n,i) */

		    i__2 = *n - i__ + 1;
		    i__3 = i__ - 1;
		    d__1 = -tau[i__];
		    dgemv_("Transpose", &i__2, &i__3, &d__1, &v[i__ + v_dim1],
			     ldv, &v[i__ + i__ * v_dim1], &c__1, &c_b29, &t[
			    i__ * t_dim1 + 1], &c__1);
		} else {

/*                 T(1:i-1,i) := - tau(i) * V(1:i-1,i:n) * V(i,i:n)' */

		    i__2 = i__ - 1;
		    i__3 = *n - i__ + 1;
		    d__1 = -tau[i__];
		    dgemv_("No transpose", &i__2, &i__3, &d__1, &v[i__ *
			    v_dim1 + 1], ldv, &v[i__ + i__ * v_dim1], ldv, &
			    c_b29, &t[i__ * t_dim1 + 1], &c__1);
		}
		v[i__ + i__ * v_dim1] = vii;

/*              T(1:i-1,i) := T(1:i-1,1:i-1) * T(1:i-1,i) */

		i__2 = i__ - 1;
		dtrmv_("Upper", "No transpose", "Non-unit", &i__2, &t[
			t_offset], ldt, &t[i__ * t_dim1 + 1], &c__1);
		t[i__ + i__ * t_dim1] = tau[i__];
	    }
/* L20: */
	}
    } else {
	for (i__ = *k; i__ >= 1; --i__) {
	    if (tau[i__] == 0.) {

/*              H(i)  =  I */

		i__1 = *k;
		for (j = i__; j <= i__1; ++j) {
		    t[j + i__ * t_dim1] = 0.;
/* L30: */
		}
	    } else {

/*              general case */

		if (i__ < *k) {
		    if (lsame_(storev, "C")) {
			vii = v[*n - *k + i__ + i__ * v_dim1];
			v[*n - *k + i__ + i__ * v_dim1] = 1.;

/*
                      T(i+1:k,i) :=
                              - tau(i) * V(1:n-k+i,i+1:k)' * V(1:n-k+i,i)
*/

			i__1 = *n - *k + i__;
			i__2 = *k - i__;
			d__1 = -tau[i__];
			dgemv_("Transpose", &i__1, &i__2, &d__1, &v[(i__ + 1)
				* v_dim1 + 1], ldv, &v[i__ * v_dim1 + 1], &
				c__1, &c_b29, &t[i__ + 1 + i__ * t_dim1], &
				c__1);
			v[*n - *k + i__ + i__ * v_dim1] = vii;
		    } else {
			vii = v[i__ + (*n - *k + i__) * v_dim1];
			v[i__ + (*n - *k + i__) * v_dim1] = 1.;

/*
                      T(i+1:k,i) :=
                              - tau(i) * V(i+1:k,1:n-k+i) * V(i,1:n-k+i)'
*/

			i__1 = *k - i__;
			i__2 = *n - *k + i__;
			d__1 = -tau[i__];
			dgemv_("No transpose", &i__1, &i__2, &d__1, &v[i__ +
				1 + v_dim1], ldv, &v[i__ + v_dim1], ldv, &
				c_b29, &t[i__ + 1 + i__ * t_dim1], &c__1);
			v[i__ + (*n - *k + i__) * v_dim1] = vii;
		    }

/*                 T(i+1:k,i) := T(i+1:k,i+1:k) * T(i+1:k,i) */

		    i__1 = *k - i__;
		    dtrmv_("Lower", "No transpose", "Non-unit", &i__1, &t[i__
			    + 1 + (i__ + 1) * t_dim1], ldt, &t[i__ + 1 + i__ *
			     t_dim1], &c__1)
			    ;
		}
		t[i__ + i__ * t_dim1] = tau[i__];
	    }
/* L40: */
	}
    }
    return 0;

/*     End of DLARFT */

} /* dlarft_ */

/* Subroutine */ int dlarfx_(char *side, integer *m, integer *n, doublereal *
	v, doublereal *tau, doublereal *c__, integer *ldc, doublereal *work)
{
    /* System generated locals */
    integer c_dim1, c_offset, i__1;
    doublereal d__1;

    /* Local variables */
    static integer j;
    static doublereal t1, t2, t3, t4, t5, t6, t7, t8, t9, v1, v2, v3, v4, v5,
	    v6, v7, v8, v9, t10, v10, sum;
    extern /* Subroutine */ int dger_(integer *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLARFX applies a real elementary reflector H to a real m by n
    matrix C, from either the left or the right. H is represented in the
    form

          H = I - tau * v * v'

    where tau is a real scalar and v is a real vector.

    If tau = 0, then H is taken to be the unit matrix

    This version uses inline code if H has order < 11.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': form  H * C
            = 'R': form  C * H

    M       (input) INTEGER
            The number of rows of the matrix C.

    N       (input) INTEGER
            The number of columns of the matrix C.

    V       (input) DOUBLE PRECISION array, dimension (M) if SIDE = 'L'
                                       or (N) if SIDE = 'R'
            The vector v in the representation of H.

    TAU     (input) DOUBLE PRECISION
            The value tau in the representation of H.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by the matrix H * C if SIDE = 'L',
            or C * H if SIDE = 'R'.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDA >= (1,M).

    WORK    (workspace) DOUBLE PRECISION array, dimension
                        (N) if SIDE = 'L'
                        or (M) if SIDE = 'R'
            WORK is not referenced if H has order < 11.

    =====================================================================
*/


    /* Parameter adjustments */
    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    if (*tau == 0.) {
	return 0;
    }
    if (lsame_(side, "L")) {

/*        Form  H * C, where H has order m. */

	switch (*m) {
	    case 1:  goto L10;
	    case 2:  goto L30;
	    case 3:  goto L50;
	    case 4:  goto L70;
	    case 5:  goto L90;
	    case 6:  goto L110;
	    case 7:  goto L130;
	    case 8:  goto L150;
	    case 9:  goto L170;
	    case 10:  goto L190;
	}

/*
          Code for general M

          w := C'*v
*/

	dgemv_("Transpose", m, n, &c_b15, &c__[c_offset], ldc, &v[1], &c__1, &
		c_b29, &work[1], &c__1);

/*        C := C - tau * v * w' */

	d__1 = -(*tau);
	dger_(m, n, &d__1, &v[1], &c__1, &work[1], &c__1, &c__[c_offset], ldc)
		;
	goto L410;
L10:

/*        Special code for 1 x 1 Householder */

	t1 = 1. - *tau * v[1] * v[1];
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    c__[j * c_dim1 + 1] = t1 * c__[j * c_dim1 + 1];
/* L20: */
	}
	goto L410;
L30:

/*        Special code for 2 x 2 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
/* L40: */
	}
	goto L410;
L50:

/*        Special code for 3 x 3 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
/* L60: */
	}
	goto L410;
L70:

/*        Special code for 4 x 4 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
/* L80: */
	}
	goto L410;
L90:

/*        Special code for 5 x 5 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
/* L100: */
	}
	goto L410;
L110:

/*        Special code for 6 x 6 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
/* L120: */
	}
	goto L410;
L130:

/*        Special code for 7 x 7 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
/* L140: */
	}
	goto L410;
L150:

/*        Special code for 8 x 8 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7] + v8 * c__[j * c_dim1 + 8];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
	    c__[j * c_dim1 + 8] -= sum * t8;
/* L160: */
	}
	goto L410;
L170:

/*        Special code for 9 x 9 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7] + v8 * c__[j * c_dim1 + 8] + v9 * c__[j *
		    c_dim1 + 9];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
	    c__[j * c_dim1 + 8] -= sum * t8;
	    c__[j * c_dim1 + 9] -= sum * t9;
/* L180: */
	}
	goto L410;
L190:

/*        Special code for 10 x 10 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	v10 = v[10];
	t10 = *tau * v10;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7] + v8 * c__[j * c_dim1 + 8] + v9 * c__[j *
		    c_dim1 + 9] + v10 * c__[j * c_dim1 + 10];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
	    c__[j * c_dim1 + 8] -= sum * t8;
	    c__[j * c_dim1 + 9] -= sum * t9;
	    c__[j * c_dim1 + 10] -= sum * t10;
/* L200: */
	}
	goto L410;
    } else {

/*        Form  C * H, where H has order n. */

	switch (*n) {
	    case 1:  goto L210;
	    case 2:  goto L230;
	    case 3:  goto L250;
	    case 4:  goto L270;
	    case 5:  goto L290;
	    case 6:  goto L310;
	    case 7:  goto L330;
	    case 8:  goto L350;
	    case 9:  goto L370;
	    case 10:  goto L390;
	}

/*
          Code for general N

          w := C * v
*/

	dgemv_("No transpose", m, n, &c_b15, &c__[c_offset], ldc, &v[1], &
		c__1, &c_b29, &work[1], &c__1);

/*        C := C - tau * w * v' */

	d__1 = -(*tau);
	dger_(m, n, &d__1, &work[1], &c__1, &v[1], &c__1, &c__[c_offset], ldc)
		;
	goto L410;
L210:

/*        Special code for 1 x 1 Householder */

	t1 = 1. - *tau * v[1] * v[1];
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    c__[j + c_dim1] = t1 * c__[j + c_dim1];
/* L220: */
	}
	goto L410;
L230:

/*        Special code for 2 x 2 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
/* L240: */
	}
	goto L410;
L250:

/*        Special code for 3 x 3 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
/* L260: */
	}
	goto L410;
L270:

/*        Special code for 4 x 4 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3] + v4 * c__[j + ((c_dim1) << (2))];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + ((c_dim1) << (2))] -= sum * t4;
/* L280: */
	}
	goto L410;
L290:

/*        Special code for 5 x 5 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3] + v4 * c__[j + ((c_dim1) << (2))] +
		    v5 * c__[j + c_dim1 * 5];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + ((c_dim1) << (2))] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
/* L300: */
	}
	goto L410;
L310:

/*        Special code for 6 x 6 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3] + v4 * c__[j + ((c_dim1) << (2))] +
		    v5 * c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + ((c_dim1) << (2))] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
/* L320: */
	}
	goto L410;
L330:

/*        Special code for 7 x 7 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3] + v4 * c__[j + ((c_dim1) << (2))] +
		    v5 * c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 *
		     c__[j + c_dim1 * 7];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + ((c_dim1) << (2))] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
/* L340: */
	}
	goto L410;
L350:

/*        Special code for 8 x 8 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3] + v4 * c__[j + ((c_dim1) << (2))] +
		    v5 * c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 *
		     c__[j + c_dim1 * 7] + v8 * c__[j + ((c_dim1) << (3))];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + ((c_dim1) << (2))] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
	    c__[j + ((c_dim1) << (3))] -= sum * t8;
/* L360: */
	}
	goto L410;
L370:

/*        Special code for 9 x 9 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3] + v4 * c__[j + ((c_dim1) << (2))] +
		    v5 * c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 *
		     c__[j + c_dim1 * 7] + v8 * c__[j + ((c_dim1) << (3))] +
		    v9 * c__[j + c_dim1 * 9];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + ((c_dim1) << (2))] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
	    c__[j + ((c_dim1) << (3))] -= sum * t8;
	    c__[j + c_dim1 * 9] -= sum * t9;
/* L380: */
	}
	goto L410;
L390:

/*        Special code for 10 x 10 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	v10 = v[10];
	t10 = *tau * v10;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + ((c_dim1) << (1))] + v3
		    * c__[j + c_dim1 * 3] + v4 * c__[j + ((c_dim1) << (2))] +
		    v5 * c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 *
		     c__[j + c_dim1 * 7] + v8 * c__[j + ((c_dim1) << (3))] +
		    v9 * c__[j + c_dim1 * 9] + v10 * c__[j + c_dim1 * 10];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + ((c_dim1) << (1))] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + ((c_dim1) << (2))] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
	    c__[j + ((c_dim1) << (3))] -= sum * t8;
	    c__[j + c_dim1 * 9] -= sum * t9;
	    c__[j + c_dim1 * 10] -= sum * t10;
/* L400: */
	}
	goto L410;
    }
L410:
    return 0;

/*     End of DLARFX */

} /* dlarfx_ */

/* Subroutine */ int dlartg_(doublereal *f, doublereal *g, doublereal *cs,
	doublereal *sn, doublereal *r__)
{
    /* Initialized data */

    static logical first = TRUE_;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double log(doublereal), pow_di(doublereal *, integer *), sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static doublereal f1, g1, eps, scale;
    static integer count;
    static doublereal safmn2, safmx2;

    static doublereal safmin;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DLARTG generate a plane rotation so that

       [  CS  SN  ]  .  [ F ]  =  [ R ]   where CS**2 + SN**2 = 1.
       [ -SN  CS  ]     [ G ]     [ 0 ]

    This is a slower, more accurate version of the BLAS1 routine DROTG,
    with the following other differences:
       F and G are unchanged on return.
       If G=0, then CS=1 and SN=0.
       If F=0 and (G .ne. 0), then CS=0 and SN=1 without doing any
          floating point operations (saves work in DBDSQR when
          there are zeros on the diagonal).

    If F exceeds G in magnitude, CS will be positive.

    Arguments
    =========

    F       (input) DOUBLE PRECISION
            The first component of vector to be rotated.

    G       (input) DOUBLE PRECISION
            The second component of vector to be rotated.

    CS      (output) DOUBLE PRECISION
            The cosine of the rotation.

    SN      (output) DOUBLE PRECISION
            The sine of the rotation.

    R       (output) DOUBLE PRECISION
            The nonzero component of the rotated vector.

    =====================================================================
*/


    if (first) {
	first = FALSE_;
	safmin = SAFEMINIMUM;
	eps = EPSILON;
	d__1 = BASE;
	i__1 = (integer) (log(safmin / eps) / log(BASE) /
		2.);
	safmn2 = pow_di(&d__1, &i__1);
	safmx2 = 1. / safmn2;
    }
    if (*g == 0.) {
	*cs = 1.;
	*sn = 0.;
	*r__ = *f;
    } else if (*f == 0.) {
	*cs = 0.;
	*sn = 1.;
	*r__ = *g;
    } else {
	f1 = *f;
	g1 = *g;
/* Computing MAX */
	d__1 = abs(f1), d__2 = abs(g1);
	scale = max(d__1,d__2);
	if (scale >= safmx2) {
	    count = 0;
L10:
	    ++count;
	    f1 *= safmn2;
	    g1 *= safmn2;
/* Computing MAX */
	    d__1 = abs(f1), d__2 = abs(g1);
	    scale = max(d__1,d__2);
	    if (scale >= safmx2) {
		goto L10;
	    }
/* Computing 2nd power */
	    d__1 = f1;
/* Computing 2nd power */
	    d__2 = g1;
	    *r__ = sqrt(d__1 * d__1 + d__2 * d__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	    i__1 = count;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		*r__ *= safmx2;
/* L20: */
	    }
	} else if (scale <= safmn2) {
	    count = 0;
L30:
	    ++count;
	    f1 *= safmx2;
	    g1 *= safmx2;
/* Computing MAX */
	    d__1 = abs(f1), d__2 = abs(g1);
	    scale = max(d__1,d__2);
	    if (scale <= safmn2) {
		goto L30;
	    }
/* Computing 2nd power */
	    d__1 = f1;
/* Computing 2nd power */
	    d__2 = g1;
	    *r__ = sqrt(d__1 * d__1 + d__2 * d__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	    i__1 = count;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		*r__ *= safmn2;
/* L40: */
	    }
	} else {
/* Computing 2nd power */
	    d__1 = f1;
/* Computing 2nd power */
	    d__2 = g1;
	    *r__ = sqrt(d__1 * d__1 + d__2 * d__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	}
	if ((abs(*f) > abs(*g) && *cs < 0.)) {
	    *cs = -(*cs);
	    *sn = -(*sn);
	    *r__ = -(*r__);
	}
    }
    return 0;

/*     End of DLARTG */

} /* dlartg_ */

/* Subroutine */ int dlas2_(doublereal *f, doublereal *g, doublereal *h__,
	doublereal *ssmin, doublereal *ssmax)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal c__, fa, ga, ha, as, at, au, fhmn, fhmx;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DLAS2  computes the singular values of the 2-by-2 matrix
       [  F   G  ]
       [  0   H  ].
    On return, SSMIN is the smaller singular value and SSMAX is the
    larger singular value.

    Arguments
    =========

    F       (input) DOUBLE PRECISION
            The (1,1) element of the 2-by-2 matrix.

    G       (input) DOUBLE PRECISION
            The (1,2) element of the 2-by-2 matrix.

    H       (input) DOUBLE PRECISION
            The (2,2) element of the 2-by-2 matrix.

    SSMIN   (output) DOUBLE PRECISION
            The smaller singular value.

    SSMAX   (output) DOUBLE PRECISION
            The larger singular value.

    Further Details
    ===============

    Barring over/underflow, all output quantities are correct to within
    a few units in the last place (ulps), even in the absence of a guard
    digit in addition/subtraction.

    In IEEE arithmetic, the code works correctly if one matrix element is
    infinite.

    Overflow will not occur unless the largest singular value itself
    overflows, or is within a few ulps of overflow. (On machines with
    partial overflow, like the Cray, overflow may occur if the largest
    singular value is within a factor of 2 of overflow.)

    Underflow is harmless if underflow is gradual. Otherwise, results
    may correspond to a matrix modified by perturbations of size near
    the underflow threshold.

    ====================================================================
*/


    fa = abs(*f);
    ga = abs(*g);
    ha = abs(*h__);
    fhmn = min(fa,ha);
    fhmx = max(fa,ha);
    if (fhmn == 0.) {
	*ssmin = 0.;
	if (fhmx == 0.) {
	    *ssmax = ga;
	} else {
/* Computing 2nd power */
	    d__1 = min(fhmx,ga) / max(fhmx,ga);
	    *ssmax = max(fhmx,ga) * sqrt(d__1 * d__1 + 1.);
	}
    } else {
	if (ga < fhmx) {
	    as = fhmn / fhmx + 1.;
	    at = (fhmx - fhmn) / fhmx;
/* Computing 2nd power */
	    d__1 = ga / fhmx;
	    au = d__1 * d__1;
	    c__ = 2. / (sqrt(as * as + au) + sqrt(at * at + au));
	    *ssmin = fhmn * c__;
	    *ssmax = fhmx / c__;
	} else {
	    au = fhmx / ga;
	    if (au == 0.) {

/*
                Avoid possible harmful underflow if exponent range
                asymmetric (true SSMIN may not underflow even if
                AU underflows)
*/

		*ssmin = fhmn * fhmx / ga;
		*ssmax = ga;
	    } else {
		as = fhmn / fhmx + 1.;
		at = (fhmx - fhmn) / fhmx;
/* Computing 2nd power */
		d__1 = as * au;
/* Computing 2nd power */
		d__2 = at * au;
		c__ = 1. / (sqrt(d__1 * d__1 + 1.) + sqrt(d__2 * d__2 + 1.));
		*ssmin = fhmn * c__ * au;
		*ssmin += *ssmin;
		*ssmax = ga / (c__ + c__);
	    }
	}
    }
    return 0;

/*     End of DLAS2 */

} /* dlas2_ */

/* Subroutine */ int dlascl_(char *type__, integer *kl, integer *ku,
	doublereal *cfrom, doublereal *cto, integer *m, integer *n,
	doublereal *a, integer *lda, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;

    /* Local variables */
    static integer i__, j, k1, k2, k3, k4;
    static doublereal mul, cto1;
    static logical done;
    static doublereal ctoc;
    extern logical lsame_(char *, char *);
    static integer itype;
    static doublereal cfrom1;

    static doublereal cfromc;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static doublereal bignum, smlnum;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DLASCL multiplies the M by N real matrix A by the real scalar
    CTO/CFROM.  This is done without over/underflow as long as the final
    result CTO*A(I,J)/CFROM does not over/underflow. TYPE specifies that
    A may be full, upper triangular, lower triangular, upper Hessenberg,
    or banded.

    Arguments
    =========

    TYPE    (input) CHARACTER*1
            TYPE indices the storage type of the input matrix.
            = 'G':  A is a full matrix.
            = 'L':  A is a lower triangular matrix.
            = 'U':  A is an upper triangular matrix.
            = 'H':  A is an upper Hessenberg matrix.
            = 'B':  A is a symmetric band matrix with lower bandwidth KL
                    and upper bandwidth KU and with the only the lower
                    half stored.
            = 'Q':  A is a symmetric band matrix with lower bandwidth KL
                    and upper bandwidth KU and with the only the upper
                    half stored.
            = 'Z':  A is a band matrix with lower bandwidth KL and upper
                    bandwidth KU.

    KL      (input) INTEGER
            The lower bandwidth of A.  Referenced only if TYPE = 'B',
            'Q' or 'Z'.

    KU      (input) INTEGER
            The upper bandwidth of A.  Referenced only if TYPE = 'B',
            'Q' or 'Z'.

    CFROM   (input) DOUBLE PRECISION
    CTO     (input) DOUBLE PRECISION
            The matrix A is multiplied by CTO/CFROM. A(I,J) is computed
            without over/underflow if the final result CTO*A(I,J)/CFROM
            can be represented without over/underflow.  CFROM must be
            nonzero.

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,M)
            The matrix to be multiplied by CTO/CFROM.  See TYPE for the
            storage type.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    INFO    (output) INTEGER
            0  - successful exit
            <0 - if INFO = -i, the i-th argument had an illegal value.

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    *info = 0;

    if (lsame_(type__, "G")) {
	itype = 0;
    } else if (lsame_(type__, "L")) {
	itype = 1;
    } else if (lsame_(type__, "U")) {
	itype = 2;
    } else if (lsame_(type__, "H")) {
	itype = 3;
    } else if (lsame_(type__, "B")) {
	itype = 4;
    } else if (lsame_(type__, "Q")) {
	itype = 5;
    } else if (lsame_(type__, "Z")) {
	itype = 6;
    } else {
	itype = -1;
    }

    if (itype == -1) {
	*info = -1;
    } else if (*cfrom == 0.) {
	*info = -4;
    } else if (*m < 0) {
	*info = -6;
    } else if (*n < 0 || (itype == 4 && *n != *m) || (itype == 5 && *n != *m))
	     {
	*info = -7;
    } else if ((itype <= 3 && *lda < max(1,*m))) {
	*info = -9;
    } else if (itype >= 4) {
/* Computing MAX */
	i__1 = *m - 1;
	if (*kl < 0 || *kl > max(i__1,0)) {
	    *info = -2;
	} else /* if(complicated condition) */ {
/* Computing MAX */
	    i__1 = *n - 1;
	    if (*ku < 0 || *ku > max(i__1,0) || ((itype == 4 || itype == 5) &&
		     *kl != *ku)) {
		*info = -3;
	    } else if ((itype == 4 && *lda < *kl + 1) || (itype == 5 && *lda <
		     *ku + 1) || (itype == 6 && *lda < ((*kl) << (1)) + *ku +
		    1)) {
		*info = -9;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASCL", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *m == 0) {
	return 0;
    }

/*     Get machine parameters */

    smlnum = SAFEMINIMUM;
    bignum = 1. / smlnum;

    cfromc = *cfrom;
    ctoc = *cto;

L10:
    cfrom1 = cfromc * smlnum;
    cto1 = ctoc / bignum;
    if ((abs(cfrom1) > abs(ctoc) && ctoc != 0.)) {
	mul = smlnum;
	done = FALSE_;
	cfromc = cfrom1;
    } else if (abs(cto1) > abs(cfromc)) {
	mul = bignum;
	done = FALSE_;
	ctoc = cto1;
    } else {
	mul = ctoc / cfromc;
	done = TRUE_;
    }

    if (itype == 0) {

/*        Full matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L20: */
	    }
/* L30: */
	}

    } else if (itype == 1) {

/*        Lower triangular matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L40: */
	    }
/* L50: */
	}

    } else if (itype == 2) {

/*        Upper triangular matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = min(j,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L60: */
	    }
/* L70: */
	}

    } else if (itype == 3) {

/*        Upper Hessenberg matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = j + 1;
	    i__2 = min(i__3,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L80: */
	    }
/* L90: */
	}

    } else if (itype == 4) {

/*        Lower half of a symmetric band matrix */

	k3 = *kl + 1;
	k4 = *n + 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = k3, i__4 = k4 - j;
	    i__2 = min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L100: */
	    }
/* L110: */
	}

    } else if (itype == 5) {

/*        Upper half of a symmetric band matrix */

	k1 = *ku + 2;
	k3 = *ku + 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__2 = k1 - j;
	    i__3 = k3;
	    for (i__ = max(i__2,1); i__ <= i__3; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L120: */
	    }
/* L130: */
	}

    } else if (itype == 6) {

/*        Band matrix */

	k1 = *kl + *ku + 2;
	k2 = *kl + 1;
	k3 = ((*kl) << (1)) + *ku + 1;
	k4 = *kl + *ku + 1 + *m;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__3 = k1 - j;
/* Computing MIN */
	    i__4 = k3, i__5 = k4 - j;
	    i__2 = min(i__4,i__5);
	    for (i__ = max(i__3,k2); i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L140: */
	    }
/* L150: */
	}

    }

    if (! done) {
	goto L10;
    }

    return 0;

/*     End of DLASCL */

} /* dlascl_ */

/* Subroutine */ int dlasd0_(integer *n, integer *sqre, doublereal *d__,
	doublereal *e, doublereal *u, integer *ldu, doublereal *vt, integer *
	ldvt, integer *smlsiz, integer *iwork, doublereal *work, integer *
	info)
{
    /* System generated locals */
    integer u_dim1, u_offset, vt_dim1, vt_offset, i__1, i__2;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__, j, m, i1, ic, lf, nd, ll, nl, nr, im1, ncc, nlf, nrf,
	    iwk, lvl, ndb1, nlp1, nrp1;
    static doublereal beta;
    static integer idxq, nlvl;
    static doublereal alpha;
    static integer inode, ndiml, idxqc, ndimr, itemp, sqrei;
    extern /* Subroutine */ int dlasd1_(integer *, integer *, integer *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     doublereal *, integer *, integer *, integer *, doublereal *,
	    integer *), dlasdq_(char *, integer *, integer *, integer *,
	    integer *, integer *, doublereal *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *), dlasdt_(integer *, integer *,
	    integer *, integer *, integer *, integer *, integer *), xerbla_(
	    char *, integer *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    Using a divide and conquer approach, DLASD0 computes the singular
    value decomposition (SVD) of a real upper bidiagonal N-by-M
    matrix B with diagonal D and offdiagonal E, where M = N + SQRE.
    The algorithm computes orthogonal matrices U and VT such that
    B = U * S * VT. The singular values S are overwritten on D.

    A related subroutine, DLASDA, computes only the singular values,
    and optionally, the singular vectors in compact form.

    Arguments
    =========

    N      (input) INTEGER
           On entry, the row dimension of the upper bidiagonal matrix.
           This is also the dimension of the main diagonal array D.

    SQRE   (input) INTEGER
           Specifies the column dimension of the bidiagonal matrix.
           = 0: The bidiagonal matrix has column dimension M = N;
           = 1: The bidiagonal matrix has column dimension M = N+1;

    D      (input/output) DOUBLE PRECISION array, dimension (N)
           On entry D contains the main diagonal of the bidiagonal
           matrix.
           On exit D, if INFO = 0, contains its singular values.

    E      (input) DOUBLE PRECISION array, dimension (M-1)
           Contains the subdiagonal entries of the bidiagonal matrix.
           On exit, E has been destroyed.

    U      (output) DOUBLE PRECISION array, dimension at least (LDQ, N)
           On exit, U contains the left singular vectors.

    LDU    (input) INTEGER
           On entry, leading dimension of U.

    VT     (output) DOUBLE PRECISION array, dimension at least (LDVT, M)
           On exit, VT' contains the right singular vectors.

    LDVT   (input) INTEGER
           On entry, leading dimension of VT.

    SMLSIZ (input) INTEGER
           On entry, maximum size of the subproblems at the
           bottom of the computation tree.

    IWORK  INTEGER work array.
           Dimension must be at least (8 * N)

    WORK   DOUBLE PRECISION work array.
           Dimension must be at least (3 * M**2 + 2 * M)

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an singular value did not converge

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    --iwork;
    --work;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -2;
    }

    m = *n + *sqre;

    if (*ldu < *n) {
	*info = -6;
    } else if (*ldvt < m) {
	*info = -8;
    } else if (*smlsiz < 3) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD0", &i__1);
	return 0;
    }

/*     If the input matrix is too small, call DLASDQ to find the SVD. */

    if (*n <= *smlsiz) {
	dlasdq_("U", sqre, n, &m, n, &c__0, &d__[1], &e[1], &vt[vt_offset],
		ldvt, &u[u_offset], ldu, &u[u_offset], ldu, &work[1], info);
	return 0;
    }

/*     Set up the computation tree. */

    inode = 1;
    ndiml = inode + *n;
    ndimr = ndiml + *n;
    idxq = ndimr + *n;
    iwk = idxq + *n;
    dlasdt_(n, &nlvl, &nd, &iwork[inode], &iwork[ndiml], &iwork[ndimr],
	    smlsiz);

/*
       For the nodes on bottom level of the tree, solve
       their subproblems by DLASDQ.
*/

    ndb1 = (nd + 1) / 2;
    ncc = 0;
    i__1 = nd;
    for (i__ = ndb1; i__ <= i__1; ++i__) {

/*
       IC : center row of each node
       NL : number of rows of left  subproblem
       NR : number of rows of right subproblem
       NLF: starting row of the left   subproblem
       NRF: starting row of the right  subproblem
*/

	i1 = i__ - 1;
	ic = iwork[inode + i1];
	nl = iwork[ndiml + i1];
	nlp1 = nl + 1;
	nr = iwork[ndimr + i1];
	nrp1 = nr + 1;
	nlf = ic - nl;
	nrf = ic + 1;
	sqrei = 1;
	dlasdq_("U", &sqrei, &nl, &nlp1, &nl, &ncc, &d__[nlf], &e[nlf], &vt[
		nlf + nlf * vt_dim1], ldvt, &u[nlf + nlf * u_dim1], ldu, &u[
		nlf + nlf * u_dim1], ldu, &work[1], info);
	if (*info != 0) {
	    return 0;
	}
	itemp = idxq + nlf - 2;
	i__2 = nl;
	for (j = 1; j <= i__2; ++j) {
	    iwork[itemp + j] = j;
/* L10: */
	}
	if (i__ == nd) {
	    sqrei = *sqre;
	} else {
	    sqrei = 1;
	}
	nrp1 = nr + sqrei;
	dlasdq_("U", &sqrei, &nr, &nrp1, &nr, &ncc, &d__[nrf], &e[nrf], &vt[
		nrf + nrf * vt_dim1], ldvt, &u[nrf + nrf * u_dim1], ldu, &u[
		nrf + nrf * u_dim1], ldu, &work[1], info);
	if (*info != 0) {
	    return 0;
	}
	itemp = idxq + ic;
	i__2 = nr;
	for (j = 1; j <= i__2; ++j) {
	    iwork[itemp + j - 1] = j;
/* L20: */
	}
/* L30: */
    }

/*     Now conquer each subproblem bottom-up. */

    for (lvl = nlvl; lvl >= 1; --lvl) {

/*
          Find the first node LF and last node LL on the
          current level LVL.
*/

	if (lvl == 1) {
	    lf = 1;
	    ll = 1;
	} else {
	    i__1 = lvl - 1;
	    lf = pow_ii(&c__2, &i__1);
	    ll = ((lf) << (1)) - 1;
	}
	i__1 = ll;
	for (i__ = lf; i__ <= i__1; ++i__) {
	    im1 = i__ - 1;
	    ic = iwork[inode + im1];
	    nl = iwork[ndiml + im1];
	    nr = iwork[ndimr + im1];
	    nlf = ic - nl;
	    if ((*sqre == 0 && i__ == ll)) {
		sqrei = *sqre;
	    } else {
		sqrei = 1;
	    }
	    idxqc = idxq + nlf - 1;
	    alpha = d__[ic];
	    beta = e[ic];
	    dlasd1_(&nl, &nr, &sqrei, &d__[nlf], &alpha, &beta, &u[nlf + nlf *
		     u_dim1], ldu, &vt[nlf + nlf * vt_dim1], ldvt, &iwork[
		    idxqc], &iwork[iwk], &work[1], info);
	    if (*info != 0) {
		return 0;
	    }
/* L40: */
	}
/* L50: */
    }

    return 0;

/*     End of DLASD0 */

} /* dlasd0_ */

/* Subroutine */ int dlasd1_(integer *nl, integer *nr, integer *sqre,
	doublereal *d__, doublereal *alpha, doublereal *beta, doublereal *u,
	integer *ldu, doublereal *vt, integer *ldvt, integer *idxq, integer *
	iwork, doublereal *work, integer *info)
{
    /* System generated locals */
    integer u_dim1, u_offset, vt_dim1, vt_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__, k, m, n, n1, n2, iq, iz, iu2, ldq, idx, ldu2, ivt2,
	    idxc, idxp, ldvt2;
    extern /* Subroutine */ int dlasd2_(integer *, integer *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, integer *,
	    integer *, integer *, integer *, integer *, integer *), dlasd3_(
	    integer *, integer *, integer *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, integer *, integer *, doublereal *, integer *),
	    dlascl_(char *, integer *, integer *, doublereal *, doublereal *,
	    integer *, integer *, doublereal *, integer *, integer *),
	     dlamrg_(integer *, integer *, doublereal *, integer *, integer *,
	     integer *);
    static integer isigma;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static doublereal orgnrm;
    static integer coltyp;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLASD1 computes the SVD of an upper bidiagonal N-by-M matrix B,
    where N = NL + NR + 1 and M = N + SQRE. DLASD1 is called from DLASD0.

    A related subroutine DLASD7 handles the case in which the singular
    values (and the singular vectors in factored form) are desired.

    DLASD1 computes the SVD as follows:

                  ( D1(in)  0    0     0 )
      B = U(in) * (   Z1'   a   Z2'    b ) * VT(in)
                  (   0     0   D2(in) 0 )

        = U(out) * ( D(out) 0) * VT(out)

    where Z' = (Z1' a Z2' b) = u' VT', and u is a vector of dimension M
    with ALPHA and BETA in the NL+1 and NL+2 th entries and zeros
    elsewhere; and the entry b is empty if SQRE = 0.

    The left singular vectors of the original matrix are stored in U, and
    the transpose of the right singular vectors are stored in VT, and the
    singular values are in D.  The algorithm consists of three stages:

       The first stage consists of deflating the size of the problem
       when there are multiple singular values or when there are zeros in
       the Z vector.  For each such occurence the dimension of the
       secular equation problem is reduced by one.  This stage is
       performed by the routine DLASD2.

       The second stage consists of calculating the updated
       singular values. This is done by finding the square roots of the
       roots of the secular equation via the routine DLASD4 (as called
       by DLASD3). This routine also calculates the singular vectors of
       the current problem.

       The final stage consists of computing the updated singular vectors
       directly using the updated singular values.  The singular vectors
       for the current problem are multiplied with the singular vectors
       from the overall problem.

    Arguments
    =========

    NL     (input) INTEGER
           The row dimension of the upper block.  NL >= 1.

    NR     (input) INTEGER
           The row dimension of the lower block.  NR >= 1.

    SQRE   (input) INTEGER
           = 0: the lower block is an NR-by-NR square matrix.
           = 1: the lower block is an NR-by-(NR+1) rectangular matrix.

           The bidiagonal matrix has row dimension N = NL + NR + 1,
           and column dimension M = N + SQRE.

    D      (input/output) DOUBLE PRECISION array,
                          dimension (N = NL+NR+1).
           On entry D(1:NL,1:NL) contains the singular values of the
           upper block; and D(NL+2:N) contains the singular values of
           the lower block. On exit D(1:N) contains the singular values
           of the modified matrix.

    ALPHA  (input) DOUBLE PRECISION
           Contains the diagonal element associated with the added row.

    BETA   (input) DOUBLE PRECISION
           Contains the off-diagonal element associated with the added
           row.

    U      (input/output) DOUBLE PRECISION array, dimension(LDU,N)
           On entry U(1:NL, 1:NL) contains the left singular vectors of
           the upper block; U(NL+2:N, NL+2:N) contains the left singular
           vectors of the lower block. On exit U contains the left
           singular vectors of the bidiagonal matrix.

    LDU    (input) INTEGER
           The leading dimension of the array U.  LDU >= max( 1, N ).

    VT     (input/output) DOUBLE PRECISION array, dimension(LDVT,M)
           where M = N + SQRE.
           On entry VT(1:NL+1, 1:NL+1)' contains the right singular
           vectors of the upper block; VT(NL+2:M, NL+2:M)' contains
           the right singular vectors of the lower block. On exit
           VT' contains the right singular vectors of the
           bidiagonal matrix.

    LDVT   (input) INTEGER
           The leading dimension of the array VT.  LDVT >= max( 1, M ).

    IDXQ  (output) INTEGER array, dimension(N)
           This contains the permutation which will reintegrate the
           subproblem just solved back into sorted order, i.e.
           D( IDXQ( I = 1, N ) ) will be in ascending order.

    IWORK  (workspace) INTEGER array, dimension( 4 * N )

    WORK   (workspace) DOUBLE PRECISION array, dimension( 3*M**2 + 2*M )

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an singular value did not converge

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    --idxq;
    --iwork;
    --work;

    /* Function Body */
    *info = 0;

    if (*nl < 1) {
	*info = -1;
    } else if (*nr < 1) {
	*info = -2;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -3;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD1", &i__1);
	return 0;
    }

    n = *nl + *nr + 1;
    m = n + *sqre;

/*
       The following values are for bookkeeping purposes only.  They are
       integer pointers which indicate the portion of the workspace
       used by a particular array in DLASD2 and DLASD3.
*/

    ldu2 = n;
    ldvt2 = m;

    iz = 1;
    isigma = iz + m;
    iu2 = isigma + n;
    ivt2 = iu2 + ldu2 * n;
    iq = ivt2 + ldvt2 * m;

    idx = 1;
    idxc = idx + n;
    coltyp = idxc + n;
    idxp = coltyp + n;

/*
       Scale.

   Computing MAX
*/
    d__1 = abs(*alpha), d__2 = abs(*beta);
    orgnrm = max(d__1,d__2);
    d__[*nl + 1] = 0.;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) > orgnrm) {
	    orgnrm = (d__1 = d__[i__], abs(d__1));
	}
/* L10: */
    }
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, &n, &c__1, &d__[1], &n, info);
    *alpha /= orgnrm;
    *beta /= orgnrm;

/*     Deflate singular values. */

    dlasd2_(nl, nr, sqre, &k, &d__[1], &work[iz], alpha, beta, &u[u_offset],
	    ldu, &vt[vt_offset], ldvt, &work[isigma], &work[iu2], &ldu2, &
	    work[ivt2], &ldvt2, &iwork[idxp], &iwork[idx], &iwork[idxc], &
	    idxq[1], &iwork[coltyp], info);

/*     Solve Secular Equation and update singular vectors. */

    ldq = k;
    dlasd3_(nl, nr, sqre, &k, &d__[1], &work[iq], &ldq, &work[isigma], &u[
	    u_offset], ldu, &work[iu2], &ldu2, &vt[vt_offset], ldvt, &work[
	    ivt2], &ldvt2, &iwork[idxc], &iwork[coltyp], &work[iz], info);
    if (*info != 0) {
	return 0;
    }

/*     Unscale. */

    dlascl_("G", &c__0, &c__0, &c_b15, &orgnrm, &n, &c__1, &d__[1], &n, info);

/*     Prepare the IDXQ sorting permutation. */

    n1 = k;
    n2 = n - k;
    dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &idxq[1]);

    return 0;

/*     End of DLASD1 */

} /* dlasd1_ */

/* Subroutine */ int dlasd2_(integer *nl, integer *nr, integer *sqre, integer
	*k, doublereal *d__, doublereal *z__, doublereal *alpha, doublereal *
	beta, doublereal *u, integer *ldu, doublereal *vt, integer *ldvt,
	doublereal *dsigma, doublereal *u2, integer *ldu2, doublereal *vt2,
	integer *ldvt2, integer *idxp, integer *idx, integer *idxc, integer *
	idxq, integer *coltyp, integer *info)
{
    /* System generated locals */
    integer u_dim1, u_offset, u2_dim1, u2_offset, vt_dim1, vt_offset,
	    vt2_dim1, vt2_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static doublereal c__;
    static integer i__, j, m, n;
    static doublereal s;
    static integer k2;
    static doublereal z1;
    static integer ct, jp;
    static doublereal eps, tau, tol;
    static integer psm[4], nlp1, nlp2, idxi, idxj;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    static integer ctot[4], idxjp;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer jprev;

    extern /* Subroutine */ int dlamrg_(integer *, integer *, doublereal *,
	    integer *, integer *, integer *), dlacpy_(char *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *), dlaset_(char *, integer *, integer *, doublereal *,
	    doublereal *, doublereal *, integer *), xerbla_(char *,
	    integer *);
    static doublereal hlftol;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       October 31, 1999


    Purpose
    =======

    DLASD2 merges the two sets of singular values together into a single
    sorted set.  Then it tries to deflate the size of the problem.
    There are two ways in which deflation can occur:  when two or more
    singular values are close together or if there is a tiny entry in the
    Z vector.  For each such occurrence the order of the related secular
    equation problem is reduced by one.

    DLASD2 is called from DLASD1.

    Arguments
    =========

    NL     (input) INTEGER
           The row dimension of the upper block.  NL >= 1.

    NR     (input) INTEGER
           The row dimension of the lower block.  NR >= 1.

    SQRE   (input) INTEGER
           = 0: the lower block is an NR-by-NR square matrix.
           = 1: the lower block is an NR-by-(NR+1) rectangular matrix.

           The bidiagonal matrix has N = NL + NR + 1 rows and
           M = N + SQRE >= N columns.

    K      (output) INTEGER
           Contains the dimension of the non-deflated matrix,
           This is the order of the related secular equation. 1 <= K <=N.

    D      (input/output) DOUBLE PRECISION array, dimension(N)
           On entry D contains the singular values of the two submatrices
           to be combined.  On exit D contains the trailing (N-K) updated
           singular values (those which were deflated) sorted into
           increasing order.

    ALPHA  (input) DOUBLE PRECISION
           Contains the diagonal element associated with the added row.

    BETA   (input) DOUBLE PRECISION
           Contains the off-diagonal element associated with the added
           row.

    U      (input/output) DOUBLE PRECISION array, dimension(LDU,N)
           On entry U contains the left singular vectors of two
           submatrices in the two square blocks with corners at (1,1),
           (NL, NL), and (NL+2, NL+2), (N,N).
           On exit U contains the trailing (N-K) updated left singular
           vectors (those which were deflated) in its last N-K columns.

    LDU    (input) INTEGER
           The leading dimension of the array U.  LDU >= N.

    Z      (output) DOUBLE PRECISION array, dimension(N)
           On exit Z contains the updating row vector in the secular
           equation.

    DSIGMA (output) DOUBLE PRECISION array, dimension (N)
           Contains a copy of the diagonal elements (K-1 singular values
           and one zero) in the secular equation.

    U2     (output) DOUBLE PRECISION array, dimension(LDU2,N)
           Contains a copy of the first K-1 left singular vectors which
           will be used by DLASD3 in a matrix multiply (DGEMM) to solve
           for the new left singular vectors. U2 is arranged into four
           blocks. The first block contains a column with 1 at NL+1 and
           zero everywhere else; the second block contains non-zero
           entries only at and above NL; the third contains non-zero
           entries only below NL+1; and the fourth is dense.

    LDU2   (input) INTEGER
           The leading dimension of the array U2.  LDU2 >= N.

    VT     (input/output) DOUBLE PRECISION array, dimension(LDVT,M)
           On entry VT' contains the right singular vectors of two
           submatrices in the two square blocks with corners at (1,1),
           (NL+1, NL+1), and (NL+2, NL+2), (M,M).
           On exit VT' contains the trailing (N-K) updated right singular
           vectors (those which were deflated) in its last N-K columns.
           In case SQRE =1, the last row of VT spans the right null
           space.

    LDVT   (input) INTEGER
           The leading dimension of the array VT.  LDVT >= M.

    VT2    (output) DOUBLE PRECISION array, dimension(LDVT2,N)
           VT2' contains a copy of the first K right singular vectors
           which will be used by DLASD3 in a matrix multiply (DGEMM) to
           solve for the new right singular vectors. VT2 is arranged into
           three blocks. The first block contains a row that corresponds
           to the special 0 diagonal element in SIGMA; the second block
           contains non-zeros only at and before NL +1; the third block
           contains non-zeros only at and after  NL +2.

    LDVT2  (input) INTEGER
           The leading dimension of the array VT2.  LDVT2 >= M.

    IDXP   (workspace) INTEGER array, dimension(N)
           This will contain the permutation used to place deflated
           values of D at the end of the array. On output IDXP(2:K)
           points to the nondeflated D-values and IDXP(K+1:N)
           points to the deflated singular values.

    IDX    (workspace) INTEGER array, dimension(N)
           This will contain the permutation used to sort the contents of
           D into ascending order.

    IDXC   (output) INTEGER array, dimension(N)
           This will contain the permutation used to arrange the columns
           of the deflated U matrix into three groups:  the first group
           contains non-zero entries only at and above NL, the second
           contains non-zero entries only below NL+2, and the third is
           dense.

    COLTYP (workspace/output) INTEGER array, dimension(N)
           As workspace, this will contain a label which will indicate
           which of the following types a column in the U2 matrix or a
           row in the VT2 matrix is:
           1 : non-zero in the upper half only
           2 : non-zero in the lower half only
           3 : dense
           4 : deflated

           On exit, it is an array of dimension 4, with COLTYP(I) being
           the dimension of the I-th type columns.

    IDXQ   (input) INTEGER array, dimension(N)
           This contains the permutation which separately sorts the two
           sub-problems in D into ascending order.  Note that entries in
           the first hlaf of this permutation must first be moved one
           position backward; and entries in the second half
           must first have NL+1 added to their values.

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --z__;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    --dsigma;
    u2_dim1 = *ldu2;
    u2_offset = 1 + u2_dim1 * 1;
    u2 -= u2_offset;
    vt2_dim1 = *ldvt2;
    vt2_offset = 1 + vt2_dim1 * 1;
    vt2 -= vt2_offset;
    --idxp;
    --idx;
    --idxc;
    --idxq;
    --coltyp;

    /* Function Body */
    *info = 0;

    if (*nl < 1) {
	*info = -1;
    } else if (*nr < 1) {
	*info = -2;
    } else if ((*sqre != 1 && *sqre != 0)) {
	*info = -3;
    }

    n = *nl + *nr + 1;
    m = n + *sqre;

    if (*ldu < n) {
	*info = -10;
    } else if (*ldvt < m) {
	*info = -12;
    } else if (*ldu2 < n) {
	*info = -15;
    } else if (*ldvt2 < m) {
	*info = -17;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD2", &i__1);
	return 0;
    }

    nlp1 = *nl + 1;
    nlp2 = *nl + 2;

/*
       Generate the first part of the vector Z; and move the singular
       values in the first part of D one position backward.
*/

    z1 = *alpha * vt[nlp1 + nlp1 * vt_dim1];
    z__[1] = z1;
    for (i__ = *nl; i__ >= 1; --i__) {
	z__[i__ + 1] = *alpha * vt[i__ + nlp1 * vt_dim1];
	d__[i__ + 1] = d__[i__];
	idxq[i__ + 1] = idxq[i__] + 1;
/* L10: */
    }

/*     Generate the second part of the vector Z. */

    i__1 = m;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	z__[i__] = *beta * vt[i__ + nlp2 * vt_dim1];
/* L20: */
    }

/*     Initialize some reference arrays. */

    i__1 = nlp1;
    for (i__ = 2; i__ <= i__1; ++i__) {
	coltyp[i__] = 1;
/* L30: */
    }
    i__1 = n;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	coltyp[i__] = 2;
/* L40: */
    }

/*     Sort the singular values into increasing order */

    i__1 = n;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	idxq[i__] += nlp1;
/* L50: */
    }

/*
       DSIGMA, IDXC, IDXC, and the first column of U2
       are used as storage space.
*/

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	dsigma[i__] = d__[idxq[i__]];
	u2[i__ + u2_dim1] = z__[idxq[i__]];
	idxc[i__] = coltyp[idxq[i__]];
/* L60: */
    }

    dlamrg_(nl, nr, &dsigma[2], &c__1, &c__1, &idx[2]);

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	idxi = idx[i__] + 1;
	d__[i__] = dsigma[idxi];
	z__[i__] = u2[idxi + u2_dim1];
	coltyp[i__] = idxc[idxi];
/* L70: */
    }

/*     Calculate the allowable deflation tolerance */

    eps = EPSILON;
/* Computing MAX */
    d__1 = abs(*alpha), d__2 = abs(*beta);
    tol = max(d__1,d__2);
/* Computing MAX */
    d__2 = (d__1 = d__[n], abs(d__1));
    tol = eps * 8. * max(d__2,tol);

/*
       There are 2 kinds of deflation -- first a value in the z-vector
       is small, second two (or more) singular values are very close
       together (their difference is small).

       If the value in the z-vector is small, we simply permute the
       array so that the corresponding singular value is moved to the
       end.

       If two values in the D-vector are close, we perform a two-sided
       rotation designed to make one of the corresponding z-vector
       entries zero, and then permute the array so that the deflated
       singular value is moved to the end.

       If there are multiple singular values then the problem deflates.
       Here the number of equal singular values are found.  As each equal
       singular value is found, an elementary reflector is computed to
       rotate the corresponding singular subspace so that the
       corresponding components of Z are zero in this new basis.
*/

    *k = 1;
    k2 = n + 1;
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    idxp[k2] = j;
	    coltyp[j] = 4;
	    if (j == n) {
		goto L120;
	    }
	} else {
	    jprev = j;
	    goto L90;
	}
/* L80: */
    }
L90:
    j = jprev;
L100:
    ++j;
    if (j > n) {
	goto L110;
    }
    if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	idxp[k2] = j;
	coltyp[j] = 4;
    } else {

/*        Check if singular values are close enough to allow deflation. */

	if ((d__1 = d__[j] - d__[jprev], abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    s = z__[jprev];
	    c__ = z__[j];

/*
             Find sqrt(a**2+b**2) without overflow or
             destructive underflow.
*/

	    tau = dlapy2_(&c__, &s);
	    c__ /= tau;
	    s = -s / tau;
	    z__[j] = tau;
	    z__[jprev] = 0.;

/*
             Apply back the Givens rotation to the left and right
             singular vector matrices.
*/

	    idxjp = idxq[idx[jprev] + 1];
	    idxj = idxq[idx[j] + 1];
	    if (idxjp <= nlp1) {
		--idxjp;
	    }
	    if (idxj <= nlp1) {
		--idxj;
	    }
	    drot_(&n, &u[idxjp * u_dim1 + 1], &c__1, &u[idxj * u_dim1 + 1], &
		    c__1, &c__, &s);
	    drot_(&m, &vt[idxjp + vt_dim1], ldvt, &vt[idxj + vt_dim1], ldvt, &
		    c__, &s);
	    if (coltyp[j] != coltyp[jprev]) {
		coltyp[j] = 3;
	    }
	    coltyp[jprev] = 4;
	    --k2;
	    idxp[k2] = jprev;
	    jprev = j;
	} else {
	    ++(*k);
	    u2[*k + u2_dim1] = z__[jprev];
	    dsigma[*k] = d__[jprev];
	    idxp[*k] = jprev;
	    jprev = j;
	}
    }
    goto L100;
L110:

/*     Record the last singular value. */

    ++(*k);
    u2[*k + u2_dim1] = z__[jprev];
    dsigma[*k] = d__[jprev];
    idxp[*k] = jprev;

L120:

/*
       Count up the total number of the various types of columns, then
       form a permutation which positions the four column types into
       four groups of uniform structure (although one or more of these
       groups may be empty).
*/

    for (j = 1; j <= 4; ++j) {
	ctot[j - 1] = 0;
/* L130: */
    }
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	ct = coltyp[j];
	++ctot[ct - 1];
/* L140: */
    }

/*     PSM(*) = Position in SubMatrix (of types 1 through 4) */

    psm[0] = 2;
    psm[1] = ctot[0] + 2;
    psm[2] = psm[1] + ctot[1];
    psm[3] = psm[2] + ctot[2];

/*
       Fill out the IDXC array so that the permutation which it induces
       will place all type-1 columns first, all type-2 columns next,
       then all type-3's, and finally all type-4's, starting from the
       second column. This applies similarly to the rows of VT.
*/

    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	jp = idxp[j];
	ct = coltyp[jp];
	idxc[psm[ct - 1]] = j;
	++psm[ct - 1];
/* L150: */
    }

/*
       Sort the singular values and corresponding singular vectors into
       DSIGMA, U2, and VT2 respectively.  The singular values/vectors
       which were not deflated go into the first K slots of DSIGMA, U2,
       and VT2 respectively, while those which were deflated go into the
       last N - K slots, except that the first column/row will be treated
       separately.
*/

    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	jp = idxp[j];
	dsigma[j] = d__[jp];
	idxj = idxq[idx[idxp[idxc[j]]] + 1];
	if (idxj <= nlp1) {
	    --idxj;
	}
	dcopy_(&n, &u[idxj * u_dim1 + 1], &c__1, &u2[j * u2_dim1 + 1], &c__1);
	dcopy_(&m, &vt[idxj + vt_dim1], ldvt, &vt2[j + vt2_dim1], ldvt2);
/* L160: */
    }

/*     Determine DSIGMA(1), DSIGMA(2) and Z(1) */

    dsigma[1] = 0.;
    hlftol = tol / 2.;
    if (abs(dsigma[2]) <= hlftol) {
	dsigma[2] = hlftol;
    }
    if (m > n) {
	z__[1] = dlapy2_(&z1, &z__[m]);
	if (z__[1] <= tol) {
	    c__ = 1.;
	    s = 0.;
	    z__[1] = tol;
	} else {
	    c__ = z1 / z__[1];
	    s = z__[m] / z__[1];
	}
    } else {
	if (abs(z1) <= tol) {
	    z__[1] = tol;
	} else {
	    z__[1] = z1;
	}
    }

/*     Move the rest of the updating row to Z. */

    i__1 = *k - 1;
    dcopy_(&i__1, &u2[u2_dim1 + 2], &c__1, &z__[2], &c__1);

/*
       Determine the first column of U2, the first row of VT2 and the
       last row of VT.
*/

    dlaset_("A", &n, &c__1, &c_b29, &c_b29, &u2[u2_offset], ldu2);
    u2[nlp1 + u2_dim1] = 1.;
    if (m > n) {
	i__1 = nlp1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    vt[m + i__ * vt_dim1] = -s * vt[nlp1 + i__ * vt_dim1];
	    vt2[i__ * vt2_dim1 + 1] = c__ * vt[nlp1 + i__ * vt_dim1];
/* L170: */
	}
	i__1 = m;
	for (i__ = nlp2; i__ <= i__1; ++i__) {
	    vt2[i__ * vt2_dim1 + 1] = s * vt[m + i__ * vt_dim1];
	    vt[m + i__ * vt_dim1] = c__ * vt[m + i__ * vt_dim1];
/* L180: */
	}
    } else {
	dcopy_(&m, &vt[nlp1 + vt_dim1], ldvt, &vt2[vt2_dim1 + 1], ldvt2);
    }
    if (m > n) {
	dcopy_(&m, &vt[m + vt_dim1], ldvt, &vt2[m + vt2_dim1], ldvt2);
    }

/*
       The deflated singular values and their corresponding vectors go
       into the back of D, U, and V respectively.
*/

    if (n > *k) {
	i__1 = n - *k;
	dcopy_(&i__1, &dsigma[*k + 1], &c__1, &d__[*k + 1], &c__1);
	i__1 = n - *k;
	dlacpy_("A", &n, &i__1, &u2[(*k + 1) * u2_dim1 + 1], ldu2, &u[(*k + 1)
		 * u_dim1 + 1], ldu);
	i__1 = n - *k;
	dlacpy_("A", &i__1, &m, &vt2[*k + 1 + vt2_dim1], ldvt2, &vt[*k + 1 +
		vt_dim1], ldvt);
    }

/*     Copy CTOT into COLTYP for referencing in DLASD3. */

    for (j = 1; j <= 4; ++j) {
	coltyp[j] = ctot[j - 1];
/* L190: */
    }

    return 0;

/*     End of DLASD2 */

} /* dlasd2_ */

/* Subroutine */ int dlasd3_(integer *nl, integer *nr, integer *sqre, integer
	*k, doublereal *d__, doublereal *q, integer *ldq, doublereal *dsigma,
	doublereal *u, integer *ldu, doublereal *u2, integer *ldu2,
	doublereal *vt, integer *ldvt, doublereal *vt2, integer *ldvt2,
	integer *idxc, integer *ctot, doublereal *z__, integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, u_dim1, u_offset, u2_dim1, u2_offset, vt_dim1,
	    vt_offset, vt2_dim1, vt2_offset, i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer i__, j, m, n, jc;
    static doublereal rho;
    static integer nlp1, nlp2, nrp1;
    static doublereal temp;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    static integer ctemp;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer ktemp;
    extern doublereal dlamc3_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlasd4_(integer *, integer *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, integer *), dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *), dlacpy_(char *, integer *, integer
	    *, doublereal *, integer *, doublereal *, integer *),
	    xerbla_(char *, integer *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       October 31, 1999


    Purpose
    =======

    DLASD3 finds all the square roots of the roots of the secular
    equation, as defined by the values in D and Z.  It makes the
    appropriate calls to DLASD4 and then updates the singular
    vectors by matrix multiplication.

    This code makes very mild assumptions about floating point
    arithmetic. It will work on machines with a guard digit in
    add/subtract, or on those binary machines without guard digits
    which subtract like the Cray XMP, Cray YMP, Cray C 90, or Cray 2.
    It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.

    DLASD3 is called from DLASD1.

    Arguments
    =========

    NL     (input) INTEGER
           The row dimension of the upper block.  NL >= 1.

    NR     (input) INTEGER
           The row dimension of the lower block.  NR >= 1.

    SQRE   (input) INTEGER
           = 0: the lower block is an NR-by-NR square matrix.
           = 1: the lower block is an NR-by-(NR+1) rectangular matrix.

           The bidiagonal matrix has N = NL + NR + 1 rows and
           M = N + SQRE >= N columns.

    K      (input) INTEGER
           The size of the secular equation, 1 =< K = < N.

    D      (output) DOUBLE PRECISION array, dimension(K)
           On exit the square roots of the roots of the secular equation,
           in ascending order.

    Q      (workspace) DOUBLE PRECISION array,
                       dimension at least (LDQ,K).

    LDQ    (input) INTEGER
           The leading dimension of the array Q.  LDQ >= K.

    DSIGMA (input) DOUBLE PRECISION array, dimension(K)
           The first K elements of this array contain the old roots
           of the deflated updating problem.  These are the poles
           of the secular equation.

    U      (input) DOUBLE PRECISION array, dimension (LDU, N)
           The last N - K columns of this matrix contain the deflated
           left singular vectors.

    LDU    (input) INTEGER
           The leading dimension of the array U.  LDU >= N.

    U2     (input) DOUBLE PRECISION array, dimension (LDU2, N)
           The first K columns of this matrix contain the non-deflated
           left singular vectors for the split problem.

    LDU2   (input) INTEGER
           The leading dimension of the array U2.  LDU2 >= N.

    VT     (input) DOUBLE PRECISION array, dimension (LDVT, M)
           The last M - K columns of VT' contain the deflated
           right singular vectors.

    LDVT   (input) INTEGER
           The leading dimension of the array VT.  LDVT >= N.

    VT2    (input) DOUBLE PRECISION array, dimension (LDVT2, N)
           The first K columns of VT2' contain the non-deflated
           right singular vectors for the split problem.

    LDVT2  (input) INTEGER
           The leading dimension of the array VT2.  LDVT2 >= N.

    IDXC   (input) INTEGER array, dimension ( N )
           The permutation used to arrange the columns of U (and rows of
           VT) into three groups:  the first group contains non-zero
           entries only at and above (or before) NL +1; the second
           contains non-zero entries only at and below (or after) NL+2;
           and the third is dense. The first column of U and the row of
           VT are treated separately, however.

           The rows of the singular vectors found by DLASD4
           must be likewise permuted before the matrix multiplies can
           take place.

    CTOT   (input) INTEGER array, dimension ( 4 )
           A count of the total number of the various types of columns
           in U (or rows in VT), as described in IDXC. The fourth column
           type is any column which has been deflated.

    Z      (input) DOUBLE PRECISION array, dimension (K)
           The first K elements of this array contain the components
           of the deflation-adjusted updating row vector.

    INFO   (output) INTEGER
           = 0:  successful exit.
           < 0:  if INFO = -i, the i-th argument had an illegal value.
           > 0:  if INFO = 1, an singular value did not converge

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --dsigma;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    u2_dim1 = *ldu2;
    u2_offset = 1 + u2_dim1 * 1;
    u2 -= u2_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    vt2_dim1 = *ldvt2;
    vt2_offset = 1 + vt2_dim1 * 1;
    vt2 -= vt2_offset;
    --idxc;
    --ctot;
    --z__;

    /* Function Body */
    *info = 0;

    if (*nl < 1) {
	*info = -1;
    } else if (*nr < 1) {
	*info = -2;
    } else if ((*sqre != 1 && *sqre != 0)) {
	*info = -3;
    }

    n = *nl + *nr + 1;
    m = n + *sqre;
    nlp1 = *nl + 1;
    nlp2 = *nl + 2;

    if (*k < 1 || *k > n) {
	*info = -4;
    } else if (*ldq < *k) {
	*info = -7;
    } else if (*ldu < n) {
	*info = -10;
    } else if (*ldu2 < n) {
	*info = -12;
    } else if (*ldvt < m) {
	*info = -14;
    } else if (*ldvt2 < m) {
	*info = -16;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD3", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 1) {
	d__[1] = abs(z__[1]);
	dcopy_(&m, &vt2[vt2_dim1 + 1], ldvt2, &vt[vt_dim1 + 1], ldvt);
	if (z__[1] > 0.) {
	    dcopy_(&n, &u2[u2_dim1 + 1], &c__1, &u[u_dim1 + 1], &c__1);
	} else {
	    i__1 = n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		u[i__ + u_dim1] = -u2[i__ + u2_dim1];
/* L10: */
	    }
	}
	return 0;
    }

/*
       Modify values DSIGMA(i) to make sure all DSIGMA(i)-DSIGMA(j) can
       be computed with high relative accuracy (barring over/underflow).
       This is a problem on machines without a guard digit in
       add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2).
       The following code replaces DSIGMA(I) by 2*DSIGMA(I)-DSIGMA(I),
       which on any of these machines zeros out the bottommost
       bit of DSIGMA(I) if it is 1; this makes the subsequent
       subtractions DSIGMA(I)-DSIGMA(J) unproblematic when cancellation
       occurs. On binary machines with a guard digit (almost all
       machines) it does not change DSIGMA(I) at all. On hexadecimal
       and decimal machines with a guard digit, it slightly
       changes the bottommost bits of DSIGMA(I). It does not account
       for hexadecimal or decimal machines without guard digits
       (we know of none). We use a subroutine call to compute
       2*DLAMBDA(I) to prevent optimizing compilers from eliminating
       this code.
*/

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dsigma[i__] = dlamc3_(&dsigma[i__], &dsigma[i__]) - dsigma[i__];
/* L20: */
    }

/*     Keep a copy of Z. */

    dcopy_(k, &z__[1], &c__1, &q[q_offset], &c__1);

/*     Normalize Z. */

    rho = dnrm2_(k, &z__[1], &c__1);
    dlascl_("G", &c__0, &c__0, &rho, &c_b15, k, &c__1, &z__[1], k, info);
    rho *= rho;

/*     Find the new singular values. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	dlasd4_(k, &j, &dsigma[1], &z__[1], &u[j * u_dim1 + 1], &rho, &d__[j],
		 &vt[j * vt_dim1 + 1], info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    return 0;
	}
/* L30: */
    }

/*     Compute updated Z. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	z__[i__] = u[i__ + *k * u_dim1] * vt[i__ + *k * vt_dim1];
	i__2 = i__ - 1;
	for (j = 1; j <= i__2; ++j) {
	    z__[i__] *= u[i__ + j * u_dim1] * vt[i__ + j * vt_dim1] / (dsigma[
		    i__] - dsigma[j]) / (dsigma[i__] + dsigma[j]);
/* L40: */
	}
	i__2 = *k - 1;
	for (j = i__; j <= i__2; ++j) {
	    z__[i__] *= u[i__ + j * u_dim1] * vt[i__ + j * vt_dim1] / (dsigma[
		    i__] - dsigma[j + 1]) / (dsigma[i__] + dsigma[j + 1]);
/* L50: */
	}
	d__2 = sqrt((d__1 = z__[i__], abs(d__1)));
	z__[i__] = d_sign(&d__2, &q[i__ + q_dim1]);
/* L60: */
    }

/*
       Compute left singular vectors of the modified diagonal matrix,
       and store related information for the right singular vectors.
*/

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	vt[i__ * vt_dim1 + 1] = z__[1] / u[i__ * u_dim1 + 1] / vt[i__ *
		vt_dim1 + 1];
	u[i__ * u_dim1 + 1] = -1.;
	i__2 = *k;
	for (j = 2; j <= i__2; ++j) {
	    vt[j + i__ * vt_dim1] = z__[j] / u[j + i__ * u_dim1] / vt[j + i__
		    * vt_dim1];
	    u[j + i__ * u_dim1] = dsigma[j] * vt[j + i__ * vt_dim1];
/* L70: */
	}
	temp = dnrm2_(k, &u[i__ * u_dim1 + 1], &c__1);
	q[i__ * q_dim1 + 1] = u[i__ * u_dim1 + 1] / temp;
	i__2 = *k;
	for (j = 2; j <= i__2; ++j) {
	    jc = idxc[j];
	    q[j + i__ * q_dim1] = u[jc + i__ * u_dim1] / temp;
/* L80: */
	}
/* L90: */
    }

/*     Update the left singular vector matrix. */

    if (*k == 2) {
	dgemm_("N", "N", &n, k, k, &c_b15, &u2[u2_offset], ldu2, &q[q_offset],
		 ldq, &c_b29, &u[u_offset], ldu);
	goto L100;
    }
    if (ctot[1] > 0) {
	dgemm_("N", "N", nl, k, &ctot[1], &c_b15, &u2[((u2_dim1) << (1)) + 1],
		 ldu2, &q[q_dim1 + 2], ldq, &c_b29, &u[u_dim1 + 1], ldu);
	if (ctot[3] > 0) {
	    ktemp = ctot[1] + 2 + ctot[2];
	    dgemm_("N", "N", nl, k, &ctot[3], &c_b15, &u2[ktemp * u2_dim1 + 1]
		    , ldu2, &q[ktemp + q_dim1], ldq, &c_b15, &u[u_dim1 + 1],
		    ldu);
	}
    } else if (ctot[3] > 0) {
	ktemp = ctot[1] + 2 + ctot[2];
	dgemm_("N", "N", nl, k, &ctot[3], &c_b15, &u2[ktemp * u2_dim1 + 1],
		ldu2, &q[ktemp + q_dim1], ldq, &c_b29, &u[u_dim1 + 1], ldu);
    } else {
	dlacpy_("F", nl, k, &u2[u2_offset], ldu2, &u[u_offset], ldu);
    }
    dcopy_(k, &q[q_dim1 + 1], ldq, &u[nlp1 + u_dim1], ldu);
    ktemp = ctot[1] + 2;
    ctemp = ctot[2] + ctot[3];
    dgemm_("N", "N", nr, k, &ctemp, &c_b15, &u2[nlp2 + ktemp * u2_dim1], ldu2,
	     &q[ktemp + q_dim1], ldq, &c_b29, &u[nlp2 + u_dim1], ldu);

/*     Generate the right singular vectors. */

L100:
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	temp = dnrm2_(k, &vt[i__ * vt_dim1 + 1], &c__1);
	q[i__ + q_dim1] = vt[i__ * vt_dim1 + 1] / temp;
	i__2 = *k;
	for (j = 2; j <= i__2; ++j) {
	    jc = idxc[j];
	    q[i__ + j * q_dim1] = vt[jc + i__ * vt_dim1] / temp;
/* L110: */
	}
/* L120: */
    }

/*     Update the right singular vector matrix. */

    if (*k == 2) {
	dgemm_("N", "N", k, &m, k, &c_b15, &q[q_offset], ldq, &vt2[vt2_offset]
		, ldvt2, &c_b29, &vt[vt_offset], ldvt);
	return 0;
    }
    ktemp = ctot[1] + 1;
    dgemm_("N", "N", k, &nlp1, &ktemp, &c_b15, &q[q_dim1 + 1], ldq, &vt2[
	    vt2_dim1 + 1], ldvt2, &c_b29, &vt[vt_dim1 + 1], ldvt);
    ktemp = ctot[1] + 2 + ctot[2];
    if (ktemp <= *ldvt2) {
	dgemm_("N", "N", k, &nlp1, &ctot[3], &c_b15, &q[ktemp * q_dim1 + 1],
		ldq, &vt2[ktemp + vt2_dim1], ldvt2, &c_b15, &vt[vt_dim1 + 1],
		ldvt);
    }

    ktemp = ctot[1] + 1;
    nrp1 = *nr + *sqre;
    if (ktemp > 1) {
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    q[i__ + ktemp * q_dim1] = q[i__ + q_dim1];
/* L130: */
	}
	i__1 = m;
	for (i__ = nlp2; i__ <= i__1; ++i__) {
	    vt2[ktemp + i__ * vt2_dim1] = vt2[i__ * vt2_dim1 + 1];
/* L140: */
	}
    }
    ctemp = ctot[2] + 1 + ctot[3];
    dgemm_("N", "N", k, &nrp1, &ctemp, &c_b15, &q[ktemp * q_dim1 + 1], ldq, &
	    vt2[ktemp + nlp2 * vt2_dim1], ldvt2, &c_b29, &vt[nlp2 * vt_dim1 +
	    1], ldvt);

    return 0;

/*     End of DLASD3 */

} /* dlasd3_ */

/* Subroutine */ int dlasd4_(integer *n, integer *i__, doublereal *d__,
	doublereal *z__, doublereal *delta, doublereal *rho, doublereal *
	sigma, doublereal *work, integer *info)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal a, b, c__;
    static integer j;
    static doublereal w, dd[3];
    static integer ii;
    static doublereal dw, zz[3];
    static integer ip1;
    static doublereal eta, phi, eps, tau, psi;
    static integer iim1, iip1;
    static doublereal dphi, dpsi;
    static integer iter;
    static doublereal temp, prew, sg2lb, sg2ub, temp1, temp2, dtiim, delsq,
	    dtiip;
    static integer niter;
    static doublereal dtisq;
    static logical swtch;
    static doublereal dtnsq;
    extern /* Subroutine */ int dlaed6_(integer *, logical *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *)
	    , dlasd5_(integer *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *);
    static doublereal delsq2, dtnsq1;
    static logical swtch3;

    static logical orgati;
    static doublereal erretm, dtipsq, rhoinv;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       October 31, 1999


    Purpose
    =======

    This subroutine computes the square root of the I-th updated
    eigenvalue of a positive symmetric rank-one modification to
    a positive diagonal matrix whose entries are given as the squares
    of the corresponding entries in the array d, and that

           0 <= D(i) < D(j)  for  i < j

    and that RHO > 0. This is arranged by the calling routine, and is
    no loss in generality.  The rank-one modified system is thus

           diag( D ) * diag( D ) +  RHO *  Z * Z_transpose.

    where we assume the Euclidean norm of Z is 1.

    The method consists of approximating the rational functions in the
    secular equation by simpler interpolating rational functions.

    Arguments
    =========

    N      (input) INTEGER
           The length of all arrays.

    I      (input) INTEGER
           The index of the eigenvalue to be computed.  1 <= I <= N.

    D      (input) DOUBLE PRECISION array, dimension ( N )
           The original eigenvalues.  It is assumed that they are in
           order, 0 <= D(I) < D(J)  for I < J.

    Z      (input) DOUBLE PRECISION array, dimension ( N )
           The components of the updating vector.

    DELTA  (output) DOUBLE PRECISION array, dimension ( N )
           If N .ne. 1, DELTA contains (D(j) - sigma_I) in its  j-th
           component.  If N = 1, then DELTA(1) = 1.  The vector DELTA
           contains the information necessary to construct the
           (singular) eigenvectors.

    RHO    (input) DOUBLE PRECISION
           The scalar in the symmetric updating formula.

    SIGMA  (output) DOUBLE PRECISION
           The computed lambda_I, the I-th updated eigenvalue.

    WORK   (workspace) DOUBLE PRECISION array, dimension ( N )
           If N .ne. 1, WORK contains (D(j) + sigma_I) in its  j-th
           component.  If N = 1, then WORK( 1 ) = 1.

    INFO   (output) INTEGER
           = 0:  successful exit
           > 0:  if INFO = 1, the updating process failed.

    Internal Parameters
    ===================

    Logical variable ORGATI (origin-at-i?) is used for distinguishing
    whether D(i) or D(i+1) is treated as the origin.

              ORGATI = .true.    origin at i
              ORGATI = .false.   origin at i+1

    Logical variable SWTCH3 (switch-for-3-poles?) is for noting
    if we are working with THREE poles!

    MAXIT is the maximum number of iterations allowed for each
    eigenvalue.

    Further Details
    ===============

    Based on contributions by
       Ren-Cang Li, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================


       Since this routine is called in an inner loop, we do no argument
       checking.

       Quick return for N=1 and 2.
*/

    /* Parameter adjustments */
    --work;
    --delta;
    --z__;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n == 1) {

/*        Presumably, I=1 upon entry */

	*sigma = sqrt(d__[1] * d__[1] + *rho * z__[1] * z__[1]);
	delta[1] = 1.;
	work[1] = 1.;
	return 0;
    }
    if (*n == 2) {
	dlasd5_(i__, &d__[1], &z__[1], &delta[1], rho, sigma, &work[1]);
	return 0;
    }

/*     Compute machine epsilon */

    eps = EPSILON;
    rhoinv = 1. / *rho;

/*     The case I = N */

    if (*i__ == *n) {

/*        Initialize some basic variables */

	ii = *n - 1;
	niter = 1;

/*        Calculate initial guess */

	temp = *rho / 2.;

/*
          If ||Z||_2 is not one, then TEMP should be set to
          RHO * ||Z||_2^2 / TWO
*/

	temp1 = temp / (d__[*n] + sqrt(d__[*n] * d__[*n] + temp));
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[j] = d__[j] + d__[*n] + temp1;
	    delta[j] = d__[j] - d__[*n] - temp1;
/* L10: */
	}

	psi = 0.;
	i__1 = *n - 2;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / (delta[j] * work[j]);
/* L20: */
	}

	c__ = rhoinv + psi;
	w = c__ + z__[ii] * z__[ii] / (delta[ii] * work[ii]) + z__[*n] * z__[*
		n] / (delta[*n] * work[*n]);

	if (w <= 0.) {
	    temp1 = sqrt(d__[*n] * d__[*n] + *rho);
	    temp = z__[*n - 1] * z__[*n - 1] / ((d__[*n - 1] + temp1) * (d__[*
		    n] - d__[*n - 1] + *rho / (d__[*n] + temp1))) + z__[*n] *
		    z__[*n] / *rho;

/*
             The following TAU is to approximate
             SIGMA_n^2 - D( N )*D( N )
*/

	    if (c__ <= temp) {
		tau = *rho;
	    } else {
		delsq = (d__[*n] - d__[*n - 1]) * (d__[*n] + d__[*n - 1]);
		a = -c__ * delsq + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*
			n];
		b = z__[*n] * z__[*n] * delsq;
		if (a < 0.) {
		    tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
		} else {
		    tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
		}
	    }

/*
             It can be proved that
                 D(N)^2+RHO/2 <= SIGMA_n^2 < D(N)^2+TAU <= D(N)^2+RHO
*/

	} else {
	    delsq = (d__[*n] - d__[*n - 1]) * (d__[*n] + d__[*n - 1]);
	    a = -c__ * delsq + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n];
	    b = z__[*n] * z__[*n] * delsq;

/*
             The following TAU is to approximate
             SIGMA_n^2 - D( N )*D( N )
*/

	    if (a < 0.) {
		tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
	    } else {
		tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
	    }

/*
             It can be proved that
             D(N)^2 < D(N)^2+TAU < SIGMA(N)^2 < D(N)^2+RHO/2
*/

	}

/*        The following ETA is to approximate SIGMA_n - D( N ) */

	eta = tau / (d__[*n] + sqrt(d__[*n] * d__[*n] + tau));

	*sigma = d__[*n] + eta;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - eta;
	    work[j] = d__[j] + d__[*i__] + eta;
/* L30: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (delta[j] * work[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L40: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / (delta[*n] * work[*n]);
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    goto L240;
	}

/*        Calculate the new step */

	++niter;
	dtnsq1 = work[*n - 1] * delta[*n - 1];
	dtnsq = work[*n] * delta[*n];
	c__ = w - dtnsq1 * dpsi - dtnsq * dphi;
	a = (dtnsq + dtnsq1) * w - dtnsq * dtnsq1 * (dpsi + dphi);
	b = dtnsq * dtnsq1 * w;
	if (c__ < 0.) {
	    c__ = abs(c__);
	}
	if (c__ == 0.) {
	    eta = *rho - *sigma * *sigma;
	} else if (a >= 0.) {
	    eta = (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (c__
		    * 2.);
	} else {
	    eta = b * 2. / (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))
		    );
	}

/*
          Note, eta should be positive if w is negative, and
          eta should be negative otherwise. However,
          if for some reason caused by roundoff, eta*w > 0,
          we simply use one Newton step instead. This way
          will guarantee eta*w < 0.
*/

	if (w * eta > 0.) {
	    eta = -w / (dpsi + dphi);
	}
	temp = eta - dtnsq;
	if (temp > *rho) {
	    eta = *rho + dtnsq;
	}

	tau += eta;
	eta /= *sigma + sqrt(eta + *sigma * *sigma);
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
	    work[j] += eta;
/* L50: */
	}

	*sigma += eta;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L60: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / (work[*n] * delta[*n]);
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 20; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		goto L240;
	    }

/*           Calculate the new step */

	    dtnsq1 = work[*n - 1] * delta[*n - 1];
	    dtnsq = work[*n] * delta[*n];
	    c__ = w - dtnsq1 * dpsi - dtnsq * dphi;
	    a = (dtnsq + dtnsq1) * w - dtnsq1 * dtnsq * (dpsi + dphi);
	    b = dtnsq1 * dtnsq * w;
	    if (a >= 0.) {
		eta = (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    } else {
		eta = b * 2. / (a - sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    }

/*
             Note, eta should be positive if w is negative, and
             eta should be negative otherwise. However,
             if for some reason caused by roundoff, eta*w > 0,
             we simply use one Newton step instead. This way
             will guarantee eta*w < 0.
*/

	    if (w * eta > 0.) {
		eta = -w / (dpsi + dphi);
	    }
	    temp = eta - dtnsq;
	    if (temp <= 0.) {
		eta /= 2.;
	    }

	    tau += eta;
	    eta /= *sigma + sqrt(eta + *sigma * *sigma);
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
		work[j] += eta;
/* L70: */
	    }

	    *sigma += eta;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = ii;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / (work[j] * delta[j]);
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L80: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    temp = z__[*n] / (work[*n] * delta[*n]);
	    phi = z__[*n] * temp;
	    dphi = temp * temp;
	    erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (
		    dpsi + dphi);

	    w = rhoinv + phi + psi;
/* L90: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	goto L240;

/*        End for the case I = N */

    } else {

/*        The case for I < N */

	niter = 1;
	ip1 = *i__ + 1;

/*        Calculate initial guess */

	delsq = (d__[ip1] - d__[*i__]) * (d__[ip1] + d__[*i__]);
	delsq2 = delsq / 2.;
	temp = delsq2 / (d__[*i__] + sqrt(d__[*i__] * d__[*i__] + delsq2));
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[j] = d__[j] + d__[*i__] + temp;
	    delta[j] = d__[j] - d__[*i__] - temp;
/* L100: */
	}

	psi = 0.;
	i__1 = *i__ - 1;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / (work[j] * delta[j]);
/* L110: */
	}

	phi = 0.;
	i__1 = *i__ + 2;
	for (j = *n; j >= i__1; --j) {
	    phi += z__[j] * z__[j] / (work[j] * delta[j]);
/* L120: */
	}
	c__ = rhoinv + psi + phi;
	w = c__ + z__[*i__] * z__[*i__] / (work[*i__] * delta[*i__]) + z__[
		ip1] * z__[ip1] / (work[ip1] * delta[ip1]);

	if (w > 0.) {

/*
             d(i)^2 < the ith sigma^2 < (d(i)^2+d(i+1)^2)/2

             We choose d(i) as origin.
*/

	    orgati = TRUE_;
	    sg2lb = 0.;
	    sg2ub = delsq2;
	    a = c__ * delsq + z__[*i__] * z__[*i__] + z__[ip1] * z__[ip1];
	    b = z__[*i__] * z__[*i__] * delsq;
	    if (a > 0.) {
		tau = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    }

/*
             TAU now is an estimation of SIGMA^2 - D( I )^2. The
             following, however, is the corresponding estimation of
             SIGMA - D( I ).
*/

	    eta = tau / (d__[*i__] + sqrt(d__[*i__] * d__[*i__] + tau));
	} else {

/*
             (d(i)^2+d(i+1)^2)/2 <= the ith sigma^2 < d(i+1)^2/2

             We choose d(i+1) as origin.
*/

	    orgati = FALSE_;
	    sg2lb = -delsq2;
	    sg2ub = 0.;
	    a = c__ * delsq - z__[*i__] * z__[*i__] - z__[ip1] * z__[ip1];
	    b = z__[ip1] * z__[ip1] * delsq;
	    if (a < 0.) {
		tau = b * 2. / (a - sqrt((d__1 = a * a + b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = -(a + sqrt((d__1 = a * a + b * 4. * c__, abs(d__1)))) /
			(c__ * 2.);
	    }

/*
             TAU now is an estimation of SIGMA^2 - D( IP1 )^2. The
             following, however, is the corresponding estimation of
             SIGMA - D( IP1 ).
*/

	    eta = tau / (d__[ip1] + sqrt((d__1 = d__[ip1] * d__[ip1] + tau,
		    abs(d__1))));
	}

	if (orgati) {
	    ii = *i__;
	    *sigma = d__[*i__] + eta;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		work[j] = d__[j] + d__[*i__] + eta;
		delta[j] = d__[j] - d__[*i__] - eta;
/* L130: */
	    }
	} else {
	    ii = *i__ + 1;
	    *sigma = d__[ip1] + eta;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		work[j] = d__[j] + d__[ip1] + eta;
		delta[j] = d__[j] - d__[ip1] - eta;
/* L140: */
	    }
	}
	iim1 = ii - 1;
	iip1 = ii + 1;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L150: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.;
	phi = 0.;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L160: */
	}

	w = rhoinv + phi + psi;

/*
          W is the value of the secular function with
          its ii-th element removed.
*/

	swtch3 = FALSE_;
	if (orgati) {
	    if (w < 0.) {
		swtch3 = TRUE_;
	    }
	} else {
	    if (w > 0.) {
		swtch3 = TRUE_;
	    }
	}
	if (ii == 1 || ii == *n) {
	    swtch3 = FALSE_;
	}

	temp = z__[ii] / (work[ii] * delta[ii]);
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w += temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. +
		abs(tau) * dw;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    goto L240;
	}

	if (w <= 0.) {
	    sg2lb = max(sg2lb,tau);
	} else {
	    sg2ub = min(sg2ub,tau);
	}

/*        Calculate the new step */

	++niter;
	if (! swtch3) {
	    dtipsq = work[ip1] * delta[ip1];
	    dtisq = work[*i__] * delta[*i__];
	    if (orgati) {
/* Computing 2nd power */
		d__1 = z__[*i__] / dtisq;
		c__ = w - dtipsq * dw + delsq * (d__1 * d__1);
	    } else {
/* Computing 2nd power */
		d__1 = z__[ip1] / dtipsq;
		c__ = w - dtisq * dw - delsq * (d__1 * d__1);
	    }
	    a = (dtipsq + dtisq) * w - dtipsq * dtisq * dw;
	    b = dtipsq * dtisq * w;
	    if (c__ == 0.) {
		if (a == 0.) {
		    if (orgati) {
			a = z__[*i__] * z__[*i__] + dtipsq * dtipsq * (dpsi +
				dphi);
		    } else {
			a = z__[ip1] * z__[ip1] + dtisq * dtisq * (dpsi +
				dphi);
		    }
		}
		eta = b / a;
	    } else if (a <= 0.) {
		eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    } else {
		eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    }
	} else {

/*           Interpolation using THREE most relevant poles */

	    dtiim = work[iim1] * delta[iim1];
	    dtiip = work[iip1] * delta[iip1];
	    temp = rhoinv + psi + phi;
	    if (orgati) {
		temp1 = z__[iim1] / dtiim;
		temp1 *= temp1;
		c__ = temp - dtiip * (dpsi + dphi) - (d__[iim1] - d__[iip1]) *
			 (d__[iim1] + d__[iip1]) * temp1;
		zz[0] = z__[iim1] * z__[iim1];
		if (dpsi < temp1) {
		    zz[2] = dtiip * dtiip * dphi;
		} else {
		    zz[2] = dtiip * dtiip * (dpsi - temp1 + dphi);
		}
	    } else {
		temp1 = z__[iip1] / dtiip;
		temp1 *= temp1;
		c__ = temp - dtiim * (dpsi + dphi) - (d__[iip1] - d__[iim1]) *
			 (d__[iim1] + d__[iip1]) * temp1;
		if (dphi < temp1) {
		    zz[0] = dtiim * dtiim * dpsi;
		} else {
		    zz[0] = dtiim * dtiim * (dpsi + (dphi - temp1));
		}
		zz[2] = z__[iip1] * z__[iip1];
	    }
	    zz[1] = z__[ii] * z__[ii];
	    dd[0] = dtiim;
	    dd[1] = delta[ii] * work[ii];
	    dd[2] = dtiip;
	    dlaed6_(&niter, &orgati, &c__, dd, zz, &w, &eta, info);
	    if (*info != 0) {
		goto L240;
	    }
	}

/*
          Note, eta should be positive if w is negative, and
          eta should be negative otherwise. However,
          if for some reason caused by roundoff, eta*w > 0,
          we simply use one Newton step instead. This way
          will guarantee eta*w < 0.
*/

	if (w * eta >= 0.) {
	    eta = -w / dw;
	}
	if (orgati) {
	    temp1 = work[*i__] * delta[*i__];
	    temp = eta - temp1;
	} else {
	    temp1 = work[ip1] * delta[ip1];
	    temp = eta - temp1;
	}
	if (temp > sg2ub || temp < sg2lb) {
	    if (w < 0.) {
		eta = (sg2ub - tau) / 2.;
	    } else {
		eta = (sg2lb - tau) / 2.;
	    }
	}

	tau += eta;
	eta /= *sigma + sqrt(*sigma * *sigma + eta);

	prew = w;

	*sigma += eta;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[j] += eta;
	    delta[j] -= eta;
/* L170: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L180: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.;
	phi = 0.;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L190: */
	}

	temp = z__[ii] / (work[ii] * delta[ii]);
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w = rhoinv + phi + psi + temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. +
		abs(tau) * dw;

	if (w <= 0.) {
	    sg2lb = max(sg2lb,tau);
	} else {
	    sg2ub = min(sg2ub,tau);
	}

	swtch = FALSE_;
	if (orgati) {
	    if (-w > abs(prew) / 10.) {
		swtch = TRUE_;
	    }
	} else {
	    if (w > abs(prew) / 10.) {
		swtch = TRUE_;
	    }
	}

/*        Main loop to update the values of the array   DELTA and WORK */

	iter = niter + 1;

	for (niter = iter; niter <= 20; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		goto L240;
	    }

/*           Calculate the new step */

	    if (! swtch3) {
		dtipsq = work[ip1] * delta[ip1];
		dtisq = work[*i__] * delta[*i__];
		if (! swtch) {
		    if (orgati) {
/* Computing 2nd power */
			d__1 = z__[*i__] / dtisq;
			c__ = w - dtipsq * dw + delsq * (d__1 * d__1);
		    } else {
/* Computing 2nd power */
			d__1 = z__[ip1] / dtipsq;
			c__ = w - dtisq * dw - delsq * (d__1 * d__1);
		    }
		} else {
		    temp = z__[ii] / (work[ii] * delta[ii]);
		    if (orgati) {
			dpsi += temp * temp;
		    } else {
			dphi += temp * temp;
		    }
		    c__ = w - dtisq * dpsi - dtipsq * dphi;
		}
		a = (dtipsq + dtisq) * w - dtipsq * dtisq * dw;
		b = dtipsq * dtisq * w;
		if (c__ == 0.) {
		    if (a == 0.) {
			if (! swtch) {
			    if (orgati) {
				a = z__[*i__] * z__[*i__] + dtipsq * dtipsq *
					(dpsi + dphi);
			    } else {
				a = z__[ip1] * z__[ip1] + dtisq * dtisq * (
					dpsi + dphi);
			    }
			} else {
			    a = dtisq * dtisq * dpsi + dtipsq * dtipsq * dphi;
			}
		    }
		    eta = b / a;
		} else if (a <= 0.) {
		    eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1))))
			     / (c__ * 2.);
		} else {
		    eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__,
			    abs(d__1))));
		}
	    } else {

/*              Interpolation using THREE most relevant poles */

		dtiim = work[iim1] * delta[iim1];
		dtiip = work[iip1] * delta[iip1];
		temp = rhoinv + psi + phi;
		if (swtch) {
		    c__ = temp - dtiim * dpsi - dtiip * dphi;
		    zz[0] = dtiim * dtiim * dpsi;
		    zz[2] = dtiip * dtiip * dphi;
		} else {
		    if (orgati) {
			temp1 = z__[iim1] / dtiim;
			temp1 *= temp1;
			temp2 = (d__[iim1] - d__[iip1]) * (d__[iim1] + d__[
				iip1]) * temp1;
			c__ = temp - dtiip * (dpsi + dphi) - temp2;
			zz[0] = z__[iim1] * z__[iim1];
			if (dpsi < temp1) {
			    zz[2] = dtiip * dtiip * dphi;
			} else {
			    zz[2] = dtiip * dtiip * (dpsi - temp1 + dphi);
			}
		    } else {
			temp1 = z__[iip1] / dtiip;
			temp1 *= temp1;
			temp2 = (d__[iip1] - d__[iim1]) * (d__[iim1] + d__[
				iip1]) * temp1;
			c__ = temp - dtiim * (dpsi + dphi) - temp2;
			if (dphi < temp1) {
			    zz[0] = dtiim * dtiim * dpsi;
			} else {
			    zz[0] = dtiim * dtiim * (dpsi + (dphi - temp1));
			}
			zz[2] = z__[iip1] * z__[iip1];
		    }
		}
		dd[0] = dtiim;
		dd[1] = delta[ii] * work[ii];
		dd[2] = dtiip;
		dlaed6_(&niter, &orgati, &c__, dd, zz, &w, &eta, info);
		if (*info != 0) {
		    goto L240;
		}
	    }

/*
             Note, eta should be positive if w is negative, and
             eta should be negative otherwise. However,
             if for some reason caused by roundoff, eta*w > 0,
             we simply use one Newton step instead. This way
             will guarantee eta*w < 0.
*/

	    if (w * eta >= 0.) {
		eta = -w / dw;
	    }
	    if (orgati) {
		temp1 = work[*i__] * delta[*i__];
		temp = eta - temp1;
	    } else {
		temp1 = work[ip1] * delta[ip1];
		temp = eta - temp1;
	    }
	    if (temp > sg2ub || temp < sg2lb) {
		if (w < 0.) {
		    eta = (sg2ub - tau) / 2.;
		} else {
		    eta = (sg2lb - tau) / 2.;
		}
	    }

	    tau += eta;
	    eta /= *sigma + sqrt(*sigma * *sigma + eta);

	    *sigma += eta;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		work[j] += eta;
		delta[j] -= eta;
/* L200: */
	    }

	    prew = w;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = iim1;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / (work[j] * delta[j]);
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L210: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    dphi = 0.;
	    phi = 0.;
	    i__1 = iip1;
	    for (j = *n; j >= i__1; --j) {
		temp = z__[j] / (work[j] * delta[j]);
		phi += z__[j] * temp;
		dphi += temp * temp;
		erretm += phi;
/* L220: */
	    }

	    temp = z__[ii] / (work[ii] * delta[ii]);
	    dw = dpsi + dphi + temp * temp;
	    temp = z__[ii] * temp;
	    w = rhoinv + phi + psi + temp;
	    erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3.
		    + abs(tau) * dw;
	    if ((w * prew > 0. && abs(w) > abs(prew) / 10.)) {
		swtch = ! swtch;
	    }

	    if (w <= 0.) {
		sg2lb = max(sg2lb,tau);
	    } else {
		sg2ub = min(sg2ub,tau);
	    }

/* L230: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;

    }

L240:
    return 0;

/*     End of DLASD4 */

} /* dlasd4_ */

/* Subroutine */ int dlasd5_(integer *i__, doublereal *d__, doublereal *z__,
	doublereal *delta, doublereal *rho, doublereal *dsigma, doublereal *
	work)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal b, c__, w, del, tau, delsq;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       June 30, 1999


    Purpose
    =======

    This subroutine computes the square root of the I-th eigenvalue
    of a positive symmetric rank-one modification of a 2-by-2 diagonal
    matrix

               diag( D ) * diag( D ) +  RHO *  Z * transpose(Z) .

    The diagonal entries in the array D are assumed to satisfy

               0 <= D(i) < D(j)  for  i < j .

    We also assume RHO > 0 and that the Euclidean norm of the vector
    Z is one.

    Arguments
    =========

    I      (input) INTEGER
           The index of the eigenvalue to be computed.  I = 1 or I = 2.

    D      (input) DOUBLE PRECISION array, dimension ( 2 )
           The original eigenvalues.  We assume 0 <= D(1) < D(2).

    Z      (input) DOUBLE PRECISION array, dimension ( 2 )
           The components of the updating vector.

    DELTA  (output) DOUBLE PRECISION array, dimension ( 2 )
           Contains (D(j) - lambda_I) in its  j-th component.
           The vector DELTA contains the information necessary
           to construct the eigenvectors.

    RHO    (input) DOUBLE PRECISION
           The scalar in the symmetric updating formula.

    DSIGMA (output) DOUBLE PRECISION
           The computed lambda_I, the I-th updated eigenvalue.

    WORK   (workspace) DOUBLE PRECISION array, dimension ( 2 )
           WORK contains (D(j) + sigma_I) in its  j-th component.

    Further Details
    ===============

    Based on contributions by
       Ren-Cang Li, Computer Science Division, University of California
       at Berkeley, USA

    =====================================================================
*/


    /* Parameter adjustments */
    --work;
    --delta;
    --z__;
    --d__;

    /* Function Body */
    del = d__[2] - d__[1];
    delsq = del * (d__[2] + d__[1]);
    if (*i__ == 1) {
	w = *rho * 4. * (z__[2] * z__[2] / (d__[1] + d__[2] * 3.) - z__[1] *
		z__[1] / (d__[1] * 3. + d__[2])) / del + 1.;
	if (w > 0.) {
	    b = delsq + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[1] * z__[1] * delsq;

/*
             B > ZERO, always

             The following TAU is DSIGMA * DSIGMA - D( 1 ) * D( 1 )
*/

	    tau = c__ * 2. / (b + sqrt((d__1 = b * b - c__ * 4., abs(d__1))));

/*           The following TAU is DSIGMA - D( 1 ) */

	    tau /= d__[1] + sqrt(d__[1] * d__[1] + tau);
	    *dsigma = d__[1] + tau;
	    delta[1] = -tau;
	    delta[2] = del - tau;
	    work[1] = d__[1] * 2. + tau;
	    work[2] = d__[1] + tau + d__[2];
/*
             DELTA( 1 ) = -Z( 1 ) / TAU
             DELTA( 2 ) = Z( 2 ) / ( DEL-TAU )
*/
	} else {
	    b = -delsq + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[2] * z__[2] * delsq;

/*           The following TAU is DSIGMA * DSIGMA - D( 2 ) * D( 2 ) */

	    if (b > 0.) {
		tau = c__ * -2. / (b + sqrt(b * b + c__ * 4.));
	    } else {
		tau = (b - sqrt(b * b + c__ * 4.)) / 2.;
	    }

/*           The following TAU is DSIGMA - D( 2 ) */

	    tau /= d__[2] + sqrt((d__1 = d__[2] * d__[2] + tau, abs(d__1)));
	    *dsigma = d__[2] + tau;
	    delta[1] = -(del + tau);
	    delta[2] = -tau;
	    work[1] = d__[1] + tau + d__[2];
	    work[2] = d__[2] * 2. + tau;
/*
             DELTA( 1 ) = -Z( 1 ) / ( DEL+TAU )
             DELTA( 2 ) = -Z( 2 ) / TAU
*/
	}
/*
          TEMP = SQRT( DELTA( 1 )*DELTA( 1 )+DELTA( 2 )*DELTA( 2 ) )
          DELTA( 1 ) = DELTA( 1 ) / TEMP
          DELTA( 2 ) = DELTA( 2 ) / TEMP
*/
    } else {

/*        Now I=2 */

	b = -delsq + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	c__ = *rho * z__[2] * z__[2] * delsq;

/*        The following TAU is DSIGMA * DSIGMA - D( 2 ) * D( 2 ) */

	if (b > 0.) {
	    tau = (b + sqrt(b * b + c__ * 4.)) / 2.;
	} else {
	    tau = c__ * 2. / (-b + sqrt(b * b + c__ * 4.));
	}

/*        The following TAU is DSIGMA - D( 2 ) */

	tau /= d__[2] + sqrt(d__[2] * d__[2] + tau);
	*dsigma = d__[2] + tau;
	delta[1] = -(del + tau);
	delta[2] = -tau;
	work[1] = d__[1] + tau + d__[2];
	work[2] = d__[2] * 2. + tau;
/*
          DELTA( 1 ) = -Z( 1 ) / ( DEL+TAU )
          DELTA( 2 ) = -Z( 2 ) / TAU
          TEMP = SQRT( DELTA( 1 )*DELTA( 1 )+DELTA( 2 )*DELTA( 2 ) )
          DELTA( 1 ) = DELTA( 1 ) / TEMP
          DELTA( 2 ) = DELTA( 2 ) / TEMP
*/
    }
    return 0;

/*     End of DLASD5 */

} /* dlasd5_ */

/* Subroutine */ int dlasd6_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, doublereal *d__, doublereal *vf, doublereal *vl,
	doublereal *alpha, doublereal *beta, integer *idxq, integer *perm,
	integer *givptr, integer *givcol, integer *ldgcol, doublereal *givnum,
	 integer *ldgnum, doublereal *poles, doublereal *difl, doublereal *
	difr, doublereal *z__, integer *k, doublereal *c__, doublereal *s,
	doublereal *work, integer *iwork, integer *info)
{
    /* System generated locals */
    integer givcol_dim1, givcol_offset, givnum_dim1, givnum_offset,
	    poles_dim1, poles_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__, m, n, n1, n2, iw, idx, idxc, idxp, ivfw, ivlw;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *), dlasd7_(integer *, integer *, integer *,
	     integer *, integer *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, integer *, integer *,
	    integer *, integer *, integer *, integer *, integer *, doublereal
	    *, integer *, doublereal *, doublereal *, integer *), dlasd8_(
	    integer *, integer *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     doublereal *, integer *), dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *), dlamrg_(integer *, integer *,
	    doublereal *, integer *, integer *, integer *);
    static integer isigma;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static doublereal orgnrm;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLASD6 computes the SVD of an updated upper bidiagonal matrix B
    obtained by merging two smaller ones by appending a row. This
    routine is used only for the problem which requires all singular
    values and optionally singular vector matrices in factored form.
    B is an N-by-M matrix with N = NL + NR + 1 and M = N + SQRE.
    A related subroutine, DLASD1, handles the case in which all singular
    values and singular vectors of the bidiagonal matrix are desired.

    DLASD6 computes the SVD as follows:

                  ( D1(in)  0    0     0 )
      B = U(in) * (   Z1'   a   Z2'    b ) * VT(in)
                  (   0     0   D2(in) 0 )

        = U(out) * ( D(out) 0) * VT(out)

    where Z' = (Z1' a Z2' b) = u' VT', and u is a vector of dimension M
    with ALPHA and BETA in the NL+1 and NL+2 th entries and zeros
    elsewhere; and the entry b is empty if SQRE = 0.

    The singular values of B can be computed using D1, D2, the first
    components of all the right singular vectors of the lower block, and
    the last components of all the right singular vectors of the upper
    block. These components are stored and updated in VF and VL,
    respectively, in DLASD6. Hence U and VT are not explicitly
    referenced.

    The singular values are stored in D. The algorithm consists of two
    stages:

          The first stage consists of deflating the size of the problem
          when there are multiple singular values or if there is a zero
          in the Z vector. For each such occurence the dimension of the
          secular equation problem is reduced by one. This stage is
          performed by the routine DLASD7.

          The second stage consists of calculating the updated
          singular values. This is done by finding the roots of the
          secular equation via the routine DLASD4 (as called by DLASD8).
          This routine also updates VF and VL and computes the distances
          between the updated singular values and the old singular
          values.

    DLASD6 is called from DLASDA.

    Arguments
    =========

    ICOMPQ (input) INTEGER
           Specifies whether singular vectors are to be computed in
           factored form:
           = 0: Compute singular values only.
           = 1: Compute singular vectors in factored form as well.

    NL     (input) INTEGER
           The row dimension of the upper block.  NL >= 1.

    NR     (input) INTEGER
           The row dimension of the lower block.  NR >= 1.

    SQRE   (input) INTEGER
           = 0: the lower block is an NR-by-NR square matrix.
           = 1: the lower block is an NR-by-(NR+1) rectangular matrix.

           The bidiagonal matrix has row dimension N = NL + NR + 1,
           and column dimension M = N + SQRE.

    D      (input/output) DOUBLE PRECISION array, dimension ( NL+NR+1 ).
           On entry D(1:NL,1:NL) contains the singular values of the
           upper block, and D(NL+2:N) contains the singular values
           of the lower block. On exit D(1:N) contains the singular
           values of the modified matrix.

    VF     (input/output) DOUBLE PRECISION array, dimension ( M )
           On entry, VF(1:NL+1) contains the first components of all
           right singular vectors of the upper block; and VF(NL+2:M)
           contains the first components of all right singular vectors
           of the lower block. On exit, VF contains the first components
           of all right singular vectors of the bidiagonal matrix.

    VL     (input/output) DOUBLE PRECISION array, dimension ( M )
           On entry, VL(1:NL+1) contains the  last components of all
           right singular vectors of the upper block; and VL(NL+2:M)
           contains the last components of all right singular vectors of
           the lower block. On exit, VL contains the last components of
           all right singular vectors of the bidiagonal matrix.

    ALPHA  (input) DOUBLE PRECISION
           Contains the diagonal element associated with the added row.

    BETA   (input) DOUBLE PRECISION
           Contains the off-diagonal element associated with the added
           row.

    IDXQ   (output) INTEGER array, dimension ( N )
           This contains the permutation which will reintegrate the
           subproblem just solved back into sorted order, i.e.
           D( IDXQ( I = 1, N ) ) will be in ascending order.

    PERM   (output) INTEGER array, dimension ( N )
           The permutations (from deflation and sorting) to be applied
           to each block. Not referenced if ICOMPQ = 0.

    GIVPTR (output) INTEGER
           The number of Givens rotations which took place in this
           subproblem. Not referenced if ICOMPQ = 0.

    GIVCOL (output) INTEGER array, dimension ( LDGCOL, 2 )
           Each pair of numbers indicates a pair of columns to take place
           in a Givens rotation. Not referenced if ICOMPQ = 0.

    LDGCOL (input) INTEGER
           leading dimension of GIVCOL, must be at least N.

    GIVNUM (output) DOUBLE PRECISION array, dimension ( LDGNUM, 2 )
           Each number indicates the C or S value to be used in the
           corresponding Givens rotation. Not referenced if ICOMPQ = 0.

    LDGNUM (input) INTEGER
           The leading dimension of GIVNUM and POLES, must be at least N.

    POLES  (output) DOUBLE PRECISION array, dimension ( LDGNUM, 2 )
           On exit, POLES(1,*) is an array containing the new singular
           values obtained from solving the secular equation, and
           POLES(2,*) is an array containing the poles in the secular
           equation. Not referenced if ICOMPQ = 0.

    DIFL   (output) DOUBLE PRECISION array, dimension ( N )
           On exit, DIFL(I) is the distance between I-th updated
           (undeflated) singular value and the I-th (undeflated) old
           singular value.

    DIFR   (output) DOUBLE PRECISION array,
                    dimension ( LDGNUM, 2 ) if ICOMPQ = 1 and
                    dimension ( N ) if ICOMPQ = 0.
           On exit, DIFR(I, 1) is the distance between I-th updated
           (undeflated) singular value and the I+1-th (undeflated) old
           singular value.

           If ICOMPQ = 1, DIFR(1:K,2) is an array containing the
           normalizing factors for the right singular vector matrix.

           See DLASD8 for details on DIFL and DIFR.

    Z      (output) DOUBLE PRECISION array, dimension ( M )
           The first elements of this array contain the components
           of the deflation-adjusted updating row vector.

    K      (output) INTEGER
           Contains the dimension of the non-deflated matrix,
           This is the order of the related secular equation. 1 <= K <=N.

    C      (output) DOUBLE PRECISION
           C contains garbage if SQRE =0 and the C-value of a Givens
           rotation related to the right null space if SQRE = 1.

    S      (output) DOUBLE PRECISION
           S contains garbage if SQRE =0 and the S-value of a Givens
           rotation related to the right null space if SQRE = 1.

    WORK   (workspace) DOUBLE PRECISION array, dimension ( 4 * M )

    IWORK  (workspace) INTEGER array, dimension ( 3 * N )

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an singular value did not converge

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --vf;
    --vl;
    --idxq;
    --perm;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1 * 1;
    givcol -= givcol_offset;
    poles_dim1 = *ldgnum;
    poles_offset = 1 + poles_dim1 * 1;
    poles -= poles_offset;
    givnum_dim1 = *ldgnum;
    givnum_offset = 1 + givnum_dim1 * 1;
    givnum -= givnum_offset;
    --difl;
    --difr;
    --z__;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    n = *nl + *nr + 1;
    m = n + *sqre;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*nl < 1) {
	*info = -2;
    } else if (*nr < 1) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    } else if (*ldgcol < n) {
	*info = -14;
    } else if (*ldgnum < n) {
	*info = -16;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD6", &i__1);
	return 0;
    }

/*
       The following values are for bookkeeping purposes only.  They are
       integer pointers which indicate the portion of the workspace
       used by a particular array in DLASD7 and DLASD8.
*/

    isigma = 1;
    iw = isigma + n;
    ivfw = iw + m;
    ivlw = ivfw + m;

    idx = 1;
    idxc = idx + n;
    idxp = idxc + n;

/*
       Scale.

   Computing MAX
*/
    d__1 = abs(*alpha), d__2 = abs(*beta);
    orgnrm = max(d__1,d__2);
    d__[*nl + 1] = 0.;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) > orgnrm) {
	    orgnrm = (d__1 = d__[i__], abs(d__1));
	}
/* L10: */
    }
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, &n, &c__1, &d__[1], &n, info);
    *alpha /= orgnrm;
    *beta /= orgnrm;

/*     Sort and Deflate singular values. */

    dlasd7_(icompq, nl, nr, sqre, k, &d__[1], &z__[1], &work[iw], &vf[1], &
	    work[ivfw], &vl[1], &work[ivlw], alpha, beta, &work[isigma], &
	    iwork[idx], &iwork[idxp], &idxq[1], &perm[1], givptr, &givcol[
	    givcol_offset], ldgcol, &givnum[givnum_offset], ldgnum, c__, s,
	    info);

/*     Solve Secular Equation, compute DIFL, DIFR, and update VF, VL. */

    dlasd8_(icompq, k, &d__[1], &z__[1], &vf[1], &vl[1], &difl[1], &difr[1],
	    ldgnum, &work[isigma], &work[iw], info);

/*     Save the poles if ICOMPQ = 1. */

    if (*icompq == 1) {
	dcopy_(k, &d__[1], &c__1, &poles[poles_dim1 + 1], &c__1);
	dcopy_(k, &work[isigma], &c__1, &poles[((poles_dim1) << (1)) + 1], &
		c__1);
    }

/*     Unscale. */

    dlascl_("G", &c__0, &c__0, &c_b15, &orgnrm, &n, &c__1, &d__[1], &n, info);

/*     Prepare the IDXQ sorting permutation. */

    n1 = *k;
    n2 = n - *k;
    dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &idxq[1]);

    return 0;

/*     End of DLASD6 */

} /* dlasd6_ */

/* Subroutine */ int dlasd7_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, integer *k, doublereal *d__, doublereal *z__,
	doublereal *zw, doublereal *vf, doublereal *vfw, doublereal *vl,
	doublereal *vlw, doublereal *alpha, doublereal *beta, doublereal *
	dsigma, integer *idx, integer *idxp, integer *idxq, integer *perm,
	integer *givptr, integer *givcol, integer *ldgcol, doublereal *givnum,
	 integer *ldgnum, doublereal *c__, doublereal *s, integer *info)
{
    /* System generated locals */
    integer givcol_dim1, givcol_offset, givnum_dim1, givnum_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__, j, m, n, k2;
    static doublereal z1;
    static integer jp;
    static doublereal eps, tau, tol;
    static integer nlp1, nlp2, idxi, idxj;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *);
    static integer idxjp;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer jprev;

    extern /* Subroutine */ int dlamrg_(integer *, integer *, doublereal *,
	    integer *, integer *, integer *), xerbla_(char *, integer *);
    static doublereal hlftol;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       June 30, 1999


    Purpose
    =======

    DLASD7 merges the two sets of singular values together into a single
    sorted set. Then it tries to deflate the size of the problem. There
    are two ways in which deflation can occur:  when two or more singular
    values are close together or if there is a tiny entry in the Z
    vector. For each such occurrence the order of the related
    secular equation problem is reduced by one.

    DLASD7 is called from DLASD6.

    Arguments
    =========

    ICOMPQ  (input) INTEGER
            Specifies whether singular vectors are to be computed
            in compact form, as follows:
            = 0: Compute singular values only.
            = 1: Compute singular vectors of upper
                 bidiagonal matrix in compact form.

    NL     (input) INTEGER
           The row dimension of the upper block. NL >= 1.

    NR     (input) INTEGER
           The row dimension of the lower block. NR >= 1.

    SQRE   (input) INTEGER
           = 0: the lower block is an NR-by-NR square matrix.
           = 1: the lower block is an NR-by-(NR+1) rectangular matrix.

           The bidiagonal matrix has
           N = NL + NR + 1 rows and
           M = N + SQRE >= N columns.

    K      (output) INTEGER
           Contains the dimension of the non-deflated matrix, this is
           the order of the related secular equation. 1 <= K <=N.

    D      (input/output) DOUBLE PRECISION array, dimension ( N )
           On entry D contains the singular values of the two submatrices
           to be combined. On exit D contains the trailing (N-K) updated
           singular values (those which were deflated) sorted into
           increasing order.

    Z      (output) DOUBLE PRECISION array, dimension ( M )
           On exit Z contains the updating row vector in the secular
           equation.

    ZW     (workspace) DOUBLE PRECISION array, dimension ( M )
           Workspace for Z.

    VF     (input/output) DOUBLE PRECISION array, dimension ( M )
           On entry, VF(1:NL+1) contains the first components of all
           right singular vectors of the upper block; and VF(NL+2:M)
           contains the first components of all right singular vectors
           of the lower block. On exit, VF contains the first components
           of all right singular vectors of the bidiagonal matrix.

    VFW    (workspace) DOUBLE PRECISION array, dimension ( M )
           Workspace for VF.

    VL     (input/output) DOUBLE PRECISION array, dimension ( M )
           On entry, VL(1:NL+1) contains the  last components of all
           right singular vectors of the upper block; and VL(NL+2:M)
           contains the last components of all right singular vectors
           of the lower block. On exit, VL contains the last components
           of all right singular vectors of the bidiagonal matrix.

    VLW    (workspace) DOUBLE PRECISION array, dimension ( M )
           Workspace for VL.

    ALPHA  (input) DOUBLE PRECISION
           Contains the diagonal element associated with the added row.

    BETA   (input) DOUBLE PRECISION
           Contains the off-diagonal element associated with the added
           row.

    DSIGMA (output) DOUBLE PRECISION array, dimension ( N )
           Contains a copy of the diagonal elements (K-1 singular values
           and one zero) in the secular equation.

    IDX    (workspace) INTEGER array, dimension ( N )
           This will contain the permutation used to sort the contents of
           D into ascending order.

    IDXP   (workspace) INTEGER array, dimension ( N )
           This will contain the permutation used to place deflated
           values of D at the end of the array. On output IDXP(2:K)
           points to the nondeflated D-values and IDXP(K+1:N)
           points to the deflated singular values.

    IDXQ   (input) INTEGER array, dimension ( N )
           This contains the permutation which separately sorts the two
           sub-problems in D into ascending order.  Note that entries in
           the first half of this permutation must first be moved one
           position backward; and entries in the second half
           must first have NL+1 added to their values.

    PERM   (output) INTEGER array, dimension ( N )
           The permutations (from deflation and sorting) to be applied
           to each singular block. Not referenced if ICOMPQ = 0.

    GIVPTR (output) INTEGER
           The number of Givens rotations which took place in this
           subproblem. Not referenced if ICOMPQ = 0.

    GIVCOL (output) INTEGER array, dimension ( LDGCOL, 2 )
           Each pair of numbers indicates a pair of columns to take place
           in a Givens rotation. Not referenced if ICOMPQ = 0.

    LDGCOL (input) INTEGER
           The leading dimension of GIVCOL, must be at least N.

    GIVNUM (output) DOUBLE PRECISION array, dimension ( LDGNUM, 2 )
           Each number indicates the C or S value to be used in the
           corresponding Givens rotation. Not referenced if ICOMPQ = 0.

    LDGNUM (input) INTEGER
           The leading dimension of GIVNUM, must be at least N.

    C      (output) DOUBLE PRECISION
           C contains garbage if SQRE =0 and the C-value of a Givens
           rotation related to the right null space if SQRE = 1.

    S      (output) DOUBLE PRECISION
           S contains garbage if SQRE =0 and the S-value of a Givens
           rotation related to the right null space if SQRE = 1.

    INFO   (output) INTEGER
           = 0:  successful exit.
           < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --z__;
    --zw;
    --vf;
    --vfw;
    --vl;
    --vlw;
    --dsigma;
    --idx;
    --idxp;
    --idxq;
    --perm;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1 * 1;
    givcol -= givcol_offset;
    givnum_dim1 = *ldgnum;
    givnum_offset = 1 + givnum_dim1 * 1;
    givnum -= givnum_offset;

    /* Function Body */
    *info = 0;
    n = *nl + *nr + 1;
    m = n + *sqre;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*nl < 1) {
	*info = -2;
    } else if (*nr < 1) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    } else if (*ldgcol < n) {
	*info = -22;
    } else if (*ldgnum < n) {
	*info = -24;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD7", &i__1);
	return 0;
    }

    nlp1 = *nl + 1;
    nlp2 = *nl + 2;
    if (*icompq == 1) {
	*givptr = 0;
    }

/*
       Generate the first part of the vector Z and move the singular
       values in the first part of D one position backward.
*/

    z1 = *alpha * vl[nlp1];
    vl[nlp1] = 0.;
    tau = vf[nlp1];
    for (i__ = *nl; i__ >= 1; --i__) {
	z__[i__ + 1] = *alpha * vl[i__];
	vl[i__] = 0.;
	vf[i__ + 1] = vf[i__];
	d__[i__ + 1] = d__[i__];
	idxq[i__ + 1] = idxq[i__] + 1;
/* L10: */
    }
    vf[1] = tau;

/*     Generate the second part of the vector Z. */

    i__1 = m;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	z__[i__] = *beta * vf[i__];
	vf[i__] = 0.;
/* L20: */
    }

/*     Sort the singular values into increasing order */

    i__1 = n;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	idxq[i__] += nlp1;
/* L30: */
    }

/*     DSIGMA, IDXC, IDXC, and ZW are used as storage space. */

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	dsigma[i__] = d__[idxq[i__]];
	zw[i__] = z__[idxq[i__]];
	vfw[i__] = vf[idxq[i__]];
	vlw[i__] = vl[idxq[i__]];
/* L40: */
    }

    dlamrg_(nl, nr, &dsigma[2], &c__1, &c__1, &idx[2]);

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	idxi = idx[i__] + 1;
	d__[i__] = dsigma[idxi];
	z__[i__] = zw[idxi];
	vf[i__] = vfw[idxi];
	vl[i__] = vlw[idxi];
/* L50: */
    }

/*     Calculate the allowable deflation tolerence */

    eps = EPSILON;
/* Computing MAX */
    d__1 = abs(*alpha), d__2 = abs(*beta);
    tol = max(d__1,d__2);
/* Computing MAX */
    d__2 = (d__1 = d__[n], abs(d__1));
    tol = eps * 64. * max(d__2,tol);

/*
       There are 2 kinds of deflation -- first a value in the z-vector
       is small, second two (or more) singular values are very close
       together (their difference is small).

       If the value in the z-vector is small, we simply permute the
       array so that the corresponding singular value is moved to the
       end.

       If two values in the D-vector are close, we perform a two-sided
       rotation designed to make one of the corresponding z-vector
       entries zero, and then permute the array so that the deflated
       singular value is moved to the end.

       If there are multiple singular values then the problem deflates.
       Here the number of equal singular values are found.  As each equal
       singular value is found, an elementary reflector is computed to
       rotate the corresponding singular subspace so that the
       corresponding components of Z are zero in this new basis.
*/

    *k = 1;
    k2 = n + 1;
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    idxp[k2] = j;
	    if (j == n) {
		goto L100;
	    }
	} else {
	    jprev = j;
	    goto L70;
	}
/* L60: */
    }
L70:
    j = jprev;
L80:
    ++j;
    if (j > n) {
	goto L90;
    }
    if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	idxp[k2] = j;
    } else {

/*        Check if singular values are close enough to allow deflation. */

	if ((d__1 = d__[j] - d__[jprev], abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    *s = z__[jprev];
	    *c__ = z__[j];

/*
             Find sqrt(a**2+b**2) without overflow or
             destructive underflow.
*/

	    tau = dlapy2_(c__, s);
	    z__[j] = tau;
	    z__[jprev] = 0.;
	    *c__ /= tau;
	    *s = -(*s) / tau;

/*           Record the appropriate Givens rotation */

	    if (*icompq == 1) {
		++(*givptr);
		idxjp = idxq[idx[jprev] + 1];
		idxj = idxq[idx[j] + 1];
		if (idxjp <= nlp1) {
		    --idxjp;
		}
		if (idxj <= nlp1) {
		    --idxj;
		}
		givcol[*givptr + ((givcol_dim1) << (1))] = idxjp;
		givcol[*givptr + givcol_dim1] = idxj;
		givnum[*givptr + ((givnum_dim1) << (1))] = *c__;
		givnum[*givptr + givnum_dim1] = *s;
	    }
	    drot_(&c__1, &vf[jprev], &c__1, &vf[j], &c__1, c__, s);
	    drot_(&c__1, &vl[jprev], &c__1, &vl[j], &c__1, c__, s);
	    --k2;
	    idxp[k2] = jprev;
	    jprev = j;
	} else {
	    ++(*k);
	    zw[*k] = z__[jprev];
	    dsigma[*k] = d__[jprev];
	    idxp[*k] = jprev;
	    jprev = j;
	}
    }
    goto L80;
L90:

/*     Record the last singular value. */

    ++(*k);
    zw[*k] = z__[jprev];
    dsigma[*k] = d__[jprev];
    idxp[*k] = jprev;

L100:

/*
       Sort the singular values into DSIGMA. The singular values which
       were not deflated go into the first K slots of DSIGMA, except
       that DSIGMA(1) is treated separately.
*/

    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	jp = idxp[j];
	dsigma[j] = d__[jp];
	vfw[j] = vf[jp];
	vlw[j] = vl[jp];
/* L110: */
    }
    if (*icompq == 1) {
	i__1 = n;
	for (j = 2; j <= i__1; ++j) {
	    jp = idxp[j];
	    perm[j] = idxq[idx[jp] + 1];
	    if (perm[j] <= nlp1) {
		--perm[j];
	    }
/* L120: */
	}
    }

/*
       The deflated singular values go back into the last N - K slots of
       D.
*/

    i__1 = n - *k;
    dcopy_(&i__1, &dsigma[*k + 1], &c__1, &d__[*k + 1], &c__1);

/*
       Determine DSIGMA(1), DSIGMA(2), Z(1), VF(1), VL(1), VF(M), and
       VL(M).
*/

    dsigma[1] = 0.;
    hlftol = tol / 2.;
    if (abs(dsigma[2]) <= hlftol) {
	dsigma[2] = hlftol;
    }
    if (m > n) {
	z__[1] = dlapy2_(&z1, &z__[m]);
	if (z__[1] <= tol) {
	    *c__ = 1.;
	    *s = 0.;
	    z__[1] = tol;
	} else {
	    *c__ = z1 / z__[1];
	    *s = -z__[m] / z__[1];
	}
	drot_(&c__1, &vf[m], &c__1, &vf[1], &c__1, c__, s);
	drot_(&c__1, &vl[m], &c__1, &vl[1], &c__1, c__, s);
    } else {
	if (abs(z1) <= tol) {
	    z__[1] = tol;
	} else {
	    z__[1] = z1;
	}
    }

/*     Restore Z, VF, and VL. */

    i__1 = *k - 1;
    dcopy_(&i__1, &zw[2], &c__1, &z__[2], &c__1);
    i__1 = n - 1;
    dcopy_(&i__1, &vfw[2], &c__1, &vf[2], &c__1);
    i__1 = n - 1;
    dcopy_(&i__1, &vlw[2], &c__1, &vl[2], &c__1);

    return 0;

/*     End of DLASD7 */

} /* dlasd7_ */

/* Subroutine */ int dlasd8_(integer *icompq, integer *k, doublereal *d__,
	doublereal *z__, doublereal *vf, doublereal *vl, doublereal *difl,
	doublereal *difr, integer *lddifr, doublereal *dsigma, doublereal *
	work, integer *info)
{
    /* System generated locals */
    integer difr_dim1, difr_offset, i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static integer i__, j;
    static doublereal dj, rho;
    static integer iwk1, iwk2, iwk3;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
	    integer *);
    static doublereal temp;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    static integer iwk2i, iwk3i;
    static doublereal diflj, difrj, dsigj;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    extern doublereal dlamc3_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlasd4_(integer *, integer *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, integer *), dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *), dlaset_(char *, integer *, integer
	    *, doublereal *, doublereal *, doublereal *, integer *),
	    xerbla_(char *, integer *);
    static doublereal dsigjp;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
       Courant Institute, NAG Ltd., and Rice University
       June 30, 1999


    Purpose
    =======

    DLASD8 finds the square roots of the roots of the secular equation,
    as defined by the values in DSIGMA and Z. It makes the appropriate
    calls to DLASD4, and stores, for each  element in D, the distance
    to its two nearest poles (elements in DSIGMA). It also updates
    the arrays VF and VL, the first and last components of all the
    right singular vectors of the original bidiagonal matrix.

    DLASD8 is called from DLASD6.

    Arguments
    =========

    ICOMPQ  (input) INTEGER
            Specifies whether singular vectors are to be computed in
            factored form in the calling routine:
            = 0: Compute singular values only.
            = 1: Compute singular vectors in factored form as well.

    K       (input) INTEGER
            The number of terms in the rational function to be solved
            by DLASD4.  K >= 1.

    D       (output) DOUBLE PRECISION array, dimension ( K )
            On output, D contains the updated singular values.

    Z       (input) DOUBLE PRECISION array, dimension ( K )
            The first K elements of this array contain the components
            of the deflation-adjusted updating row vector.

    VF      (input/output) DOUBLE PRECISION array, dimension ( K )
            On entry, VF contains  information passed through DBEDE8.
            On exit, VF contains the first K components of the first
            components of all right singular vectors of the bidiagonal
            matrix.

    VL      (input/output) DOUBLE PRECISION array, dimension ( K )
            On entry, VL contains  information passed through DBEDE8.
            On exit, VL contains the first K components of the last
            components of all right singular vectors of the bidiagonal
            matrix.

    DIFL    (output) DOUBLE PRECISION array, dimension ( K )
            On exit, DIFL(I) = D(I) - DSIGMA(I).

    DIFR    (output) DOUBLE PRECISION array,
                     dimension ( LDDIFR, 2 ) if ICOMPQ = 1 and
                     dimension ( K ) if ICOMPQ = 0.
            On exit, DIFR(I,1) = D(I) - DSIGMA(I+1), DIFR(K,1) is not
            defined and will not be referenced.

            If ICOMPQ = 1, DIFR(1:K,2) is an array containing the
            normalizing factors for the right singular vector matrix.

    LDDIFR  (input) INTEGER
            The leading dimension of DIFR, must be at least K.

    DSIGMA  (input) DOUBLE PRECISION array, dimension ( K )
            The first K elements of this array contain the old roots
            of the deflated updating problem.  These are the poles
            of the secular equation.

    WORK    (workspace) DOUBLE PRECISION array, dimension at least 3 * K

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an singular value did not converge

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --z__;
    --vf;
    --vl;
    --difl;
    difr_dim1 = *lddifr;
    difr_offset = 1 + difr_dim1 * 1;
    difr -= difr_offset;
    --dsigma;
    --work;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*k < 1) {
	*info = -2;
    } else if (*lddifr < *k) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD8", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 1) {
	d__[1] = abs(z__[1]);
	difl[1] = d__[1];
	if (*icompq == 1) {
	    difl[2] = 1.;
	    difr[((difr_dim1) << (1)) + 1] = 1.;
	}
	return 0;
    }

/*
       Modify values DSIGMA(i) to make sure all DSIGMA(i)-DSIGMA(j) can
       be computed with high relative accuracy (barring over/underflow).
       This is a problem on machines without a guard digit in
       add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2).
       The following code replaces DSIGMA(I) by 2*DSIGMA(I)-DSIGMA(I),
       which on any of these machines zeros out the bottommost
       bit of DSIGMA(I) if it is 1; this makes the subsequent
       subtractions DSIGMA(I)-DSIGMA(J) unproblematic when cancellation
       occurs. On binary machines with a guard digit (almost all
       machines) it does not change DSIGMA(I) at all. On hexadecimal
       and decimal machines with a guard digit, it slightly
       changes the bottommost bits of DSIGMA(I). It does not account
       for hexadecimal or decimal machines without guard digits
       (we know of none). We use a subroutine call to compute
       2*DLAMBDA(I) to prevent optimizing compilers from eliminating
       this code.
*/

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dsigma[i__] = dlamc3_(&dsigma[i__], &dsigma[i__]) - dsigma[i__];
/* L10: */
    }

/*     Book keeping. */

    iwk1 = 1;
    iwk2 = iwk1 + *k;
    iwk3 = iwk2 + *k;
    iwk2i = iwk2 - 1;
    iwk3i = iwk3 - 1;

/*     Normalize Z. */

    rho = dnrm2_(k, &z__[1], &c__1);
    dlascl_("G", &c__0, &c__0, &rho, &c_b15, k, &c__1, &z__[1], k, info);
    rho *= rho;

/*     Initialize WORK(IWK3). */

    dlaset_("A", k, &c__1, &c_b15, &c_b15, &work[iwk3], k);

/*
       Compute the updated singular values, the arrays DIFL, DIFR,
       and the updated Z.
*/

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	dlasd4_(k, &j, &dsigma[1], &z__[1], &work[iwk1], &rho, &d__[j], &work[
		iwk2], info);

/*        If the root finder fails, the computation is terminated. */

	if (*info != 0) {
	    return 0;
	}
	work[iwk3i + j] = work[iwk3i + j] * work[j] * work[iwk2i + j];
	difl[j] = -work[j];
	difr[j + difr_dim1] = -work[j + 1];
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[iwk3i + i__] = work[iwk3i + i__] * work[i__] * work[iwk2i +
		    i__] / (dsigma[i__] - dsigma[j]) / (dsigma[i__] + dsigma[
		    j]);
/* L20: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    work[iwk3i + i__] = work[iwk3i + i__] * work[i__] * work[iwk2i +
		    i__] / (dsigma[i__] - dsigma[j]) / (dsigma[i__] + dsigma[
		    j]);
/* L30: */
	}
/* L40: */
    }

/*     Compute updated Z. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__2 = sqrt((d__1 = work[iwk3i + i__], abs(d__1)));
	z__[i__] = d_sign(&d__2, &z__[i__]);
/* L50: */
    }

/*     Update VF and VL. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	diflj = difl[j];
	dj = d__[j];
	dsigj = -dsigma[j];
	if (j < *k) {
	    difrj = -difr[j + difr_dim1];
	    dsigjp = -dsigma[j + 1];
	}
	work[j] = -z__[j] / diflj / (dsigma[j] + dj);
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = z__[i__] / (dlamc3_(&dsigma[i__], &dsigj) - diflj) / (
		    dsigma[i__] + dj);
/* L60: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    work[i__] = z__[i__] / (dlamc3_(&dsigma[i__], &dsigjp) + difrj) /
		    (dsigma[i__] + dj);
/* L70: */
	}
	temp = dnrm2_(k, &work[1], &c__1);
	work[iwk2i + j] = ddot_(k, &work[1], &c__1, &vf[1], &c__1) / temp;
	work[iwk3i + j] = ddot_(k, &work[1], &c__1, &vl[1], &c__1) / temp;
	if (*icompq == 1) {
	    difr[j + ((difr_dim1) << (1))] = temp;
	}
/* L80: */
    }

    dcopy_(k, &work[iwk2], &c__1, &vf[1], &c__1);
    dcopy_(k, &work[iwk3], &c__1, &vl[1], &c__1);

    return 0;

/*     End of DLASD8 */

} /* dlasd8_ */

/* Subroutine */ int dlasda_(integer *icompq, integer *smlsiz, integer *n,
	integer *sqre, doublereal *d__, doublereal *e, doublereal *u, integer
	*ldu, doublereal *vt, integer *k, doublereal *difl, doublereal *difr,
	doublereal *z__, doublereal *poles, integer *givptr, integer *givcol,
	integer *ldgcol, integer *perm, doublereal *givnum, doublereal *c__,
	doublereal *s, doublereal *work, integer *iwork, integer *info)
{
    /* System generated locals */
    integer givcol_dim1, givcol_offset, perm_dim1, perm_offset, difl_dim1,
	    difl_offset, difr_dim1, difr_offset, givnum_dim1, givnum_offset,
	    poles_dim1, poles_offset, u_dim1, u_offset, vt_dim1, vt_offset,
	    z_dim1, z_offset, i__1, i__2;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__, j, m, i1, ic, lf, nd, ll, nl, vf, nr, vl, im1, ncc,
	    nlf, nrf, vfi, iwk, vli, lvl, nru, ndb1, nlp1, lvl2, nrp1;
    static doublereal beta;
    static integer idxq, nlvl;
    static doublereal alpha;
    static integer inode, ndiml, ndimr, idxqi, itemp;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer sqrei;
    extern /* Subroutine */ int dlasd6_(integer *, integer *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, integer *, integer *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     doublereal *, integer *, integer *);
    static integer nwork1, nwork2;
    extern /* Subroutine */ int dlasdq_(char *, integer *, integer *, integer
	    *, integer *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *), dlasdt_(integer *, integer *,
	    integer *, integer *, integer *, integer *, integer *), dlaset_(
	    char *, integer *, integer *, doublereal *, doublereal *,
	    doublereal *, integer *), xerbla_(char *, integer *);
    static integer smlszp;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    Using a divide and conquer approach, DLASDA computes the singular
    value decomposition (SVD) of a real upper bidiagonal N-by-M matrix
    B with diagonal D and offdiagonal E, where M = N + SQRE. The
    algorithm computes the singular values in the SVD B = U * S * VT.
    The orthogonal matrices U and VT are optionally computed in
    compact form.

    A related subroutine, DLASD0, computes the singular values and
    the singular vectors in explicit form.

    Arguments
    =========

    ICOMPQ (input) INTEGER
           Specifies whether singular vectors are to be computed
           in compact form, as follows
           = 0: Compute singular values only.
           = 1: Compute singular vectors of upper bidiagonal
                matrix in compact form.

    SMLSIZ (input) INTEGER
           The maximum size of the subproblems at the bottom of the
           computation tree.

    N      (input) INTEGER
           The row dimension of the upper bidiagonal matrix. This is
           also the dimension of the main diagonal array D.

    SQRE   (input) INTEGER
           Specifies the column dimension of the bidiagonal matrix.
           = 0: The bidiagonal matrix has column dimension M = N;
           = 1: The bidiagonal matrix has column dimension M = N + 1.

    D      (input/output) DOUBLE PRECISION array, dimension ( N )
           On entry D contains the main diagonal of the bidiagonal
           matrix. On exit D, if INFO = 0, contains its singular values.

    E      (input) DOUBLE PRECISION array, dimension ( M-1 )
           Contains the subdiagonal entries of the bidiagonal matrix.
           On exit, E has been destroyed.

    U      (output) DOUBLE PRECISION array,
           dimension ( LDU, SMLSIZ ) if ICOMPQ = 1, and not referenced
           if ICOMPQ = 0. If ICOMPQ = 1, on exit, U contains the left
           singular vector matrices of all subproblems at the bottom
           level.

    LDU    (input) INTEGER, LDU = > N.
           The leading dimension of arrays U, VT, DIFL, DIFR, POLES,
           GIVNUM, and Z.

    VT     (output) DOUBLE PRECISION array,
           dimension ( LDU, SMLSIZ+1 ) if ICOMPQ = 1, and not referenced
           if ICOMPQ = 0. If ICOMPQ = 1, on exit, VT' contains the right
           singular vector matrices of all subproblems at the bottom
           level.

    K      (output) INTEGER array,
           dimension ( N ) if ICOMPQ = 1 and dimension 1 if ICOMPQ = 0.
           If ICOMPQ = 1, on exit, K(I) is the dimension of the I-th
           secular equation on the computation tree.

    DIFL   (output) DOUBLE PRECISION array, dimension ( LDU, NLVL ),
           where NLVL = floor(log_2 (N/SMLSIZ))).

    DIFR   (output) DOUBLE PRECISION array,
                    dimension ( LDU, 2 * NLVL ) if ICOMPQ = 1 and
                    dimension ( N ) if ICOMPQ = 0.
           If ICOMPQ = 1, on exit, DIFL(1:N, I) and DIFR(1:N, 2 * I - 1)
           record distances between singular values on the I-th
           level and singular values on the (I -1)-th level, and
           DIFR(1:N, 2 * I ) contains the normalizing factors for
           the right singular vector matrix. See DLASD8 for details.

    Z      (output) DOUBLE PRECISION array,
                    dimension ( LDU, NLVL ) if ICOMPQ = 1 and
                    dimension ( N ) if ICOMPQ = 0.
           The first K elements of Z(1, I) contain the components of
           the deflation-adjusted updating row vector for subproblems
           on the I-th level.

    POLES  (output) DOUBLE PRECISION array,
           dimension ( LDU, 2 * NLVL ) if ICOMPQ = 1, and not referenced
           if ICOMPQ = 0. If ICOMPQ = 1, on exit, POLES(1, 2*I - 1) and
           POLES(1, 2*I) contain  the new and old singular values
           involved in the secular equations on the I-th level.

    GIVPTR (output) INTEGER array,
           dimension ( N ) if ICOMPQ = 1, and not referenced if
           ICOMPQ = 0. If ICOMPQ = 1, on exit, GIVPTR( I ) records
           the number of Givens rotations performed on the I-th
           problem on the computation tree.

    GIVCOL (output) INTEGER array,
           dimension ( LDGCOL, 2 * NLVL ) if ICOMPQ = 1, and not
           referenced if ICOMPQ = 0. If ICOMPQ = 1, on exit, for each I,
           GIVCOL(1, 2 *I - 1) and GIVCOL(1, 2 *I) record the locations
           of Givens rotations performed on the I-th level on the
           computation tree.

    LDGCOL (input) INTEGER, LDGCOL = > N.
           The leading dimension of arrays GIVCOL and PERM.

    PERM   (output) INTEGER array,
           dimension ( LDGCOL, NLVL ) if ICOMPQ = 1, and not referenced
           if ICOMPQ = 0. If ICOMPQ = 1, on exit, PERM(1, I) records
           permutations done on the I-th level of the computation tree.

    GIVNUM (output) DOUBLE PRECISION array,
           dimension ( LDU,  2 * NLVL ) if ICOMPQ = 1, and not
           referenced if ICOMPQ = 0. If ICOMPQ = 1, on exit, for each I,
           GIVNUM(1, 2 *I - 1) and GIVNUM(1, 2 *I) record the C- and S-
           values of Givens rotations performed on the I-th level on
           the computation tree.

    C      (output) DOUBLE PRECISION array,
           dimension ( N ) if ICOMPQ = 1, and dimension 1 if ICOMPQ = 0.
           If ICOMPQ = 1 and the I-th subproblem is not square, on exit,
           C( I ) contains the C-value of a Givens rotation related to
           the right null space of the I-th subproblem.

    S      (output) DOUBLE PRECISION array, dimension ( N ) if
           ICOMPQ = 1, and dimension 1 if ICOMPQ = 0. If ICOMPQ = 1
           and the I-th subproblem is not square, on exit, S( I )
           contains the S-value of a Givens rotation related to
           the right null space of the I-th subproblem.

    WORK   (workspace) DOUBLE PRECISION array, dimension
           (6 * N + (SMLSIZ + 1)*(SMLSIZ + 1)).

    IWORK  (workspace) INTEGER array.
           Dimension must be at least (7 * N).

    INFO   (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, an singular value did not converge

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    givnum_dim1 = *ldu;
    givnum_offset = 1 + givnum_dim1 * 1;
    givnum -= givnum_offset;
    poles_dim1 = *ldu;
    poles_offset = 1 + poles_dim1 * 1;
    poles -= poles_offset;
    z_dim1 = *ldu;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    difr_dim1 = *ldu;
    difr_offset = 1 + difr_dim1 * 1;
    difr -= difr_offset;
    difl_dim1 = *ldu;
    difl_offset = 1 + difl_dim1 * 1;
    difl -= difl_offset;
    vt_dim1 = *ldu;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    --k;
    --givptr;
    perm_dim1 = *ldgcol;
    perm_offset = 1 + perm_dim1 * 1;
    perm -= perm_offset;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1 * 1;
    givcol -= givcol_offset;
    --c__;
    --s;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*smlsiz < 3) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    } else if (*ldu < *n + *sqre) {
	*info = -8;
    } else if (*ldgcol < *n) {
	*info = -17;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASDA", &i__1);
	return 0;
    }

    m = *n + *sqre;

/*     If the input matrix is too small, call DLASDQ to find the SVD. */

    if (*n <= *smlsiz) {
	if (*icompq == 0) {
	    dlasdq_("U", sqre, n, &c__0, &c__0, &c__0, &d__[1], &e[1], &vt[
		    vt_offset], ldu, &u[u_offset], ldu, &u[u_offset], ldu, &
		    work[1], info);
	} else {
	    dlasdq_("U", sqre, n, &m, n, &c__0, &d__[1], &e[1], &vt[vt_offset]
		    , ldu, &u[u_offset], ldu, &u[u_offset], ldu, &work[1],
		    info);
	}
	return 0;
    }

/*     Book-keeping and  set up the computation tree. */

    inode = 1;
    ndiml = inode + *n;
    ndimr = ndiml + *n;
    idxq = ndimr + *n;
    iwk = idxq + *n;

    ncc = 0;
    nru = 0;

    smlszp = *smlsiz + 1;
    vf = 1;
    vl = vf + m;
    nwork1 = vl + m;
    nwork2 = nwork1 + smlszp * smlszp;

    dlasdt_(n, &nlvl, &nd, &iwork[inode], &iwork[ndiml], &iwork[ndimr],
	    smlsiz);

/*
       for the nodes on bottom level of the tree, solve
       their subproblems by DLASDQ.
*/

    ndb1 = (nd + 1) / 2;
    i__1 = nd;
    for (i__ = ndb1; i__ <= i__1; ++i__) {

/*
          IC : center row of each node
          NL : number of rows of left  subproblem
          NR : number of rows of right subproblem
          NLF: starting row of the left   subproblem
          NRF: starting row of the right  subproblem
*/

	i1 = i__ - 1;
	ic = iwork[inode + i1];
	nl = iwork[ndiml + i1];
	nlp1 = nl + 1;
	nr = iwork[ndimr + i1];
	nlf = ic - nl;
	nrf = ic + 1;
	idxqi = idxq + nlf - 2;
	vfi = vf + nlf - 1;
	vli = vl + nlf - 1;
	sqrei = 1;
	if (*icompq == 0) {
	    dlaset_("A", &nlp1, &nlp1, &c_b29, &c_b15, &work[nwork1], &smlszp);
	    dlasdq_("U", &sqrei, &nl, &nlp1, &nru, &ncc, &d__[nlf], &e[nlf], &
		    work[nwork1], &smlszp, &work[nwork2], &nl, &work[nwork2],
		    &nl, &work[nwork2], info);
	    itemp = nwork1 + nl * smlszp;
	    dcopy_(&nlp1, &work[nwork1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nlp1, &work[itemp], &c__1, &work[vli], &c__1);
	} else {
	    dlaset_("A", &nl, &nl, &c_b29, &c_b15, &u[nlf + u_dim1], ldu);
	    dlaset_("A", &nlp1, &nlp1, &c_b29, &c_b15, &vt[nlf + vt_dim1],
		    ldu);
	    dlasdq_("U", &sqrei, &nl, &nlp1, &nl, &ncc, &d__[nlf], &e[nlf], &
		    vt[nlf + vt_dim1], ldu, &u[nlf + u_dim1], ldu, &u[nlf +
		    u_dim1], ldu, &work[nwork1], info);
	    dcopy_(&nlp1, &vt[nlf + vt_dim1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nlp1, &vt[nlf + nlp1 * vt_dim1], &c__1, &work[vli], &c__1)
		    ;
	}
	if (*info != 0) {
	    return 0;
	}
	i__2 = nl;
	for (j = 1; j <= i__2; ++j) {
	    iwork[idxqi + j] = j;
/* L10: */
	}
	if ((i__ == nd && *sqre == 0)) {
	    sqrei = 0;
	} else {
	    sqrei = 1;
	}
	idxqi += nlp1;
	vfi += nlp1;
	vli += nlp1;
	nrp1 = nr + sqrei;
	if (*icompq == 0) {
	    dlaset_("A", &nrp1, &nrp1, &c_b29, &c_b15, &work[nwork1], &smlszp);
	    dlasdq_("U", &sqrei, &nr, &nrp1, &nru, &ncc, &d__[nrf], &e[nrf], &
		    work[nwork1], &smlszp, &work[nwork2], &nr, &work[nwork2],
		    &nr, &work[nwork2], info);
	    itemp = nwork1 + (nrp1 - 1) * smlszp;
	    dcopy_(&nrp1, &work[nwork1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nrp1, &work[itemp], &c__1, &work[vli], &c__1);
	} else {
	    dlaset_("A", &nr, &nr, &c_b29, &c_b15, &u[nrf + u_dim1], ldu);
	    dlaset_("A", &nrp1, &nrp1, &c_b29, &c_b15, &vt[nrf + vt_dim1],
		    ldu);
	    dlasdq_("U", &sqrei, &nr, &nrp1, &nr, &ncc, &d__[nrf], &e[nrf], &
		    vt[nrf + vt_dim1], ldu, &u[nrf + u_dim1], ldu, &u[nrf +
		    u_dim1], ldu, &work[nwork1], info);
	    dcopy_(&nrp1, &vt[nrf + vt_dim1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nrp1, &vt[nrf + nrp1 * vt_dim1], &c__1, &work[vli], &c__1)
		    ;
	}
	if (*info != 0) {
	    return 0;
	}
	i__2 = nr;
	for (j = 1; j <= i__2; ++j) {
	    iwork[idxqi + j] = j;
/* L20: */
	}
/* L30: */
    }

/*     Now conquer each subproblem bottom-up. */

    j = pow_ii(&c__2, &nlvl);
    for (lvl = nlvl; lvl >= 1; --lvl) {
	lvl2 = ((lvl) << (1)) - 1;

/*
          Find the first node LF and last node LL on
          the current level LVL.
*/

	if (lvl == 1) {
	    lf = 1;
	    ll = 1;
	} else {
	    i__1 = lvl - 1;
	    lf = pow_ii(&c__2, &i__1);
	    ll = ((lf) << (1)) - 1;
	}
	i__1 = ll;
	for (i__ = lf; i__ <= i__1; ++i__) {
	    im1 = i__ - 1;
	    ic = iwork[inode + im1];
	    nl = iwork[ndiml + im1];
	    nr = iwork[ndimr + im1];
	    nlf = ic - nl;
	    nrf = ic + 1;
	    if (i__ == ll) {
		sqrei = *sqre;
	    } else {
		sqrei = 1;
	    }
	    vfi = vf + nlf - 1;
	    vli = vl + nlf - 1;
	    idxqi = idxq + nlf - 1;
	    alpha = d__[ic];
	    beta = e[ic];
	    if (*icompq == 0) {
		dlasd6_(icompq, &nl, &nr, &sqrei, &d__[nlf], &work[vfi], &
			work[vli], &alpha, &beta, &iwork[idxqi], &perm[
			perm_offset], &givptr[1], &givcol[givcol_offset],
			ldgcol, &givnum[givnum_offset], ldu, &poles[
			poles_offset], &difl[difl_offset], &difr[difr_offset],
			 &z__[z_offset], &k[1], &c__[1], &s[1], &work[nwork1],
			 &iwork[iwk], info);
	    } else {
		--j;
		dlasd6_(icompq, &nl, &nr, &sqrei, &d__[nlf], &work[vfi], &
			work[vli], &alpha, &beta, &iwork[idxqi], &perm[nlf +
			lvl * perm_dim1], &givptr[j], &givcol[nlf + lvl2 *
			givcol_dim1], ldgcol, &givnum[nlf + lvl2 *
			givnum_dim1], ldu, &poles[nlf + lvl2 * poles_dim1], &
			difl[nlf + lvl * difl_dim1], &difr[nlf + lvl2 *
			difr_dim1], &z__[nlf + lvl * z_dim1], &k[j], &c__[j],
			&s[j], &work[nwork1], &iwork[iwk], info);
	    }
	    if (*info != 0) {
		return 0;
	    }
/* L40: */
	}
/* L50: */
    }

    return 0;

/*     End of DLASDA */

} /* dlasda_ */

/* Subroutine */ int dlasdq_(char *uplo, integer *sqre, integer *n, integer *
	ncvt, integer *nru, integer *ncc, doublereal *d__, doublereal *e,
	doublereal *vt, integer *ldvt, doublereal *u, integer *ldu,
	doublereal *c__, integer *ldc, doublereal *work, integer *info)
{
    /* System generated locals */
    integer c_dim1, c_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1,
	    i__2;

    /* Local variables */
    static integer i__, j;
    static doublereal r__, cs, sn;
    static integer np1, isub;
    static doublereal smin;
    static integer sqre1;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dlasr_(char *, char *, char *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *), dswap_(integer *, doublereal *, integer *
	    , doublereal *, integer *);
    static integer iuplo;
    extern /* Subroutine */ int dlartg_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *), xerbla_(char *,
	    integer *), dbdsqr_(char *, integer *, integer *, integer
	    *, integer *, doublereal *, doublereal *, doublereal *, integer *,
	     doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *);
    static logical rotate;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DLASDQ computes the singular value decomposition (SVD) of a real
    (upper or lower) bidiagonal matrix with diagonal D and offdiagonal
    E, accumulating the transformations if desired. Letting B denote
    the input bidiagonal matrix, the algorithm computes orthogonal
    matrices Q and P such that B = Q * S * P' (P' denotes the transpose
    of P). The singular values S are overwritten on D.

    The input matrix U  is changed to U  * Q  if desired.
    The input matrix VT is changed to P' * VT if desired.
    The input matrix C  is changed to Q' * C  if desired.

    See "Computing  Small Singular Values of Bidiagonal Matrices With
    Guaranteed High Relative Accuracy," by J. Demmel and W. Kahan,
    LAPACK Working Note #3, for a detailed description of the algorithm.

    Arguments
    =========

    UPLO  (input) CHARACTER*1
          On entry, UPLO specifies whether the input bidiagonal matrix
          is upper or lower bidiagonal, and wether it is square are
          not.
             UPLO = 'U' or 'u'   B is upper bidiagonal.
             UPLO = 'L' or 'l'   B is lower bidiagonal.

    SQRE  (input) INTEGER
          = 0: then the input matrix is N-by-N.
          = 1: then the input matrix is N-by-(N+1) if UPLU = 'U' and
               (N+1)-by-N if UPLU = 'L'.

          The bidiagonal matrix has
          N = NL + NR + 1 rows and
          M = N + SQRE >= N columns.

    N     (input) INTEGER
          On entry, N specifies the number of rows and columns
          in the matrix. N must be at least 0.

    NCVT  (input) INTEGER
          On entry, NCVT specifies the number of columns of
          the matrix VT. NCVT must be at least 0.

    NRU   (input) INTEGER
          On entry, NRU specifies the number of rows of
          the matrix U. NRU must be at least 0.

    NCC   (input) INTEGER
          On entry, NCC specifies the number of columns of
          the matrix C. NCC must be at least 0.

    D     (input/output) DOUBLE PRECISION array, dimension (N)
          On entry, D contains the diagonal entries of the
          bidiagonal matrix whose SVD is desired. On normal exit,
          D contains the singular values in ascending order.

    E     (input/output) DOUBLE PRECISION array.
          dimension is (N-1) if SQRE = 0 and N if SQRE = 1.
          On entry, the entries of E contain the offdiagonal entries
          of the bidiagonal matrix whose SVD is desired. On normal
          exit, E will contain 0. If the algorithm does not converge,
          D and E will contain the diagonal and superdiagonal entries
          of a bidiagonal matrix orthogonally equivalent to the one
          given as input.

    VT    (input/output) DOUBLE PRECISION array, dimension (LDVT, NCVT)
          On entry, contains a matrix which on exit has been
          premultiplied by P', dimension N-by-NCVT if SQRE = 0
          and (N+1)-by-NCVT if SQRE = 1 (not referenced if NCVT=0).

    LDVT  (input) INTEGER
          On entry, LDVT specifies the leading dimension of VT as
          declared in the calling (sub) program. LDVT must be at
          least 1. If NCVT is nonzero LDVT must also be at least N.

    U     (input/output) DOUBLE PRECISION array, dimension (LDU, N)
          On entry, contains a  matrix which on exit has been
          postmultiplied by Q, dimension NRU-by-N if SQRE = 0
          and NRU-by-(N+1) if SQRE = 1 (not referenced if NRU=0).

    LDU   (input) INTEGER
          On entry, LDU  specifies the leading dimension of U as
          declared in the calling (sub) program. LDU must be at
          least max( 1, NRU ) .

    C     (input/output) DOUBLE PRECISION array, dimension (LDC, NCC)
          On entry, contains an N-by-NCC matrix which on exit
          has been premultiplied by Q'  dimension N-by-NCC if SQRE = 0
          and (N+1)-by-NCC if SQRE = 1 (not referenced if NCC=0).

    LDC   (input) INTEGER
          On entry, LDC  specifies the leading dimension of C as
          declared in the calling (sub) program. LDC must be at
          least 1. If NCC is nonzero, LDC must also be at least N.

    WORK  (workspace) DOUBLE PRECISION array, dimension (4*N)
          Workspace. Only referenced if one of NCVT, NRU, or NCC is
          nonzero, and if N is at least 2.

    INFO  (output) INTEGER
          On exit, a value of 0 indicates a successful exit.
          If INFO < 0, argument number -INFO is illegal.
          If INFO > 0, the algorithm did not converge, and INFO
          specifies how many superdiagonals did not converge.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    iuplo = 0;
    if (lsame_(uplo, "U")) {
	iuplo = 1;
    }
    if (lsame_(uplo, "L")) {
	iuplo = 2;
    }
    if (iuplo == 0) {
	*info = -1;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ncvt < 0) {
	*info = -4;
    } else if (*nru < 0) {
	*info = -5;
    } else if (*ncc < 0) {
	*info = -6;
    } else if ((*ncvt == 0 && *ldvt < 1) || (*ncvt > 0 && *ldvt < max(1,*n)))
	    {
	*info = -10;
    } else if (*ldu < max(1,*nru)) {
	*info = -12;
    } else if ((*ncc == 0 && *ldc < 1) || (*ncc > 0 && *ldc < max(1,*n))) {
	*info = -14;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASDQ", &i__1);
	return 0;
    }
    if (*n == 0) {
	return 0;
    }

/*     ROTATE is true if any singular vectors desired, false otherwise */

    rotate = *ncvt > 0 || *nru > 0 || *ncc > 0;
    np1 = *n + 1;
    sqre1 = *sqre;

/*
       If matrix non-square upper bidiagonal, rotate to be lower
       bidiagonal.  The rotations are on the right.
*/

    if ((iuplo == 1 && sqre1 == 1)) {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    if (rotate) {
		work[i__] = cs;
		work[*n + i__] = sn;
	    }
/* L10: */
	}
	dlartg_(&d__[*n], &e[*n], &cs, &sn, &r__);
	d__[*n] = r__;
	e[*n] = 0.;
	if (rotate) {
	    work[*n] = cs;
	    work[*n + *n] = sn;
	}
	iuplo = 2;
	sqre1 = 0;

/*        Update singular vectors if desired. */

	if (*ncvt > 0) {
	    dlasr_("L", "V", "F", &np1, ncvt, &work[1], &work[np1], &vt[
		    vt_offset], ldvt);
	}
    }

/*
       If matrix lower bidiagonal, rotate to be upper bidiagonal
       by applying Givens rotations on the left.
*/

    if (iuplo == 2) {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    if (rotate) {
		work[i__] = cs;
		work[*n + i__] = sn;
	    }
/* L20: */
	}

/*
          If matrix (N+1)-by-N lower bidiagonal, one additional
          rotation is needed.
*/

	if (sqre1 == 1) {
	    dlartg_(&d__[*n], &e[*n], &cs, &sn, &r__);
	    d__[*n] = r__;
	    if (rotate) {
		work[*n] = cs;
		work[*n + *n] = sn;
	    }
	}

/*        Update singular vectors if desired. */

	if (*nru > 0) {
	    if (sqre1 == 0) {
		dlasr_("R", "V", "F", nru, n, &work[1], &work[np1], &u[
			u_offset], ldu);
	    } else {
		dlasr_("R", "V", "F", nru, &np1, &work[1], &work[np1], &u[
			u_offset], ldu);
	    }
	}
	if (*ncc > 0) {
	    if (sqre1 == 0) {
		dlasr_("L", "V", "F", n, ncc, &work[1], &work[np1], &c__[
			c_offset], ldc);
	    } else {
		dlasr_("L", "V", "F", &np1, ncc, &work[1], &work[np1], &c__[
			c_offset], ldc);
	    }
	}
    }

/*
       Call DBDSQR to compute the SVD of the reduced real
       N-by-N upper bidiagonal matrix.
*/

    dbdsqr_("U", n, ncvt, nru, ncc, &d__[1], &e[1], &vt[vt_offset], ldvt, &u[
	    u_offset], ldu, &c__[c_offset], ldc, &work[1], info);

/*
       Sort the singular values into ascending order (insertion sort on
       singular values, but only one transposition per singular vector)
*/

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Scan for smallest D(I). */

	isub = i__;
	smin = d__[i__];
	i__2 = *n;
	for (j = i__ + 1; j <= i__2; ++j) {
	    if (d__[j] < smin) {
		isub = j;
		smin = d__[j];
	    }
/* L30: */
	}
	if (isub != i__) {

/*           Swap singular values and vectors. */

	    d__[isub] = d__[i__];
	    d__[i__] = smin;
	    if (*ncvt > 0) {
		dswap_(ncvt, &vt[isub + vt_dim1], ldvt, &vt[i__ + vt_dim1],
			ldvt);
	    }
	    if (*nru > 0) {
		dswap_(nru, &u[isub * u_dim1 + 1], &c__1, &u[i__ * u_dim1 + 1]
			, &c__1);
	    }
	    if (*ncc > 0) {
		dswap_(ncc, &c__[isub + c_dim1], ldc, &c__[i__ + c_dim1], ldc)
			;
	    }
	}
/* L40: */
    }

    return 0;

/*     End of DLASDQ */

} /* dlasdq_ */

/* Subroutine */ int dlasdt_(integer *n, integer *lvl, integer *nd, integer *
	inode, integer *ndiml, integer *ndimr, integer *msub)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double log(doublereal);

    /* Local variables */
    static integer i__, il, ir, maxn;
    static doublereal temp;
    static integer nlvl, llst, ncrnt;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLASDT creates a tree of subproblems for bidiagonal divide and
    conquer.

    Arguments
    =========

     N      (input) INTEGER
            On entry, the number of diagonal elements of the
            bidiagonal matrix.

     LVL    (output) INTEGER
            On exit, the number of levels on the computation tree.

     ND     (output) INTEGER
            On exit, the number of nodes on the tree.

     INODE  (output) INTEGER array, dimension ( N )
            On exit, centers of subproblems.

     NDIML  (output) INTEGER array, dimension ( N )
            On exit, row dimensions of left children.

     NDIMR  (output) INTEGER array, dimension ( N )
            On exit, row dimensions of right children.

     MSUB   (input) INTEGER.
            On entry, the maximum row dimension each subproblem at the
            bottom of the tree can be of.

    Further Details
    ===============

    Based on contributions by
       Ming Gu and Huan Ren, Computer Science Division, University of
       California at Berkeley, USA

    =====================================================================


       Find the number of levels on the tree.
*/

    /* Parameter adjustments */
    --ndimr;
    --ndiml;
    --inode;

    /* Function Body */
    maxn = max(1,*n);
    temp = log((doublereal) maxn / (doublereal) (*msub + 1)) / log(2.);
    *lvl = (integer) temp + 1;

    i__ = *n / 2;
    inode[1] = i__ + 1;
    ndiml[1] = i__;
    ndimr[1] = *n - i__ - 1;
    il = 0;
    ir = 1;
    llst = 1;
    i__1 = *lvl - 1;
    for (nlvl = 1; nlvl <= i__1; ++nlvl) {

/*
          Constructing the tree at (NLVL+1)-st level. The number of
          nodes created on this level is LLST * 2.
*/

	i__2 = llst - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    il += 2;
	    ir += 2;
	    ncrnt = llst + i__;
	    ndiml[il] = ndiml[ncrnt] / 2;
	    ndimr[il] = ndiml[ncrnt] - ndiml[il] - 1;
	    inode[il] = inode[ncrnt] - ndimr[il] - 1;
	    ndiml[ir] = ndimr[ncrnt] / 2;
	    ndimr[ir] = ndimr[ncrnt] - ndiml[ir] - 1;
	    inode[ir] = inode[ncrnt] + ndiml[ir] + 1;
/* L10: */
	}
	llst <<= 1;
/* L20: */
    }
    *nd = ((llst) << (1)) - 1;

    return 0;

/*     End of DLASDT */

} /* dlasdt_ */

/* Subroutine */ int dlaset_(char *uplo, integer *m, integer *n, doublereal *
	alpha, doublereal *beta, doublereal *a, integer *lda)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j;
    extern logical lsame_(char *, char *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLASET initializes an m-by-n matrix A to BETA on the diagonal and
    ALPHA on the offdiagonals.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            Specifies the part of the matrix A to be set.
            = 'U':      Upper triangular part is set; the strictly lower
                        triangular part of A is not changed.
            = 'L':      Lower triangular part is set; the strictly upper
                        triangular part of A is not changed.
            Otherwise:  All of the matrix A is set.

    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    ALPHA   (input) DOUBLE PRECISION
            The constant to which the offdiagonal elements are to be set.

    BETA    (input) DOUBLE PRECISION
            The constant to which the diagonal elements are to be set.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On exit, the leading m-by-n submatrix of A is set as follows:

            if UPLO = 'U', A(i,j) = ALPHA, 1<=i<=j-1, 1<=j<=n,
            if UPLO = 'L', A(i,j) = ALPHA, j+1<=i<=m, 1<=j<=n,
            otherwise,     A(i,j) = ALPHA, 1<=i<=m, 1<=j<=n, i.ne.j,

            and, for all UPLO, A(i,i) = BETA, 1<=i<=min(m,n).

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

   =====================================================================
*/


    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    if (lsame_(uplo, "U")) {

/*
          Set the strictly upper triangular or trapezoidal part of the
          array to ALPHA.
*/

	i__1 = *n;
	for (j = 2; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = j - 1;
	    i__2 = min(i__3,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = *alpha;
/* L10: */
	    }
/* L20: */
	}

    } else if (lsame_(uplo, "L")) {

/*
          Set the strictly lower triangular or trapezoidal part of the
          array to ALPHA.
*/

	i__1 = min(*m,*n);
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = *alpha;
/* L30: */
	    }
/* L40: */
	}

    } else {

/*        Set the leading m-by-n submatrix to ALPHA. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = *alpha;
/* L50: */
	    }
/* L60: */
	}
    }

/*     Set the first min(M,N) diagonal elements to BETA. */

    i__1 = min(*m,*n);
    for (i__ = 1; i__ <= i__1; ++i__) {
	a[i__ + i__ * a_dim1] = *beta;
/* L70: */
    }

    return 0;

/*     End of DLASET */

} /* dlaset_ */

/* Subroutine */ int dlasq1_(integer *n, doublereal *d__, doublereal *e,
	doublereal *work, integer *info)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static doublereal eps;
    extern /* Subroutine */ int dlas2_(doublereal *, doublereal *, doublereal
	    *, doublereal *, doublereal *);
    static doublereal scale;
    static integer iinfo;
    static doublereal sigmn;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static doublereal sigmx;
    extern /* Subroutine */ int dlasq2_(integer *, doublereal *, integer *);

    extern /* Subroutine */ int dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *);
    static doublereal safmin;
    extern /* Subroutine */ int xerbla_(char *, integer *), dlasrt_(
	    char *, integer *, doublereal *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DLASQ1 computes the singular values of a real N-by-N bidiagonal
    matrix with diagonal D and off-diagonal E. The singular values
    are computed to high relative accuracy, in the absence of
    denormalization, underflow and overflow. The algorithm was first
    presented in

    "Accurate singular values and differential qd algorithms" by K. V.
    Fernando and B. N. Parlett, Numer. Math., Vol-67, No. 2, pp. 191-230,
    1994,

    and the present implementation is described in "An implementation of
    the dqds Algorithm (Positive Case)", LAPACK Working Note.

    Arguments
    =========

    N     (input) INTEGER
          The number of rows and columns in the matrix. N >= 0.

    D     (input/output) DOUBLE PRECISION array, dimension (N)
          On entry, D contains the diagonal elements of the
          bidiagonal matrix whose SVD is desired. On normal exit,
          D contains the singular values in decreasing order.

    E     (input/output) DOUBLE PRECISION array, dimension (N)
          On entry, elements E(1:N-1) contain the off-diagonal elements
          of the bidiagonal matrix whose SVD is desired.
          On exit, E is overwritten.

    WORK  (workspace) DOUBLE PRECISION array, dimension (4*N)

    INFO  (output) INTEGER
          = 0: successful exit
          < 0: if INFO = -i, the i-th argument had an illegal value
          > 0: the algorithm failed
               = 1, a split was marked by a positive value in E
               = 2, current block of Z not diagonalized after 30*N
                    iterations (in inner while loop)
               = 3, termination criterion of outer while loop not met
                    (program created more than N unreduced blocks)

    =====================================================================
*/


    /* Parameter adjustments */
    --work;
    --e;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -2;
	i__1 = -(*info);
	xerbla_("DLASQ1", &i__1);
	return 0;
    } else if (*n == 0) {
	return 0;
    } else if (*n == 1) {
	d__[1] = abs(d__[1]);
	return 0;
    } else if (*n == 2) {
	dlas2_(&d__[1], &e[1], &d__[2], &sigmn, &sigmx);
	d__[1] = sigmx;
	d__[2] = sigmn;
	return 0;
    }

/*     Estimate the largest singular value. */

    sigmx = 0.;
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__[i__] = (d__1 = d__[i__], abs(d__1));
/* Computing MAX */
	d__2 = sigmx, d__3 = (d__1 = e[i__], abs(d__1));
	sigmx = max(d__2,d__3);
/* L10: */
    }
    d__[*n] = (d__1 = d__[*n], abs(d__1));

/*     Early return if SIGMX is zero (matrix is already diagonal). */

    if (sigmx == 0.) {
	dlasrt_("D", n, &d__[1], &iinfo);
	return 0;
    }

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__1 = sigmx, d__2 = d__[i__];
	sigmx = max(d__1,d__2);
/* L20: */
    }

/*
       Copy D and E into WORK (in the Z format) and scale (squaring the
       input data makes scaling by a power of the radix pointless).
*/

    eps = PRECISION;
    safmin = SAFEMINIMUM;
    scale = sqrt(eps / safmin);
    dcopy_(n, &d__[1], &c__1, &work[1], &c__2);
    i__1 = *n - 1;
    dcopy_(&i__1, &e[1], &c__1, &work[2], &c__2);
    i__1 = ((*n) << (1)) - 1;
    i__2 = ((*n) << (1)) - 1;
    dlascl_("G", &c__0, &c__0, &sigmx, &scale, &i__1, &c__1, &work[1], &i__2,
	    &iinfo);

/*     Compute the q's and e's. */

    i__1 = ((*n) << (1)) - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	d__1 = work[i__];
	work[i__] = d__1 * d__1;
/* L30: */
    }
    work[*n * 2] = 0.;

    dlasq2_(n, &work[1], info);

    if (*info == 0) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__[i__] = sqrt(work[i__]);
/* L40: */
	}
	dlascl_("G", &c__0, &c__0, &scale, &sigmx, n, &c__1, &d__[1], n, &
		iinfo);
    }

    return 0;

/*     End of DLASQ1 */

} /* dlasq1_ */

/* Subroutine */ int dlasq2_(integer *n, doublereal *z__, integer *info)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal d__, e;
    static integer k;
    static doublereal s, t;
    static integer i0, i4, n0, pp;
    static doublereal eps, tol;
    static integer ipn4;
    static doublereal tol2;
    static logical ieee;
    static integer nbig;
    static doublereal dmin__, emin, emax;
    static integer ndiv, iter;
    static doublereal qmin, temp, qmax, zmax;
    static integer splt, nfail;
    static doublereal desig, trace, sigma;
    static integer iinfo;
    extern /* Subroutine */ int dlasq3_(integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     integer *, integer *, integer *, logical *);

    static integer iwhila, iwhilb;
    static doublereal oldemn, safmin;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int dlasrt_(char *, integer *, doublereal *,
	    integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DLASQ2 computes all the eigenvalues of the symmetric positive
    definite tridiagonal matrix associated with the qd array Z to high
    relative accuracy are computed to high relative accuracy, in the
    absence of denormalization, underflow and overflow.

    To see the relation of Z to the tridiagonal matrix, let L be a
    unit lower bidiagonal matrix with subdiagonals Z(2,4,6,,..) and
    let U be an upper bidiagonal matrix with 1's above and diagonal
    Z(1,3,5,,..). The tridiagonal is L*U or, if you prefer, the
    symmetric tridiagonal to which it is similar.

    Note : DLASQ2 defines a logical variable, IEEE, which is true
    on machines which follow ieee-754 floating-point standard in their
    handling of infinities and NaNs, and false otherwise. This variable
    is passed to DLASQ3.

    Arguments
    =========

    N     (input) INTEGER
          The number of rows and columns in the matrix. N >= 0.

    Z     (workspace) DOUBLE PRECISION array, dimension ( 4*N )
          On entry Z holds the qd array. On exit, entries 1 to N hold
          the eigenvalues in decreasing order, Z( 2*N+1 ) holds the
          trace, and Z( 2*N+2 ) holds the sum of the eigenvalues. If
          N > 2, then Z( 2*N+3 ) holds the iteration count, Z( 2*N+4 )
          holds NDIVS/NIN^2, and Z( 2*N+5 ) holds the percentage of
          shifts that failed.

    INFO  (output) INTEGER
          = 0: successful exit
          < 0: if the i-th argument is a scalar and had an illegal
               value, then INFO = -i, if the i-th argument is an
               array and the j-entry had an illegal value, then
               INFO = -(i*100+j)
          > 0: the algorithm failed
                = 1, a split was marked by a positive value in E
                = 2, current block of Z not diagonalized after 30*N
                     iterations (in inner while loop)
                = 3, termination criterion of outer while loop not met
                     (program created more than N unreduced blocks)

    Further Details
    ===============
    Local Variables: I0:N0 defines a current unreduced segment of Z.
    The shifts are accumulated in SIGMA. Iteration count is in ITER.
    Ping-pong is controlled by PP (alternates between 0 and 1).

    =====================================================================


       Test the input arguments.
       (in case DLASQ2 is not called by DLASQ1)
*/

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    *info = 0;
    eps = PRECISION;
    safmin = SAFEMINIMUM;
    tol = eps * 100.;
/* Computing 2nd power */
    d__1 = tol;
    tol2 = d__1 * d__1;

    if (*n < 0) {
	*info = -1;
	xerbla_("DLASQ2", &c__1);
	return 0;
    } else if (*n == 0) {
	return 0;
    } else if (*n == 1) {

/*        1-by-1 case. */

	if (z__[1] < 0.) {
	    *info = -201;
	    xerbla_("DLASQ2", &c__2);
	}
	return 0;
    } else if (*n == 2) {

/*        2-by-2 case. */

	if (z__[2] < 0. || z__[3] < 0.) {
	    *info = -2;
	    xerbla_("DLASQ2", &c__2);
	    return 0;
	} else if (z__[3] > z__[1]) {
	    d__ = z__[3];
	    z__[3] = z__[1];
	    z__[1] = d__;
	}
	z__[5] = z__[1] + z__[2] + z__[3];
	if (z__[2] > z__[3] * tol2) {
	    t = (z__[1] - z__[3] + z__[2]) * .5;
	    s = z__[3] * (z__[2] / t);
	    if (s <= t) {
		s = z__[3] * (z__[2] / (t * (sqrt(s / t + 1.) + 1.)));
	    } else {
		s = z__[3] * (z__[2] / (t + sqrt(t) * sqrt(t + s)));
	    }
	    t = z__[1] + (s + z__[2]);
	    z__[3] *= z__[1] / t;
	    z__[1] = t;
	}
	z__[2] = z__[3];
	z__[6] = z__[2] + z__[1];
	return 0;
    }

/*     Check for negative data and compute sums of q's and e's. */

    z__[*n * 2] = 0.;
    emin = z__[2];
    qmax = 0.;
    zmax = 0.;
    d__ = 0.;
    e = 0.;

    i__1 = (*n - 1) << (1);
    for (k = 1; k <= i__1; k += 2) {
	if (z__[k] < 0.) {
	    *info = -(k + 200);
	    xerbla_("DLASQ2", &c__2);
	    return 0;
	} else if (z__[k + 1] < 0.) {
	    *info = -(k + 201);
	    xerbla_("DLASQ2", &c__2);
	    return 0;
	}
	d__ += z__[k];
	e += z__[k + 1];
/* Computing MAX */
	d__1 = qmax, d__2 = z__[k];
	qmax = max(d__1,d__2);
/* Computing MIN */
	d__1 = emin, d__2 = z__[k + 1];
	emin = min(d__1,d__2);
/* Computing MAX */
	d__1 = max(qmax,zmax), d__2 = z__[k + 1];
	zmax = max(d__1,d__2);
/* L10: */
    }
    if (z__[((*n) << (1)) - 1] < 0.) {
	*info = -(((*n) << (1)) + 199);
	xerbla_("DLASQ2", &c__2);
	return 0;
    }
    d__ += z__[((*n) << (1)) - 1];
/* Computing MAX */
    d__1 = qmax, d__2 = z__[((*n) << (1)) - 1];
    qmax = max(d__1,d__2);
    zmax = max(qmax,zmax);

/*     Check for diagonality. */

    if (e == 0.) {
	i__1 = *n;
	for (k = 2; k <= i__1; ++k) {
	    z__[k] = z__[((k) << (1)) - 1];
/* L20: */
	}
	dlasrt_("D", n, &z__[1], &iinfo);
	z__[((*n) << (1)) - 1] = d__;
	return 0;
    }

    trace = d__ + e;

/*     Check for zero data. */

    if (trace == 0.) {
	z__[((*n) << (1)) - 1] = 0.;
	return 0;
    }

/*     Check whether the machine is IEEE conformable. */

    ieee = (ilaenv_(&c__10, "DLASQ2", "N", &c__1, &c__2, &c__3, &c__4, (
	    ftnlen)6, (ftnlen)1) == 1 && ilaenv_(&c__11, "DLASQ2", "N", &c__1,
	     &c__2, &c__3, &c__4, (ftnlen)6, (ftnlen)1) == 1);

/*     Rearrange data for locality: Z=(q1,qq1,e1,ee1,q2,qq2,e2,ee2,...). */

    for (k = (*n) << (1); k >= 2; k += -2) {
	z__[k * 2] = 0.;
	z__[((k) << (1)) - 1] = z__[k];
	z__[((k) << (1)) - 2] = 0.;
	z__[((k) << (1)) - 3] = z__[k - 1];
/* L30: */
    }

    i0 = 1;
    n0 = *n;

/*     Reverse the qd-array, if warranted. */

    if (z__[((i0) << (2)) - 3] * 1.5 < z__[((n0) << (2)) - 3]) {
	ipn4 = (i0 + n0) << (2);
	i__1 = (i0 + n0 - 1) << (1);
	for (i4 = (i0) << (2); i4 <= i__1; i4 += 4) {
	    temp = z__[i4 - 3];
	    z__[i4 - 3] = z__[ipn4 - i4 - 3];
	    z__[ipn4 - i4 - 3] = temp;
	    temp = z__[i4 - 1];
	    z__[i4 - 1] = z__[ipn4 - i4 - 5];
	    z__[ipn4 - i4 - 5] = temp;
/* L40: */
	}
    }

/*     Initial split checking via dqd and Li's test. */

    pp = 0;

    for (k = 1; k <= 2; ++k) {

	d__ = z__[((n0) << (2)) + pp - 3];
	i__1 = ((i0) << (2)) + pp;
	for (i4 = ((n0 - 1) << (2)) + pp; i4 >= i__1; i4 += -4) {
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = -0.;
		d__ = z__[i4 - 3];
	    } else {
		d__ = z__[i4 - 3] * (d__ / (d__ + z__[i4 - 1]));
	    }
/* L50: */
	}

/*        dqd maps Z to ZZ plus Li's test. */

	emin = z__[((i0) << (2)) + pp + 1];
	d__ = z__[((i0) << (2)) + pp - 3];
	i__1 = ((n0 - 1) << (2)) + pp;
	for (i4 = ((i0) << (2)) + pp; i4 <= i__1; i4 += 4) {
	    z__[i4 - ((pp) << (1)) - 2] = d__ + z__[i4 - 1];
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = -0.;
		z__[i4 - ((pp) << (1)) - 2] = d__;
		z__[i4 - ((pp) << (1))] = 0.;
		d__ = z__[i4 + 1];
	    } else if ((safmin * z__[i4 + 1] < z__[i4 - ((pp) << (1)) - 2] &&
		    safmin * z__[i4 - ((pp) << (1)) - 2] < z__[i4 + 1])) {
		temp = z__[i4 + 1] / z__[i4 - ((pp) << (1)) - 2];
		z__[i4 - ((pp) << (1))] = z__[i4 - 1] * temp;
		d__ *= temp;
	    } else {
		z__[i4 - ((pp) << (1))] = z__[i4 + 1] * (z__[i4 - 1] / z__[i4
			- ((pp) << (1)) - 2]);
		d__ = z__[i4 + 1] * (d__ / z__[i4 - ((pp) << (1)) - 2]);
	    }
/* Computing MIN */
	    d__1 = emin, d__2 = z__[i4 - ((pp) << (1))];
	    emin = min(d__1,d__2);
/* L60: */
	}
	z__[((n0) << (2)) - pp - 2] = d__;

/*        Now find qmax. */

	qmax = z__[((i0) << (2)) - pp - 2];
	i__1 = ((n0) << (2)) - pp - 2;
	for (i4 = ((i0) << (2)) - pp + 2; i4 <= i__1; i4 += 4) {
/* Computing MAX */
	    d__1 = qmax, d__2 = z__[i4];
	    qmax = max(d__1,d__2);
/* L70: */
	}

/*        Prepare for the next iteration on K. */

	pp = 1 - pp;
/* L80: */
    }

    iter = 2;
    nfail = 0;
    ndiv = (n0 - i0) << (1);

    i__1 = *n + 1;
    for (iwhila = 1; iwhila <= i__1; ++iwhila) {
	if (n0 < 1) {
	    goto L150;
	}

/*
          While array unfinished do

          E(N0) holds the value of SIGMA when submatrix in I0:N0
          splits from the rest of the array, but is negated.
*/

	desig = 0.;
	if (n0 == *n) {
	    sigma = 0.;
	} else {
	    sigma = -z__[((n0) << (2)) - 1];
	}
	if (sigma < 0.) {
	    *info = 1;
	    return 0;
	}

/*
          Find last unreduced submatrix's top index I0, find QMAX and
          EMIN. Find Gershgorin-type bound if Q's much greater than E's.
*/

	emax = 0.;
	if (n0 > i0) {
	    emin = (d__1 = z__[((n0) << (2)) - 5], abs(d__1));
	} else {
	    emin = 0.;
	}
	qmin = z__[((n0) << (2)) - 3];
	qmax = qmin;
	for (i4 = (n0) << (2); i4 >= 8; i4 += -4) {
	    if (z__[i4 - 5] <= 0.) {
		goto L100;
	    }
	    if (qmin >= emax * 4.) {
/* Computing MIN */
		d__1 = qmin, d__2 = z__[i4 - 3];
		qmin = min(d__1,d__2);
/* Computing MAX */
		d__1 = emax, d__2 = z__[i4 - 5];
		emax = max(d__1,d__2);
	    }
/* Computing MAX */
	    d__1 = qmax, d__2 = z__[i4 - 7] + z__[i4 - 5];
	    qmax = max(d__1,d__2);
/* Computing MIN */
	    d__1 = emin, d__2 = z__[i4 - 5];
	    emin = min(d__1,d__2);
/* L90: */
	}
	i4 = 4;

L100:
	i0 = i4 / 4;

/*        Store EMIN for passing to DLASQ3. */

	z__[((n0) << (2)) - 1] = emin;

/*
          Put -(initial shift) into DMIN.

   Computing MAX
*/
	d__1 = 0., d__2 = qmin - sqrt(qmin) * 2. * sqrt(emax);
	dmin__ = -max(d__1,d__2);

/*        Now I0:N0 is unreduced. PP = 0 for ping, PP = 1 for pong. */

	pp = 0;

	nbig = (n0 - i0 + 1) * 30;
	i__2 = nbig;
	for (iwhilb = 1; iwhilb <= i__2; ++iwhilb) {
	    if (i0 > n0) {
		goto L130;
	    }

/*           While submatrix unfinished take a good dqds step. */

	    dlasq3_(&i0, &n0, &z__[1], &pp, &dmin__, &sigma, &desig, &qmax, &
		    nfail, &iter, &ndiv, &ieee);

	    pp = 1 - pp;

/*           When EMIN is very small check for splits. */

	    if ((pp == 0 && n0 - i0 >= 3)) {
		if (z__[n0 * 4] <= tol2 * qmax || z__[((n0) << (2)) - 1] <=
			tol2 * sigma) {
		    splt = i0 - 1;
		    qmax = z__[((i0) << (2)) - 3];
		    emin = z__[((i0) << (2)) - 1];
		    oldemn = z__[i0 * 4];
		    i__3 = (n0 - 3) << (2);
		    for (i4 = (i0) << (2); i4 <= i__3; i4 += 4) {
			if (z__[i4] <= tol2 * z__[i4 - 3] || z__[i4 - 1] <=
				tol2 * sigma) {
			    z__[i4 - 1] = -sigma;
			    splt = i4 / 4;
			    qmax = 0.;
			    emin = z__[i4 + 3];
			    oldemn = z__[i4 + 4];
			} else {
/* Computing MAX */
			    d__1 = qmax, d__2 = z__[i4 + 1];
			    qmax = max(d__1,d__2);
/* Computing MIN */
			    d__1 = emin, d__2 = z__[i4 - 1];
			    emin = min(d__1,d__2);
/* Computing MIN */
			    d__1 = oldemn, d__2 = z__[i4];
			    oldemn = min(d__1,d__2);
			}
/* L110: */
		    }
		    z__[((n0) << (2)) - 1] = emin;
		    z__[n0 * 4] = oldemn;
		    i0 = splt + 1;
		}
	    }

/* L120: */
	}

	*info = 2;
	return 0;

/*        end IWHILB */

L130:

/* L140: */
	;
    }

    *info = 3;
    return 0;

/*     end IWHILA */

L150:

/*     Move q's to the front. */

    i__1 = *n;
    for (k = 2; k <= i__1; ++k) {
	z__[k] = z__[((k) << (2)) - 3];
/* L160: */
    }

/*     Sort and compute sum of eigenvalues. */

    dlasrt_("D", n, &z__[1], &iinfo);

    e = 0.;
    for (k = *n; k >= 1; --k) {
	e += z__[k];
/* L170: */
    }

/*     Store trace, sum(eigenvalues) and information on performance. */

    z__[((*n) << (1)) + 1] = trace;
    z__[((*n) << (1)) + 2] = e;
    z__[((*n) << (1)) + 3] = (doublereal) iter;
/* Computing 2nd power */
    i__1 = *n;
    z__[((*n) << (1)) + 4] = (doublereal) ndiv / (doublereal) (i__1 * i__1);
    z__[((*n) << (1)) + 5] = nfail * 100. / (doublereal) iter;
    return 0;

/*     End of DLASQ2 */

} /* dlasq2_ */

/* Subroutine */ int dlasq3_(integer *i0, integer *n0, doublereal *z__,
	integer *pp, doublereal *dmin__, doublereal *sigma, doublereal *desig,
	 doublereal *qmax, integer *nfail, integer *iter, integer *ndiv,
	logical *ieee)
{
    /* Initialized data */

    static integer ttype = 0;
    static doublereal dmin1 = 0.;
    static doublereal dmin2 = 0.;
    static doublereal dn = 0.;
    static doublereal dn1 = 0.;
    static doublereal dn2 = 0.;
    static doublereal tau = 0.;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal s, t;
    static integer j4, nn;
    static doublereal eps, tol;
    static integer n0in, ipn4;
    static doublereal tol2, temp;
    extern /* Subroutine */ int dlasq4_(integer *, integer *, doublereal *,
	    integer *, integer *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *)
	    , dlasq5_(integer *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, logical *), dlasq6_(
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *);

    static doublereal safmin;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       May 17, 2000


    Purpose
    =======

    DLASQ3 checks for deflation, computes a shift (TAU) and calls dqds.
    In case of failure it changes shifts, and tries again until output
    is positive.

    Arguments
    =========

    I0     (input) INTEGER
           First index.

    N0     (input) INTEGER
           Last index.

    Z      (input) DOUBLE PRECISION array, dimension ( 4*N )
           Z holds the qd array.

    PP     (input) INTEGER
           PP=0 for ping, PP=1 for pong.

    DMIN   (output) DOUBLE PRECISION
           Minimum value of d.

    SIGMA  (output) DOUBLE PRECISION
           Sum of shifts used in current segment.

    DESIG  (input/output) DOUBLE PRECISION
           Lower order part of SIGMA

    QMAX   (input) DOUBLE PRECISION
           Maximum value of q.

    NFAIL  (output) INTEGER
           Number of times shift was too big.

    ITER   (output) INTEGER
           Number of iterations.

    NDIV   (output) INTEGER
           Number of divisions.

    TTYPE  (output) INTEGER
           Shift type.

    IEEE   (input) LOGICAL
           Flag for IEEE or non IEEE arithmetic (passed to DLASQ5).

    =====================================================================
*/

    /* Parameter adjustments */
    --z__;

    /* Function Body */

    n0in = *n0;
    eps = PRECISION;
    safmin = SAFEMINIMUM;
    tol = eps * 100.;
/* Computing 2nd power */
    d__1 = tol;
    tol2 = d__1 * d__1;

/*     Check for deflation. */

L10:

    if (*n0 < *i0) {
	return 0;
    }
    if (*n0 == *i0) {
	goto L20;
    }
    nn = ((*n0) << (2)) + *pp;
    if (*n0 == *i0 + 1) {
	goto L40;
    }

/*     Check whether E(N0-1) is negligible, 1 eigenvalue. */

    if ((z__[nn - 5] > tol2 * (*sigma + z__[nn - 3]) && z__[nn - ((*pp) << (1)
	    ) - 4] > tol2 * z__[nn - 7])) {
	goto L30;
    }

L20:

    z__[((*n0) << (2)) - 3] = z__[((*n0) << (2)) + *pp - 3] + *sigma;
    --(*n0);
    goto L10;

/*     Check  whether E(N0-2) is negligible, 2 eigenvalues. */

L30:

    if ((z__[nn - 9] > tol2 * *sigma && z__[nn - ((*pp) << (1)) - 8] > tol2 *
	    z__[nn - 11])) {
	goto L50;
    }

L40:

    if (z__[nn - 3] > z__[nn - 7]) {
	s = z__[nn - 3];
	z__[nn - 3] = z__[nn - 7];
	z__[nn - 7] = s;
    }
    if (z__[nn - 5] > z__[nn - 3] * tol2) {
	t = (z__[nn - 7] - z__[nn - 3] + z__[nn - 5]) * .5;
	s = z__[nn - 3] * (z__[nn - 5] / t);
	if (s <= t) {
	    s = z__[nn - 3] * (z__[nn - 5] / (t * (sqrt(s / t + 1.) + 1.)));
	} else {
	    s = z__[nn - 3] * (z__[nn - 5] / (t + sqrt(t) * sqrt(t + s)));
	}
	t = z__[nn - 7] + (s + z__[nn - 5]);
	z__[nn - 3] *= z__[nn - 7] / t;
	z__[nn - 7] = t;
    }
    z__[((*n0) << (2)) - 7] = z__[nn - 7] + *sigma;
    z__[((*n0) << (2)) - 3] = z__[nn - 3] + *sigma;
    *n0 += -2;
    goto L10;

L50:

/*     Reverse the qd-array, if warranted. */

    if (*dmin__ <= 0. || *n0 < n0in) {
	if (z__[((*i0) << (2)) + *pp - 3] * 1.5 < z__[((*n0) << (2)) + *pp -
		3]) {
	    ipn4 = (*i0 + *n0) << (2);
	    i__1 = (*i0 + *n0 - 1) << (1);
	    for (j4 = (*i0) << (2); j4 <= i__1; j4 += 4) {
		temp = z__[j4 - 3];
		z__[j4 - 3] = z__[ipn4 - j4 - 3];
		z__[ipn4 - j4 - 3] = temp;
		temp = z__[j4 - 2];
		z__[j4 - 2] = z__[ipn4 - j4 - 2];
		z__[ipn4 - j4 - 2] = temp;
		temp = z__[j4 - 1];
		z__[j4 - 1] = z__[ipn4 - j4 - 5];
		z__[ipn4 - j4 - 5] = temp;
		temp = z__[j4];
		z__[j4] = z__[ipn4 - j4 - 4];
		z__[ipn4 - j4 - 4] = temp;
/* L60: */
	    }
	    if (*n0 - *i0 <= 4) {
		z__[((*n0) << (2)) + *pp - 1] = z__[((*i0) << (2)) + *pp - 1];
		z__[((*n0) << (2)) - *pp] = z__[((*i0) << (2)) - *pp];
	    }
/* Computing MIN */
	    d__1 = dmin2, d__2 = z__[((*n0) << (2)) + *pp - 1];
	    dmin2 = min(d__1,d__2);
/* Computing MIN */
	    d__1 = z__[((*n0) << (2)) + *pp - 1], d__2 = z__[((*i0) << (2)) +
		    *pp - 1], d__1 = min(d__1,d__2), d__2 = z__[((*i0) << (2))
		     + *pp + 3];
	    z__[((*n0) << (2)) + *pp - 1] = min(d__1,d__2);
/* Computing MIN */
	    d__1 = z__[((*n0) << (2)) - *pp], d__2 = z__[((*i0) << (2)) - *pp]
		    , d__1 = min(d__1,d__2), d__2 = z__[((*i0) << (2)) - *pp
		    + 4];
	    z__[((*n0) << (2)) - *pp] = min(d__1,d__2);
/* Computing MAX */
	    d__1 = *qmax, d__2 = z__[((*i0) << (2)) + *pp - 3], d__1 = max(
		    d__1,d__2), d__2 = z__[((*i0) << (2)) + *pp + 1];
	    *qmax = max(d__1,d__2);
	    *dmin__ = -0.;
	}
    }

/*
   L70:

   Computing MIN
*/
    d__1 = z__[((*n0) << (2)) + *pp - 1], d__2 = z__[((*n0) << (2)) + *pp - 9]
	    , d__1 = min(d__1,d__2), d__2 = dmin2 + z__[((*n0) << (2)) - *pp];
    if (*dmin__ < 0. || safmin * *qmax < min(d__1,d__2)) {

/*        Choose a shift. */

	dlasq4_(i0, n0, &z__[1], pp, &n0in, dmin__, &dmin1, &dmin2, &dn, &dn1,
		 &dn2, &tau, &ttype);

/*        Call dqds until DMIN > 0. */

L80:

	dlasq5_(i0, n0, &z__[1], pp, &tau, dmin__, &dmin1, &dmin2, &dn, &dn1,
		&dn2, ieee);

	*ndiv += *n0 - *i0 + 2;
	++(*iter);

/*        Check status. */

	if ((*dmin__ >= 0. && dmin1 > 0.)) {

/*           Success. */

	    goto L100;

	} else if ((((*dmin__ < 0. && dmin1 > 0.) && z__[((*n0 - 1) << (2)) -
		*pp] < tol * (*sigma + dn1)) && abs(dn) < tol * *sigma)) {

/*           Convergence hidden by negative DN. */

	    z__[((*n0 - 1) << (2)) - *pp + 2] = 0.;
	    *dmin__ = 0.;
	    goto L100;
	} else if (*dmin__ < 0.) {

/*           TAU too big. Select new TAU and try again. */

	    ++(*nfail);
	    if (ttype < -22) {

/*              Failed twice. Play it safe. */

		tau = 0.;
	    } else if (dmin1 > 0.) {

/*              Late failure. Gives excellent shift. */

		tau = (tau + *dmin__) * (1. - eps * 2.);
		ttype += -11;
	    } else {

/*              Early failure. Divide by 4. */

		tau *= .25;
		ttype += -12;
	    }
	    goto L80;
	} else if (*dmin__ != *dmin__) {

/*           NaN. */

	    tau = 0.;
	    goto L80;
	} else {

/*           Possible underflow. Play it safe. */

	    goto L90;
	}
    }

/*     Risk of underflow. */

L90:
    dlasq6_(i0, n0, &z__[1], pp, dmin__, &dmin1, &dmin2, &dn, &dn1, &dn2);
    *ndiv += *n0 - *i0 + 2;
    ++(*iter);
    tau = 0.;

L100:
    if (tau < *sigma) {
	*desig += tau;
	t = *sigma + *desig;
	*desig -= t - *sigma;
    } else {
	t = *sigma + tau;
	*desig = *sigma - (t - tau) + *desig;
    }
    *sigma = t;

    return 0;

/*     End of DLASQ3 */

} /* dlasq3_ */

/* Subroutine */ int dlasq4_(integer *i0, integer *n0, doublereal *z__,
	integer *pp, integer *n0in, doublereal *dmin__, doublereal *dmin1,
	doublereal *dmin2, doublereal *dn, doublereal *dn1, doublereal *dn2,
	doublereal *tau, integer *ttype)
{
    /* Initialized data */

    static doublereal g = 0.;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal s, a2, b1, b2;
    static integer i4, nn, np;
    static doublereal gam, gap1, gap2;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DLASQ4 computes an approximation TAU to the smallest eigenvalue
    using values of d from the previous transform.

    I0    (input) INTEGER
          First index.

    N0    (input) INTEGER
          Last index.

    Z     (input) DOUBLE PRECISION array, dimension ( 4*N )
          Z holds the qd array.

    PP    (input) INTEGER
          PP=0 for ping, PP=1 for pong.

    NOIN  (input) INTEGER
          The value of N0 at start of EIGTEST.

    DMIN  (input) DOUBLE PRECISION
          Minimum value of d.

    DMIN1 (input) DOUBLE PRECISION
          Minimum value of d, excluding D( N0 ).

    DMIN2 (input) DOUBLE PRECISION
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).

    DN    (input) DOUBLE PRECISION
          d(N)

    DN1   (input) DOUBLE PRECISION
          d(N-1)

    DN2   (input) DOUBLE PRECISION
          d(N-2)

    TAU   (output) DOUBLE PRECISION
          This is the shift.

    TTYPE (output) INTEGER
          Shift type.

    Further Details
    ===============
    CNST1 = 9/16

    =====================================================================
*/

    /* Parameter adjustments */
    --z__;

    /* Function Body */

/*
       A negative DMIN forces the shift to take that absolute value
       TTYPE records the type of shift.
*/

    if (*dmin__ <= 0.) {
	*tau = -(*dmin__);
	*ttype = -1;
	return 0;
    }

    nn = ((*n0) << (2)) + *pp;
    if (*n0in == *n0) {

/*        No eigenvalues deflated. */

	if (*dmin__ == *dn || *dmin__ == *dn1) {

	    b1 = sqrt(z__[nn - 3]) * sqrt(z__[nn - 5]);
	    b2 = sqrt(z__[nn - 7]) * sqrt(z__[nn - 9]);
	    a2 = z__[nn - 7] + z__[nn - 5];

/*           Cases 2 and 3. */

	    if ((*dmin__ == *dn && *dmin1 == *dn1)) {
		gap2 = *dmin2 - a2 - *dmin2 * .25;
		if ((gap2 > 0. && gap2 > b2)) {
		    gap1 = a2 - *dn - b2 / gap2 * b2;
		} else {
		    gap1 = a2 - *dn - (b1 + b2);
		}
		if ((gap1 > 0. && gap1 > b1)) {
/* Computing MAX */
		    d__1 = *dn - b1 / gap1 * b1, d__2 = *dmin__ * .5;
		    s = max(d__1,d__2);
		    *ttype = -2;
		} else {
		    s = 0.;
		    if (*dn > b1) {
			s = *dn - b1;
		    }
		    if (a2 > b1 + b2) {
/* Computing MIN */
			d__1 = s, d__2 = a2 - (b1 + b2);
			s = min(d__1,d__2);
		    }
/* Computing MAX */
		    d__1 = s, d__2 = *dmin__ * .333;
		    s = max(d__1,d__2);
		    *ttype = -3;
		}
	    } else {

/*              Case 4. */

		*ttype = -4;
		s = *dmin__ * .25;
		if (*dmin__ == *dn) {
		    gam = *dn;
		    a2 = 0.;
		    if (z__[nn - 5] > z__[nn - 7]) {
			return 0;
		    }
		    b2 = z__[nn - 5] / z__[nn - 7];
		    np = nn - 9;
		} else {
		    np = nn - ((*pp) << (1));
		    b2 = z__[np - 2];
		    gam = *dn1;
		    if (z__[np - 4] > z__[np - 2]) {
			return 0;
		    }
		    a2 = z__[np - 4] / z__[np - 2];
		    if (z__[nn - 9] > z__[nn - 11]) {
			return 0;
		    }
		    b2 = z__[nn - 9] / z__[nn - 11];
		    np = nn - 13;
		}

/*              Approximate contribution to norm squared from I < NN-1. */

		a2 += b2;
		i__1 = ((*i0) << (2)) - 1 + *pp;
		for (i4 = np; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.) {
			goto L20;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (max(b2,b1) * 100. < a2 || .563 < a2) {
			goto L20;
		    }
/* L10: */
		}
L20:
		a2 *= 1.05;

/*              Rayleigh quotient residual bound. */

		if (a2 < .563) {
		    s = gam * (1. - sqrt(a2)) / (a2 + 1.);
		}
	    }
	} else if (*dmin__ == *dn2) {

/*           Case 5. */

	    *ttype = -5;
	    s = *dmin__ * .25;

/*           Compute contribution to norm squared from I > NN-2. */

	    np = nn - ((*pp) << (1));
	    b1 = z__[np - 2];
	    b2 = z__[np - 6];
	    gam = *dn2;
	    if (z__[np - 8] > b2 || z__[np - 4] > b1) {
		return 0;
	    }
	    a2 = z__[np - 8] / b2 * (z__[np - 4] / b1 + 1.);

/*           Approximate contribution to norm squared from I < NN-2. */

	    if (*n0 - *i0 > 2) {
		b2 = z__[nn - 13] / z__[nn - 15];
		a2 += b2;
		i__1 = ((*i0) << (2)) - 1 + *pp;
		for (i4 = nn - 17; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.) {
			goto L40;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (max(b2,b1) * 100. < a2 || .563 < a2) {
			goto L40;
		    }
/* L30: */
		}
L40:
		a2 *= 1.05;
	    }

	    if (a2 < .563) {
		s = gam * (1. - sqrt(a2)) / (a2 + 1.);
	    }
	} else {

/*           Case 6, no information to guide us. */

	    if (*ttype == -6) {
		g += (1. - g) * .333;
	    } else if (*ttype == -18) {
		g = .083250000000000005;
	    } else {
		g = .25;
	    }
	    s = g * *dmin__;
	    *ttype = -6;
	}

    } else if (*n0in == *n0 + 1) {

/*        One eigenvalue just deflated. Use DMIN1, DN1 for DMIN and DN. */

	if ((*dmin1 == *dn1 && *dmin2 == *dn2)) {

/*           Cases 7 and 8. */

	    *ttype = -7;
	    s = *dmin1 * .333;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.) {
		goto L60;
	    }
	    i__1 = ((*i0) << (2)) - 1 + *pp;
	    for (i4 = ((*n0) << (2)) - 9 + *pp; i4 >= i__1; i4 += -4) {
		a2 = b1;
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (max(b1,a2) * 100. < b2) {
		    goto L60;
		}
/* L50: */
	    }
L60:
	    b2 = sqrt(b2 * 1.05);
/* Computing 2nd power */
	    d__1 = b2;
	    a2 = *dmin1 / (d__1 * d__1 + 1.);
	    gap2 = *dmin2 * .5 - a2;
	    if ((gap2 > 0. && gap2 > b2 * a2)) {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
		s = max(d__1,d__2);
	    } else {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
		s = max(d__1,d__2);
		*ttype = -8;
	    }
	} else {

/*           Case 9. */

	    s = *dmin1 * .25;
	    if (*dmin1 == *dn1) {
		s = *dmin1 * .5;
	    }
	    *ttype = -9;
	}

    } else if (*n0in == *n0 + 2) {

/*
          Two eigenvalues deflated. Use DMIN2, DN2 for DMIN and DN.

          Cases 10 and 11.
*/

	if ((*dmin2 == *dn2 && z__[nn - 5] * 2. < z__[nn - 7])) {
	    *ttype = -10;
	    s = *dmin2 * .333;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.) {
		goto L80;
	    }
	    i__1 = ((*i0) << (2)) - 1 + *pp;
	    for (i4 = ((*n0) << (2)) - 9 + *pp; i4 >= i__1; i4 += -4) {
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (b1 * 100. < b2) {
		    goto L80;
		}
/* L70: */
	    }
L80:
	    b2 = sqrt(b2 * 1.05);
/* Computing 2nd power */
	    d__1 = b2;
	    a2 = *dmin2 / (d__1 * d__1 + 1.);
	    gap2 = z__[nn - 7] + z__[nn - 9] - sqrt(z__[nn - 11]) * sqrt(z__[
		    nn - 9]) - a2;
	    if ((gap2 > 0. && gap2 > b2 * a2)) {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
		s = max(d__1,d__2);
	    } else {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
		s = max(d__1,d__2);
	    }
	} else {
	    s = *dmin2 * .25;
	    *ttype = -11;
	}
    } else if (*n0in > *n0 + 2) {

/*        Case 12, more than two eigenvalues deflated. No information. */

	s = 0.;
	*ttype = -12;
    }

    *tau = s;
    return 0;

/*     End of DLASQ4 */

} /* dlasq4_ */

/* Subroutine */ int dlasq5_(integer *i0, integer *n0, doublereal *z__,
	integer *pp, doublereal *tau, doublereal *dmin__, doublereal *dmin1,
	doublereal *dmin2, doublereal *dn, doublereal *dnm1, doublereal *dnm2,
	 logical *ieee)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static doublereal d__;
    static integer j4, j4p2;
    static doublereal emin, temp;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       May 17, 2000


    Purpose
    =======

    DLASQ5 computes one dqds transform in ping-pong form, one
    version for IEEE machines another for non IEEE machines.

    Arguments
    =========

    I0    (input) INTEGER
          First index.

    N0    (input) INTEGER
          Last index.

    Z     (input) DOUBLE PRECISION array, dimension ( 4*N )
          Z holds the qd array. EMIN is stored in Z(4*N0) to avoid
          an extra argument.

    PP    (input) INTEGER
          PP=0 for ping, PP=1 for pong.

    TAU   (input) DOUBLE PRECISION
          This is the shift.

    DMIN  (output) DOUBLE PRECISION
          Minimum value of d.

    DMIN1 (output) DOUBLE PRECISION
          Minimum value of d, excluding D( N0 ).

    DMIN2 (output) DOUBLE PRECISION
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).

    DN    (output) DOUBLE PRECISION
          d(N0), the last value of d.

    DNM1  (output) DOUBLE PRECISION
          d(N0-1).

    DNM2  (output) DOUBLE PRECISION
          d(N0-2).

    IEEE  (input) LOGICAL
          Flag for IEEE or non IEEE arithmetic.

    =====================================================================
*/


    /* Parameter adjustments */
    --z__;

    /* Function Body */
    if (*n0 - *i0 - 1 <= 0) {
	return 0;
    }

    j4 = ((*i0) << (2)) + *pp - 3;
    emin = z__[j4 + 4];
    d__ = z__[j4] - *tau;
    *dmin__ = d__;
    *dmin1 = -z__[j4];

    if (*ieee) {

/*        Code for IEEE arithmetic. */

	if (*pp == 0) {
	    i__1 = (*n0 - 3) << (2);
	    for (j4 = (*i0) << (2); j4 <= i__1; j4 += 4) {
		z__[j4 - 2] = d__ + z__[j4 - 1];
		temp = z__[j4 + 1] / z__[j4 - 2];
		d__ = d__ * temp - *tau;
		*dmin__ = min(*dmin__,d__);
		z__[j4] = z__[j4 - 1] * temp;
/* Computing MIN */
		d__1 = z__[j4];
		emin = min(d__1,emin);
/* L10: */
	    }
	} else {
	    i__1 = (*n0 - 3) << (2);
	    for (j4 = (*i0) << (2); j4 <= i__1; j4 += 4) {
		z__[j4 - 3] = d__ + z__[j4];
		temp = z__[j4 + 2] / z__[j4 - 3];
		d__ = d__ * temp - *tau;
		*dmin__ = min(*dmin__,d__);
		z__[j4 - 1] = z__[j4] * temp;
/* Computing MIN */
		d__1 = z__[j4 - 1];
		emin = min(d__1,emin);
/* L20: */
	    }
	}

/*        Unroll last two steps. */

	*dnm2 = d__;
	*dmin2 = *dmin__;
	j4 = ((*n0 - 2) << (2)) - *pp;
	j4p2 = j4 + ((*pp) << (1)) - 1;
	z__[j4 - 2] = *dnm2 + z__[j4p2];
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
	*dmin__ = min(*dmin__,*dnm1);

	*dmin1 = *dmin__;
	j4 += 4;
	j4p2 = j4 + ((*pp) << (1)) - 1;
	z__[j4 - 2] = *dnm1 + z__[j4p2];
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
	*dmin__ = min(*dmin__,*dn);

    } else {

/*        Code for non IEEE arithmetic. */

	if (*pp == 0) {
	    i__1 = (*n0 - 3) << (2);
	    for (j4 = (*i0) << (2); j4 <= i__1; j4 += 4) {
		z__[j4 - 2] = d__ + z__[j4 - 1];
		if (d__ < 0.) {
		    return 0;
		} else {
		    z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
		    d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]) - *tau;
		}
		*dmin__ = min(*dmin__,d__);
/* Computing MIN */
		d__1 = emin, d__2 = z__[j4];
		emin = min(d__1,d__2);
/* L30: */
	    }
	} else {
	    i__1 = (*n0 - 3) << (2);
	    for (j4 = (*i0) << (2); j4 <= i__1; j4 += 4) {
		z__[j4 - 3] = d__ + z__[j4];
		if (d__ < 0.) {
		    return 0;
		} else {
		    z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
		    d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]) - *tau;
		}
		*dmin__ = min(*dmin__,d__);
/* Computing MIN */
		d__1 = emin, d__2 = z__[j4 - 1];
		emin = min(d__1,d__2);
/* L40: */
	    }
	}

/*        Unroll last two steps. */

	*dnm2 = d__;
	*dmin2 = *dmin__;
	j4 = ((*n0 - 2) << (2)) - *pp;
	j4p2 = j4 + ((*pp) << (1)) - 1;
	z__[j4 - 2] = *dnm2 + z__[j4p2];
	if (*dnm2 < 0.) {
	    return 0;
	} else {
	    z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	    *dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
	}
	*dmin__ = min(*dmin__,*dnm1);

	*dmin1 = *dmin__;
	j4 += 4;
	j4p2 = j4 + ((*pp) << (1)) - 1;
	z__[j4 - 2] = *dnm1 + z__[j4p2];
	if (*dnm1 < 0.) {
	    return 0;
	} else {
	    z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	    *dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
	}
	*dmin__ = min(*dmin__,*dn);

    }

    z__[j4 + 2] = *dn;
    z__[((*n0) << (2)) - *pp] = emin;
    return 0;

/*     End of DLASQ5 */

} /* dlasq5_ */

/* Subroutine */ int dlasq6_(integer *i0, integer *n0, doublereal *z__,
	integer *pp, doublereal *dmin__, doublereal *dmin1, doublereal *dmin2,
	 doublereal *dn, doublereal *dnm1, doublereal *dnm2)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static doublereal d__;
    static integer j4, j4p2;
    static doublereal emin, temp;

    static doublereal safmin;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1999


    Purpose
    =======

    DLASQ6 computes one dqd (shift equal to zero) transform in
    ping-pong form, with protection against underflow and overflow.

    Arguments
    =========

    I0    (input) INTEGER
          First index.

    N0    (input) INTEGER
          Last index.

    Z     (input) DOUBLE PRECISION array, dimension ( 4*N )
          Z holds the qd array. EMIN is stored in Z(4*N0) to avoid
          an extra argument.

    PP    (input) INTEGER
          PP=0 for ping, PP=1 for pong.

    DMIN  (output) DOUBLE PRECISION
          Minimum value of d.

    DMIN1 (output) DOUBLE PRECISION
          Minimum value of d, excluding D( N0 ).

    DMIN2 (output) DOUBLE PRECISION
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).

    DN    (output) DOUBLE PRECISION
          d(N0), the last value of d.

    DNM1  (output) DOUBLE PRECISION
          d(N0-1).

    DNM2  (output) DOUBLE PRECISION
          d(N0-2).

    =====================================================================
*/


    /* Parameter adjustments */
    --z__;

    /* Function Body */
    if (*n0 - *i0 - 1 <= 0) {
	return 0;
    }

    safmin = SAFEMINIMUM;
    j4 = ((*i0) << (2)) + *pp - 3;
    emin = z__[j4 + 4];
    d__ = z__[j4];
    *dmin__ = d__;

    if (*pp == 0) {
	i__1 = (*n0 - 3) << (2);
	for (j4 = (*i0) << (2); j4 <= i__1; j4 += 4) {
	    z__[j4 - 2] = d__ + z__[j4 - 1];
	    if (z__[j4 - 2] == 0.) {
		z__[j4] = 0.;
		d__ = z__[j4 + 1];
		*dmin__ = d__;
		emin = 0.;
	    } else if ((safmin * z__[j4 + 1] < z__[j4 - 2] && safmin * z__[j4
		    - 2] < z__[j4 + 1])) {
		temp = z__[j4 + 1] / z__[j4 - 2];
		z__[j4] = z__[j4 - 1] * temp;
		d__ *= temp;
	    } else {
		z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
		d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]);
	    }
	    *dmin__ = min(*dmin__,d__);
/* Computing MIN */
	    d__1 = emin, d__2 = z__[j4];
	    emin = min(d__1,d__2);
/* L10: */
	}
    } else {
	i__1 = (*n0 - 3) << (2);
	for (j4 = (*i0) << (2); j4 <= i__1; j4 += 4) {
	    z__[j4 - 3] = d__ + z__[j4];
	    if (z__[j4 - 3] == 0.) {
		z__[j4 - 1] = 0.;
		d__ = z__[j4 + 2];
		*dmin__ = d__;
		emin = 0.;
	    } else if ((safmin * z__[j4 + 2] < z__[j4 - 3] && safmin * z__[j4
		    - 3] < z__[j4 + 2])) {
		temp = z__[j4 + 2] / z__[j4 - 3];
		z__[j4 - 1] = z__[j4] * temp;
		d__ *= temp;
	    } else {
		z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
		d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]);
	    }
	    *dmin__ = min(*dmin__,d__);
/* Computing MIN */
	    d__1 = emin, d__2 = z__[j4 - 1];
	    emin = min(d__1,d__2);
/* L20: */
	}
    }

/*     Unroll last two steps. */

    *dnm2 = d__;
    *dmin2 = *dmin__;
    j4 = ((*n0 - 2) << (2)) - *pp;
    j4p2 = j4 + ((*pp) << (1)) - 1;
    z__[j4 - 2] = *dnm2 + z__[j4p2];
    if (z__[j4 - 2] == 0.) {
	z__[j4] = 0.;
	*dnm1 = z__[j4p2 + 2];
	*dmin__ = *dnm1;
	emin = 0.;
    } else if ((safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] <
	     z__[j4p2 + 2])) {
	temp = z__[j4p2 + 2] / z__[j4 - 2];
	z__[j4] = z__[j4p2] * temp;
	*dnm1 = *dnm2 * temp;
    } else {
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]);
    }
    *dmin__ = min(*dmin__,*dnm1);

    *dmin1 = *dmin__;
    j4 += 4;
    j4p2 = j4 + ((*pp) << (1)) - 1;
    z__[j4 - 2] = *dnm1 + z__[j4p2];
    if (z__[j4 - 2] == 0.) {
	z__[j4] = 0.;
	*dn = z__[j4p2 + 2];
	*dmin__ = *dn;
	emin = 0.;
    } else if ((safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] <
	     z__[j4p2 + 2])) {
	temp = z__[j4p2 + 2] / z__[j4 - 2];
	z__[j4] = z__[j4p2] * temp;
	*dn = *dnm1 * temp;
    } else {
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]);
    }
    *dmin__ = min(*dmin__,*dn);

    z__[j4 + 2] = *dn;
    z__[((*n0) << (2)) - *pp] = emin;
    return 0;

/*     End of DLASQ6 */

} /* dlasq6_ */

/* Subroutine */ int dlasr_(char *side, char *pivot, char *direct, integer *m,
	 integer *n, doublereal *c__, doublereal *s, doublereal *a, integer *
	lda)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j, info;
    static doublereal temp;
    extern logical lsame_(char *, char *);
    static doublereal ctemp, stemp;
    extern /* Subroutine */ int xerbla_(char *, integer *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLASR   performs the transformation

       A := P*A,   when SIDE = 'L' or 'l'  (  Left-hand side )

       A := A*P',  when SIDE = 'R' or 'r'  ( Right-hand side )

    where A is an m by n real matrix and P is an orthogonal matrix,
    consisting of a sequence of plane rotations determined by the
    parameters PIVOT and DIRECT as follows ( z = m when SIDE = 'L' or 'l'
    and z = n when SIDE = 'R' or 'r' ):

    When  DIRECT = 'F' or 'f'  ( Forward sequence ) then

       P = P( z - 1 )*...*P( 2 )*P( 1 ),

    and when DIRECT = 'B' or 'b'  ( Backward sequence ) then

       P = P( 1 )*P( 2 )*...*P( z - 1 ),

    where  P( k ) is a plane rotation matrix for the following planes:

       when  PIVOT = 'V' or 'v'  ( Variable pivot ),
          the plane ( k, k + 1 )

       when  PIVOT = 'T' or 't'  ( Top pivot ),
          the plane ( 1, k + 1 )

       when  PIVOT = 'B' or 'b'  ( Bottom pivot ),
          the plane ( k, z )

    c( k ) and s( k )  must contain the  cosine and sine that define the
    matrix  P( k ).  The two by two plane rotation part of the matrix
    P( k ), R( k ), is assumed to be of the form

       R( k ) = (  c( k )  s( k ) ).
                ( -s( k )  c( k ) )

    This version vectorises across rows of the array A when SIDE = 'L'.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            Specifies whether the plane rotation matrix P is applied to
            A on the left or the right.
            = 'L':  Left, compute A := P*A
            = 'R':  Right, compute A:= A*P'

    DIRECT  (input) CHARACTER*1
            Specifies whether P is a forward or backward sequence of
            plane rotations.
            = 'F':  Forward, P = P( z - 1 )*...*P( 2 )*P( 1 )
            = 'B':  Backward, P = P( 1 )*P( 2 )*...*P( z - 1 )

    PIVOT   (input) CHARACTER*1
            Specifies the plane for which P(k) is a plane rotation
            matrix.
            = 'V':  Variable pivot, the plane (k,k+1)
            = 'T':  Top pivot, the plane (1,k+1)
            = 'B':  Bottom pivot, the plane (k,z)

    M       (input) INTEGER
            The number of rows of the matrix A.  If m <= 1, an immediate
            return is effected.

    N       (input) INTEGER
            The number of columns of the matrix A.  If n <= 1, an
            immediate return is effected.

    C, S    (input) DOUBLE PRECISION arrays, dimension
                    (M-1) if SIDE = 'L'
                    (N-1) if SIDE = 'R'
            c(k) and s(k) contain the cosine and sine that define the
            matrix P(k).  The two by two plane rotation part of the
            matrix P(k), R(k), is assumed to be of the form
            R( k ) = (  c( k )  s( k ) ).
                     ( -s( k )  c( k ) )

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            The m by n matrix A.  On exit, A is overwritten by P*A if
            SIDE = 'R' or by A*P' if SIDE = 'L'.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    --c__;
    --s;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    info = 0;
    if (! (lsame_(side, "L") || lsame_(side, "R"))) {
	info = 1;
    } else if (! (lsame_(pivot, "V") || lsame_(pivot,
	    "T") || lsame_(pivot, "B"))) {
	info = 2;
    } else if (! (lsame_(direct, "F") || lsame_(direct,
	    "B"))) {
	info = 3;
    } else if (*m < 0) {
	info = 4;
    } else if (*n < 0) {
	info = 5;
    } else if (*lda < max(1,*m)) {
	info = 9;
    }
    if (info != 0) {
	xerbla_("DLASR ", &info);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }
    if (lsame_(side, "L")) {

/*        Form  P * A */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[j + 1 + i__ * a_dim1];
			    a[j + 1 + i__ * a_dim1] = ctemp * temp - stemp *
				    a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * temp + ctemp * a[j
				    + i__ * a_dim1];
/* L10: */
			}
		    }
/* L20: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[j + 1 + i__ * a_dim1];
			    a[j + 1 + i__ * a_dim1] = ctemp * temp - stemp *
				    a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * temp + ctemp * a[j
				    + i__ * a_dim1];
/* L30: */
			}
		    }
/* L40: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m;
		for (j = 2; j <= i__1; ++j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = ctemp * temp - stemp * a[
				    i__ * a_dim1 + 1];
			    a[i__ * a_dim1 + 1] = stemp * temp + ctemp * a[
				    i__ * a_dim1 + 1];
/* L50: */
			}
		    }
/* L60: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m; j >= 2; --j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = ctemp * temp - stemp * a[
				    i__ * a_dim1 + 1];
			    a[i__ * a_dim1 + 1] = stemp * temp + ctemp * a[
				    i__ * a_dim1 + 1];
/* L70: */
			}
		    }
/* L80: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * a[*m + i__ * a_dim1]
				     + ctemp * temp;
			    a[*m + i__ * a_dim1] = ctemp * a[*m + i__ *
				    a_dim1] - stemp * temp;
/* L90: */
			}
		    }
/* L100: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * a[*m + i__ * a_dim1]
				     + ctemp * temp;
			    a[*m + i__ * a_dim1] = ctemp * a[*m + i__ *
				    a_dim1] - stemp * temp;
/* L110: */
			}
		    }
/* L120: */
		}
	    }
	}
    } else if (lsame_(side, "R")) {

/*        Form A * P' */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[i__ + (j + 1) * a_dim1];
			    a[i__ + (j + 1) * a_dim1] = ctemp * temp - stemp *
				     a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * temp + ctemp * a[
				    i__ + j * a_dim1];
/* L130: */
			}
		    }
/* L140: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[i__ + (j + 1) * a_dim1];
			    a[i__ + (j + 1) * a_dim1] = ctemp * temp - stemp *
				     a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * temp + ctemp * a[
				    i__ + j * a_dim1];
/* L150: */
			}
		    }
/* L160: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n;
		for (j = 2; j <= i__1; ++j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = ctemp * temp - stemp * a[
				    i__ + a_dim1];
			    a[i__ + a_dim1] = stemp * temp + ctemp * a[i__ +
				    a_dim1];
/* L170: */
			}
		    }
/* L180: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n; j >= 2; --j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = ctemp * temp - stemp * a[
				    i__ + a_dim1];
			    a[i__ + a_dim1] = stemp * temp + ctemp * a[i__ +
				    a_dim1];
/* L190: */
			}
		    }
/* L200: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * a[i__ + *n * a_dim1]
				     + ctemp * temp;
			    a[i__ + *n * a_dim1] = ctemp * a[i__ + *n *
				    a_dim1] - stemp * temp;
/* L210: */
			}
		    }
/* L220: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * a[i__ + *n * a_dim1]
				     + ctemp * temp;
			    a[i__ + *n * a_dim1] = ctemp * a[i__ + *n *
				    a_dim1] - stemp * temp;
/* L230: */
			}
		    }
/* L240: */
		}
	    }
	}
    }

    return 0;

/*     End of DLASR */

} /* dlasr_ */

/* Subroutine */ int dlasrt_(char *id, integer *n, doublereal *d__, integer *
	info)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j;
    static doublereal d1, d2, d3;
    static integer dir;
    static doublereal tmp;
    static integer endd;
    extern logical lsame_(char *, char *);
    static integer stack[64]	/* was [2][32] */;
    static doublereal dmnmx;
    static integer start;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static integer stkpnt;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    Sort the numbers in D in increasing order (if ID = 'I') or
    in decreasing order (if ID = 'D' ).

    Use Quick Sort, reverting to Insertion sort on arrays of
    size <= 20. Dimension of STACK limits N to about 2**32.

    Arguments
    =========

    ID      (input) CHARACTER*1
            = 'I': sort D in increasing order;
            = 'D': sort D in decreasing order.

    N       (input) INTEGER
            The length of the array D.

    D       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the array to be sorted.
            On exit, D has been sorted into increasing order
            (D(1) <= ... <= D(N) ) or into decreasing order
            (D(1) >= ... >= D(N) ), depending on ID.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input paramters.
*/

    /* Parameter adjustments */
    --d__;

    /* Function Body */
    *info = 0;
    dir = -1;
    if (lsame_(id, "D")) {
	dir = 0;
    } else if (lsame_(id, "I")) {
	dir = 1;
    }
    if (dir == -1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASRT", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 1) {
	return 0;
    }

    stkpnt = 1;
    stack[0] = 1;
    stack[1] = *n;
L10:
    start = stack[((stkpnt) << (1)) - 2];
    endd = stack[((stkpnt) << (1)) - 1];
    --stkpnt;
    if ((endd - start <= 20 && endd - start > 0)) {

/*        Do Insertion sort on D( START:ENDD ) */

	if (dir == 0) {

/*           Sort into decreasing order */

	    i__1 = endd;
	    for (i__ = start + 1; i__ <= i__1; ++i__) {
		i__2 = start + 1;
		for (j = i__; j >= i__2; --j) {
		    if (d__[j] > d__[j - 1]) {
			dmnmx = d__[j];
			d__[j] = d__[j - 1];
			d__[j - 1] = dmnmx;
		    } else {
			goto L30;
		    }
/* L20: */
		}
L30:
		;
	    }

	} else {

/*           Sort into increasing order */

	    i__1 = endd;
	    for (i__ = start + 1; i__ <= i__1; ++i__) {
		i__2 = start + 1;
		for (j = i__; j >= i__2; --j) {
		    if (d__[j] < d__[j - 1]) {
			dmnmx = d__[j];
			d__[j] = d__[j - 1];
			d__[j - 1] = dmnmx;
		    } else {
			goto L50;
		    }
/* L40: */
		}
L50:
		;
	    }

	}

    } else if (endd - start > 20) {

/*
          Partition D( START:ENDD ) and stack parts, largest one first

          Choose partition entry as median of 3
*/

	d1 = d__[start];
	d2 = d__[endd];
	i__ = (start + endd) / 2;
	d3 = d__[i__];
	if (d1 < d2) {
	    if (d3 < d1) {
		dmnmx = d1;
	    } else if (d3 < d2) {
		dmnmx = d3;
	    } else {
		dmnmx = d2;
	    }
	} else {
	    if (d3 < d2) {
		dmnmx = d2;
	    } else if (d3 < d1) {
		dmnmx = d3;
	    } else {
		dmnmx = d1;
	    }
	}

	if (dir == 0) {

/*           Sort into decreasing order */

	    i__ = start - 1;
	    j = endd + 1;
L60:
L70:
	    --j;
	    if (d__[j] < dmnmx) {
		goto L70;
	    }
L80:
	    ++i__;
	    if (d__[i__] > dmnmx) {
		goto L80;
	    }
	    if (i__ < j) {
		tmp = d__[i__];
		d__[i__] = d__[j];
		d__[j] = tmp;
		goto L60;
	    }
	    if (j - start > endd - j - 1) {
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = start;
		stack[((stkpnt) << (1)) - 1] = j;
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = j + 1;
		stack[((stkpnt) << (1)) - 1] = endd;
	    } else {
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = j + 1;
		stack[((stkpnt) << (1)) - 1] = endd;
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = start;
		stack[((stkpnt) << (1)) - 1] = j;
	    }
	} else {

/*           Sort into increasing order */

	    i__ = start - 1;
	    j = endd + 1;
L90:
L100:
	    --j;
	    if (d__[j] > dmnmx) {
		goto L100;
	    }
L110:
	    ++i__;
	    if (d__[i__] < dmnmx) {
		goto L110;
	    }
	    if (i__ < j) {
		tmp = d__[i__];
		d__[i__] = d__[j];
		d__[j] = tmp;
		goto L90;
	    }
	    if (j - start > endd - j - 1) {
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = start;
		stack[((stkpnt) << (1)) - 1] = j;
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = j + 1;
		stack[((stkpnt) << (1)) - 1] = endd;
	    } else {
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = j + 1;
		stack[((stkpnt) << (1)) - 1] = endd;
		++stkpnt;
		stack[((stkpnt) << (1)) - 2] = start;
		stack[((stkpnt) << (1)) - 1] = j;
	    }
	}
    }
    if (stkpnt > 0) {
	goto L10;
    }
    return 0;

/*     End of DLASRT */

} /* dlasrt_ */

/* Subroutine */ int dlassq_(integer *n, doublereal *x, integer *incx,
	doublereal *scale, doublereal *sumsq)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer ix;
    static doublereal absxi;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLASSQ  returns the values  scl  and  smsq  such that

       ( scl**2 )*smsq = x( 1 )**2 +...+ x( n )**2 + ( scale**2 )*sumsq,

    where  x( i ) = X( 1 + ( i - 1 )*INCX ). The value of  sumsq  is
    assumed to be non-negative and  scl  returns the value

       scl = max( scale, abs( x( i ) ) ).

    scale and sumsq must be supplied in SCALE and SUMSQ and
    scl and smsq are overwritten on SCALE and SUMSQ respectively.

    The routine makes only one pass through the vector x.

    Arguments
    =========

    N       (input) INTEGER
            The number of elements to be used from the vector X.

    X       (input) DOUBLE PRECISION array, dimension (N)
            The vector for which a scaled sum of squares is computed.
               x( i )  = X( 1 + ( i - 1 )*INCX ), 1 <= i <= n.

    INCX    (input) INTEGER
            The increment between successive values of the vector X.
            INCX > 0.

    SCALE   (input/output) DOUBLE PRECISION
            On entry, the value  scale  in the equation above.
            On exit, SCALE is overwritten with  scl , the scaling factor
            for the sum of squares.

    SUMSQ   (input/output) DOUBLE PRECISION
            On entry, the value  sumsq  in the equation above.
            On exit, SUMSQ is overwritten with  smsq , the basic sum of
            squares from which  scl  has been factored out.

   =====================================================================
*/


    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n > 0) {
	i__1 = (*n - 1) * *incx + 1;
	i__2 = *incx;
	for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
	    if (x[ix] != 0.) {
		absxi = (d__1 = x[ix], abs(d__1));
		if (*scale < absxi) {
/* Computing 2nd power */
		    d__1 = *scale / absxi;
		    *sumsq = *sumsq * (d__1 * d__1) + 1;
		    *scale = absxi;
		} else {
/* Computing 2nd power */
		    d__1 = absxi / *scale;
		    *sumsq += d__1 * d__1;
		}
	    }
/* L10: */
	}
    }
    return 0;

/*     End of DLASSQ */

} /* dlassq_ */

/* Subroutine */ int dlasv2_(doublereal *f, doublereal *g, doublereal *h__,
	doublereal *ssmin, doublereal *ssmax, doublereal *snr, doublereal *
	csr, doublereal *snl, doublereal *csl)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static doublereal a, d__, l, m, r__, s, t, fa, ga, ha, ft, gt, ht, mm, tt,
	     clt, crt, slt, srt;
    static integer pmax;
    static doublereal temp;
    static logical swap;
    static doublereal tsign;

    static logical gasmal;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLASV2 computes the singular value decomposition of a 2-by-2
    triangular matrix
       [  F   G  ]
       [  0   H  ].
    On return, abs(SSMAX) is the larger singular value, abs(SSMIN) is the
    smaller singular value, and (CSL,SNL) and (CSR,SNR) are the left and
    right singular vectors for abs(SSMAX), giving the decomposition

       [ CSL  SNL ] [  F   G  ] [ CSR -SNR ]  =  [ SSMAX   0   ]
       [-SNL  CSL ] [  0   H  ] [ SNR  CSR ]     [  0    SSMIN ].

    Arguments
    =========

    F       (input) DOUBLE PRECISION
            The (1,1) element of the 2-by-2 matrix.

    G       (input) DOUBLE PRECISION
            The (1,2) element of the 2-by-2 matrix.

    H       (input) DOUBLE PRECISION
            The (2,2) element of the 2-by-2 matrix.

    SSMIN   (output) DOUBLE PRECISION
            abs(SSMIN) is the smaller singular value.

    SSMAX   (output) DOUBLE PRECISION
            abs(SSMAX) is the larger singular value.

    SNL     (output) DOUBLE PRECISION
    CSL     (output) DOUBLE PRECISION
            The vector (CSL, SNL) is a unit left singular vector for the
            singular value abs(SSMAX).

    SNR     (output) DOUBLE PRECISION
    CSR     (output) DOUBLE PRECISION
            The vector (CSR, SNR) is a unit right singular vector for the
            singular value abs(SSMAX).

    Further Details
    ===============

    Any input parameter may be aliased with any output parameter.

    Barring over/underflow and assuming a guard digit in subtraction, all
    output quantities are correct to within a few units in the last
    place (ulps).

    In IEEE arithmetic, the code works correctly if one matrix element is
    infinite.

    Overflow will not occur unless the largest singular value itself
    overflows or is within a few ulps of overflow. (On machines with
    partial overflow, like the Cray, overflow may occur if the largest
    singular value is within a factor of 2 of overflow.)

    Underflow is harmless if underflow is gradual. Otherwise, results
    may correspond to a matrix modified by perturbations of size near
    the underflow threshold.

   =====================================================================
*/


    ft = *f;
    fa = abs(ft);
    ht = *h__;
    ha = abs(*h__);

/*
       PMAX points to the maximum absolute element of matrix
         PMAX = 1 if F largest in absolute values
         PMAX = 2 if G largest in absolute values
         PMAX = 3 if H largest in absolute values
*/

    pmax = 1;
    swap = ha > fa;
    if (swap) {
	pmax = 3;
	temp = ft;
	ft = ht;
	ht = temp;
	temp = fa;
	fa = ha;
	ha = temp;

/*        Now FA .ge. HA */

    }
    gt = *g;
    ga = abs(gt);
    if (ga == 0.) {

/*        Diagonal matrix */

	*ssmin = ha;
	*ssmax = fa;
	clt = 1.;
	crt = 1.;
	slt = 0.;
	srt = 0.;
    } else {
	gasmal = TRUE_;
	if (ga > fa) {
	    pmax = 2;
	    if (fa / ga < EPSILON) {

/*              Case of very large GA */

		gasmal = FALSE_;
		*ssmax = ga;
		if (ha > 1.) {
		    *ssmin = fa / (ga / ha);
		} else {
		    *ssmin = fa / ga * ha;
		}
		clt = 1.;
		slt = ht / gt;
		srt = 1.;
		crt = ft / gt;
	    }
	}
	if (gasmal) {

/*           Normal case */

	    d__ = fa - ha;
	    if (d__ == fa) {

/*              Copes with infinite F or H */

		l = 1.;
	    } else {
		l = d__ / fa;
	    }

/*           Note that 0 .le. L .le. 1 */

	    m = gt / ft;

/*           Note that abs(M) .le. 1/macheps */

	    t = 2. - l;

/*           Note that T .ge. 1 */

	    mm = m * m;
	    tt = t * t;
	    s = sqrt(tt + mm);

/*           Note that 1 .le. S .le. 1 + 1/macheps */

	    if (l == 0.) {
		r__ = abs(m);
	    } else {
		r__ = sqrt(l * l + mm);
	    }

/*           Note that 0 .le. R .le. 1 + 1/macheps */

	    a = (s + r__) * .5;

/*           Note that 1 .le. A .le. 1 + abs(M) */

	    *ssmin = ha / a;
	    *ssmax = fa * a;
	    if (mm == 0.) {

/*              Note that M is very tiny */

		if (l == 0.) {
		    t = d_sign(&c_b2804, &ft) * d_sign(&c_b15, &gt);
		} else {
		    t = gt / d_sign(&d__, &ft) + m / t;
		}
	    } else {
		t = (m / (s + t) + m / (r__ + l)) * (a + 1.);
	    }
	    l = sqrt(t * t + 4.);
	    crt = 2. / l;
	    srt = t / l;
	    clt = (crt + srt * m) / a;
	    slt = ht / ft * srt / a;
	}
    }
    if (swap) {
	*csl = srt;
	*snl = crt;
	*csr = slt;
	*snr = clt;
    } else {
	*csl = clt;
	*snl = slt;
	*csr = crt;
	*snr = srt;
    }

/*     Correct signs of SSMAX and SSMIN */

    if (pmax == 1) {
	tsign = d_sign(&c_b15, csr) * d_sign(&c_b15, csl) * d_sign(&c_b15, f);
    }
    if (pmax == 2) {
	tsign = d_sign(&c_b15, snr) * d_sign(&c_b15, csl) * d_sign(&c_b15, g);
    }
    if (pmax == 3) {
	tsign = d_sign(&c_b15, snr) * d_sign(&c_b15, snl) * d_sign(&c_b15,
		h__);
    }
    *ssmax = d_sign(ssmax, &tsign);
    d__1 = tsign * d_sign(&c_b15, f) * d_sign(&c_b15, h__);
    *ssmin = d_sign(ssmin, &d__1);
    return 0;

/*     End of DLASV2 */

} /* dlasv2_ */

/* Subroutine */ int dlaswp_(integer *n, doublereal *a, integer *lda, integer
	*k1, integer *k2, integer *ipiv, integer *incx)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer i__, j, k, i1, i2, n32, ip, ix, ix0, inc;
    static doublereal temp;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DLASWP performs a series of row interchanges on the matrix A.
    One row interchange is initiated for each of rows K1 through K2 of A.

    Arguments
    =========

    N       (input) INTEGER
            The number of columns of the matrix A.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the matrix of column dimension N to which the row
            interchanges will be applied.
            On exit, the permuted matrix.

    LDA     (input) INTEGER
            The leading dimension of the array A.

    K1      (input) INTEGER
            The first element of IPIV for which a row interchange will
            be done.

    K2      (input) INTEGER
            The last element of IPIV for which a row interchange will
            be done.

    IPIV    (input) INTEGER array, dimension (M*abs(INCX))
            The vector of pivot indices.  Only the elements in positions
            K1 through K2 of IPIV are accessed.
            IPIV(K) = L implies rows K and L are to be interchanged.

    INCX    (input) INTEGER
            The increment between successive values of IPIV.  If IPIV
            is negative, the pivots are applied in reverse order.

    Further Details
    ===============

    Modified by
     R. C. Whaley, Computer Science Dept., Univ. of Tenn., Knoxville, USA

   =====================================================================


       Interchange row I with row IPIV(I) for each of rows K1 through K2.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --ipiv;

    /* Function Body */
    if (*incx > 0) {
	ix0 = *k1;
	i1 = *k1;
	i2 = *k2;
	inc = 1;
    } else if (*incx < 0) {
	ix0 = (1 - *k2) * *incx + 1;
	i1 = *k2;
	i2 = *k1;
	inc = -1;
    } else {
	return 0;
    }

    n32 = (*n / 32) << (5);
    if (n32 != 0) {
	i__1 = n32;
	for (j = 1; j <= i__1; j += 32) {
	    ix = ix0;
	    i__2 = i2;
	    i__3 = inc;
	    for (i__ = i1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__3)
		    {
		ip = ipiv[ix];
		if (ip != i__) {
		    i__4 = j + 31;
		    for (k = j; k <= i__4; ++k) {
			temp = a[i__ + k * a_dim1];
			a[i__ + k * a_dim1] = a[ip + k * a_dim1];
			a[ip + k * a_dim1] = temp;
/* L10: */
		    }
		}
		ix += *incx;
/* L20: */
	    }
/* L30: */
	}
    }
    if (n32 != *n) {
	++n32;
	ix = ix0;
	i__1 = i2;
	i__3 = inc;
	for (i__ = i1; i__3 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__3) {
	    ip = ipiv[ix];
	    if (ip != i__) {
		i__2 = *n;
		for (k = n32; k <= i__2; ++k) {
		    temp = a[i__ + k * a_dim1];
		    a[i__ + k * a_dim1] = a[ip + k * a_dim1];
		    a[ip + k * a_dim1] = temp;
/* L40: */
		}
	    }
	    ix += *incx;
/* L50: */
	}
    }

    return 0;

/*     End of DLASWP */

} /* dlaswp_ */

/* Subroutine */ int dlatrd_(char *uplo, integer *n, integer *nb, doublereal *
	a, integer *lda, doublereal *e, doublereal *tau, doublereal *w,
	integer *ldw)
{
    /* System generated locals */
    integer a_dim1, a_offset, w_dim1, w_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, iw;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
	    integer *);
    static doublereal alpha;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *), daxpy_(integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *),
	    dsymv_(char *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *), dlarfg_(integer *, doublereal *, doublereal *, integer *,
	     doublereal *);


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DLATRD reduces NB rows and columns of a real symmetric matrix A to
    symmetric tridiagonal form by an orthogonal similarity
    transformation Q' * A * Q, and returns the matrices V and W which are
    needed to apply the transformation to the unreduced part of A.

    If UPLO = 'U', DLATRD reduces the last NB rows and columns of a
    matrix, of which the upper triangle is supplied;
    if UPLO = 'L', DLATRD reduces the first NB rows and columns of a
    matrix, of which the lower triangle is supplied.

    This is an auxiliary routine called by DSYTRD.

    Arguments
    =========

    UPLO    (input) CHARACTER
            Specifies whether the upper or lower triangular part of the
            symmetric matrix A is stored:
            = 'U': Upper triangular
            = 'L': Lower triangular

    N       (input) INTEGER
            The order of the matrix A.

    NB      (input) INTEGER
            The number of rows and columns to be reduced.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            n-by-n upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading n-by-n lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.
            On exit:
            if UPLO = 'U', the last NB columns have been reduced to
              tridiagonal form, with the diagonal elements overwriting
              the diagonal elements of A; the elements above the diagonal
              with the array TAU, represent the orthogonal matrix Q as a
              product of elementary reflectors;
            if UPLO = 'L', the first NB columns have been reduced to
              tridiagonal form, with the diagonal elements overwriting
              the diagonal elements of A; the elements below the diagonal
              with the array TAU, represent the  orthogonal matrix Q as a
              product of elementary reflectors.
            See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= (1,N).

    E       (output) DOUBLE PRECISION array, dimension (N-1)
            If UPLO = 'U', E(n-nb:n-1) contains the superdiagonal
            elements of the last NB columns of the reduced matrix;
            if UPLO = 'L', E(1:nb) contains the subdiagonal elements of
            the first NB columns of the reduced matrix.

    TAU     (output) DOUBLE PRECISION array, dimension (N-1)
            The scalar factors of the elementary reflectors, stored in
            TAU(n-nb:n-1) if UPLO = 'U', and in TAU(1:nb) if UPLO = 'L'.
            See Further Details.

    W       (output) DOUBLE PRECISION array, dimension (LDW,NB)
            The n-by-nb matrix W required to update the unreduced part
            of A.

    LDW     (input) INTEGER
            The leading dimension of the array W. LDW >= max(1,N).

    Further Details
    ===============

    If UPLO = 'U', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(n) H(n-1) . . . H(n-nb+1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(i:n) = 0 and v(i-1) = 1; v(1:i-1) is stored on exit in A(1:i-1,i),
    and tau in TAU(i-1).

    If UPLO = 'L', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(1) H(2) . . . H(nb).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0 and v(i+1) = 1; v(i+1:n) is stored on exit in A(i+1:n,i),
    and tau in TAU(i).

    The elements of the vectors v together form the n-by-nb matrix V
    which is needed, with W, to apply the transformation to the unreduced
    part of the matrix, using a symmetric rank-2k update of the form:
    A := A - V*W' - W*V'.

    The contents of A on exit are illustrated by the following examples
    with n = 5 and nb = 2:

    if UPLO = 'U':                       if UPLO = 'L':

      (  a   a   a   v4  v5 )              (  d                  )
      (      a   a   v4  v5 )              (  1   d              )
      (          a   1   v5 )              (  v1  1   a          )
      (              d   1  )              (  v1  v2  a   a      )
      (                  d  )              (  v1  v2  a   a   a  )

    where d denotes a diagonal element of the reduced matrix, a denotes
    an element of the original matrix that is unchanged, and vi denotes
    an element of the vector defining H(i).

    =====================================================================


       Quick return if possible
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --e;
    --tau;
    w_dim1 = *ldw;
    w_offset = 1 + w_dim1 * 1;
    w -= w_offset;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }

    if (lsame_(uplo, "U")) {

/*        Reduce last NB columns of upper triangle */

	i__1 = *n - *nb + 1;
	for (i__ = *n; i__ >= i__1; --i__) {
	    iw = i__ - *n + *nb;
	    if (i__ < *n) {

/*              Update A(1:i,i) */

		i__2 = *n - i__;
		dgemv_("No transpose", &i__, &i__2, &c_b151, &a[(i__ + 1) *
			a_dim1 + 1], lda, &w[i__ + (iw + 1) * w_dim1], ldw, &
			c_b15, &a[i__ * a_dim1 + 1], &c__1);
		i__2 = *n - i__;
		dgemv_("No transpose", &i__, &i__2, &c_b151, &w[(iw + 1) *
			w_dim1 + 1], ldw, &a[i__ + (i__ + 1) * a_dim1], lda, &
			c_b15, &a[i__ * a_dim1 + 1], &c__1);
	    }
	    if (i__ > 1) {

/*
                Generate elementary reflector H(i) to annihilate
                A(1:i-2,i)
*/

		i__2 = i__ - 1;
		dlarfg_(&i__2, &a[i__ - 1 + i__ * a_dim1], &a[i__ * a_dim1 +
			1], &c__1, &tau[i__ - 1]);
		e[i__ - 1] = a[i__ - 1 + i__ * a_dim1];
		a[i__ - 1 + i__ * a_dim1] = 1.;

/*              Compute W(1:i-1,i) */

		i__2 = i__ - 1;
		dsymv_("Upper", &i__2, &c_b15, &a[a_offset], lda, &a[i__ *
			a_dim1 + 1], &c__1, &c_b29, &w[iw * w_dim1 + 1], &
			c__1);
		if (i__ < *n) {
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("Transpose", &i__2, &i__3, &c_b15, &w[(iw + 1) *
			    w_dim1 + 1], ldw, &a[i__ * a_dim1 + 1], &c__1, &
			    c_b29, &w[i__ + 1 + iw * w_dim1], &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[(i__ + 1)
			     * a_dim1 + 1], lda, &w[i__ + 1 + iw * w_dim1], &
			    c__1, &c_b15, &w[iw * w_dim1 + 1], &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[(i__ + 1) *
			    a_dim1 + 1], lda, &a[i__ * a_dim1 + 1], &c__1, &
			    c_b29, &w[i__ + 1 + iw * w_dim1], &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("No transpose", &i__2, &i__3, &c_b151, &w[(iw + 1)
			    * w_dim1 + 1], ldw, &w[i__ + 1 + iw * w_dim1], &
			    c__1, &c_b15, &w[iw * w_dim1 + 1], &c__1);
		}
		i__2 = i__ - 1;
		dscal_(&i__2, &tau[i__ - 1], &w[iw * w_dim1 + 1], &c__1);
		i__2 = i__ - 1;
		alpha = tau[i__ - 1] * -.5 * ddot_(&i__2, &w[iw * w_dim1 + 1],
			 &c__1, &a[i__ * a_dim1 + 1], &c__1);
		i__2 = i__ - 1;
		daxpy_(&i__2, &alpha, &a[i__ * a_dim1 + 1], &c__1, &w[iw *
			w_dim1 + 1], &c__1);
	    }

/* L10: */
	}
    } else {

/*        Reduce first NB columns of lower triangle */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i:n,i) */

	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[i__ + a_dim1],
		    lda, &w[i__ + w_dim1], ldw, &c_b15, &a[i__ + i__ * a_dim1]
		    , &c__1);
	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b151, &w[i__ + w_dim1],
		    ldw, &a[i__ + a_dim1], lda, &c_b15, &a[i__ + i__ * a_dim1]
		    , &c__1);
	    if (i__ < *n) {

/*
                Generate elementary reflector H(i) to annihilate
                A(i+2:n,i)
*/

		i__2 = *n - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[min(i__3,*n) +
			i__ * a_dim1], &c__1, &tau[i__]);
		e[i__] = a[i__ + 1 + i__ * a_dim1];
		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Compute W(i+1:n,i) */

		i__2 = *n - i__;
		dsymv_("Lower", &i__2, &c_b15, &a[i__ + 1 + (i__ + 1) *
			a_dim1], lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &
			c_b29, &w[i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &w[i__ + 1 + w_dim1]
			, ldw, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b29, &w[
			i__ * w_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[i__ + 1 +
			a_dim1], lda, &w[i__ * w_dim1 + 1], &c__1, &c_b15, &w[
			i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b15, &a[i__ + 1 + a_dim1]
			, lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b29, &w[
			i__ * w_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &w[i__ + 1 +
			w_dim1], ldw, &w[i__ * w_dim1 + 1], &c__1, &c_b15, &w[
			i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		dscal_(&i__2, &tau[i__], &w[i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		alpha = tau[i__] * -.5 * ddot_(&i__2, &w[i__ + 1 + i__ *
			w_dim1], &c__1, &a[i__ + 1 + i__ * a_dim1], &c__1);
		i__2 = *n - i__;
		daxpy_(&i__2, &alpha, &a[i__ + 1 + i__ * a_dim1], &c__1, &w[
			i__ + 1 + i__ * w_dim1], &c__1);
	    }

/* L20: */
	}
    }

    return 0;

/*     End of DLATRD */

} /* dlatrd_ */

/* Subroutine */ int dorg2r_(integer *m, integer *n, integer *k, doublereal *
	a, integer *lda, doublereal *tau, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer i__, j, l;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *), dlarf_(char *, integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DORG2R generates an m by n real matrix Q with orthonormal columns,
    which is defined as the first n columns of a product of k elementary
    reflectors of order m

          Q  =  H(1) H(2) . . . H(k)

    as returned by DGEQRF.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix Q. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix Q. M >= N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines the
            matrix Q. N >= K >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the i-th column must contain the vector which
            defines the elementary reflector H(i), for i = 1,2,...,k, as
            returned by DGEQRF in the first k columns of its array
            argument A.
            On exit, the m-by-n matrix Q.

    LDA     (input) INTEGER
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    WORK    (workspace) DOUBLE PRECISION array, dimension (N)

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument has an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORG2R", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

/*     Initialise columns k+1:n to columns of the unit matrix */

    i__1 = *n;
    for (j = *k + 1; j <= i__1; ++j) {
	i__2 = *m;
	for (l = 1; l <= i__2; ++l) {
	    a[l + j * a_dim1] = 0.;
/* L10: */
	}
	a[j + j * a_dim1] = 1.;
/* L20: */
    }

    for (i__ = *k; i__ >= 1; --i__) {

/*        Apply H(i) to A(i:m,i:n) from the left */

	if (i__ < *n) {
	    a[i__ + i__ * a_dim1] = 1.;
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__;
	    dlarf_("Left", &i__1, &i__2, &a[i__ + i__ * a_dim1], &c__1, &tau[
		    i__], &a[i__ + (i__ + 1) * a_dim1], lda, &work[1]);
	}
	if (i__ < *m) {
	    i__1 = *m - i__;
	    d__1 = -tau[i__];
	    dscal_(&i__1, &d__1, &a[i__ + 1 + i__ * a_dim1], &c__1);
	}
	a[i__ + i__ * a_dim1] = 1. - tau[i__];

/*        Set A(1:i-1,i) to zero */

	i__1 = i__ - 1;
	for (l = 1; l <= i__1; ++l) {
	    a[l + i__ * a_dim1] = 0.;
/* L30: */
	}
/* L40: */
    }
    return 0;

/*     End of DORG2R */

} /* dorg2r_ */

/* Subroutine */ int dorgbr_(char *vect, integer *m, integer *n, integer *k,
	doublereal *a, integer *lda, doublereal *tau, doublereal *work,
	integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, nb, mn;
    extern logical lsame_(char *, char *);
    static integer iinfo;
    static logical wantq;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int dorglq_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    integer *), dorgqr_(integer *, integer *, integer *, doublereal *,
	     integer *, doublereal *, doublereal *, integer *, integer *);
    static integer lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORGBR generates one of the real orthogonal matrices Q or P**T
    determined by DGEBRD when reducing a real matrix A to bidiagonal
    form: A = Q * B * P**T.  Q and P**T are defined as products of
    elementary reflectors H(i) or G(i) respectively.

    If VECT = 'Q', A is assumed to have been an M-by-K matrix, and Q
    is of order M:
    if m >= k, Q = H(1) H(2) . . . H(k) and DORGBR returns the first n
    columns of Q, where m >= n >= k;
    if m < k, Q = H(1) H(2) . . . H(m-1) and DORGBR returns Q as an
    M-by-M matrix.

    If VECT = 'P', A is assumed to have been a K-by-N matrix, and P**T
    is of order N:
    if k < n, P**T = G(k) . . . G(2) G(1) and DORGBR returns the first m
    rows of P**T, where n >= m >= k;
    if k >= n, P**T = G(n-1) . . . G(2) G(1) and DORGBR returns P**T as
    an N-by-N matrix.

    Arguments
    =========

    VECT    (input) CHARACTER*1
            Specifies whether the matrix Q or the matrix P**T is
            required, as defined in the transformation applied by DGEBRD:
            = 'Q':  generate Q;
            = 'P':  generate P**T.

    M       (input) INTEGER
            The number of rows of the matrix Q or P**T to be returned.
            M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix Q or P**T to be returned.
            N >= 0.
            If VECT = 'Q', M >= N >= min(M,K);
            if VECT = 'P', N >= M >= min(N,K).

    K       (input) INTEGER
            If VECT = 'Q', the number of columns in the original M-by-K
            matrix reduced by DGEBRD.
            If VECT = 'P', the number of rows in the original K-by-N
            matrix reduced by DGEBRD.
            K >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the vectors which define the elementary reflectors,
            as returned by DGEBRD.
            On exit, the M-by-N matrix Q or P**T.

    LDA     (input) INTEGER
            The leading dimension of the array A. LDA >= max(1,M).

    TAU     (input) DOUBLE PRECISION array, dimension
                                  (min(M,K)) if VECT = 'Q'
                                  (min(N,K)) if VECT = 'P'
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i) or G(i), which determines Q or P**T, as
            returned by DGEBRD in its array argument TAUQ or TAUP.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK. LWORK >= max(1,min(M,N)).
            For optimum performance LWORK >= min(M,N)*NB, where NB
            is the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    wantq = lsame_(vect, "Q");
    mn = min(*m,*n);
    lquery = *lwork == -1;
    if ((! wantq && ! lsame_(vect, "P"))) {
	*info = -1;
    } else if (*m < 0) {
	*info = -2;
    } else if (*n < 0 || (wantq && (*n > *m || *n < min(*m,*k))) || (! wantq
	    && (*m > *n || *m < min(*n,*k)))) {
	*info = -3;
    } else if (*k < 0) {
	*info = -4;
    } else if (*lda < max(1,*m)) {
	*info = -6;
    } else if ((*lwork < max(1,mn) && ! lquery)) {
	*info = -9;
    }

    if (*info == 0) {
	if (wantq) {
	    nb = ilaenv_(&c__1, "DORGQR", " ", m, n, k, &c_n1, (ftnlen)6, (
		    ftnlen)1);
	} else {
	    nb = ilaenv_(&c__1, "DORGLQ", " ", m, n, k, &c_n1, (ftnlen)6, (
		    ftnlen)1);
	}
	lwkopt = max(1,mn) * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORGBR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	work[1] = 1.;
	return 0;
    }

    if (wantq) {

/*
          Form Q, determined by a call to DGEBRD to reduce an m-by-k
          matrix
*/

	if (*m >= *k) {

/*           If m >= k, assume m >= n >= k */

	    dorgqr_(m, n, k, &a[a_offset], lda, &tau[1], &work[1], lwork, &
		    iinfo);

	} else {

/*
             If m < k, assume m = n

             Shift the vectors which define the elementary reflectors one
             column to the right, and set the first row and column of Q
             to those of the unit matrix
*/

	    for (j = *m; j >= 2; --j) {
		a[j * a_dim1 + 1] = 0.;
		i__1 = *m;
		for (i__ = j + 1; i__ <= i__1; ++i__) {
		    a[i__ + j * a_dim1] = a[i__ + (j - 1) * a_dim1];
/* L10: */
		}
/* L20: */
	    }
	    a[a_dim1 + 1] = 1.;
	    i__1 = *m;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		a[i__ + a_dim1] = 0.;
/* L30: */
	    }
	    if (*m > 1) {

/*              Form Q(2:m,2:m) */

		i__1 = *m - 1;
		i__2 = *m - 1;
		i__3 = *m - 1;
		dorgqr_(&i__1, &i__2, &i__3, &a[((a_dim1) << (1)) + 2], lda, &
			tau[1], &work[1], lwork, &iinfo);
	    }
	}
    } else {

/*
          Form P', determined by a call to DGEBRD to reduce a k-by-n
          matrix
*/

	if (*k < *n) {

/*           If k < n, assume k <= m <= n */

	    dorglq_(m, n, k, &a[a_offset], lda, &tau[1], &work[1], lwork, &
		    iinfo);

	} else {

/*
             If k >= n, assume m = n

             Shift the vectors which define the elementary reflectors one
             row downward, and set the first row and column of P' to
             those of the unit matrix
*/

	    a[a_dim1 + 1] = 1.;
	    i__1 = *n;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		a[i__ + a_dim1] = 0.;
/* L40: */
	    }
	    i__1 = *n;
	    for (j = 2; j <= i__1; ++j) {
		for (i__ = j - 1; i__ >= 2; --i__) {
		    a[i__ + j * a_dim1] = a[i__ - 1 + j * a_dim1];
/* L50: */
		}
		a[j * a_dim1 + 1] = 0.;
/* L60: */
	    }
	    if (*n > 1) {

/*              Form P'(2:n,2:n) */

		i__1 = *n - 1;
		i__2 = *n - 1;
		i__3 = *n - 1;
		dorglq_(&i__1, &i__2, &i__3, &a[((a_dim1) << (1)) + 2], lda, &
			tau[1], &work[1], lwork, &iinfo);
	    }
	}
    }
    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DORGBR */

} /* dorgbr_ */

/* Subroutine */ int dorghr_(integer *n, integer *ilo, integer *ihi,
	doublereal *a, integer *lda, doublereal *tau, doublereal *work,
	integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j, nb, nh, iinfo;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int dorgqr_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    integer *);
    static integer lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORGHR generates a real orthogonal matrix Q which is defined as the
    product of IHI-ILO elementary reflectors of order N, as returned by
    DGEHRD:

    Q = H(ilo) H(ilo+1) . . . H(ihi-1).

    Arguments
    =========

    N       (input) INTEGER
            The order of the matrix Q. N >= 0.

    ILO     (input) INTEGER
    IHI     (input) INTEGER
            ILO and IHI must have the same values as in the previous call
            of DGEHRD. Q is equal to the unit matrix except in the
            submatrix Q(ilo+1:ihi,ilo+1:ihi).
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the vectors which define the elementary reflectors,
            as returned by DGEHRD.
            On exit, the N-by-N orthogonal matrix Q.

    LDA     (input) INTEGER
            The leading dimension of the array A. LDA >= max(1,N).

    TAU     (input) DOUBLE PRECISION array, dimension (N-1)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEHRD.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK. LWORK >= IHI-ILO.
            For optimum performance LWORK >= (IHI-ILO)*NB, where NB is
            the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nh = *ihi - *ilo;
    lquery = *lwork == -1;
    if (*n < 0) {
	*info = -1;
    } else if (*ilo < 1 || *ilo > max(1,*n)) {
	*info = -2;
    } else if (*ihi < min(*ilo,*n) || *ihi > *n) {
	*info = -3;
    } else if (*lda < max(1,*n)) {
	*info = -5;
    } else if ((*lwork < max(1,nh) && ! lquery)) {
	*info = -8;
    }

    if (*info == 0) {
	nb = ilaenv_(&c__1, "DORGQR", " ", &nh, &nh, &nh, &c_n1, (ftnlen)6, (
		ftnlen)1);
	lwkopt = max(1,nh) * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORGHR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	work[1] = 1.;
	return 0;
    }

/*
       Shift the vectors which define the elementary reflectors one
       column to the right, and set the first ilo and the last n-ihi
       rows and columns to those of the unit matrix
*/

    i__1 = *ilo + 1;
    for (j = *ihi; j >= i__1; --j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] = 0.;
/* L10: */
	}
	i__2 = *ihi;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] = a[i__ + (j - 1) * a_dim1];
/* L20: */
	}
	i__2 = *n;
	for (i__ = *ihi + 1; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] = 0.;
/* L30: */
	}
/* L40: */
    }
    i__1 = *ilo;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] = 0.;
/* L50: */
	}
	a[j + j * a_dim1] = 1.;
/* L60: */
    }
    i__1 = *n;
    for (j = *ihi + 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] = 0.;
/* L70: */
	}
	a[j + j * a_dim1] = 1.;
/* L80: */
    }

    if (nh > 0) {

/*        Generate Q(ilo+1:ihi,ilo+1:ihi) */

	dorgqr_(&nh, &nh, &nh, &a[*ilo + 1 + (*ilo + 1) * a_dim1], lda, &tau[*
		ilo], &work[1], lwork, &iinfo);
    }
    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DORGHR */

} /* dorghr_ */

/* Subroutine */ int dorgl2_(integer *m, integer *n, integer *k, doublereal *
	a, integer *lda, doublereal *tau, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer i__, j, l;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *), dlarf_(char *, integer *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *), xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORGL2 generates an m by n real matrix Q with orthonormal rows,
    which is defined as the first m rows of a product of k elementary
    reflectors of order n

          Q  =  H(k) . . . H(2) H(1)

    as returned by DGELQF.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix Q. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix Q. N >= M.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines the
            matrix Q. M >= K >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the i-th row must contain the vector which defines
            the elementary reflector H(i), for i = 1,2,...,k, as returned
            by DGELQF in the first k rows of its array argument A.
            On exit, the m-by-n matrix Q.

    LDA     (input) INTEGER
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGELQF.

    WORK    (workspace) DOUBLE PRECISION array, dimension (M)

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument has an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < *m) {
	*info = -2;
    } else if (*k < 0 || *k > *m) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORGL2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	return 0;
    }

    if (*k < *m) {

/*        Initialise rows k+1:m to rows of the unit matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (l = *k + 1; l <= i__2; ++l) {
		a[l + j * a_dim1] = 0.;
/* L10: */
	    }
	    if ((j > *k && j <= *m)) {
		a[j + j * a_dim1] = 1.;
	    }
/* L20: */
	}
    }

    for (i__ = *k; i__ >= 1; --i__) {

/*        Apply H(i) to A(i:m,i:n) from the right */

	if (i__ < *n) {
	    if (i__ < *m) {
		a[i__ + i__ * a_dim1] = 1.;
		i__1 = *m - i__;
		i__2 = *n - i__ + 1;
		dlarf_("Right", &i__1, &i__2, &a[i__ + i__ * a_dim1], lda, &
			tau[i__], &a[i__ + 1 + i__ * a_dim1], lda, &work[1]);
	    }
	    i__1 = *n - i__;
	    d__1 = -tau[i__];
	    dscal_(&i__1, &d__1, &a[i__ + (i__ + 1) * a_dim1], lda);
	}
	a[i__ + i__ * a_dim1] = 1. - tau[i__];

/*        Set A(i,1:i-1) to zero */

	i__1 = i__ - 1;
	for (l = 1; l <= i__1; ++l) {
	    a[i__ + l * a_dim1] = 0.;
/* L30: */
	}
/* L40: */
    }
    return 0;

/*     End of DORGL2 */

} /* dorgl2_ */

/* Subroutine */ int dorglq_(integer *m, integer *n, integer *k, doublereal *
	a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
	integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, l, ib, nb, ki, kk, nx, iws, nbmin, iinfo;
    extern /* Subroutine */ int dorgl2_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *),
	    dlarfb_(char *, char *, char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, integer *), dlarft_(char *, char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORGLQ generates an M-by-N real matrix Q with orthonormal rows,
    which is defined as the first M rows of a product of K elementary
    reflectors of order N

          Q  =  H(k) . . . H(2) H(1)

    as returned by DGELQF.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix Q. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix Q. N >= M.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines the
            matrix Q. M >= K >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the i-th row must contain the vector which defines
            the elementary reflector H(i), for i = 1,2,...,k, as returned
            by DGELQF in the first k rows of its array argument A.
            On exit, the M-by-N matrix Q.

    LDA     (input) INTEGER
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGELQF.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK. LWORK >= max(1,M).
            For optimum performance LWORK >= M*NB, where NB is
            the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument has an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "DORGLQ", " ", m, n, k, &c_n1, (ftnlen)6, (ftnlen)1);
    lwkopt = max(1,*m) * nb;
    work[1] = (doublereal) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < *m) {
	*info = -2;
    } else if (*k < 0 || *k > *m) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    } else if ((*lwork < max(1,*m) && ! lquery)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORGLQ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *m;
    if ((nb > 1 && nb < *k)) {

/*
          Determine when to cross over from blocked to unblocked code.

   Computing MAX
*/
	i__1 = 0, i__2 = ilaenv_(&c__3, "DORGLQ", " ", m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = max(i__1,i__2);
	if (nx < *k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *m;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*
                Not enough workspace to use optimal NB:  reduce NB and
                determine the minimum value of NB.
*/

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DORGLQ", " ", m, n, k, &c_n1,
			 (ftnlen)6, (ftnlen)1);
		nbmin = max(i__1,i__2);
	    }
	}
    }

    if (((nb >= nbmin && nb < *k) && nx < *k)) {

/*
          Use blocked code after the last block.
          The first kk rows are handled by the block method.
*/

	ki = (*k - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = *k, i__2 = ki + nb;
	kk = min(i__1,i__2);

/*        Set A(kk+1:m,1:kk) to zero. */

	i__1 = kk;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = kk + 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = 0.;
/* L10: */
	    }
/* L20: */
	}
    } else {
	kk = 0;
    }

/*     Use unblocked code for the last or only block. */

    if (kk < *m) {
	i__1 = *m - kk;
	i__2 = *n - kk;
	i__3 = *k - kk;
	dorgl2_(&i__1, &i__2, &i__3, &a[kk + 1 + (kk + 1) * a_dim1], lda, &
		tau[kk + 1], &work[1], &iinfo);
    }

    if (kk > 0) {

/*        Use blocked code */

	i__1 = -nb;
	for (i__ = ki + 1; i__1 < 0 ? i__ >= 1 : i__ <= 1; i__ += i__1) {
/* Computing MIN */
	    i__2 = nb, i__3 = *k - i__ + 1;
	    ib = min(i__2,i__3);
	    if (i__ + ib <= *m) {

/*
                Form the triangular factor of the block reflector
                H = H(i) H(i+1) . . . H(i+ib-1)
*/

		i__2 = *n - i__ + 1;
		dlarft_("Forward", "Rowwise", &i__2, &ib, &a[i__ + i__ *
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H' to A(i+ib:m,i:n) from the right */

		i__2 = *m - i__ - ib + 1;
		i__3 = *n - i__ + 1;
		dlarfb_("Right", "Transpose", "Forward", "Rowwise", &i__2, &
			i__3, &ib, &a[i__ + i__ * a_dim1], lda, &work[1], &
			ldwork, &a[i__ + ib + i__ * a_dim1], lda, &work[ib +
			1], &ldwork);
	    }

/*           Apply H' to columns i:n of current block */

	    i__2 = *n - i__ + 1;
	    dorgl2_(&ib, &i__2, &ib, &a[i__ + i__ * a_dim1], lda, &tau[i__], &
		    work[1], &iinfo);

/*           Set columns 1:i-1 of current block to zero */

	    i__2 = i__ - 1;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + ib - 1;
		for (l = i__; l <= i__3; ++l) {
		    a[l + j * a_dim1] = 0.;
/* L30: */
		}
/* L40: */
	    }
/* L50: */
	}
    }

    work[1] = (doublereal) iws;
    return 0;

/*     End of DORGLQ */

} /* dorglq_ */

/* Subroutine */ int dorgqr_(integer *m, integer *n, integer *k, doublereal *
	a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
	integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, l, ib, nb, ki, kk, nx, iws, nbmin, iinfo;
    extern /* Subroutine */ int dorg2r_(integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *),
	    dlarfb_(char *, char *, char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, integer *, doublereal *, integer *), dlarft_(char *, char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORGQR generates an M-by-N real matrix Q with orthonormal columns,
    which is defined as the first N columns of a product of K elementary
    reflectors of order M

          Q  =  H(1) H(2) . . . H(k)

    as returned by DGEQRF.

    Arguments
    =========

    M       (input) INTEGER
            The number of rows of the matrix Q. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix Q. M >= N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines the
            matrix Q. N >= K >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the i-th column must contain the vector which
            defines the elementary reflector H(i), for i = 1,2,...,k, as
            returned by DGEQRF in the first k columns of its array
            argument A.
            On exit, the M-by-N matrix Q.

    LDA     (input) INTEGER
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK. LWORK >= max(1,N).
            For optimum performance LWORK >= N*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument has an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "DORGQR", " ", m, n, k, &c_n1, (ftnlen)6, (ftnlen)1);
    lwkopt = max(1,*n) * nb;
    work[1] = (doublereal) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    } else if ((*lwork < max(1,*n) && ! lquery)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORGQR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *n;
    if ((nb > 1 && nb < *k)) {

/*
          Determine when to cross over from blocked to unblocked code.

   Computing MAX
*/
	i__1 = 0, i__2 = ilaenv_(&c__3, "DORGQR", " ", m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = max(i__1,i__2);
	if (nx < *k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*
                Not enough workspace to use optimal NB:  reduce NB and
                determine the minimum value of NB.
*/

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DORGQR", " ", m, n, k, &c_n1,
			 (ftnlen)6, (ftnlen)1);
		nbmin = max(i__1,i__2);
	    }
	}
    }

    if (((nb >= nbmin && nb < *k) && nx < *k)) {

/*
          Use blocked code after the last block.
          The first kk columns are handled by the block method.
*/

	ki = (*k - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = *k, i__2 = ki + nb;
	kk = min(i__1,i__2);

/*        Set A(1:kk,kk+1:n) to zero. */

	i__1 = *n;
	for (j = kk + 1; j <= i__1; ++j) {
	    i__2 = kk;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = 0.;
/* L10: */
	    }
/* L20: */
	}
    } else {
	kk = 0;
    }

/*     Use unblocked code for the last or only block. */

    if (kk < *n) {
	i__1 = *m - kk;
	i__2 = *n - kk;
	i__3 = *k - kk;
	dorg2r_(&i__1, &i__2, &i__3, &a[kk + 1 + (kk + 1) * a_dim1], lda, &
		tau[kk + 1], &work[1], &iinfo);
    }

    if (kk > 0) {

/*        Use blocked code */

	i__1 = -nb;
	for (i__ = ki + 1; i__1 < 0 ? i__ >= 1 : i__ <= 1; i__ += i__1) {
/* Computing MIN */
	    i__2 = nb, i__3 = *k - i__ + 1;
	    ib = min(i__2,i__3);
	    if (i__ + ib <= *n) {

/*
                Form the triangular factor of the block reflector
                H = H(i) H(i+1) . . . H(i+ib-1)
*/

		i__2 = *m - i__ + 1;
		dlarft_("Forward", "Columnwise", &i__2, &ib, &a[i__ + i__ *
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(i:m,i+ib:n) from the left */

		i__2 = *m - i__ + 1;
		i__3 = *n - i__ - ib + 1;
		dlarfb_("Left", "No transpose", "Forward", "Columnwise", &
			i__2, &i__3, &ib, &a[i__ + i__ * a_dim1], lda, &work[
			1], &ldwork, &a[i__ + (i__ + ib) * a_dim1], lda, &
			work[ib + 1], &ldwork);
	    }

/*           Apply H to rows i:m of current block */

	    i__2 = *m - i__ + 1;
	    dorg2r_(&i__2, &ib, &ib, &a[i__ + i__ * a_dim1], lda, &tau[i__], &
		    work[1], &iinfo);

/*           Set rows 1:i-1 of current block to zero */

	    i__2 = i__ + ib - 1;
	    for (j = i__; j <= i__2; ++j) {
		i__3 = i__ - 1;
		for (l = 1; l <= i__3; ++l) {
		    a[l + j * a_dim1] = 0.;
/* L30: */
		}
/* L40: */
	    }
/* L50: */
	}
    }

    work[1] = (doublereal) iws;
    return 0;

/*     End of DORGQR */

} /* dorgqr_ */

/* Subroutine */ int dorm2l_(char *side, char *trans, integer *m, integer *n,
	integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *
	c__, integer *ldc, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;

    /* Local variables */
    static integer i__, i1, i2, i3, mi, ni, nq;
    static doublereal aii;
    static logical left;
    extern /* Subroutine */ int dlarf_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static logical notran;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DORM2L overwrites the general real m by n matrix C with

          Q * C  if SIDE = 'L' and TRANS = 'N', or

          Q'* C  if SIDE = 'L' and TRANS = 'T', or

          C * Q  if SIDE = 'R' and TRANS = 'N', or

          C * Q' if SIDE = 'R' and TRANS = 'T',

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(k) . . . H(2) H(1)

    as returned by DGEQLF. Q is of order m if SIDE = 'L' and of order n
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply Q or Q' from the Left
            = 'R': apply Q or Q' from the Right

    TRANS   (input) CHARACTER*1
            = 'N': apply Q  (No transpose)
            = 'T': apply Q' (Transpose)

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) DOUBLE PRECISION array, dimension (LDA,K)
            The i-th column must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGEQLF in the last k columns of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) INTEGER
            The leading dimension of the array A.
            If SIDE = 'L', LDA >= max(1,M);
            if SIDE = 'R', LDA >= max(1,N).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQLF.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) DOUBLE PRECISION array, dimension
                                     (N) if SIDE = 'L',
                                     (M) if SIDE = 'R'

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");

/*     NQ is the order of Q */

    if (left) {
	nq = *m;
    } else {
	nq = *n;
    }
    if ((! left && ! lsame_(side, "R"))) {
	*info = -1;
    } else if ((! notran && ! lsame_(trans, "T"))) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < max(1,nq)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORM2L", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	return 0;
    }

    if ((left && notran) || (! left && ! notran)) {
	i1 = 1;
	i2 = *k;
	i3 = 1;
    } else {
	i1 = *k;
	i2 = 1;
	i3 = -1;
    }

    if (left) {
	ni = *n;
    } else {
	mi = *m;
    }

    i__1 = i2;
    i__2 = i3;
    for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
	if (left) {

/*           H(i) is applied to C(1:m-k+i,1:n) */

	    mi = *m - *k + i__;
	} else {

/*           H(i) is applied to C(1:m,1:n-k+i) */

	    ni = *n - *k + i__;
	}

/*        Apply H(i) */

	aii = a[nq - *k + i__ + i__ * a_dim1];
	a[nq - *k + i__ + i__ * a_dim1] = 1.;
	dlarf_(side, &mi, &ni, &a[i__ * a_dim1 + 1], &c__1, &tau[i__], &c__[
		c_offset], ldc, &work[1]);
	a[nq - *k + i__ + i__ * a_dim1] = aii;
/* L10: */
    }
    return 0;

/*     End of DORM2L */

} /* dorm2l_ */

/* Subroutine */ int dorm2r_(char *side, char *trans, integer *m, integer *n,
	integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *
	c__, integer *ldc, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;

    /* Local variables */
    static integer i__, i1, i2, i3, ic, jc, mi, ni, nq;
    static doublereal aii;
    static logical left;
    extern /* Subroutine */ int dlarf_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static logical notran;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DORM2R overwrites the general real m by n matrix C with

          Q * C  if SIDE = 'L' and TRANS = 'N', or

          Q'* C  if SIDE = 'L' and TRANS = 'T', or

          C * Q  if SIDE = 'R' and TRANS = 'N', or

          C * Q' if SIDE = 'R' and TRANS = 'T',

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(1) H(2) . . . H(k)

    as returned by DGEQRF. Q is of order m if SIDE = 'L' and of order n
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply Q or Q' from the Left
            = 'R': apply Q or Q' from the Right

    TRANS   (input) CHARACTER*1
            = 'N': apply Q  (No transpose)
            = 'T': apply Q' (Transpose)

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) DOUBLE PRECISION array, dimension (LDA,K)
            The i-th column must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGEQRF in the first k columns of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) INTEGER
            The leading dimension of the array A.
            If SIDE = 'L', LDA >= max(1,M);
            if SIDE = 'R', LDA >= max(1,N).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) DOUBLE PRECISION array, dimension
                                     (N) if SIDE = 'L',
                                     (M) if SIDE = 'R'

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");

/*     NQ is the order of Q */

    if (left) {
	nq = *m;
    } else {
	nq = *n;
    }
    if ((! left && ! lsame_(side, "R"))) {
	*info = -1;
    } else if ((! notran && ! lsame_(trans, "T"))) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < max(1,nq)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORM2R", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	return 0;
    }

    if ((left && ! notran) || (! left && notran)) {
	i1 = 1;
	i2 = *k;
	i3 = 1;
    } else {
	i1 = *k;
	i2 = 1;
	i3 = -1;
    }

    if (left) {
	ni = *n;
	jc = 1;
    } else {
	mi = *m;
	ic = 1;
    }

    i__1 = i2;
    i__2 = i3;
    for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
	if (left) {

/*           H(i) is applied to C(i:m,1:n) */

	    mi = *m - i__ + 1;
	    ic = i__;
	} else {

/*           H(i) is applied to C(1:m,i:n) */

	    ni = *n - i__ + 1;
	    jc = i__;
	}

/*        Apply H(i) */

	aii = a[i__ + i__ * a_dim1];
	a[i__ + i__ * a_dim1] = 1.;
	dlarf_(side, &mi, &ni, &a[i__ + i__ * a_dim1], &c__1, &tau[i__], &c__[
		ic + jc * c_dim1], ldc, &work[1]);
	a[i__ + i__ * a_dim1] = aii;
/* L10: */
    }
    return 0;

/*     End of DORM2R */

} /* dorm2r_ */

/* Subroutine */ int dormbr_(char *vect, char *side, char *trans, integer *m,
	integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
	doublereal *c__, integer *ldc, doublereal *work, integer *lwork,
	integer *info)
{
    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2];
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i1, i2, nb, mi, ni, nq, nw;
    static logical left;
    extern logical lsame_(char *, char *);
    static integer iinfo;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int dormlq_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *, integer *);
    static logical notran;
    extern /* Subroutine */ int dormqr_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *, integer *);
    static logical applyq;
    static char transt[1];
    static integer lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    If VECT = 'Q', DORMBR overwrites the general real M-by-N matrix C
    with
                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    If VECT = 'P', DORMBR overwrites the general real M-by-N matrix C
    with
                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      P * C          C * P
    TRANS = 'T':      P**T * C       C * P**T

    Here Q and P**T are the orthogonal matrices determined by DGEBRD when
    reducing a real matrix A to bidiagonal form: A = Q * B * P**T. Q and
    P**T are defined as products of elementary reflectors H(i) and G(i)
    respectively.

    Let nq = m if SIDE = 'L' and nq = n if SIDE = 'R'. Thus nq is the
    order of the orthogonal matrix Q or P**T that is applied.

    If VECT = 'Q', A is assumed to have been an NQ-by-K matrix:
    if nq >= k, Q = H(1) H(2) . . . H(k);
    if nq < k, Q = H(1) H(2) . . . H(nq-1).

    If VECT = 'P', A is assumed to have been a K-by-NQ matrix:
    if k < nq, P = G(1) G(2) . . . G(k);
    if k >= nq, P = G(1) G(2) . . . G(nq-1).

    Arguments
    =========

    VECT    (input) CHARACTER*1
            = 'Q': apply Q or Q**T;
            = 'P': apply P or P**T.

    SIDE    (input) CHARACTER*1
            = 'L': apply Q, Q**T, P or P**T from the Left;
            = 'R': apply Q, Q**T, P or P**T from the Right.

    TRANS   (input) CHARACTER*1
            = 'N':  No transpose, apply Q  or P;
            = 'T':  Transpose, apply Q**T or P**T.

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    K       (input) INTEGER
            If VECT = 'Q', the number of columns in the original
            matrix reduced by DGEBRD.
            If VECT = 'P', the number of rows in the original
            matrix reduced by DGEBRD.
            K >= 0.

    A       (input) DOUBLE PRECISION array, dimension
                                  (LDA,min(nq,K)) if VECT = 'Q'
                                  (LDA,nq)        if VECT = 'P'
            The vectors which define the elementary reflectors H(i) and
            G(i), whose products determine the matrices Q and P, as
            returned by DGEBRD.

    LDA     (input) INTEGER
            The leading dimension of the array A.
            If VECT = 'Q', LDA >= max(1,nq);
            if VECT = 'P', LDA >= max(1,min(nq,K)).

    TAU     (input) DOUBLE PRECISION array, dimension (min(nq,K))
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i) or G(i) which determines Q or P, as returned
            by DGEBRD in the array argument TAUQ or TAUP.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q
            or P*C or P**T*C or C*P or C*P**T.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    applyq = lsame_(vect, "Q");
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

/*     NQ is the order of Q or P and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if ((! applyq && ! lsame_(vect, "P"))) {
	*info = -1;
    } else if ((! left && ! lsame_(side, "R"))) {
	*info = -2;
    } else if ((! notran && ! lsame_(trans, "T"))) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*k < 0) {
	*info = -6;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = 1, i__2 = min(nq,*k);
	if ((applyq && *lda < max(1,nq)) || (! applyq && *lda < max(i__1,i__2)
		)) {
	    *info = -8;
	} else if (*ldc < max(1,*m)) {
	    *info = -11;
	} else if ((*lwork < max(1,nw) && ! lquery)) {
	    *info = -13;
	}
    }

    if (*info == 0) {
	if (applyq) {
	    if (left) {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *m - 1;
		i__2 = *m - 1;
		nb = ilaenv_(&c__1, "DORMQR", ch__1, &i__1, n, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    } else {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *n - 1;
		i__2 = *n - 1;
		nb = ilaenv_(&c__1, "DORMQR", ch__1, m, &i__1, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    }
	} else {
	    if (left) {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *m - 1;
		i__2 = *m - 1;
		nb = ilaenv_(&c__1, "DORMLQ", ch__1, &i__1, n, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    } else {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *n - 1;
		i__2 = *n - 1;
		nb = ilaenv_(&c__1, "DORMLQ", ch__1, m, &i__1, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    }
	}
	lwkopt = max(1,nw) * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORMBR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    work[1] = 1.;
    if (*m == 0 || *n == 0) {
	return 0;
    }

    if (applyq) {

/*        Apply Q */

	if (nq >= *k) {

/*           Q was determined by a call to DGEBRD with nq >= k */

	    dormqr_(side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		    c_offset], ldc, &work[1], lwork, &iinfo);
	} else if (nq > 1) {

/*           Q was determined by a call to DGEBRD with nq < k */

	    if (left) {
		mi = *m - 1;
		ni = *n;
		i1 = 2;
		i2 = 1;
	    } else {
		mi = *m;
		ni = *n - 1;
		i1 = 1;
		i2 = 2;
	    }
	    i__1 = nq - 1;
	    dormqr_(side, trans, &mi, &ni, &i__1, &a[a_dim1 + 2], lda, &tau[1]
		    , &c__[i1 + i2 * c_dim1], ldc, &work[1], lwork, &iinfo);
	}
    } else {

/*        Apply P */

	if (notran) {
	    *(unsigned char *)transt = 'T';
	} else {
	    *(unsigned char *)transt = 'N';
	}
	if (nq > *k) {

/*           P was determined by a call to DGEBRD with nq > k */

	    dormlq_(side, transt, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		    c_offset], ldc, &work[1], lwork, &iinfo);
	} else if (nq > 1) {

/*           P was determined by a call to DGEBRD with nq <= k */

	    if (left) {
		mi = *m - 1;
		ni = *n;
		i1 = 2;
		i2 = 1;
	    } else {
		mi = *m;
		ni = *n - 1;
		i1 = 1;
		i2 = 2;
	    }
	    i__1 = nq - 1;
	    dormlq_(side, transt, &mi, &ni, &i__1, &a[((a_dim1) << (1)) + 1],
		    lda, &tau[1], &c__[i1 + i2 * c_dim1], ldc, &work[1],
		    lwork, &iinfo);
	}
    }
    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DORMBR */

} /* dormbr_ */

/* Subroutine */ int dorml2_(char *side, char *trans, integer *m, integer *n,
	integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *
	c__, integer *ldc, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;

    /* Local variables */
    static integer i__, i1, i2, i3, ic, jc, mi, ni, nq;
    static doublereal aii;
    static logical left;
    extern /* Subroutine */ int dlarf_(char *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static logical notran;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DORML2 overwrites the general real m by n matrix C with

          Q * C  if SIDE = 'L' and TRANS = 'N', or

          Q'* C  if SIDE = 'L' and TRANS = 'T', or

          C * Q  if SIDE = 'R' and TRANS = 'N', or

          C * Q' if SIDE = 'R' and TRANS = 'T',

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(k) . . . H(2) H(1)

    as returned by DGELQF. Q is of order m if SIDE = 'L' and of order n
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply Q or Q' from the Left
            = 'R': apply Q or Q' from the Right

    TRANS   (input) CHARACTER*1
            = 'N': apply Q  (No transpose)
            = 'T': apply Q' (Transpose)

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) DOUBLE PRECISION array, dimension
                                 (LDA,M) if SIDE = 'L',
                                 (LDA,N) if SIDE = 'R'
            The i-th row must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGELQF in the first k rows of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) INTEGER
            The leading dimension of the array A. LDA >= max(1,K).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGELQF.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) DOUBLE PRECISION array, dimension
                                     (N) if SIDE = 'L',
                                     (M) if SIDE = 'R'

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");

/*     NQ is the order of Q */

    if (left) {
	nq = *m;
    } else {
	nq = *n;
    }
    if ((! left && ! lsame_(side, "R"))) {
	*info = -1;
    } else if ((! notran && ! lsame_(trans, "T"))) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < max(1,*k)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORML2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	return 0;
    }

    if ((left && notran) || (! left && ! notran)) {
	i1 = 1;
	i2 = *k;
	i3 = 1;
    } else {
	i1 = *k;
	i2 = 1;
	i3 = -1;
    }

    if (left) {
	ni = *n;
	jc = 1;
    } else {
	mi = *m;
	ic = 1;
    }

    i__1 = i2;
    i__2 = i3;
    for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
	if (left) {

/*           H(i) is applied to C(i:m,1:n) */

	    mi = *m - i__ + 1;
	    ic = i__;
	} else {

/*           H(i) is applied to C(1:m,i:n) */

	    ni = *n - i__ + 1;
	    jc = i__;
	}

/*        Apply H(i) */

	aii = a[i__ + i__ * a_dim1];
	a[i__ + i__ * a_dim1] = 1.;
	dlarf_(side, &mi, &ni, &a[i__ + i__ * a_dim1], lda, &tau[i__], &c__[
		ic + jc * c_dim1], ldc, &work[1]);
	a[i__ + i__ * a_dim1] = aii;
/* L10: */
    }
    return 0;

/*     End of DORML2 */

} /* dorml2_ */

/* Subroutine */ int dormlq_(char *side, char *trans, integer *m, integer *n,
	integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *
	c__, integer *ldc, doublereal *work, integer *lwork, integer *info)
{
    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2], i__4,
	    i__5;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__;
    static doublereal t[4160]	/* was [65][64] */;
    static integer i1, i2, i3, ib, ic, jc, nb, mi, ni, nq, nw, iws;
    static logical left;
    extern logical lsame_(char *, char *);
    static integer nbmin, iinfo;
    extern /* Subroutine */ int dorml2_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *), dlarfb_(char
	    *, char *, char *, char *, integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, integer *), dlarft_(char *, char *, integer *, integer *, doublereal
	    *, integer *, doublereal *, doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static logical notran;
    static integer ldwork;
    static char transt[1];
    static integer lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORMLQ overwrites the general real M-by-N matrix C with

                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(k) . . . H(2) H(1)

    as returned by DGELQF. Q is of order M if SIDE = 'L' and of order N
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply Q or Q**T from the Left;
            = 'R': apply Q or Q**T from the Right.

    TRANS   (input) CHARACTER*1
            = 'N':  No transpose, apply Q;
            = 'T':  Transpose, apply Q**T.

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) DOUBLE PRECISION array, dimension
                                 (LDA,M) if SIDE = 'L',
                                 (LDA,N) if SIDE = 'R'
            The i-th row must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGELQF in the first k rows of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) INTEGER
            The leading dimension of the array A. LDA >= max(1,K).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGELQF.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if ((! left && ! lsame_(side, "R"))) {
	*info = -1;
    } else if ((! notran && ! lsame_(trans, "T"))) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < max(1,*k)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    } else if ((*lwork < max(1,nw) && ! lquery)) {
	*info = -12;
    }

    if (*info == 0) {

/*
          Determine the block size.  NB may be at most NBMAX, where NBMAX
          is used to define the local array T.

   Computing MIN
   Writing concatenation
*/
	i__3[0] = 1, a__1[0] = side;
	i__3[1] = 1, a__1[1] = trans;
	s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	i__1 = 64, i__2 = ilaenv_(&c__1, "DORMLQ", ch__1, m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)2);
	nb = min(i__1,i__2);
	lwkopt = max(1,nw) * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORMLQ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    ldwork = nw;
    if ((nb > 1 && nb < *k)) {
	iws = nw * nb;
	if (*lwork < iws) {
	    nb = *lwork / ldwork;
/*
   Computing MAX
   Writing concatenation
*/
	    i__3[0] = 1, a__1[0] = side;
	    i__3[1] = 1, a__1[1] = trans;
	    s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	    i__1 = 2, i__2 = ilaenv_(&c__2, "DORMLQ", ch__1, m, n, k, &c_n1, (
		    ftnlen)6, (ftnlen)2);
	    nbmin = max(i__1,i__2);
	}
    } else {
	iws = nw;
    }

    if (nb < nbmin || nb >= *k) {

/*        Use unblocked code */

	dorml2_(side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		c_offset], ldc, &work[1], &iinfo);
    } else {

/*        Use blocked code */

	if ((left && notran) || (! left && ! notran)) {
	    i1 = 1;
	    i2 = *k;
	    i3 = nb;
	} else {
	    i1 = (*k - 1) / nb * nb + 1;
	    i2 = 1;
	    i3 = -nb;
	}

	if (left) {
	    ni = *n;
	    jc = 1;
	} else {
	    mi = *m;
	    ic = 1;
	}

	if (notran) {
	    *(unsigned char *)transt = 'T';
	} else {
	    *(unsigned char *)transt = 'N';
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__4 = nb, i__5 = *k - i__ + 1;
	    ib = min(i__4,i__5);

/*
             Form the triangular factor of the block reflector
             H = H(i) H(i+1) . . . H(i+ib-1)
*/

	    i__4 = nq - i__ + 1;
	    dlarft_("Forward", "Rowwise", &i__4, &ib, &a[i__ + i__ * a_dim1],
		    lda, &tau[i__], t, &c__65);
	    if (left) {

/*              H or H' is applied to C(i:m,1:n) */

		mi = *m - i__ + 1;
		ic = i__;
	    } else {

/*              H or H' is applied to C(1:m,i:n) */

		ni = *n - i__ + 1;
		jc = i__;
	    }

/*           Apply H or H' */

	    dlarfb_(side, transt, "Forward", "Rowwise", &mi, &ni, &ib, &a[i__
		    + i__ * a_dim1], lda, t, &c__65, &c__[ic + jc * c_dim1],
		    ldc, &work[1], &ldwork);
/* L10: */
	}
    }
    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DORMLQ */

} /* dormlq_ */

/* Subroutine */ int dormql_(char *side, char *trans, integer *m, integer *n,
	integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *
	c__, integer *ldc, doublereal *work, integer *lwork, integer *info)
{
    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2], i__4,
	    i__5;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__;
    static doublereal t[4160]	/* was [65][64] */;
    static integer i1, i2, i3, ib, nb, mi, ni, nq, nw, iws;
    static logical left;
    extern logical lsame_(char *, char *);
    static integer nbmin, iinfo;
    extern /* Subroutine */ int dorm2l_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *), dlarfb_(char
	    *, char *, char *, char *, integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, integer *), dlarft_(char *, char *, integer *, integer *, doublereal
	    *, integer *, doublereal *, doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static logical notran;
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORMQL overwrites the general real M-by-N matrix C with

                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(k) . . . H(2) H(1)

    as returned by DGEQLF. Q is of order M if SIDE = 'L' and of order N
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply Q or Q**T from the Left;
            = 'R': apply Q or Q**T from the Right.

    TRANS   (input) CHARACTER*1
            = 'N':  No transpose, apply Q;
            = 'T':  Transpose, apply Q**T.

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) DOUBLE PRECISION array, dimension (LDA,K)
            The i-th column must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGEQLF in the last k columns of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) INTEGER
            The leading dimension of the array A.
            If SIDE = 'L', LDA >= max(1,M);
            if SIDE = 'R', LDA >= max(1,N).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQLF.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if ((! left && ! lsame_(side, "R"))) {
	*info = -1;
    } else if ((! notran && ! lsame_(trans, "T"))) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < max(1,nq)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    } else if ((*lwork < max(1,nw) && ! lquery)) {
	*info = -12;
    }

    if (*info == 0) {

/*
          Determine the block size.  NB may be at most NBMAX, where NBMAX
          is used to define the local array T.

   Computing MIN
   Writing concatenation
*/
	i__3[0] = 1, a__1[0] = side;
	i__3[1] = 1, a__1[1] = trans;
	s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	i__1 = 64, i__2 = ilaenv_(&c__1, "DORMQL", ch__1, m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)2);
	nb = min(i__1,i__2);
	lwkopt = max(1,nw) * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORMQL", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    ldwork = nw;
    if ((nb > 1 && nb < *k)) {
	iws = nw * nb;
	if (*lwork < iws) {
	    nb = *lwork / ldwork;
/*
   Computing MAX
   Writing concatenation
*/
	    i__3[0] = 1, a__1[0] = side;
	    i__3[1] = 1, a__1[1] = trans;
	    s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	    i__1 = 2, i__2 = ilaenv_(&c__2, "DORMQL", ch__1, m, n, k, &c_n1, (
		    ftnlen)6, (ftnlen)2);
	    nbmin = max(i__1,i__2);
	}
    } else {
	iws = nw;
    }

    if (nb < nbmin || nb >= *k) {

/*        Use unblocked code */

	dorm2l_(side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		c_offset], ldc, &work[1], &iinfo);
    } else {

/*        Use blocked code */

	if ((left && notran) || (! left && ! notran)) {
	    i1 = 1;
	    i2 = *k;
	    i3 = nb;
	} else {
	    i1 = (*k - 1) / nb * nb + 1;
	    i2 = 1;
	    i3 = -nb;
	}

	if (left) {
	    ni = *n;
	} else {
	    mi = *m;
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__4 = nb, i__5 = *k - i__ + 1;
	    ib = min(i__4,i__5);

/*
             Form the triangular factor of the block reflector
             H = H(i+ib-1) . . . H(i+1) H(i)
*/

	    i__4 = nq - *k + i__ + ib - 1;
	    dlarft_("Backward", "Columnwise", &i__4, &ib, &a[i__ * a_dim1 + 1]
		    , lda, &tau[i__], t, &c__65);
	    if (left) {

/*              H or H' is applied to C(1:m-k+i+ib-1,1:n) */

		mi = *m - *k + i__ + ib - 1;
	    } else {

/*              H or H' is applied to C(1:m,1:n-k+i+ib-1) */

		ni = *n - *k + i__ + ib - 1;
	    }

/*           Apply H or H' */

	    dlarfb_(side, trans, "Backward", "Columnwise", &mi, &ni, &ib, &a[
		    i__ * a_dim1 + 1], lda, t, &c__65, &c__[c_offset], ldc, &
		    work[1], &ldwork);
/* L10: */
	}
    }
    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DORMQL */

} /* dormql_ */

/* Subroutine */ int dormqr_(char *side, char *trans, integer *m, integer *n,
	integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *
	c__, integer *ldc, doublereal *work, integer *lwork, integer *info)
{
    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2], i__4,
	    i__5;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__;
    static doublereal t[4160]	/* was [65][64] */;
    static integer i1, i2, i3, ib, ic, jc, nb, mi, ni, nq, nw, iws;
    static logical left;
    extern logical lsame_(char *, char *);
    static integer nbmin, iinfo;
    extern /* Subroutine */ int dorm2r_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *), dlarfb_(char
	    *, char *, char *, char *, integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, integer *), dlarft_(char *, char *, integer *, integer *, doublereal
	    *, integer *, doublereal *, doublereal *, integer *), xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static logical notran;
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORMQR overwrites the general real M-by-N matrix C with

                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(1) H(2) . . . H(k)

    as returned by DGEQRF. Q is of order M if SIDE = 'L' and of order N
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply Q or Q**T from the Left;
            = 'R': apply Q or Q**T from the Right.

    TRANS   (input) CHARACTER*1
            = 'N':  No transpose, apply Q;
            = 'T':  Transpose, apply Q**T.

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    K       (input) INTEGER
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) DOUBLE PRECISION array, dimension (LDA,K)
            The i-th column must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGEQRF in the first k columns of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) INTEGER
            The leading dimension of the array A.
            If SIDE = 'L', LDA >= max(1,M);
            if SIDE = 'R', LDA >= max(1,N).

    TAU     (input) DOUBLE PRECISION array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if ((! left && ! lsame_(side, "R"))) {
	*info = -1;
    } else if ((! notran && ! lsame_(trans, "T"))) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < max(1,nq)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    } else if ((*lwork < max(1,nw) && ! lquery)) {
	*info = -12;
    }

    if (*info == 0) {

/*
          Determine the block size.  NB may be at most NBMAX, where NBMAX
          is used to define the local array T.

   Computing MIN
   Writing concatenation
*/
	i__3[0] = 1, a__1[0] = side;
	i__3[1] = 1, a__1[1] = trans;
	s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	i__1 = 64, i__2 = ilaenv_(&c__1, "DORMQR", ch__1, m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)2);
	nb = min(i__1,i__2);
	lwkopt = max(1,nw) * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DORMQR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    ldwork = nw;
    if ((nb > 1 && nb < *k)) {
	iws = nw * nb;
	if (*lwork < iws) {
	    nb = *lwork / ldwork;
/*
   Computing MAX
   Writing concatenation
*/
	    i__3[0] = 1, a__1[0] = side;
	    i__3[1] = 1, a__1[1] = trans;
	    s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	    i__1 = 2, i__2 = ilaenv_(&c__2, "DORMQR", ch__1, m, n, k, &c_n1, (
		    ftnlen)6, (ftnlen)2);
	    nbmin = max(i__1,i__2);
	}
    } else {
	iws = nw;
    }

    if (nb < nbmin || nb >= *k) {

/*        Use unblocked code */

	dorm2r_(side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		c_offset], ldc, &work[1], &iinfo);
    } else {

/*        Use blocked code */

	if ((left && ! notran) || (! left && notran)) {
	    i1 = 1;
	    i2 = *k;
	    i3 = nb;
	} else {
	    i1 = (*k - 1) / nb * nb + 1;
	    i2 = 1;
	    i3 = -nb;
	}

	if (left) {
	    ni = *n;
	    jc = 1;
	} else {
	    mi = *m;
	    ic = 1;
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__4 = nb, i__5 = *k - i__ + 1;
	    ib = min(i__4,i__5);

/*
             Form the triangular factor of the block reflector
             H = H(i) H(i+1) . . . H(i+ib-1)
*/

	    i__4 = nq - i__ + 1;
	    dlarft_("Forward", "Columnwise", &i__4, &ib, &a[i__ + i__ *
		    a_dim1], lda, &tau[i__], t, &c__65)
		    ;
	    if (left) {

/*              H or H' is applied to C(i:m,1:n) */

		mi = *m - i__ + 1;
		ic = i__;
	    } else {

/*              H or H' is applied to C(1:m,i:n) */

		ni = *n - i__ + 1;
		jc = i__;
	    }

/*           Apply H or H' */

	    dlarfb_(side, trans, "Forward", "Columnwise", &mi, &ni, &ib, &a[
		    i__ + i__ * a_dim1], lda, t, &c__65, &c__[ic + jc *
		    c_dim1], ldc, &work[1], &ldwork);
/* L10: */
	}
    }
    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DORMQR */

} /* dormqr_ */

/* Subroutine */ int dormtr_(char *side, char *uplo, char *trans, integer *m,
	integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *
	c__, integer *ldc, doublereal *work, integer *lwork, integer *info)
{
    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1[2], i__2, i__3;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i1, i2, nb, mi, ni, nq, nw;
    static logical left;
    extern logical lsame_(char *, char *);
    static integer iinfo;
    static logical upper;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int dormql_(char *, char *, integer *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *, integer *),
	    dormqr_(char *, char *, integer *, integer *, integer *,
	    doublereal *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *, integer *);
    static integer lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DORMTR overwrites the general real M-by-N matrix C with

                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    where Q is a real orthogonal matrix of order nq, with nq = m if
    SIDE = 'L' and nq = n if SIDE = 'R'. Q is defined as the product of
    nq-1 elementary reflectors, as returned by DSYTRD:

    if UPLO = 'U', Q = H(nq-1) . . . H(2) H(1);

    if UPLO = 'L', Q = H(1) H(2) . . . H(nq-1).

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'L': apply Q or Q**T from the Left;
            = 'R': apply Q or Q**T from the Right.

    UPLO    (input) CHARACTER*1
            = 'U': Upper triangle of A contains elementary reflectors
                   from DSYTRD;
            = 'L': Lower triangle of A contains elementary reflectors
                   from DSYTRD.

    TRANS   (input) CHARACTER*1
            = 'N':  No transpose, apply Q;
            = 'T':  Transpose, apply Q**T.

    M       (input) INTEGER
            The number of rows of the matrix C. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix C. N >= 0.

    A       (input) DOUBLE PRECISION array, dimension
                                 (LDA,M) if SIDE = 'L'
                                 (LDA,N) if SIDE = 'R'
            The vectors which define the elementary reflectors, as
            returned by DSYTRD.

    LDA     (input) INTEGER
            The leading dimension of the array A.
            LDA >= max(1,M) if SIDE = 'L'; LDA >= max(1,N) if SIDE = 'R'.

    TAU     (input) DOUBLE PRECISION array, dimension
                                 (M-1) if SIDE = 'L'
                                 (N-1) if SIDE = 'R'
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DSYTRD.

    C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.

    LDC     (input) INTEGER
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================


       Test the input arguments
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1;

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if ((! left && ! lsame_(side, "R"))) {
	*info = -1;
    } else if ((! upper && ! lsame_(uplo, "L"))) {
	*info = -2;
    } else if ((! lsame_(trans, "N") && ! lsame_(trans,
	    "T"))) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < max(1,nq)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    } else if ((*lwork < max(1,nw) && ! lquery)) {
	*info = -12;
    }

    if (*info == 0) {
	if (upper) {
	    if (left) {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (ftnlen)2);
		i__2 = *m - 1;
		i__3 = *m - 1;
		nb = ilaenv_(&c__1, "DORMQL", ch__1, &i__2, n, &i__3, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    } else {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (ftnlen)2);
		i__2 = *n - 1;
		i__3 = *n - 1;
		nb = ilaenv_(&c__1, "DORMQL", ch__1, m, &i__2, &i__3, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    }
	} else {
	    if (left) {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (ftnlen)2);
		i__2 = *m - 1;
		i__3 = *m - 1;
		nb = ilaenv_(&c__1, "DORMQR", ch__1, &i__2, n, &i__3, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    } else {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (ftnlen)2);
		i__2 = *n - 1;
		i__3 = *n - 1;
		nb = ilaenv_(&c__1, "DORMQR", ch__1, m, &i__2, &i__3, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    }
	}
	lwkopt = max(1,nw) * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__2 = -(*info);
	xerbla_("DORMTR", &i__2);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || nq == 1) {
	work[1] = 1.;
	return 0;
    }

    if (left) {
	mi = *m - 1;
	ni = *n;
    } else {
	mi = *m;
	ni = *n - 1;
    }

    if (upper) {

/*        Q was determined by a call to DSYTRD with UPLO = 'U' */

	i__2 = nq - 1;
	dormql_(side, trans, &mi, &ni, &i__2, &a[((a_dim1) << (1)) + 1], lda,
		&tau[1], &c__[c_offset], ldc, &work[1], lwork, &iinfo);
    } else {

/*        Q was determined by a call to DSYTRD with UPLO = 'L' */

	if (left) {
	    i1 = 2;
	    i2 = 1;
	} else {
	    i1 = 1;
	    i2 = 2;
	}
	i__2 = nq - 1;
	dormqr_(side, trans, &mi, &ni, &i__2, &a[a_dim1 + 2], lda, &tau[1], &
		c__[i1 + i2 * c_dim1], ldc, &work[1], lwork, &iinfo);
    }
    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DORMTR */

} /* dormtr_ */

/* Subroutine */ int dpotf2_(char *uplo, integer *n, doublereal *a, integer *
	lda, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer j;
    static doublereal ajj;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
	    integer *);
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *);
    static logical upper;
    extern /* Subroutine */ int xerbla_(char *, integer *);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    DPOTF2 computes the Cholesky factorization of a real symmetric
    positive definite matrix A.

    The factorization has the form
       A = U' * U ,  if UPLO = 'U', or
       A = L  * L',  if UPLO = 'L',
    where U is an upper triangular matrix and L is lower triangular.

    This is the unblocked version of the algorithm, calling Level 2 BLAS.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            Specifies whether the upper or lower triangular part of the
            symmetric matrix A is stored.
            = 'U':  Upper triangular
            = 'L':  Lower triangular

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            n by n upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading n by n lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.

            On exit, if INFO = 0, the factor U or L from the Cholesky
            factorization A = U'*U  or A = L*L'.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    INFO    (output) INTEGER
            = 0: successful exit
            < 0: if INFO = -k, the k-th argument had an illegal value
            > 0: if INFO = k, the leading minor of order k is not
                 positive definite, and the factorization could not be
                 completed.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if ((! upper && ! lsame_(uplo, "L"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DPOTF2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (upper) {

/*        Compute the Cholesky factorization A = U'*U. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {

/*           Compute U(J,J) and test for non-positive-definiteness. */

	    i__2 = j - 1;
	    ajj = a[j + j * a_dim1] - ddot_(&i__2, &a[j * a_dim1 + 1], &c__1,
		    &a[j * a_dim1 + 1], &c__1);
	    if (ajj <= 0.) {
		a[j + j * a_dim1] = ajj;
		goto L30;
	    }
	    ajj = sqrt(ajj);
	    a[j + j * a_dim1] = ajj;

/*           Compute elements J+1:N of row J. */

	    if (j < *n) {
		i__2 = j - 1;
		i__3 = *n - j;
		dgemv_("Transpose", &i__2, &i__3, &c_b151, &a[(j + 1) *
			a_dim1 + 1], lda, &a[j * a_dim1 + 1], &c__1, &c_b15, &
			a[j + (j + 1) * a_dim1], lda);
		i__2 = *n - j;
		d__1 = 1. / ajj;
		dscal_(&i__2, &d__1, &a[j + (j + 1) * a_dim1], lda);
	    }
/* L10: */
	}
    } else {

/*        Compute the Cholesky factorization A = L*L'. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {

/*           Compute L(J,J) and test for non-positive-definiteness. */

	    i__2 = j - 1;
	    ajj = a[j + j * a_dim1] - ddot_(&i__2, &a[j + a_dim1], lda, &a[j
		    + a_dim1], lda);
	    if (ajj <= 0.) {
		a[j + j * a_dim1] = ajj;
		goto L30;
	    }
	    ajj = sqrt(ajj);
	    a[j + j * a_dim1] = ajj;

/*           Compute elements J+1:N of column J. */

	    if (j < *n) {
		i__2 = *n - j;
		i__3 = j - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b151, &a[j + 1 +
			a_dim1], lda, &a[j + a_dim1], lda, &c_b15, &a[j + 1 +
			j * a_dim1], &c__1);
		i__2 = *n - j;
		d__1 = 1. / ajj;
		dscal_(&i__2, &d__1, &a[j + 1 + j * a_dim1], &c__1);
	    }
/* L20: */
	}
    }
    goto L40;

L30:
    *info = j;

L40:
    return 0;

/*     End of DPOTF2 */

} /* dpotf2_ */

/* Subroutine */ int dpotrf_(char *uplo, integer *n, doublereal *a, integer *
	lda, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer j, jb, nb;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dtrsm_(char *, char *, char *, char *,
	    integer *, integer *, doublereal *, doublereal *, integer *,
	    doublereal *, integer *);
    static logical upper;
    extern /* Subroutine */ int dsyrk_(char *, char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *,
	     integer *), dpotf2_(char *, integer *,
	    doublereal *, integer *, integer *), xerbla_(char *,
	    integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       March 31, 1993


    Purpose
    =======

    DPOTRF computes the Cholesky factorization of a real symmetric
    positive definite matrix A.

    The factorization has the form
       A = U**T * U,  if UPLO = 'U', or
       A = L  * L**T,  if UPLO = 'L',
    where U is an upper triangular matrix and L is lower triangular.

    This is the block version of the algorithm, calling Level 3 BLAS.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            = 'U':  Upper triangle of A is stored;
            = 'L':  Lower triangle of A is stored.

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            N-by-N upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading N-by-N lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.

            On exit, if INFO = 0, the factor U or L from the Cholesky
            factorization A = U**T*U or A = L*L**T.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, the leading minor of order i is not
                  positive definite, and the factorization could not be
                  completed.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if ((! upper && ! lsame_(uplo, "L"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DPOTRF", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine the block size for this environment. */

    nb = ilaenv_(&c__1, "DPOTRF", uplo, n, &c_n1, &c_n1, &c_n1, (ftnlen)6, (
	    ftnlen)1);
    if (nb <= 1 || nb >= *n) {

/*        Use unblocked code. */

	dpotf2_(uplo, n, &a[a_offset], lda, info);
    } else {

/*        Use blocked code. */

	if (upper) {

/*           Compute the Cholesky factorization A = U'*U. */

	    i__1 = *n;
	    i__2 = nb;
	    for (j = 1; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*
                Update and factorize the current diagonal block and test
                for non-positive-definiteness.

   Computing MIN
*/
		i__3 = nb, i__4 = *n - j + 1;
		jb = min(i__3,i__4);
		i__3 = j - 1;
		dsyrk_("Upper", "Transpose", &jb, &i__3, &c_b151, &a[j *
			a_dim1 + 1], lda, &c_b15, &a[j + j * a_dim1], lda);
		dpotf2_("Upper", &jb, &a[j + j * a_dim1], lda, info);
		if (*info != 0) {
		    goto L30;
		}
		if (j + jb <= *n) {

/*                 Compute the current block row. */

		    i__3 = *n - j - jb + 1;
		    i__4 = j - 1;
		    dgemm_("Transpose", "No transpose", &jb, &i__3, &i__4, &
			    c_b151, &a[j * a_dim1 + 1], lda, &a[(j + jb) *
			    a_dim1 + 1], lda, &c_b15, &a[j + (j + jb) *
			    a_dim1], lda);
		    i__3 = *n - j - jb + 1;
		    dtrsm_("Left", "Upper", "Transpose", "Non-unit", &jb, &
			    i__3, &c_b15, &a[j + j * a_dim1], lda, &a[j + (j
			    + jb) * a_dim1], lda);
		}
/* L10: */
	    }

	} else {

/*           Compute the Cholesky factorization A = L*L'. */

	    i__2 = *n;
	    i__1 = nb;
	    for (j = 1; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*
                Update and factorize the current diagonal block and test
                for non-positive-definiteness.

   Computing MIN
*/
		i__3 = nb, i__4 = *n - j + 1;
		jb = min(i__3,i__4);
		i__3 = j - 1;
		dsyrk_("Lower", "No transpose", &jb, &i__3, &c_b151, &a[j +
			a_dim1], lda, &c_b15, &a[j + j * a_dim1], lda);
		dpotf2_("Lower", &jb, &a[j + j * a_dim1], lda, info);
		if (*info != 0) {
		    goto L30;
		}
		if (j + jb <= *n) {

/*                 Compute the current block column. */

		    i__3 = *n - j - jb + 1;
		    i__4 = j - 1;
		    dgemm_("No transpose", "Transpose", &i__3, &jb, &i__4, &
			    c_b151, &a[j + jb + a_dim1], lda, &a[j + a_dim1],
			    lda, &c_b15, &a[j + jb + j * a_dim1], lda);
		    i__3 = *n - j - jb + 1;
		    dtrsm_("Right", "Lower", "Transpose", "Non-unit", &i__3, &
			    jb, &c_b15, &a[j + j * a_dim1], lda, &a[j + jb +
			    j * a_dim1], lda);
		}
/* L20: */
	    }
	}
    }
    goto L40;

L30:
    *info = *info + j - 1;

L40:
    return 0;

/*     End of DPOTRF */

} /* dpotrf_ */

/* Subroutine */ int dstedc_(char *compz, integer *n, doublereal *d__,
	doublereal *e, doublereal *z__, integer *ldz, doublereal *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double log(doublereal);
    integer pow_ii(integer *, integer *);
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j, k, m;
    static doublereal p;
    static integer ii, end, lgn;
    static doublereal eps, tiny;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *,
	    integer *, doublereal *, doublereal *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dswap_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static integer lwmin;
    extern /* Subroutine */ int dlaed0_(integer *, integer *, integer *,
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     integer *, doublereal *, integer *, integer *);
    static integer start;

    extern /* Subroutine */ int dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *), dlacpy_(char *, integer *, integer
	    *, doublereal *, integer *, doublereal *, integer *),
	    dlaset_(char *, integer *, integer *, doublereal *, doublereal *,
	    doublereal *, integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern doublereal dlanst_(char *, integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int dsterf_(integer *, doublereal *, doublereal *,
	     integer *), dlasrt_(char *, integer *, doublereal *, integer *);
    static integer liwmin, icompz;
    extern /* Subroutine */ int dsteqr_(char *, integer *, doublereal *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *);
    static doublereal orgnrm;
    static logical lquery;
    static integer smlsiz, dtrtrw, storez;


/*
    -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DSTEDC computes all eigenvalues and, optionally, eigenvectors of a
    symmetric tridiagonal matrix using the divide and conquer method.
    The eigenvectors of a full or band real symmetric matrix can also be
    found if DSYTRD or DSPTRD or DSBTRD has been used to reduce this
    matrix to tridiagonal form.

    This code makes very mild assumptions about floating point
    arithmetic. It will work on machines with a guard digit in
    add/subtract, or on those binary machines without guard digits
    which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or Cray-2.
    It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.  See DLAED3 for details.

    Arguments
    =========

    COMPZ   (input) CHARACTER*1
            = 'N':  Compute eigenvalues only.
            = 'I':  Compute eigenvectors of tridiagonal matrix also.
            = 'V':  Compute eigenvectors of original dense symmetric
                    matrix also.  On entry, Z contains the orthogonal
                    matrix used to reduce the original matrix to
                    tridiagonal form.

    N       (input) INTEGER
            The dimension of the symmetric tridiagonal matrix.  N >= 0.

    D       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the diagonal elements of the tridiagonal matrix.
            On exit, if INFO = 0, the eigenvalues in ascending order.

    E       (input/output) DOUBLE PRECISION array, dimension (N-1)
            On entry, the subdiagonal elements of the tridiagonal matrix.
            On exit, E has been destroyed.

    Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N)
            On entry, if COMPZ = 'V', then Z contains the orthogonal
            matrix used in the reduction to tridiagonal form.
            On exit, if INFO = 0, then if COMPZ = 'V', Z contains the
            orthonormal eigenvectors of the original symmetric matrix,
            and if COMPZ = 'I', Z contains the orthonormal eigenvectors
            of the symmetric tridiagonal matrix.
            If  COMPZ = 'N', then Z is not referenced.

    LDZ     (input) INTEGER
            The leading dimension of the array Z.  LDZ >= 1.
            If eigenvectors are desired, then LDZ >= max(1,N).

    WORK    (workspace/output) DOUBLE PRECISION array,
                                           dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.
            If COMPZ = 'N' or N <= 1 then LWORK must be at least 1.
            If COMPZ = 'V' and N > 1 then LWORK must be at least
                           ( 1 + 3*N + 2*N*lg N + 3*N**2 ),
                           where lg( N ) = smallest integer k such
                           that 2**k >= N.
            If COMPZ = 'I' and N > 1 then LWORK must be at least
                           ( 1 + 4*N + N**2 ).

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    IWORK   (workspace/output) INTEGER array, dimension (LIWORK)
            On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK.

    LIWORK  (input) INTEGER
            The dimension of the array IWORK.
            If COMPZ = 'N' or N <= 1 then LIWORK must be at least 1.
            If COMPZ = 'V' and N > 1 then LIWORK must be at least
                           ( 6 + 6*N + 5*N*lg N ).
            If COMPZ = 'I' and N > 1 then LIWORK must be at least
                           ( 3 + 5*N ).

            If LIWORK = -1, then a workspace query is assumed; the
            routine only calculates the optimal size of the IWORK array,
            returns this value as the first entry of the IWORK array, and
            no error message related to LIWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  The algorithm failed to compute an eigenvalue while
                  working on the submatrix lying in rows and columns
                  INFO/(N+1) through mod(INFO,N+1).

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA
    Modified by Francoise Tisseur, University of Tennessee.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1 || *liwork == -1;

    if (lsame_(compz, "N")) {
	icompz = 0;
    } else if (lsame_(compz, "V")) {
	icompz = 1;
    } else if (lsame_(compz, "I")) {
	icompz = 2;
    } else {
	icompz = -1;
    }
    if (*n <= 1 || icompz <= 0) {
	liwmin = 1;
	lwmin = 1;
    } else {
	lgn = (integer) (log((doublereal) (*n)) / log(2.));
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	if (icompz == 1) {
/* Computing 2nd power */
	    i__1 = *n;
	    lwmin = *n * 3 + 1 + ((*n) << (1)) * lgn + i__1 * i__1 * 3;
	    liwmin = *n * 6 + 6 + *n * 5 * lgn;
	} else if (icompz == 2) {
/* Computing 2nd power */
	    i__1 = *n;
	    lwmin = ((*n) << (2)) + 1 + i__1 * i__1;
	    liwmin = *n * 5 + 3;
	}
    }
    if (icompz < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || (icompz > 0 && *ldz < max(1,*n))) {
	*info = -6;
    } else if ((*lwork < lwmin && ! lquery)) {
	*info = -8;
    } else if ((*liwork < liwmin && ! lquery)) {
	*info = -10;
    }

    if (*info == 0) {
	work[1] = (doublereal) lwmin;
	iwork[1] = liwmin;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEDC", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*n == 1) {
	if (icompz != 0) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

    smlsiz = ilaenv_(&c__9, "DSTEDC", " ", &c__0, &c__0, &c__0, &c__0, (
	    ftnlen)6, (ftnlen)1);

/*
       If the following conditional clause is removed, then the routine
       will use the Divide and Conquer routine to compute only the
       eigenvalues, which requires (3N + 3N**2) real workspace and
       (2 + 5N + 2N lg(N)) integer workspace.
       Since on many architectures DSTERF is much faster than any other
       algorithm for finding eigenvalues only, it is used here
       as the default.

       If COMPZ = 'N', use DSTERF to compute the eigenvalues.
*/

    if (icompz == 0) {
	dsterf_(n, &d__[1], &e[1], info);
	return 0;
    }

/*
       If N is smaller than the minimum divide size (SMLSIZ+1), then
       solve the problem with another solver.
*/

    if (*n <= smlsiz) {
	if (icompz == 0) {
	    dsterf_(n, &d__[1], &e[1], info);
	    return 0;
	} else if (icompz == 2) {
	    dsteqr_("I", n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1],
		    info);
	    return 0;
	} else {
	    dsteqr_("V", n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1],
		    info);
	    return 0;
	}
    }

/*
       If COMPZ = 'V', the Z matrix must be stored elsewhere for later
       use.
*/

    if (icompz == 1) {
	storez = *n * *n + 1;
    } else {
	storez = 1;
    }

    if (icompz == 2) {
	dlaset_("Full", n, n, &c_b29, &c_b15, &z__[z_offset], ldz);
    }

/*     Scale. */

    orgnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (orgnrm == 0.) {
	return 0;
    }

    eps = EPSILON;

    start = 1;

/*     while ( START <= N ) */

L10:
    if (start <= *n) {

/*
       Let END be the position of the next subdiagonal entry such that
       E( END ) <= TINY or END = N if no such subdiagonal exists.  The
       matrix identified by the elements between START and END
       constitutes an independent sub-problem.
*/

	end = start;
L20:
	if (end < *n) {
	    tiny = eps * sqrt((d__1 = d__[end], abs(d__1))) * sqrt((d__2 =
		    d__[end + 1], abs(d__2)));
	    if ((d__1 = e[end], abs(d__1)) > tiny) {
		++end;
		goto L20;
	    }
	}

/*        (Sub) Problem determined.  Compute its size and solve it. */

	m = end - start + 1;
	if (m == 1) {
	    start = end + 1;
	    goto L10;
	}
	if (m > smlsiz) {
	    *info = smlsiz;

/*           Scale. */

	    orgnrm = dlanst_("M", &m, &d__[start], &e[start]);
	    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, &m, &c__1, &d__[start]
		    , &m, info);
	    i__1 = m - 1;
	    i__2 = m - 1;
	    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b15, &i__1, &c__1, &e[
		    start], &i__2, info);

	    if (icompz == 1) {
		dtrtrw = 1;
	    } else {
		dtrtrw = start;
	    }
	    dlaed0_(&icompz, n, &m, &d__[start], &e[start], &z__[dtrtrw +
		    start * z_dim1], ldz, &work[1], n, &work[storez], &iwork[
		    1], info);
	    if (*info != 0) {
		*info = (*info / (m + 1) + start - 1) * (*n + 1) + *info % (m
			+ 1) + start - 1;
		return 0;
	    }

/*           Scale back. */

	    dlascl_("G", &c__0, &c__0, &c_b15, &orgnrm, &m, &c__1, &d__[start]
		    , &m, info);

	} else {
	    if (icompz == 1) {

/*
       Since QR won't update a Z matrix which is larger than the
       length of D, we must solve the sub-problem in a workspace and
       then multiply back into Z.
*/

		dsteqr_("I", &m, &d__[start], &e[start], &work[1], &m, &work[
			m * m + 1], info);
		dlacpy_("A", n, &m, &z__[start * z_dim1 + 1], ldz, &work[
			storez], n);
		dgemm_("N", "N", n, &m, &m, &c_b15, &work[storez], ldz, &work[
			1], &m, &c_b29, &z__[start * z_dim1 + 1], ldz);
	    } else if (icompz == 2) {
		dsteqr_("I", &m, &d__[start], &e[start], &z__[start + start *
			z_dim1], ldz, &work[1], info);
	    } else {
		dsterf_(&m, &d__[start], &e[start], info);
	    }
	    if (*info != 0) {
		*info = start * (*n + 1) + end;
		return 0;
	    }
	}

	start = end + 1;
	goto L10;
    }

/*
       endwhile

       If the problem split any number of times, then the eigenvalues
       will not be properly ordered.  Here we permute the eigenvalues
       (and the associated eigenvectors) into ascending order.
*/

    if (m != *n) {
	if (icompz == 0) {

/*        Use Quick Sort */

	    dlasrt_("I", n, &d__[1], info);

	} else {

/*        Use Selection Sort to minimize swaps of eigenvectors */

	    i__1 = *n;
	    for (ii = 2; ii <= i__1; ++ii) {
		i__ = ii - 1;
		k = i__;
		p = d__[i__];
		i__2 = *n;
		for (j = ii; j <= i__2; ++j) {
		    if (d__[j] < p) {
			k = j;
			p = d__[j];
		    }
/* L30: */
		}
		if (k != i__) {
		    d__[k] = d__[i__];
		    d__[i__] = p;
		    dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[k * z_dim1
			    + 1], &c__1);
		}
/* L40: */
	    }
	}
    }

    work[1] = (doublereal) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of DSTEDC */

} /* dstedc_ */

/* Subroutine */ int dsteqr_(char *compz, integer *n, doublereal *d__,
	doublereal *e, doublereal *z__, integer *ldz, doublereal *work,
	integer *info)
{
    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static doublereal b, c__, f, g;
    static integer i__, j, k, l, m;
    static doublereal p, r__, s;
    static integer l1, ii, mm, lm1, mm1, nm1;
    static doublereal rt1, rt2, eps;
    static integer lsv;
    static doublereal tst, eps2;
    static integer lend, jtot;
    extern /* Subroutine */ int dlae2_(doublereal *, doublereal *, doublereal
	    *, doublereal *, doublereal *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dlasr_(char *, char *, char *, integer *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *);
    static doublereal anorm;
    extern /* Subroutine */ int dswap_(integer *, doublereal *, integer *,
	    doublereal *, integer *), dlaev2_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *);
    static integer lendm1, lendp1;

    static integer iscale;
    extern /* Subroutine */ int dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *), dlaset_(char *, integer *, integer
	    *, doublereal *, doublereal *, doublereal *, integer *);
    static doublereal safmin;
    extern /* Subroutine */ int dlartg_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *);
    static doublereal safmax;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern doublereal dlanst_(char *, integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int dlasrt_(char *, integer *, doublereal *,
	    integer *);
    static integer lendsv;
    static doublereal ssfmin;
    static integer nmaxit, icompz;
    static doublereal ssfmax;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    DSTEQR computes all eigenvalues and, optionally, eigenvectors of a
    symmetric tridiagonal matrix using the implicit QL or QR method.
    The eigenvectors of a full or band symmetric matrix can also be found
    if DSYTRD or DSPTRD or DSBTRD has been used to reduce this matrix to
    tridiagonal form.

    Arguments
    =========

    COMPZ   (input) CHARACTER*1
            = 'N':  Compute eigenvalues only.
            = 'V':  Compute eigenvalues and eigenvectors of the original
                    symmetric matrix.  On entry, Z must contain the
                    orthogonal matrix used to reduce the original matrix
                    to tridiagonal form.
            = 'I':  Compute eigenvalues and eigenvectors of the
                    tridiagonal matrix.  Z is initialized to the identity
                    matrix.

    N       (input) INTEGER
            The order of the matrix.  N >= 0.

    D       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the diagonal elements of the tridiagonal matrix.
            On exit, if INFO = 0, the eigenvalues in ascending order.

    E       (input/output) DOUBLE PRECISION array, dimension (N-1)
            On entry, the (n-1) subdiagonal elements of the tridiagonal
            matrix.
            On exit, E has been destroyed.

    Z       (input/output) DOUBLE PRECISION array, dimension (LDZ, N)
            On entry, if  COMPZ = 'V', then Z contains the orthogonal
            matrix used in the reduction to tridiagonal form.
            On exit, if INFO = 0, then if  COMPZ = 'V', Z contains the
            orthonormal eigenvectors of the original symmetric matrix,
            and if COMPZ = 'I', Z contains the orthonormal eigenvectors
            of the symmetric tridiagonal matrix.
            If COMPZ = 'N', then Z is not referenced.

    LDZ     (input) INTEGER
            The leading dimension of the array Z.  LDZ >= 1, and if
            eigenvectors are desired, then  LDZ >= max(1,N).

    WORK    (workspace) DOUBLE PRECISION array, dimension (max(1,2*N-2))
            If COMPZ = 'N', then WORK is not referenced.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  the algorithm has failed to find all the eigenvalues in
                  a total of 30*N iterations; if INFO = i, then i
                  elements of E have not converged to zero; on exit, D
                  and E contain the elements of a symmetric tridiagonal
                  matrix which is orthogonally similar to the original
                  matrix.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --d__;
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;

    if (lsame_(compz, "N")) {
	icompz = 0;
    } else if (lsame_(compz, "V")) {
	icompz = 1;
    } else if (lsame_(compz, "I")) {
	icompz = 2;
    } else {
	icompz = -1;
    }
    if (icompz < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || (icompz > 0 && *ldz < max(1,*n))) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEQR", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (icompz == 2) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Determine the unit roundoff and over/underflow thresholds. */

    eps = EPSILON;
/* Computing 2nd power */
    d__1 = eps;
    eps2 = d__1 * d__1;
    safmin = SAFEMINIMUM;
    safmax = 1. / safmin;
    ssfmax = sqrt(safmax) / 3.;
    ssfmin = sqrt(safmin) / eps2;

/*
       Compute the eigenvalues and eigenvectors of the tridiagonal
       matrix.
*/

    if (icompz == 2) {
	dlaset_("Full", n, n, &c_b29, &c_b15, &z__[z_offset], ldz);
    }

    nmaxit = *n * 30;
    jtot = 0;

/*
       Determine where the matrix splits and choose QL or QR iteration
       for each block, according to whether top or bottom diagonal
       element is smaller.
*/

    l1 = 1;
    nm1 = *n - 1;

L10:
    if (l1 > *n) {
	goto L160;
    }
    if (l1 > 1) {
	e[l1 - 1] = 0.;
    }
    if (l1 <= nm1) {
	i__1 = nm1;
	for (m = l1; m <= i__1; ++m) {
	    tst = (d__1 = e[m], abs(d__1));
	    if (tst == 0.) {
		goto L30;
	    }
	    if (tst <= sqrt((d__1 = d__[m], abs(d__1))) * sqrt((d__2 = d__[m
		    + 1], abs(d__2))) * eps) {
		e[m] = 0.;
		goto L30;
	    }
/* L20: */
	}
    }
    m = *n;

L30:
    l = l1;
    lsv = l;
    lend = m;
    lendsv = lend;
    l1 = m + 1;
    if (lend == l) {
	goto L10;
    }

/*     Scale submatrix in rows and columns L to LEND */

    i__1 = lend - l + 1;
    anorm = dlanst_("I", &i__1, &d__[l], &e[l]);
    iscale = 0;
    if (anorm == 0.) {
	goto L10;
    }
    if (anorm > ssfmax) {
	iscale = 1;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n,
		info);
    } else if (anorm < ssfmin) {
	iscale = 2;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n,
		info);
    }

/*     Choose between QL and QR iteration */

    if ((d__1 = d__[lend], abs(d__1)) < (d__2 = d__[l], abs(d__2))) {
	lend = lsv;
	l = lendsv;
    }

    if (lend > l) {

/*
          QL Iteration

          Look for small subdiagonal element.
*/

L40:
	if (l != lend) {
	    lendm1 = lend - 1;
	    i__1 = lendm1;
	    for (m = l; m <= i__1; ++m) {
/* Computing 2nd power */
		d__2 = (d__1 = e[m], abs(d__1));
		tst = d__2 * d__2;
		if (tst <= eps2 * (d__1 = d__[m], abs(d__1)) * (d__2 = d__[m
			+ 1], abs(d__2)) + safmin) {
		    goto L60;
		}
/* L50: */
	    }
	}

	m = lend;

L60:
	if (m < lend) {
	    e[m] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L80;
	}

/*
          If remaining matrix is 2-by-2, use DLAE2 or SLAEV2
          to compute its eigensystem.
*/

	if (m == l + 1) {
	    if (icompz > 0) {
		dlaev2_(&d__[l], &e[l], &d__[l + 1], &rt1, &rt2, &c__, &s);
		work[l] = c__;
		work[*n - 1 + l] = s;
		dlasr_("R", "V", "B", n, &c__2, &work[l], &work[*n - 1 + l], &
			z__[l * z_dim1 + 1], ldz);
	    } else {
		dlae2_(&d__[l], &e[l], &d__[l + 1], &rt1, &rt2);
	    }
	    d__[l] = rt1;
	    d__[l + 1] = rt2;
	    e[l] = 0.;
	    l += 2;
	    if (l <= lend) {
		goto L40;
	    }
	    goto L140;
	}

	if (jtot == nmaxit) {
	    goto L140;
	}
	++jtot;

/*        Form shift. */

	g = (d__[l + 1] - p) / (e[l] * 2.);
	r__ = dlapy2_(&g, &c_b15);
	g = d__[m] - p + e[l] / (g + d_sign(&r__, &g));

	s = 1.;
	c__ = 1.;
	p = 0.;

/*        Inner loop */

	mm1 = m - 1;
	i__1 = l;
	for (i__ = mm1; i__ >= i__1; --i__) {
	    f = s * e[i__];
	    b = c__ * e[i__];
	    dlartg_(&g, &f, &c__, &s, &r__);
	    if (i__ != m - 1) {
		e[i__ + 1] = r__;
	    }
	    g = d__[i__ + 1] - p;
	    r__ = (d__[i__] - g) * s + c__ * 2. * b;
	    p = s * r__;
	    d__[i__ + 1] = g + p;
	    g = c__ * r__ - b;

/*           If eigenvectors are desired, then save rotations. */

	    if (icompz > 0) {
		work[i__] = c__;
		work[*n - 1 + i__] = -s;
	    }

/* L70: */
	}

/*        If eigenvectors are desired, then apply saved rotations. */

	if (icompz > 0) {
	    mm = m - l + 1;
	    dlasr_("R", "V", "B", n, &mm, &work[l], &work[*n - 1 + l], &z__[l
		    * z_dim1 + 1], ldz);
	}

	d__[l] -= p;
	e[l] = g;
	goto L40;

/*        Eigenvalue found. */

L80:
	d__[l] = p;

	++l;
	if (l <= lend) {
	    goto L40;
	}
	goto L140;

    } else {

/*
          QR Iteration

          Look for small superdiagonal element.
*/

L90:
	if (l != lend) {
	    lendp1 = lend + 1;
	    i__1 = lendp1;
	    for (m = l; m >= i__1; --m) {
/* Computing 2nd power */
		d__2 = (d__1 = e[m - 1], abs(d__1));
		tst = d__2 * d__2;
		if (tst <= eps2 * (d__1 = d__[m], abs(d__1)) * (d__2 = d__[m
			- 1], abs(d__2)) + safmin) {
		    goto L110;
		}
/* L100: */
	    }
	}

	m = lend;

L110:
	if (m > lend) {
	    e[m - 1] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L130;
	}

/*
          If remaining matrix is 2-by-2, use DLAE2 or SLAEV2
          to compute its eigensystem.
*/

	if (m == l - 1) {
	    if (icompz > 0) {
		dlaev2_(&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2, &c__, &s)
			;
		work[m] = c__;
		work[*n - 1 + m] = s;
		dlasr_("R", "V", "F", n, &c__2, &work[m], &work[*n - 1 + m], &
			z__[(l - 1) * z_dim1 + 1], ldz);
	    } else {
		dlae2_(&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2);
	    }
	    d__[l - 1] = rt1;
	    d__[l] = rt2;
	    e[l - 1] = 0.;
	    l += -2;
	    if (l >= lend) {
		goto L90;
	    }
	    goto L140;
	}

	if (jtot == nmaxit) {
	    goto L140;
	}
	++jtot;

/*        Form shift. */

	g = (d__[l - 1] - p) / (e[l - 1] * 2.);
	r__ = dlapy2_(&g, &c_b15);
	g = d__[m] - p + e[l - 1] / (g + d_sign(&r__, &g));

	s = 1.;
	c__ = 1.;
	p = 0.;

/*        Inner loop */

	lm1 = l - 1;
	i__1 = lm1;
	for (i__ = m; i__ <= i__1; ++i__) {
	    f = s * e[i__];
	    b = c__ * e[i__];
	    dlartg_(&g, &f, &c__, &s, &r__);
	    if (i__ != m) {
		e[i__ - 1] = r__;
	    }
	    g = d__[i__] - p;
	    r__ = (d__[i__ + 1] - g) * s + c__ * 2. * b;
	    p = s * r__;
	    d__[i__] = g + p;
	    g = c__ * r__ - b;

/*           If eigenvectors are desired, then save rotations. */

	    if (icompz > 0) {
		work[i__] = c__;
		work[*n - 1 + i__] = s;
	    }

/* L120: */
	}

/*        If eigenvectors are desired, then apply saved rotations. */

	if (icompz > 0) {
	    mm = l - m + 1;
	    dlasr_("R", "V", "F", n, &mm, &work[m], &work[*n - 1 + m], &z__[m
		    * z_dim1 + 1], ldz);
	}

	d__[l] -= p;
	e[lm1] = g;
	goto L90;

/*        Eigenvalue found. */

L130:
	d__[l] = p;

	--l;
	if (l >= lend) {
	    goto L90;
	}
	goto L140;

    }

/*     Undo scaling if necessary */

L140:
    if (iscale == 1) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
	i__1 = lendsv - lsv;
	dlascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &e[lsv], n,
		info);
    } else if (iscale == 2) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
	i__1 = lendsv - lsv;
	dlascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &e[lsv], n,
		info);
    }

/*
       Check for no convergence to an eigenvalue after a total
       of N*MAXIT iterations.
*/

    if (jtot < nmaxit) {
	goto L10;
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.) {
	    ++(*info);
	}
/* L150: */
    }
    goto L190;

/*     Order eigenvalues and eigenvectors. */

L160:
    if (icompz == 0) {

/*        Use Quick Sort */

	dlasrt_("I", n, &d__[1], info);

    } else {

/*        Use Selection Sort to minimize swaps of eigenvectors */

	i__1 = *n;
	for (ii = 2; ii <= i__1; ++ii) {
	    i__ = ii - 1;
	    k = i__;
	    p = d__[i__];
	    i__2 = *n;
	    for (j = ii; j <= i__2; ++j) {
		if (d__[j] < p) {
		    k = j;
		    p = d__[j];
		}
/* L170: */
	    }
	    if (k != i__) {
		d__[k] = d__[i__];
		d__[i__] = p;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[k * z_dim1 + 1],
			 &c__1);
	    }
/* L180: */
	}
    }

L190:
    return 0;

/*     End of DSTEQR */

} /* dsteqr_ */

/* Subroutine */ int dsterf_(integer *n, doublereal *d__, doublereal *e,
	integer *info)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static doublereal c__;
    static integer i__, l, m;
    static doublereal p, r__, s;
    static integer l1;
    static doublereal bb, rt1, rt2, eps, rte;
    static integer lsv;
    static doublereal eps2, oldc;
    static integer lend, jtot;
    extern /* Subroutine */ int dlae2_(doublereal *, doublereal *, doublereal
	    *, doublereal *, doublereal *);
    static doublereal gamma, alpha, sigma, anorm;

    static integer iscale;
    extern /* Subroutine */ int dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *);
    static doublereal oldgam, safmin;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static doublereal safmax;
    extern doublereal dlanst_(char *, integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int dlasrt_(char *, integer *, doublereal *,
	    integer *);
    static integer lendsv;
    static doublereal ssfmin;
    static integer nmaxit;
    static doublereal ssfmax;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DSTERF computes all eigenvalues of a symmetric tridiagonal matrix
    using the Pal-Walker-Kahan variant of the QL or QR algorithm.

    Arguments
    =========

    N       (input) INTEGER
            The order of the matrix.  N >= 0.

    D       (input/output) DOUBLE PRECISION array, dimension (N)
            On entry, the n diagonal elements of the tridiagonal matrix.
            On exit, if INFO = 0, the eigenvalues in ascending order.

    E       (input/output) DOUBLE PRECISION array, dimension (N-1)
            On entry, the (n-1) subdiagonal elements of the tridiagonal
            matrix.
            On exit, E has been destroyed.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  the algorithm failed to find all of the eigenvalues in
                  a total of 30*N iterations; if INFO = i, then i
                  elements of E have not converged to zero.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    --e;
    --d__;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n < 0) {
	*info = -1;
	i__1 = -(*info);
	xerbla_("DSTERF", &i__1);
	return 0;
    }
    if (*n <= 1) {
	return 0;
    }

/*     Determine the unit roundoff for this environment. */

    eps = EPSILON;
/* Computing 2nd power */
    d__1 = eps;
    eps2 = d__1 * d__1;
    safmin = SAFEMINIMUM;
    safmax = 1. / safmin;
    ssfmax = sqrt(safmax) / 3.;
    ssfmin = sqrt(safmin) / eps2;

/*     Compute the eigenvalues of the tridiagonal matrix. */

    nmaxit = *n * 30;
    sigma = 0.;
    jtot = 0;

/*
       Determine where the matrix splits and choose QL or QR iteration
       for each block, according to whether top or bottom diagonal
       element is smaller.
*/

    l1 = 1;

L10:
    if (l1 > *n) {
	goto L170;
    }
    if (l1 > 1) {
	e[l1 - 1] = 0.;
    }
    i__1 = *n - 1;
    for (m = l1; m <= i__1; ++m) {
	if ((d__3 = e[m], abs(d__3)) <= sqrt((d__1 = d__[m], abs(d__1))) *
		sqrt((d__2 = d__[m + 1], abs(d__2))) * eps) {
	    e[m] = 0.;
	    goto L30;
	}
/* L20: */
    }
    m = *n;

L30:
    l = l1;
    lsv = l;
    lend = m;
    lendsv = lend;
    l1 = m + 1;
    if (lend == l) {
	goto L10;
    }

/*     Scale submatrix in rows and columns L to LEND */

    i__1 = lend - l + 1;
    anorm = dlanst_("I", &i__1, &d__[l], &e[l]);
    iscale = 0;
    if (anorm > ssfmax) {
	iscale = 1;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n,
		info);
    } else if (anorm < ssfmin) {
	iscale = 2;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n,
		info);
    }

    i__1 = lend - 1;
    for (i__ = l; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	d__1 = e[i__];
	e[i__] = d__1 * d__1;
/* L40: */
    }

/*     Choose between QL and QR iteration */

    if ((d__1 = d__[lend], abs(d__1)) < (d__2 = d__[l], abs(d__2))) {
	lend = lsv;
	l = lendsv;
    }

    if (lend >= l) {

/*
          QL Iteration

          Look for small subdiagonal element.
*/

L50:
	if (l != lend) {
	    i__1 = lend - 1;
	    for (m = l; m <= i__1; ++m) {
		if ((d__2 = e[m], abs(d__2)) <= eps2 * (d__1 = d__[m] * d__[m
			+ 1], abs(d__1))) {
		    goto L70;
		}
/* L60: */
	    }
	}
	m = lend;

L70:
	if (m < lend) {
	    e[m] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L90;
	}

/*
          If remaining matrix is 2 by 2, use DLAE2 to compute its
          eigenvalues.
*/

	if (m == l + 1) {
	    rte = sqrt(e[l]);
	    dlae2_(&d__[l], &rte, &d__[l + 1], &rt1, &rt2);
	    d__[l] = rt1;
	    d__[l + 1] = rt2;
	    e[l] = 0.;
	    l += 2;
	    if (l <= lend) {
		goto L50;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(e[l]);
	sigma = (d__[l + 1] - p) / (rte * 2.);
	r__ = dlapy2_(&sigma, &c_b15);
	sigma = p - rte / (sigma + d_sign(&r__, &sigma));

	c__ = 1.;
	s = 0.;
	gamma = d__[m] - sigma;
	p = gamma * gamma;

/*        Inner loop */

	i__1 = l;
	for (i__ = m - 1; i__ >= i__1; --i__) {
	    bb = e[i__];
	    r__ = p + bb;
	    if (i__ != m - 1) {
		e[i__ + 1] = s * r__;
	    }
	    oldc = c__;
	    c__ = p / r__;
	    s = bb / r__;
	    oldgam = gamma;
	    alpha = d__[i__];
	    gamma = c__ * (alpha - sigma) - s * oldgam;
	    d__[i__ + 1] = oldgam + (alpha - gamma);
	    if (c__ != 0.) {
		p = gamma * gamma / c__;
	    } else {
		p = oldc * bb;
	    }
/* L80: */
	}

	e[l] = s * p;
	d__[l] = sigma + gamma;
	goto L50;

/*        Eigenvalue found. */

L90:
	d__[l] = p;

	++l;
	if (l <= lend) {
	    goto L50;
	}
	goto L150;

    } else {

/*
          QR Iteration

          Look for small superdiagonal element.
*/

L100:
	i__1 = lend + 1;
	for (m = l; m >= i__1; --m) {
	    if ((d__2 = e[m - 1], abs(d__2)) <= eps2 * (d__1 = d__[m] * d__[m
		    - 1], abs(d__1))) {
		goto L120;
	    }
/* L110: */
	}
	m = lend;

L120:
	if (m > lend) {
	    e[m - 1] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L140;
	}

/*
          If remaining matrix is 2 by 2, use DLAE2 to compute its
          eigenvalues.
*/

	if (m == l - 1) {
	    rte = sqrt(e[l - 1]);
	    dlae2_(&d__[l], &rte, &d__[l - 1], &rt1, &rt2);
	    d__[l] = rt1;
	    d__[l - 1] = rt2;
	    e[l - 1] = 0.;
	    l += -2;
	    if (l >= lend) {
		goto L100;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(e[l - 1]);
	sigma = (d__[l - 1] - p) / (rte * 2.);
	r__ = dlapy2_(&sigma, &c_b15);
	sigma = p - rte / (sigma + d_sign(&r__, &sigma));

	c__ = 1.;
	s = 0.;
	gamma = d__[m] - sigma;
	p = gamma * gamma;

/*        Inner loop */

	i__1 = l - 1;
	for (i__ = m; i__ <= i__1; ++i__) {
	    bb = e[i__];
	    r__ = p + bb;
	    if (i__ != m) {
		e[i__ - 1] = s * r__;
	    }
	    oldc = c__;
	    c__ = p / r__;
	    s = bb / r__;
	    oldgam = gamma;
	    alpha = d__[i__ + 1];
	    gamma = c__ * (alpha - sigma) - s * oldgam;
	    d__[i__] = oldgam + (alpha - gamma);
	    if (c__ != 0.) {
		p = gamma * gamma / c__;
	    } else {
		p = oldc * bb;
	    }
/* L130: */
	}

	e[l - 1] = s * p;
	d__[l] = sigma + gamma;
	goto L100;

/*        Eigenvalue found. */

L140:
	d__[l] = p;

	--l;
	if (l >= lend) {
	    goto L100;
	}
	goto L150;

    }

/*     Undo scaling if necessary */

L150:
    if (iscale == 1) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
    }
    if (iscale == 2) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
    }

/*
       Check for no convergence to an eigenvalue after a total
       of N*MAXIT iterations.
*/

    if (jtot < nmaxit) {
	goto L10;
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.) {
	    ++(*info);
	}
/* L160: */
    }
    goto L180;

/*     Sort eigenvalues in increasing order. */

L170:
    dlasrt_("I", n, &d__[1], info);

L180:
    return 0;

/*     End of DSTERF */

} /* dsterf_ */

/* Subroutine */ int dsyevd_(char *jobz, char *uplo, integer *n, doublereal *
	a, integer *lda, doublereal *w, doublereal *work, integer *lwork,
	integer *iwork, integer *liwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal eps;
    static integer inde;
    static doublereal anrm, rmin, rmax;
    static integer lopt;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    static doublereal sigma;
    extern logical lsame_(char *, char *);
    static integer iinfo, lwmin, liopt;
    static logical lower, wantz;
    static integer indwk2, llwrk2;

    static integer iscale;
    extern /* Subroutine */ int dlascl_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, integer *, doublereal *,
	    integer *, integer *), dstedc_(char *, integer *,
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     integer *, integer *, integer *, integer *), dlacpy_(
	    char *, integer *, integer *, doublereal *, integer *, doublereal
	    *, integer *);
    static doublereal safmin;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static doublereal bignum;
    static integer indtau;
    extern /* Subroutine */ int dsterf_(integer *, doublereal *, doublereal *,
	     integer *);
    extern doublereal dlansy_(char *, char *, integer *, doublereal *,
	    integer *, doublereal *);
    static integer indwrk, liwmin;
    extern /* Subroutine */ int dormtr_(char *, char *, char *, integer *,
	    integer *, doublereal *, integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *, integer *), dsytrd_(char *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     integer *);
    static integer llwork;
    static doublereal smlnum;
    static logical lquery;


/*
    -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DSYEVD computes all eigenvalues and, optionally, eigenvectors of a
    real symmetric matrix A. If eigenvectors are desired, it uses a
    divide and conquer algorithm.

    The divide and conquer algorithm makes very mild assumptions about
    floating point arithmetic. It will work on machines with a guard
    digit in add/subtract, or on those binary machines without guard
    digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
    Cray-2. It could conceivably fail on hexadecimal or decimal machines
    without guard digits, but we know of none.

    Because of large use of BLAS of level 3, DSYEVD needs N**2 more
    workspace than DSYEVX.

    Arguments
    =========

    JOBZ    (input) CHARACTER*1
            = 'N':  Compute eigenvalues only;
            = 'V':  Compute eigenvalues and eigenvectors.

    UPLO    (input) CHARACTER*1
            = 'U':  Upper triangle of A is stored;
            = 'L':  Lower triangle of A is stored.

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA, N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the
            leading N-by-N upper triangular part of A contains the
            upper triangular part of the matrix A.  If UPLO = 'L',
            the leading N-by-N lower triangular part of A contains
            the lower triangular part of the matrix A.
            On exit, if JOBZ = 'V', then if INFO = 0, A contains the
            orthonormal eigenvectors of the matrix A.
            If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')
            or the upper triangle (if UPLO='U') of A, including the
            diagonal, is destroyed.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    W       (output) DOUBLE PRECISION array, dimension (N)
            If INFO = 0, the eigenvalues in ascending order.

    WORK    (workspace/output) DOUBLE PRECISION array,
                                           dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.
            If N <= 1,               LWORK must be at least 1.
            If JOBZ = 'N' and N > 1, LWORK must be at least 2*N+1.
            If JOBZ = 'V' and N > 1, LWORK must be at least
                                                  1 + 6*N + 2*N**2.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    IWORK   (workspace/output) INTEGER array, dimension (LIWORK)
            On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK.

    LIWORK  (input) INTEGER
            The dimension of the array IWORK.
            If N <= 1,                LIWORK must be at least 1.
            If JOBZ  = 'N' and N > 1, LIWORK must be at least 1.
            If JOBZ  = 'V' and N > 1, LIWORK must be at least 3 + 5*N.

            If LIWORK = -1, then a workspace query is assumed; the
            routine only calculates the optimal size of the IWORK array,
            returns this value as the first entry of the IWORK array, and
            no error message related to LIWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, the algorithm failed to converge; i
                  off-diagonal elements of an intermediate tridiagonal
                  form did not converge to zero.

    Further Details
    ===============

    Based on contributions by
       Jeff Rutter, Computer Science Division, University of California
       at Berkeley, USA
    Modified by Francoise Tisseur, University of Tennessee.

    =====================================================================


       Test the input parameters.
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --w;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lower = lsame_(uplo, "L");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    if (*n <= 1) {
	liwmin = 1;
	lwmin = 1;
	lopt = lwmin;
	liopt = liwmin;
    } else {
	if (wantz) {
	    liwmin = *n * 5 + 3;
/* Computing 2nd power */
	    i__1 = *n;
	    lwmin = *n * 6 + 1 + ((i__1 * i__1) << (1));
	} else {
	    liwmin = 1;
	    lwmin = ((*n) << (1)) + 1;
	}
	lopt = lwmin;
	liopt = liwmin;
    }
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < max(1,*n)) {
	*info = -5;
    } else if ((*lwork < lwmin && ! lquery)) {
	*info = -8;
    } else if ((*liwork < liwmin && ! lquery)) {
	*info = -10;
    }

    if (*info == 0) {
	work[1] = (doublereal) lopt;
	iwork[1] = liopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYEVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	w[1] = a[a_dim1 + 1];
	if (wantz) {
	    a[a_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = SAFEMINIMUM;
    eps = PRECISION;
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = dlansy_("M", uplo, n, &a[a_offset], lda, &work[1]);
    iscale = 0;
    if ((anrm > 0. && anrm < rmin)) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	dlascl_(uplo, &c__0, &c__0, &c_b15, &sigma, n, n, &a[a_offset], lda,
		info);
    }

/*     Call DSYTRD to reduce symmetric matrix to tridiagonal form. */

    inde = 1;
    indtau = inde + *n;
    indwrk = indtau + *n;
    llwork = *lwork - indwrk + 1;
    indwk2 = indwrk + *n * *n;
    llwrk2 = *lwork - indwk2 + 1;

    dsytrd_(uplo, n, &a[a_offset], lda, &w[1], &work[inde], &work[indtau], &
	    work[indwrk], &llwork, &iinfo);
    lopt = (integer) (((*n) << (1)) + work[indwrk]);

/*
       For eigenvalues only, call DSTERF.  For eigenvectors, first call
       DSTEDC to generate the eigenvector matrix, WORK(INDWRK), of the
       tridiagonal matrix, then call DORMTR to multiply it by the
       Householder transformations stored in A.
*/

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	dstedc_("I", n, &w[1], &work[inde], &work[indwrk], n, &work[indwk2], &
		llwrk2, &iwork[1], liwork, info);
	dormtr_("L", uplo, "N", n, n, &a[a_offset], lda, &work[indtau], &work[
		indwrk], n, &work[indwk2], &llwrk2, &iinfo);
	dlacpy_("A", n, n, &work[indwrk], n, &a[a_offset], lda);
/*
   Computing MAX
   Computing 2nd power
*/
	i__3 = *n;
	i__1 = lopt, i__2 = *n * 6 + 1 + ((i__3 * i__3) << (1));
	lopt = max(i__1,i__2);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	d__1 = 1. / sigma;
	dscal_(n, &d__1, &w[1], &c__1);
    }

    work[1] = (doublereal) lopt;
    iwork[1] = liopt;

    return 0;

/*     End of DSYEVD */

} /* dsyevd_ */

/* Subroutine */ int dsytd2_(char *uplo, integer *n, doublereal *a, integer *
	lda, doublereal *d__, doublereal *e, doublereal *tau, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
	    integer *);
    static doublereal taui;
    extern /* Subroutine */ int dsyr2_(char *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    integer *);
    static doublereal alpha;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int daxpy_(integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *);
    static logical upper;
    extern /* Subroutine */ int dsymv_(char *, integer *, doublereal *,
	    doublereal *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, integer *), dlarfg_(integer *, doublereal *,
	     doublereal *, integer *, doublereal *), xerbla_(char *, integer *
	    );


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    DSYTD2 reduces a real symmetric matrix A to symmetric tridiagonal
    form T by an orthogonal similarity transformation: Q' * A * Q = T.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            Specifies whether the upper or lower triangular part of the
            symmetric matrix A is stored:
            = 'U':  Upper triangular
            = 'L':  Lower triangular

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            n-by-n upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading n-by-n lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.
            On exit, if UPLO = 'U', the diagonal and first superdiagonal
            of A are overwritten by the corresponding elements of the
            tridiagonal matrix T, and the elements above the first
            superdiagonal, with the array TAU, represent the orthogonal
            matrix Q as a product of elementary reflectors; if UPLO
            = 'L', the diagonal and first subdiagonal of A are over-
            written by the corresponding elements of the tridiagonal
            matrix T, and the elements below the first subdiagonal, with
            the array TAU, represent the orthogonal matrix Q as a product
            of elementary reflectors. See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    D       (output) DOUBLE PRECISION array, dimension (N)
            The diagonal elements of the tridiagonal matrix T:
            D(i) = A(i,i).

    E       (output) DOUBLE PRECISION array, dimension (N-1)
            The off-diagonal elements of the tridiagonal matrix T:
            E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'.

    TAU     (output) DOUBLE PRECISION array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details).

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    If UPLO = 'U', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(n-1) . . . H(2) H(1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in
    A(1:i-1,i+1), and tau in TAU(i).

    If UPLO = 'L', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(1) H(2) . . . H(n-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i),
    and tau in TAU(i).

    The contents of A on exit are illustrated by the following examples
    with n = 5:

    if UPLO = 'U':                       if UPLO = 'L':

      (  d   e   v2  v3  v4 )              (  d                  )
      (      d   e   v3  v4 )              (  e   d              )
      (          d   e   v4 )              (  v1  e   d          )
      (              d   e  )              (  v1  v2  e   d      )
      (                  d  )              (  v1  v2  v3  e   d  )

    where d and e denote diagonal and off-diagonal elements of T, and vi
    denotes an element of the vector defining H(i).

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tau;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if ((! upper && ! lsame_(uplo, "L"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYTD2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

    if (upper) {

/*        Reduce the upper triangle of A */

	for (i__ = *n - 1; i__ >= 1; --i__) {

/*
             Generate elementary reflector H(i) = I - tau * v * v'
             to annihilate A(1:i-1,i+1)
*/

	    dlarfg_(&i__, &a[i__ + (i__ + 1) * a_dim1], &a[(i__ + 1) * a_dim1
		    + 1], &c__1, &taui);
	    e[i__] = a[i__ + (i__ + 1) * a_dim1];

	    if (taui != 0.) {

/*              Apply H(i) from both sides to A(1:i,1:i) */

		a[i__ + (i__ + 1) * a_dim1] = 1.;

/*              Compute  x := tau * A * v  storing x in TAU(1:i) */

		dsymv_(uplo, &i__, &taui, &a[a_offset], lda, &a[(i__ + 1) *
			a_dim1 + 1], &c__1, &c_b29, &tau[1], &c__1)
			;

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		alpha = taui * -.5 * ddot_(&i__, &tau[1], &c__1, &a[(i__ + 1)
			* a_dim1 + 1], &c__1);
		daxpy_(&i__, &alpha, &a[(i__ + 1) * a_dim1 + 1], &c__1, &tau[
			1], &c__1);

/*
                Apply the transformation as a rank-2 update:
                   A := A - v * w' - w * v'
*/

		dsyr2_(uplo, &i__, &c_b151, &a[(i__ + 1) * a_dim1 + 1], &c__1,
			 &tau[1], &c__1, &a[a_offset], lda);

		a[i__ + (i__ + 1) * a_dim1] = e[i__];
	    }
	    d__[i__ + 1] = a[i__ + 1 + (i__ + 1) * a_dim1];
	    tau[i__] = taui;
/* L10: */
	}
	d__[1] = a[a_dim1 + 1];
    } else {

/*        Reduce the lower triangle of A */

	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*
             Generate elementary reflector H(i) = I - tau * v * v'
             to annihilate A(i+2:n,i)
*/

	    i__2 = *n - i__;
/* Computing MIN */
	    i__3 = i__ + 2;
	    dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[min(i__3,*n) + i__ *
		     a_dim1], &c__1, &taui);
	    e[i__] = a[i__ + 1 + i__ * a_dim1];

	    if (taui != 0.) {

/*              Apply H(i) from both sides to A(i+1:n,i+1:n) */

		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Compute  x := tau * A * v  storing y in TAU(i:n-1) */

		i__2 = *n - i__;
		dsymv_(uplo, &i__2, &taui, &a[i__ + 1 + (i__ + 1) * a_dim1],
			lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b29, &tau[
			i__], &c__1);

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		i__2 = *n - i__;
		alpha = taui * -.5 * ddot_(&i__2, &tau[i__], &c__1, &a[i__ +
			1 + i__ * a_dim1], &c__1);
		i__2 = *n - i__;
		daxpy_(&i__2, &alpha, &a[i__ + 1 + i__ * a_dim1], &c__1, &tau[
			i__], &c__1);

/*
                Apply the transformation as a rank-2 update:
                   A := A - v * w' - w * v'
*/

		i__2 = *n - i__;
		dsyr2_(uplo, &i__2, &c_b151, &a[i__ + 1 + i__ * a_dim1], &
			c__1, &tau[i__], &c__1, &a[i__ + 1 + (i__ + 1) *
			a_dim1], lda);

		a[i__ + 1 + i__ * a_dim1] = e[i__];
	    }
	    d__[i__] = a[i__ + i__ * a_dim1];
	    tau[i__] = taui;
/* L20: */
	}
	d__[*n] = a[*n + *n * a_dim1];
    }

    return 0;

/*     End of DSYTD2 */

} /* dsytd2_ */

/* Subroutine */ int dsytrd_(char *uplo, integer *n, doublereal *a, integer *
	lda, doublereal *d__, doublereal *e, doublereal *tau, doublereal *
	work, integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, nb, kk, nx, iws;
    extern logical lsame_(char *, char *);
    static integer nbmin, iinfo;
    static logical upper;
    extern /* Subroutine */ int dsytd2_(char *, integer *, doublereal *,
	    integer *, doublereal *, doublereal *, doublereal *, integer *), dsyr2k_(char *, char *, integer *, integer *, doublereal
	    *, doublereal *, integer *, doublereal *, integer *, doublereal *,
	     doublereal *, integer *), dlatrd_(char *,
	    integer *, integer *, doublereal *, integer *, doublereal *,
	    doublereal *, doublereal *, integer *), xerbla_(char *,
	    integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *,
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwork, lwkopt;
    static logical lquery;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DSYTRD reduces a real symmetric matrix A to real symmetric
    tridiagonal form T by an orthogonal similarity transformation:
    Q**T * A * Q = T.

    Arguments
    =========

    UPLO    (input) CHARACTER*1
            = 'U':  Upper triangle of A is stored;
            = 'L':  Lower triangle of A is stored.

    N       (input) INTEGER
            The order of the matrix A.  N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            N-by-N upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading N-by-N lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.
            On exit, if UPLO = 'U', the diagonal and first superdiagonal
            of A are overwritten by the corresponding elements of the
            tridiagonal matrix T, and the elements above the first
            superdiagonal, with the array TAU, represent the orthogonal
            matrix Q as a product of elementary reflectors; if UPLO
            = 'L', the diagonal and first subdiagonal of A are over-
            written by the corresponding elements of the tridiagonal
            matrix T, and the elements below the first subdiagonal, with
            the array TAU, represent the orthogonal matrix Q as a product
            of elementary reflectors. See Further Details.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    D       (output) DOUBLE PRECISION array, dimension (N)
            The diagonal elements of the tridiagonal matrix T:
            D(i) = A(i,i).

    E       (output) DOUBLE PRECISION array, dimension (N-1)
            The off-diagonal elements of the tridiagonal matrix T:
            E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'.

    TAU     (output) DOUBLE PRECISION array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.  LWORK >= 1.
            For optimum performance LWORK >= N*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    If UPLO = 'U', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(n-1) . . . H(2) H(1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in
    A(1:i-1,i+1), and tau in TAU(i).

    If UPLO = 'L', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(1) H(2) . . . H(n-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i),
    and tau in TAU(i).

    The contents of A on exit are illustrated by the following examples
    with n = 5:

    if UPLO = 'U':                       if UPLO = 'L':

      (  d   e   v2  v3  v4 )              (  d                  )
      (      d   e   v3  v4 )              (  e   d              )
      (          d   e   v4 )              (  v1  e   d          )
      (              d   e  )              (  v1  v2  e   d      )
      (                  d  )              (  v1  v2  v3  e   d  )

    where d and e denote diagonal and off-diagonal elements of T, and vi
    denotes an element of the vector defining H(i).

    =====================================================================


       Test the input parameters
*/

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1;
    if ((! upper && ! lsame_(uplo, "L"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    } else if ((*lwork < 1 && ! lquery)) {
	*info = -9;
    }

    if (*info == 0) {

/*        Determine the block size. */

	nb = ilaenv_(&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, (ftnlen)6,
		 (ftnlen)1);
	lwkopt = *n * nb;
	work[1] = (doublereal) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYTRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	work[1] = 1.;
	return 0;
    }

    nx = *n;
    iws = 1;
    if ((nb > 1 && nb < *n)) {

/*
          Determine when to cross over from blocked to unblocked code
          (last block is always handled by unblocked code).

   Computing MAX
*/
	i__1 = nb, i__2 = ilaenv_(&c__3, "DSYTRD", uplo, n, &c_n1, &c_n1, &
		c_n1, (ftnlen)6, (ftnlen)1);
	nx = max(i__1,i__2);
	if (nx < *n) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*
                Not enough workspace to use optimal NB:  determine the
                minimum value of NB, and reduce NB or force use of
                unblocked code by setting NX = N.

   Computing MAX
*/
		i__1 = *lwork / ldwork;
		nb = max(i__1,1);
		nbmin = ilaenv_(&c__2, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1,
			 (ftnlen)6, (ftnlen)1);
		if (nb < nbmin) {
		    nx = *n;
		}
	    }
	} else {
	    nx = *n;
	}
    } else {
	nb = 1;
    }

    if (upper) {

/*
          Reduce the upper triangle of A.
          Columns 1:kk are handled by the unblocked method.
*/

	kk = *n - (*n - nx + nb - 1) / nb * nb;
	i__1 = kk + 1;
	i__2 = -nb;
	for (i__ = *n - nb + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ +=
		i__2) {

/*
             Reduce columns i:i+nb-1 to tridiagonal form and form the
             matrix W which is needed to update the unreduced part of
             the matrix
*/

	    i__3 = i__ + nb - 1;
	    dlatrd_(uplo, &i__3, &nb, &a[a_offset], lda, &e[1], &tau[1], &
		    work[1], &ldwork);

/*
             Update the unreduced submatrix A(1:i-1,1:i-1), using an
             update of the form:  A := A - V*W' - W*V'
*/

	    i__3 = i__ - 1;
	    dsyr2k_(uplo, "No transpose", &i__3, &nb, &c_b151, &a[i__ *
		    a_dim1 + 1], lda, &work[1], &ldwork, &c_b15, &a[a_offset],
		     lda);

/*
             Copy superdiagonal elements back into A, and diagonal
             elements into D
*/

	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j - 1 + j * a_dim1] = e[j - 1];
		d__[j] = a[j + j * a_dim1];
/* L10: */
	    }
/* L20: */
	}

/*        Use unblocked code to reduce the last or only block */

	dsytd2_(uplo, &kk, &a[a_offset], lda, &d__[1], &e[1], &tau[1], &iinfo);
    } else {

/*        Reduce the lower triangle of A */

	i__2 = *n - nx;
	i__1 = nb;
	for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__1) {

/*
             Reduce columns i:i+nb-1 to tridiagonal form and form the
             matrix W which is needed to update the unreduced part of
             the matrix
*/

	    i__3 = *n - i__ + 1;
	    dlatrd_(uplo, &i__3, &nb, &a[i__ + i__ * a_dim1], lda, &e[i__], &
		    tau[i__], &work[1], &ldwork);

/*
             Update the unreduced submatrix A(i+ib:n,i+ib:n), using
             an update of the form:  A := A - V*W' - W*V'
*/

	    i__3 = *n - i__ - nb + 1;
	    dsyr2k_(uplo, "No transpose", &i__3, &nb, &c_b151, &a[i__ + nb +
		    i__ * a_dim1], lda, &work[nb + 1], &ldwork, &c_b15, &a[
		    i__ + nb + (i__ + nb) * a_dim1], lda);

/*
             Copy subdiagonal elements back into A, and diagonal
             elements into D
*/

	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j + 1 + j * a_dim1] = e[j];
		d__[j] = a[j + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}

/*        Use unblocked code to reduce the last or only block */

	i__1 = *n - i__ + 1;
	dsytd2_(uplo, &i__1, &a[i__ + i__ * a_dim1], lda, &d__[i__], &e[i__],
		&tau[i__], &iinfo);
    }

    work[1] = (doublereal) lwkopt;
    return 0;

/*     End of DSYTRD */

} /* dsytrd_ */

/* Subroutine */ int dtrevc_(char *side, char *howmny, logical *select,
	integer *n, doublereal *t, integer *ldt, doublereal *vl, integer *
	ldvl, doublereal *vr, integer *ldvr, integer *mm, integer *m,
	doublereal *work, integer *info)
{
    /* System generated locals */
    integer t_dim1, t_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1,
	    i__2, i__3;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j, k;
    static doublereal x[4]	/* was [2][2] */;
    static integer j1, j2, n2, ii, ki, ip, is;
    static doublereal wi, wr, rec, ulp, beta, emax;
    static logical pair;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
	    integer *);
    static logical allv;
    static integer ierr;
    static doublereal unfl, ovfl, smin;
    static logical over;
    static doublereal vmax;
    static integer jnxt;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
	    integer *);
    static doublereal scale;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
	    doublereal *, doublereal *, integer *, doublereal *, integer *,
	    doublereal *, doublereal *, integer *);
    static doublereal remax;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
	    doublereal *, integer *);
    static logical leftv, bothv;
    extern /* Subroutine */ int daxpy_(integer *, doublereal *, doublereal *,
	    integer *, doublereal *, integer *);
    static doublereal vcrit;
    static logical somev;
    static doublereal xnorm;
    extern /* Subroutine */ int dlaln2_(logical *, integer *, integer *,
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     doublereal *, doublereal *, integer *, doublereal *, doublereal *
	    , doublereal *, integer *, doublereal *, doublereal *, integer *),
	     dlabad_(doublereal *, doublereal *);

    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static doublereal bignum;
    static logical rightv;
    static doublereal smlnum;


/*
    -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    DTREVC computes some or all of the right and/or left eigenvectors of
    a real upper quasi-triangular matrix T.

    The right eigenvector x and the left eigenvector y of T corresponding
    to an eigenvalue w are defined by:

                 T*x = w*x,     y'*T = w*y'

    where y' denotes the conjugate transpose of the vector y.

    If all eigenvectors are requested, the routine may either return the
    matrices X and/or Y of right or left eigenvectors of T, or the
    products Q*X and/or Q*Y, where Q is an input orthogonal
    matrix. If T was obtained from the real-Schur factorization of an
    original matrix A = Q*T*Q', then Q*X and Q*Y are the matrices of
    right or left eigenvectors of A.

    T must be in Schur canonical form (as returned by DHSEQR), that is,
    block upper triangular with 1-by-1 and 2-by-2 diagonal blocks; each
    2-by-2 diagonal block has its diagonal elements equal and its
    off-diagonal elements of opposite sign.  Corresponding to each 2-by-2
    diagonal block is a complex conjugate pair of eigenvalues and
    eigenvectors; only one eigenvector of the pair is computed, namely
    the one corresponding to the eigenvalue with positive imaginary part.

    Arguments
    =========

    SIDE    (input) CHARACTER*1
            = 'R':  compute right eigenvectors only;
            = 'L':  compute left eigenvectors only;
            = 'B':  compute both right and left eigenvectors.

    HOWMNY  (input) CHARACTER*1
            = 'A':  compute all right and/or left eigenvectors;
            = 'B':  compute all right and/or left eigenvectors,
                    and backtransform them using the input matrices
                    supplied in VR and/or VL;
            = 'S':  compute selected right and/or left eigenvectors,
                    specified by the logical array SELECT.

    SELECT  (input/output) LOGICAL array, dimension (N)
            If HOWMNY = 'S', SELECT specifies the eigenvectors to be
            computed.
            If HOWMNY = 'A' or 'B', SELECT is not referenced.
            To select the real eigenvector corresponding to a real
            eigenvalue w(j), SELECT(j) must be set to .TRUE..  To select
            the complex eigenvector corresponding to a complex conjugate
            pair w(j) and w(j+1), either SELECT(j) or SELECT(j+1) must be
            set to .TRUE.; then on exit SELECT(j) is .TRUE. and
            SELECT(j+1) is .FALSE..

    N       (input) INTEGER
            The order of the matrix T. N >= 0.

    T       (input) DOUBLE PRECISION array, dimension (LDT,N)
            The upper quasi-triangular matrix T in Schur canonical form.

    LDT     (input) INTEGER
            The leading dimension of the array T. LDT >= max(1,N).

    VL      (input/output) DOUBLE PRECISION array, dimension (LDVL,MM)
            On entry, if SIDE = 'L' or 'B' and HOWMNY = 'B', VL must
            contain an N-by-N matrix Q (usually the orthogonal matrix Q
            of Schur vectors returned by DHSEQR).
            On exit, if SIDE = 'L' or 'B', VL contains:
            if HOWMNY = 'A', the matrix Y of left eigenvectors of T;
                             VL has the same quasi-lower triangular form
                             as T'. If T(i,i) is a real eigenvalue, then
                             the i-th column VL(i) of VL  is its
                             corresponding eigenvector. If T(i:i+1,i:i+1)
                             is a 2-by-2 block whose eigenvalues are
                             complex-conjugate eigenvalues of T, then
                             VL(i)+sqrt(-1)*VL(i+1) is the complex
                             eigenvector corresponding to the eigenvalue
                             with positive real part.
            if HOWMNY = 'B', the matrix Q*Y;
            if HOWMNY = 'S', the left eigenvectors of T specified by
                             SELECT, stored consecutively in the columns
                             of VL, in the same order as their
                             eigenvalues.
            A complex eigenvector corresponding to a complex eigenvalue
            is stored in two consecutive columns, the first holding the
            real part, and the second the imaginary part.
            If SIDE = 'R', VL is not referenced.

    LDVL    (input) INTEGER
            The leading dimension of the array VL.  LDVL >= max(1,N) if
            SIDE = 'L' or 'B'; LDVL >= 1 otherwise.

    VR      (input/output) DOUBLE PRECISION array, dimension (LDVR,MM)
            On entry, if SIDE = 'R' or 'B' and HOWMNY = 'B', VR must
            contain an N-by-N matrix Q (usually the orthogonal matrix Q
            of Schur vectors returned by DHSEQR).
            On exit, if SIDE = 'R' or 'B', VR contains:
            if HOWMNY = 'A', the matrix X of right eigenvectors of T;
                             VR has the same quasi-upper triangular form
                             as T. If T(i,i) is a real eigenvalue, then
                             the i-th column VR(i) of VR  is its
                             corresponding eigenvector. If T(i:i+1,i:i+1)
                             is a 2-by-2 block whose eigenvalues are
                             complex-conjugate eigenvalues of T, then
                             VR(i)+sqrt(-1)*VR(i+1) is the complex
                             eigenvector corresponding to the eigenvalue
                             with positive real part.
            if HOWMNY = 'B', the matrix Q*X;
            if HOWMNY = 'S', the right eigenvectors of T specified by
                             SELECT, stored consecutively in the columns
                             of VR, in the same order as their
                             eigenvalues.
            A complex eigenvector corresponding to a complex eigenvalue
            is stored in two consecutive columns, the first holding the
            real part and the second the imaginary part.
            If SIDE = 'L', VR is not referenced.

    LDVR    (input) INTEGER
            The leading dimension of the array VR.  LDVR >= max(1,N) if
            SIDE = 'R' or 'B'; LDVR >= 1 otherwise.

    MM      (input) INTEGER
            The number of columns in the arrays VL and/or VR. MM >= M.

    M       (output) INTEGER
            The number of columns in the arrays VL and/or VR actually
            used to store the eigenvectors.
            If HOWMNY = 'A' or 'B', M is set to N.
            Each selected real eigenvector occupies one column and each
            selected complex eigenvector occupies two columns.

    WORK    (workspace) DOUBLE PRECISION array, dimension (3*N)

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The algorithm used in this program is basically backward (forward)
    substitution, with scaling to make the the code robust against
    possible overflow.

    Each eigenvector is normalized so that the element of largest
    magnitude has magnitude 1; here the magnitude of a complex number
    (x,y) is taken to be |x| + |y|.

    =====================================================================


       Decode and test the input parameters
*/

    /* Parameter adjustments */
    --select;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1 * 1;
    t -= t_offset;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1 * 1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1 * 1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    bothv = lsame_(side, "B");
    rightv = lsame_(side, "R") || bothv;
    leftv = lsame_(side, "L") || bothv;

    allv = lsame_(howmny, "A");
    over = lsame_(howmny, "B");
    somev = lsame_(howmny, "S");

    *info = 0;
    if ((! rightv && ! leftv)) {
	*info = -1;
    } else if (((! allv && ! over) && ! somev)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ldt < max(1,*n)) {
	*info = -6;
    } else if (*ldvl < 1 || (leftv && *ldvl < *n)) {
	*info = -8;
    } else if (*ldvr < 1 || (rightv && *ldvr < *n)) {
	*info = -10;
    } else {

/*
          Set M to the number of columns required to store the selected
          eigenvectors, standardize the array SELECT if necessary, and
          test MM.
*/

	if (somev) {
	    *m = 0;
	    pair = FALSE_;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (pair) {
		    pair = FALSE_;
		    select[j] = FALSE_;
		} else {
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] == 0.) {
			    if (select[j]) {
				++(*m);
			    }
			} else {
			    pair = TRUE_;
			    if (select[j] || select[j + 1]) {
				select[j] = TRUE_;
				*m += 2;
			    }
			}
		    } else {
			if (select[*n]) {
			    ++(*m);
			}
		    }
		}
/* L10: */
	    }
	} else {
	    *m = *n;
	}

	if (*mm < *m) {
	    *info = -11;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTREVC", &i__1);
	return 0;
    }

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    }

/*     Set the constants to control overflow. */

    unfl = SAFEMINIMUM;
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    ulp = PRECISION;
    smlnum = unfl * (*n / ulp);
    bignum = (1. - ulp) / smlnum;

/*
       Compute 1-norm of each column of strictly upper triangular
       part of T to control overflow in triangular solver.
*/

    work[1] = 0.;
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
	work[j] = 0.;
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[j] += (d__1 = t[i__ + j * t_dim1], abs(d__1));
/* L20: */
	}
/* L30: */
    }

/*
       Index IP is used to specify the real or complex eigenvalue:
         IP = 0, real eigenvalue,
              1, first of conjugate complex pair: (wr,wi)
             -1, second of conjugate complex pair: (wr,wi)
*/

    n2 = (*n) << (1);

    if (rightv) {

/*        Compute right eigenvectors. */

	ip = 0;
	is = *m;
	for (ki = *n; ki >= 1; --ki) {

	    if (ip == 1) {
		goto L130;
	    }
	    if (ki == 1) {
		goto L40;
	    }
	    if (t[ki + (ki - 1) * t_dim1] == 0.) {
		goto L40;
	    }
	    ip = -1;

L40:
	    if (somev) {
		if (ip == 0) {
		    if (! select[ki]) {
			goto L130;
		    }
		} else {
		    if (! select[ki - 1]) {
			goto L130;
		    }
		}
	    }

/*           Compute the KI-th eigenvalue (WR,WI). */

	    wr = t[ki + ki * t_dim1];
	    wi = 0.;
	    if (ip != 0) {
		wi = sqrt((d__1 = t[ki + (ki - 1) * t_dim1], abs(d__1))) *
			sqrt((d__2 = t[ki - 1 + ki * t_dim1], abs(d__2)));
	    }
/* Computing MAX */
	    d__1 = ulp * (abs(wr) + abs(wi));
	    smin = max(d__1,smlnum);

	    if (ip == 0) {

/*              Real right eigenvector */

		work[ki + *n] = 1.;

/*              Form right-hand side */

		i__1 = ki - 1;
		for (k = 1; k <= i__1; ++k) {
		    work[k + *n] = -t[k + ki * t_dim1];
/* L50: */
		}

/*
                Solve the upper quasi-triangular system:
                   (T(1:KI-1,1:KI-1) - WR)*X = SCALE*WORK.
*/

		jnxt = ki - 1;
		for (j = ki - 1; j >= 1; --j) {
		    if (j > jnxt) {
			goto L60;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j - 1;
		    if (j > 1) {
			if (t[j + (j - 1) * t_dim1] != 0.) {
			    j1 = j - 1;
			    jnxt = j - 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

			dlaln2_(&c_false, &c__1, &c__1, &smin, &c_b15, &t[j +
				j * t_dim1], ldt, &c_b15, &c_b15, &work[j + *
				n], n, &wr, &c_b29, x, &c__2, &scale, &xnorm,
				&ierr);

/*
                      Scale X(1,1) to avoid overflow when updating
                      the right-hand side.
*/

			if (xnorm > 1.) {
			    if (work[j] > bignum / xnorm) {
				x[0] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			}
			work[j + *n] = x[0];

/*                    Update right-hand side */

			i__1 = j - 1;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);

		    } else {

/*                    2-by-2 diagonal block */

			dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b15, &t[j -
				1 + (j - 1) * t_dim1], ldt, &c_b15, &c_b15, &
				work[j - 1 + *n], n, &wr, &c_b29, x, &c__2, &
				scale, &xnorm, &ierr);

/*
                      Scale X(1,1) and X(2,1) to avoid overflow when
                      updating the right-hand side.
*/

			if (xnorm > 1.) {
/* Computing MAX */
			    d__1 = work[j - 1], d__2 = work[j];
			    beta = max(d__1,d__2);
			    if (beta > bignum / xnorm) {
				x[0] /= xnorm;
				x[1] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			}
			work[j - 1 + *n] = x[0];
			work[j + *n] = x[1];

/*                    Update right-hand side */

			i__1 = j - 2;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1,
				&work[*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[1];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
		    }
L60:
		    ;
		}

/*              Copy the vector x or Q*x to VR and normalize. */

		if (! over) {
		    dcopy_(&ki, &work[*n + 1], &c__1, &vr[is * vr_dim1 + 1], &
			    c__1);

		    ii = idamax_(&ki, &vr[is * vr_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vr[ii + is * vr_dim1], abs(d__1));
		    dscal_(&ki, &remax, &vr[is * vr_dim1 + 1], &c__1);

		    i__1 = *n;
		    for (k = ki + 1; k <= i__1; ++k) {
			vr[k + is * vr_dim1] = 0.;
/* L70: */
		    }
		} else {
		    if (ki > 1) {
			i__1 = ki - 1;
			dgemv_("N", n, &i__1, &c_b15, &vr[vr_offset], ldvr, &
				work[*n + 1], &c__1, &work[ki + *n], &vr[ki *
				vr_dim1 + 1], &c__1);
		    }

		    ii = idamax_(n, &vr[ki * vr_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vr[ii + ki * vr_dim1], abs(d__1));
		    dscal_(n, &remax, &vr[ki * vr_dim1 + 1], &c__1);
		}

	    } else {

/*
                Complex right eigenvector.

                Initial solve
                  [ (T(KI-1,KI-1) T(KI-1,KI) ) - (WR + I* WI)]*X = 0.
                  [ (T(KI,KI-1)   T(KI,KI)   )               ]
*/

		if ((d__1 = t[ki - 1 + ki * t_dim1], abs(d__1)) >= (d__2 = t[
			ki + (ki - 1) * t_dim1], abs(d__2))) {
		    work[ki - 1 + *n] = 1.;
		    work[ki + n2] = wi / t[ki - 1 + ki * t_dim1];
		} else {
		    work[ki - 1 + *n] = -wi / t[ki + (ki - 1) * t_dim1];
		    work[ki + n2] = 1.;
		}
		work[ki + *n] = 0.;
		work[ki - 1 + n2] = 0.;

/*              Form right-hand side */

		i__1 = ki - 2;
		for (k = 1; k <= i__1; ++k) {
		    work[k + *n] = -work[ki - 1 + *n] * t[k + (ki - 1) *
			    t_dim1];
		    work[k + n2] = -work[ki + n2] * t[k + ki * t_dim1];
/* L80: */
		}

/*
                Solve upper quasi-triangular system:
                (T(1:KI-2,1:KI-2) - (WR+i*WI))*X = SCALE*(WORK+i*WORK2)
*/

		jnxt = ki - 2;
		for (j = ki - 2; j >= 1; --j) {
		    if (j > jnxt) {
			goto L90;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j - 1;
		    if (j > 1) {
			if (t[j + (j - 1) * t_dim1] != 0.) {
			    j1 = j - 1;
			    jnxt = j - 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

			dlaln2_(&c_false, &c__1, &c__2, &smin, &c_b15, &t[j +
				j * t_dim1], ldt, &c_b15, &c_b15, &work[j + *
				n], n, &wr, &wi, x, &c__2, &scale, &xnorm, &
				ierr);

/*
                      Scale X(1,1) and X(1,2) to avoid overflow when
                      updating the right-hand side.
*/

			if (xnorm > 1.) {
			    if (work[j] > bignum / xnorm) {
				x[0] /= xnorm;
				x[2] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			    dscal_(&ki, &scale, &work[n2 + 1], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];

/*                    Update the right-hand side */

			i__1 = j - 1;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
			i__1 = j - 1;
			d__1 = -x[2];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				n2 + 1], &c__1);

		    } else {

/*                    2-by-2 diagonal block */

			dlaln2_(&c_false, &c__2, &c__2, &smin, &c_b15, &t[j -
				1 + (j - 1) * t_dim1], ldt, &c_b15, &c_b15, &
				work[j - 1 + *n], n, &wr, &wi, x, &c__2, &
				scale, &xnorm, &ierr);

/*
                      Scale X to avoid overflow when updating
                      the right-hand side.
*/

			if (xnorm > 1.) {
/* Computing MAX */
			    d__1 = work[j - 1], d__2 = work[j];
			    beta = max(d__1,d__2);
			    if (beta > bignum / xnorm) {
				rec = 1. / xnorm;
				x[0] *= rec;
				x[2] *= rec;
				x[1] *= rec;
				x[3] *= rec;
				scale *= rec;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			    dscal_(&ki, &scale, &work[n2 + 1], &c__1);
			}
			work[j - 1 + *n] = x[0];
			work[j + *n] = x[1];
			work[j - 1 + n2] = x[2];
			work[j + n2] = x[3];

/*                    Update the right-hand side */

			i__1 = j - 2;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1,
				&work[*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[1];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[2];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1,
				&work[n2 + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[3];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				n2 + 1], &c__1);
		    }
L90:
		    ;
		}

/*              Copy the vector x or Q*x to VR and normalize. */

		if (! over) {
		    dcopy_(&ki, &work[*n + 1], &c__1, &vr[(is - 1) * vr_dim1
			    + 1], &c__1);
		    dcopy_(&ki, &work[n2 + 1], &c__1, &vr[is * vr_dim1 + 1], &
			    c__1);

		    emax = 0.;
		    i__1 = ki;
		    for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vr[k + (is - 1) * vr_dim1]
				, abs(d__1)) + (d__2 = vr[k + is * vr_dim1],
				abs(d__2));
			emax = max(d__3,d__4);
/* L100: */
		    }

		    remax = 1. / emax;
		    dscal_(&ki, &remax, &vr[(is - 1) * vr_dim1 + 1], &c__1);
		    dscal_(&ki, &remax, &vr[is * vr_dim1 + 1], &c__1);

		    i__1 = *n;
		    for (k = ki + 1; k <= i__1; ++k) {
			vr[k + (is - 1) * vr_dim1] = 0.;
			vr[k + is * vr_dim1] = 0.;
/* L110: */
		    }

		} else {

		    if (ki > 2) {
			i__1 = ki - 2;
			dgemv_("N", n, &i__1, &c_b15, &vr[vr_offset], ldvr, &
				work[*n + 1], &c__1, &work[ki - 1 + *n], &vr[(
				ki - 1) * vr_dim1 + 1], &c__1);
			i__1 = ki - 2;
			dgemv_("N", n, &i__1, &c_b15, &vr[vr_offset], ldvr, &
				work[n2 + 1], &c__1, &work[ki + n2], &vr[ki *
				vr_dim1 + 1], &c__1);
		    } else {
			dscal_(n, &work[ki - 1 + *n], &vr[(ki - 1) * vr_dim1
				+ 1], &c__1);
			dscal_(n, &work[ki + n2], &vr[ki * vr_dim1 + 1], &
				c__1);
		    }

		    emax = 0.;
		    i__1 = *n;
		    for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vr[k + (ki - 1) * vr_dim1]
				, abs(d__1)) + (d__2 = vr[k + ki * vr_dim1],
				abs(d__2));
			emax = max(d__3,d__4);
/* L120: */
		    }
		    remax = 1. / emax;
		    dscal_(n, &remax, &vr[(ki - 1) * vr_dim1 + 1], &c__1);
		    dscal_(n, &remax, &vr[ki * vr_dim1 + 1], &c__1);
		}
	    }

	    --is;
	    if (ip != 0) {
		--is;
	    }
L130:
	    if (ip == 1) {
		ip = 0;
	    }
	    if (ip == -1) {
		ip = 1;
	    }
/* L140: */
	}
    }

    if (leftv) {

/*        Compute left eigenvectors. */

	ip = 0;
	is = 1;
	i__1 = *n;
	for (ki = 1; ki <= i__1; ++ki) {

	    if (ip == -1) {
		goto L250;
	    }
	    if (ki == *n) {
		goto L150;
	    }
	    if (t[ki + 1 + ki * t_dim1] == 0.) {
		goto L150;
	    }
	    ip = 1;

L150:
	    if (somev) {
		if (! select[ki]) {
		    goto L250;
		}
	    }

/*           Compute the KI-th eigenvalue (WR,WI). */

	    wr = t[ki + ki * t_dim1];
	    wi = 0.;
	    if (ip != 0) {
		wi = sqrt((d__1 = t[ki + (ki + 1) * t_dim1], abs(d__1))) *
			sqrt((d__2 = t[ki + 1 + ki * t_dim1], abs(d__2)));
	    }
/* Computing MAX */
	    d__1 = ulp * (abs(wr) + abs(wi));
	    smin = max(d__1,smlnum);

	    if (ip == 0) {

/*              Real left eigenvector. */

		work[ki + *n] = 1.;

/*              Form right-hand side */

		i__2 = *n;
		for (k = ki + 1; k <= i__2; ++k) {
		    work[k + *n] = -t[ki + k * t_dim1];
/* L160: */
		}

/*
                Solve the quasi-triangular system:
                   (T(KI+1:N,KI+1:N) - WR)'*X = SCALE*WORK
*/

		vmax = 1.;
		vcrit = bignum;

		jnxt = ki + 1;
		i__2 = *n;
		for (j = ki + 1; j <= i__2; ++j) {
		    if (j < jnxt) {
			goto L170;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j + 1;
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] != 0.) {
			    j2 = j + 1;
			    jnxt = j + 2;
			}
		    }

		    if (j1 == j2) {

/*
                      1-by-1 diagonal block

                      Scale if necessary to avoid overflow when forming
                      the right-hand side.
*/

			if (work[j] > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 1;
			work[j + *n] -= ddot_(&i__3, &t[ki + 1 + j * t_dim1],
				&c__1, &work[ki + 1 + *n], &c__1);

/*                    Solve (T(J,J)-WR)'*X = WORK */

			dlaln2_(&c_false, &c__1, &c__1, &smin, &c_b15, &t[j +
				j * t_dim1], ldt, &c_b15, &c_b15, &work[j + *
				n], n, &wr, &c_b29, x, &c__2, &scale, &xnorm,
				&ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			}
			work[j + *n] = x[0];
/* Computing MAX */
			d__2 = (d__1 = work[j + *n], abs(d__1));
			vmax = max(d__2,vmax);
			vcrit = bignum / vmax;

		    } else {

/*
                      2-by-2 diagonal block

                      Scale if necessary to avoid overflow when forming
                      the right-hand side.

   Computing MAX
*/
			d__1 = work[j], d__2 = work[j + 1];
			beta = max(d__1,d__2);
			if (beta > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 1;
			work[j + *n] -= ddot_(&i__3, &t[ki + 1 + j * t_dim1],
				&c__1, &work[ki + 1 + *n], &c__1);

			i__3 = j - ki - 1;
			work[j + 1 + *n] -= ddot_(&i__3, &t[ki + 1 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 1 + *n], &c__1);

/*
                      Solve
                        [T(J,J)-WR   T(J,J+1)     ]'* X = SCALE*( WORK1 )
                        [T(J+1,J)    T(J+1,J+1)-WR]             ( WORK2 )
*/

			dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b15, &t[j +
				j * t_dim1], ldt, &c_b15, &c_b15, &work[j + *
				n], n, &wr, &c_b29, x, &c__2, &scale, &xnorm,
				&ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			}
			work[j + *n] = x[0];
			work[j + 1 + *n] = x[1];

/* Computing MAX */
			d__3 = (d__1 = work[j + *n], abs(d__1)), d__4 = (d__2
				= work[j + 1 + *n], abs(d__2)), d__3 = max(
				d__3,d__4);
			vmax = max(d__3,vmax);
			vcrit = bignum / vmax;

		    }
L170:
		    ;
		}

/*              Copy the vector x or Q*x to VL and normalize. */

		if (! over) {
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + *n], &c__1, &vl[ki + is *
			    vl_dim1], &c__1);

		    i__2 = *n - ki + 1;
		    ii = idamax_(&i__2, &vl[ki + is * vl_dim1], &c__1) + ki -
			    1;
		    remax = 1. / (d__1 = vl[ii + is * vl_dim1], abs(d__1));
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + is * vl_dim1], &c__1);

		    i__2 = ki - 1;
		    for (k = 1; k <= i__2; ++k) {
			vl[k + is * vl_dim1] = 0.;
/* L180: */
		    }

		} else {

		    if (ki < *n) {
			i__2 = *n - ki;
			dgemv_("N", n, &i__2, &c_b15, &vl[(ki + 1) * vl_dim1
				+ 1], ldvl, &work[ki + 1 + *n], &c__1, &work[
				ki + *n], &vl[ki * vl_dim1 + 1], &c__1);
		    }

		    ii = idamax_(n, &vl[ki * vl_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vl[ii + ki * vl_dim1], abs(d__1));
		    dscal_(n, &remax, &vl[ki * vl_dim1 + 1], &c__1);

		}

	    } else {

/*
                Complex left eigenvector.

                 Initial solve:
                   ((T(KI,KI)    T(KI,KI+1) )' - (WR - I* WI))*X = 0.
                   ((T(KI+1,KI) T(KI+1,KI+1))                )
*/

		if ((d__1 = t[ki + (ki + 1) * t_dim1], abs(d__1)) >= (d__2 =
			t[ki + 1 + ki * t_dim1], abs(d__2))) {
		    work[ki + *n] = wi / t[ki + (ki + 1) * t_dim1];
		    work[ki + 1 + n2] = 1.;
		} else {
		    work[ki + *n] = 1.;
		    work[ki + 1 + n2] = -wi / t[ki + 1 + ki * t_dim1];
		}
		work[ki + 1 + *n] = 0.;
		work[ki + n2] = 0.;

/*              Form right-hand side */

		i__2 = *n;
		for (k = ki + 2; k <= i__2; ++k) {
		    work[k + *n] = -work[ki + *n] * t[ki + k * t_dim1];
		    work[k + n2] = -work[ki + 1 + n2] * t[ki + 1 + k * t_dim1]
			    ;
/* L190: */
		}

/*
                Solve complex quasi-triangular system:
                ( T(KI+2,N:KI+2,N) - (WR-i*WI) )*X = WORK1+i*WORK2
*/

		vmax = 1.;
		vcrit = bignum;

		jnxt = ki + 2;
		i__2 = *n;
		for (j = ki + 2; j <= i__2; ++j) {
		    if (j < jnxt) {
			goto L200;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j + 1;
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] != 0.) {
			    j2 = j + 1;
			    jnxt = j + 2;
			}
		    }

		    if (j1 == j2) {

/*
                      1-by-1 diagonal block

                      Scale if necessary to avoid overflow when
                      forming the right-hand side elements.
*/

			if (work[j] > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + n2], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 2;
			work[j + *n] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + *n], &c__1);
			i__3 = j - ki - 2;
			work[j + n2] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + n2], &c__1);

/*                    Solve (T(J,J)-(WR-i*WI))*(X11+i*X12)= WK+I*WK2 */

			d__1 = -wi;
			dlaln2_(&c_false, &c__1, &c__2, &smin, &c_b15, &t[j +
				j * t_dim1], ldt, &c_b15, &c_b15, &work[j + *
				n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + n2], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];
/* Computing MAX */
			d__3 = (d__1 = work[j + *n], abs(d__1)), d__4 = (d__2
				= work[j + n2], abs(d__2)), d__3 = max(d__3,
				d__4);
			vmax = max(d__3,vmax);
			vcrit = bignum / vmax;

		    } else {

/*
                      2-by-2 diagonal block

                      Scale if necessary to avoid overflow when forming
                      the right-hand side elements.

   Computing MAX
*/
			d__1 = work[j], d__2 = work[j + 1];
			beta = max(d__1,d__2);
			if (beta > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + n2], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 2;
			work[j + *n] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + *n], &c__1);

			i__3 = j - ki - 2;
			work[j + n2] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + n2], &c__1);

			i__3 = j - ki - 2;
			work[j + 1 + *n] -= ddot_(&i__3, &t[ki + 2 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 2 + *n], &c__1);

			i__3 = j - ki - 2;
			work[j + 1 + n2] -= ddot_(&i__3, &t[ki + 2 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 2 + n2], &c__1);

/*
                      Solve 2-by-2 complex linear equation
                        ([T(j,j)   T(j,j+1)  ]'-(wr-i*wi)*I)*X = SCALE*B
                        ([T(j+1,j) T(j+1,j+1)]             )
*/

			d__1 = -wi;
			dlaln2_(&c_true, &c__2, &c__2, &smin, &c_b15, &t[j +
				j * t_dim1], ldt, &c_b15, &c_b15, &work[j + *
				n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + n2], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];
			work[j + 1 + *n] = x[1];
			work[j + 1 + n2] = x[3];
/* Computing MAX */
			d__1 = abs(x[0]), d__2 = abs(x[2]), d__1 = max(d__1,
				d__2), d__2 = abs(x[1]), d__1 = max(d__1,d__2)
				, d__2 = abs(x[3]), d__1 = max(d__1,d__2);
			vmax = max(d__1,vmax);
			vcrit = bignum / vmax;

		    }
L200:
		    ;
		}

/*
                Copy the vector x or Q*x to VL and normalize.

   L210:
*/
		if (! over) {
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + *n], &c__1, &vl[ki + is *
			    vl_dim1], &c__1);
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + n2], &c__1, &vl[ki + (is + 1) *
			    vl_dim1], &c__1);

		    emax = 0.;
		    i__2 = *n;
		    for (k = ki; k <= i__2; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vl[k + is * vl_dim1], abs(
				d__1)) + (d__2 = vl[k + (is + 1) * vl_dim1],
				abs(d__2));
			emax = max(d__3,d__4);
/* L220: */
		    }
		    remax = 1. / emax;
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + is * vl_dim1], &c__1);
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + (is + 1) * vl_dim1], &c__1)
			    ;

		    i__2 = ki - 1;
		    for (k = 1; k <= i__2; ++k) {
			vl[k + is * vl_dim1] = 0.;
			vl[k + (is + 1) * vl_dim1] = 0.;
/* L230: */
		    }
		} else {
		    if (ki < *n - 1) {
			i__2 = *n - ki - 1;
			dgemv_("N", n, &i__2, &c_b15, &vl[(ki + 2) * vl_dim1
				+ 1], ldvl, &work[ki + 2 + *n], &c__1, &work[
				ki + *n], &vl[ki * vl_dim1 + 1], &c__1);
			i__2 = *n - ki - 1;
			dgemv_("N", n, &i__2, &c_b15, &vl[(ki + 2) * vl_dim1
				+ 1], ldvl, &work[ki + 2 + n2], &c__1, &work[
				ki + 1 + n2], &vl[(ki + 1) * vl_dim1 + 1], &
				c__1);
		    } else {
			dscal_(n, &work[ki + *n], &vl[ki * vl_dim1 + 1], &
				c__1);
			dscal_(n, &work[ki + 1 + n2], &vl[(ki + 1) * vl_dim1
				+ 1], &c__1);
		    }

		    emax = 0.;
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vl[k + ki * vl_dim1], abs(
				d__1)) + (d__2 = vl[k + (ki + 1) * vl_dim1],
				abs(d__2));
			emax = max(d__3,d__4);
/* L240: */
		    }
		    remax = 1. / emax;
		    dscal_(n, &remax, &vl[ki * vl_dim1 + 1], &c__1);
		    dscal_(n, &remax, &vl[(ki + 1) * vl_dim1 + 1], &c__1);

		}

	    }

	    ++is;
	    if (ip != 0) {
		++is;
	    }
L250:
	    if (ip == -1) {
		ip = 0;
	    }
	    if (ip == 1) {
		ip = -1;
	    }

/* L260: */
	}

    }

    return 0;

/*     End of DTREVC */

} /* dtrevc_ */

integer ieeeck_(integer *ispec, real *zero, real *one)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    static real nan1, nan2, nan3, nan4, nan5, nan6, neginf, posinf, negzro,
	    newzro;


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1998


    Purpose
    =======

    IEEECK is called from the ILAENV to verify that Infinity and
    possibly NaN arithmetic is safe (i.e. will not trap).

    Arguments
    =========

    ISPEC   (input) INTEGER
            Specifies whether to test just for inifinity arithmetic
            or whether to test for infinity and NaN arithmetic.
            = 0: Verify infinity arithmetic only.
            = 1: Verify infinity and NaN arithmetic.

    ZERO    (input) REAL
            Must contain the value 0.0
            This is passed to prevent the compiler from optimizing
            away this code.

    ONE     (input) REAL
            Must contain the value 1.0
            This is passed to prevent the compiler from optimizing
            away this code.

    RETURN VALUE:  INTEGER
            = 0:  Arithmetic failed to produce the correct answers
            = 1:  Arithmetic produced the correct answers
*/

    ret_val = 1;

    posinf = *one / *zero;
    if (posinf <= *one) {
	ret_val = 0;
	return ret_val;
    }

    neginf = -(*one) / *zero;
    if (neginf >= *zero) {
	ret_val = 0;
	return ret_val;
    }

    negzro = *one / (neginf + *one);
    if (negzro != *zero) {
	ret_val = 0;
	return ret_val;
    }

    neginf = *one / negzro;
    if (neginf >= *zero) {
	ret_val = 0;
	return ret_val;
    }

    newzro = negzro + *zero;
    if (newzro != *zero) {
	ret_val = 0;
	return ret_val;
    }

    posinf = *one / newzro;
    if (posinf <= *one) {
	ret_val = 0;
	return ret_val;
    }

    neginf *= posinf;
    if (neginf >= *zero) {
	ret_val = 0;
	return ret_val;
    }

    posinf *= posinf;
    if (posinf <= *one) {
	ret_val = 0;
	return ret_val;
    }


/*     Return if we were only asked to check infinity arithmetic */

    if (*ispec == 0) {
	return ret_val;
    }

    nan1 = posinf + neginf;

    nan2 = posinf / neginf;

    nan3 = posinf / posinf;

    nan4 = posinf * *zero;

    nan5 = neginf * negzro;

    nan6 = nan5 * 0.f;

    if (nan1 == nan1) {
	ret_val = 0;
	return ret_val;
    }

    if (nan2 == nan2) {
	ret_val = 0;
	return ret_val;
    }

    if (nan3 == nan3) {
	ret_val = 0;
	return ret_val;
    }

    if (nan4 == nan4) {
	ret_val = 0;
	return ret_val;
    }

    if (nan5 == nan5) {
	ret_val = 0;
	return ret_val;
    }

    if (nan6 == nan6) {
	ret_val = 0;
	return ret_val;
    }

    return ret_val;
} /* ieeeck_ */

integer ilaenv_(integer *ispec, char *name__, char *opts, integer *n1,
	integer *n2, integer *n3, integer *n4, ftnlen name_len, ftnlen
	opts_len)
{
    /* System generated locals */
    integer ret_val;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__;
    static char c1[1], c2[2], c3[3], c4[2];
    static integer ic, nb, iz, nx;
    static logical cname, sname;
    static integer nbmin;
    extern integer ieeeck_(integer *, real *, real *);
    static char subnam[6];


/*
    -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    ILAENV is called from the LAPACK routines to choose problem-dependent
    parameters for the local environment.  See ISPEC for a description of
    the parameters.

    This version provides a set of parameters which should give good,
    but not optimal, performance on many of the currently available
    computers.  Users are encouraged to modify this subroutine to set
    the tuning parameters for their particular machine using the option
    and problem size information in the arguments.

    This routine will not function correctly if it is converted to all
    lower case.  Converting it to all upper case is allowed.

    Arguments
    =========

    ISPEC   (input) INTEGER
            Specifies the parameter to be returned as the value of
            ILAENV.
            = 1: the optimal blocksize; if this value is 1, an unblocked
                 algorithm will give the best performance.
            = 2: the minimum block size for which the block routine
                 should be used; if the usable block size is less than
                 this value, an unblocked routine should be used.
            = 3: the crossover point (in a block routine, for N less
                 than this value, an unblocked routine should be used)
            = 4: the number of shifts, used in the nonsymmetric
                 eigenvalue routines
            = 5: the minimum column dimension for blocking to be used;
                 rectangular blocks must have dimension at least k by m,
                 where k is given by ILAENV(2,...) and m by ILAENV(5,...)
            = 6: the crossover point for the SVD (when reducing an m by n
                 matrix to bidiagonal form, if max(m,n)/min(m,n) exceeds
                 this value, a QR factorization is used first to reduce
                 the matrix to a triangular form.)
            = 7: the number of processors
            = 8: the crossover point for the multishift QR and QZ methods
                 for nonsymmetric eigenvalue problems.
            = 9: maximum size of the subproblems at the bottom of the
                 computation tree in the divide-and-conquer algorithm
                 (used by xGELSD and xGESDD)
            =10: ieee NaN arithmetic can be trusted not to trap
            =11: infinity arithmetic can be trusted not to trap

    NAME    (input) CHARACTER*(*)
            The name of the calling subroutine, in either upper case or
            lower case.

    OPTS    (input) CHARACTER*(*)
            The character options to the subroutine NAME, concatenated
            into a single character string.  For example, UPLO = 'U',
            TRANS = 'T', and DIAG = 'N' for a triangular routine would
            be specified as OPTS = 'UTN'.

    N1      (input) INTEGER
    N2      (input) INTEGER
    N3      (input) INTEGER
    N4      (input) INTEGER
            Problem dimensions for the subroutine NAME; these may not all
            be required.

   (ILAENV) (output) INTEGER
            >= 0: the value of the parameter specified by ISPEC
            < 0:  if ILAENV = -k, the k-th argument had an illegal value.

    Further Details
    ===============

    The following conventions have been used when calling ILAENV from the
    LAPACK routines:
    1)  OPTS is a concatenation of all of the character options to
        subroutine NAME, in the same order that they appear in the
        argument list for NAME, even if they are not used in determining
        the value of the parameter specified by ISPEC.
    2)  The problem dimensions N1, N2, N3, N4 are specified in the order
        that they appear in the argument list for NAME.  N1 is used
        first, N2 second, and so on, and unused problem dimensions are
        passed a value of -1.
    3)  The parameter value returned by ILAENV is checked for validity in
        the calling subroutine.  For example, ILAENV is used to retrieve
        the optimal blocksize for STRTRI as follows:

        NB = ILAENV( 1, 'STRTRI', UPLO // DIAG, N, -1, -1, -1 )
        IF( NB.LE.1 ) NB = MAX( 1, N )

    =====================================================================
*/


    switch (*ispec) {
	case 1:  goto L100;
	case 2:  goto L100;
	case 3:  goto L100;
	case 4:  goto L400;
	case 5:  goto L500;
	case 6:  goto L600;
	case 7:  goto L700;
	case 8:  goto L800;
	case 9:  goto L900;
	case 10:  goto L1000;
	case 11:  goto L1100;
    }

/*     Invalid value for ISPEC */

    ret_val = -1;
    return ret_val;

L100:

/*     Convert NAME to upper case if the first character is lower case. */

    ret_val = 1;
    s_copy(subnam, name__, (ftnlen)6, name_len);
    ic = *(unsigned char *)subnam;
    iz = 'Z';
    if (iz == 90 || iz == 122) {

/*        ASCII character set */

	if ((ic >= 97 && ic <= 122)) {
	    *(unsigned char *)subnam = (char) (ic - 32);
	    for (i__ = 2; i__ <= 6; ++i__) {
		ic = *(unsigned char *)&subnam[i__ - 1];
		if ((ic >= 97 && ic <= 122)) {
		    *(unsigned char *)&subnam[i__ - 1] = (char) (ic - 32);
		}
/* L10: */
	    }
	}

    } else if (iz == 233 || iz == 169) {

/*        EBCDIC character set */

	if ((ic >= 129 && ic <= 137) || (ic >= 145 && ic <= 153) || (ic >=
		162 && ic <= 169)) {
	    *(unsigned char *)subnam = (char) (ic + 64);
	    for (i__ = 2; i__ <= 6; ++i__) {
		ic = *(unsigned char *)&subnam[i__ - 1];
		if ((ic >= 129 && ic <= 137) || (ic >= 145 && ic <= 153) || (
			ic >= 162 && ic <= 169)) {
		    *(unsigned char *)&subnam[i__ - 1] = (char) (ic + 64);
		}
/* L20: */
	    }
	}

    } else if (iz == 218 || iz == 250) {

/*        Prime machines:  ASCII+128 */

	if ((ic >= 225 && ic <= 250)) {
	    *(unsigned char *)subnam = (char) (ic - 32);
	    for (i__ = 2; i__ <= 6; ++i__) {
		ic = *(unsigned char *)&subnam[i__ - 1];
		if ((ic >= 225 && ic <= 250)) {
		    *(unsigned char *)&subnam[i__ - 1] = (char) (ic - 32);
		}
/* L30: */
	    }
	}
    }

    *(unsigned char *)c1 = *(unsigned char *)subnam;
    sname = *(unsigned char *)c1 == 'S' || *(unsigned char *)c1 == 'D';
    cname = *(unsigned char *)c1 == 'C' || *(unsigned char *)c1 == 'Z';
    if (! (cname || sname)) {
	return ret_val;
    }
    s_copy(c2, subnam + 1, (ftnlen)2, (ftnlen)2);
    s_copy(c3, subnam + 3, (ftnlen)3, (ftnlen)3);
    s_copy(c4, c3 + 1, (ftnlen)2, (ftnlen)2);

    switch (*ispec) {
	case 1:  goto L110;
	case 2:  goto L200;
	case 3:  goto L300;
    }

L110:

/*
       ISPEC = 1:  block size

       In these examples, separate code is provided for setting NB for
       real and complex.  We assume that NB will take the same value in
       single or double precision.
*/

    nb = 1;

    if (s_cmp(c2, "GE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	} else if (s_cmp(c3, "QRF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3,
		"RQF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "LQF", (ftnlen)
		3, (ftnlen)3) == 0 || s_cmp(c3, "QLF", (ftnlen)3, (ftnlen)3)
		== 0) {
	    if (sname) {
		nb = 32;
	    } else {
		nb = 32;
	    }
	} else if (s_cmp(c3, "HRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 32;
	    } else {
		nb = 32;
	    }
	} else if (s_cmp(c3, "BRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 32;
	    } else {
		nb = 32;
	    }
	} else if (s_cmp(c3, "TRI", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if (s_cmp(c2, "PO", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if (s_cmp(c2, "SY", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	} else if ((sname && s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0)) {
	    nb = 32;
	} else if ((sname && s_cmp(c3, "GST", (ftnlen)3, (ftnlen)3) == 0)) {
	    nb = 64;
	}
    } else if ((cname && s_cmp(c2, "HE", (ftnlen)2, (ftnlen)2) == 0)) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 64;
	} else if (s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 32;
	} else if (s_cmp(c3, "GST", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 64;
	}
    } else if ((sname && s_cmp(c2, "OR", (ftnlen)2, (ftnlen)2) == 0)) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	}
    } else if ((cname && s_cmp(c2, "UN", (ftnlen)2, (ftnlen)2) == 0)) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	}
    } else if (s_cmp(c2, "GB", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		if (*n4 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    } else {
		if (*n4 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    }
	}
    } else if (s_cmp(c2, "PB", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		if (*n2 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    } else {
		if (*n2 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    }
	}
    } else if (s_cmp(c2, "TR", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRI", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if (s_cmp(c2, "LA", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "UUM", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if ((sname && s_cmp(c2, "ST", (ftnlen)2, (ftnlen)2) == 0)) {
	if (s_cmp(c3, "EBZ", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 1;
	}
    }
    ret_val = nb;
    return ret_val;

L200:

/*     ISPEC = 2:  minimum block size */

    nbmin = 2;
    if (s_cmp(c2, "GE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "QRF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "RQF", (
		ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "LQF", (ftnlen)3, (
		ftnlen)3) == 0 || s_cmp(c3, "QLF", (ftnlen)3, (ftnlen)3) == 0)
		 {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	} else if (s_cmp(c3, "HRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	} else if (s_cmp(c3, "BRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	} else if (s_cmp(c3, "TRI", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	}
    } else if (s_cmp(c2, "SY", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 8;
	    } else {
		nbmin = 8;
	    }
	} else if ((sname && s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0)) {
	    nbmin = 2;
	}
    } else if ((cname && s_cmp(c2, "HE", (ftnlen)2, (ftnlen)2) == 0)) {
	if (s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nbmin = 2;
	}
    } else if ((sname && s_cmp(c2, "OR", (ftnlen)2, (ftnlen)2) == 0)) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	}
    } else if ((cname && s_cmp(c2, "UN", (ftnlen)2, (ftnlen)2) == 0)) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	}
    }
    ret_val = nbmin;
    return ret_val;

L300:

/*     ISPEC = 3:  crossover point */

    nx = 0;
    if (s_cmp(c2, "GE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "QRF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "RQF", (
		ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "LQF", (ftnlen)3, (
		ftnlen)3) == 0 || s_cmp(c3, "QLF", (ftnlen)3, (ftnlen)3) == 0)
		 {
	    if (sname) {
		nx = 128;
	    } else {
		nx = 128;
	    }
	} else if (s_cmp(c3, "HRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nx = 128;
	    } else {
		nx = 128;
	    }
	} else if (s_cmp(c3, "BRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nx = 128;
	    } else {
		nx = 128;
	    }
	}
    } else if (s_cmp(c2, "SY", (ftnlen)2, (ftnlen)2) == 0) {
	if ((sname && s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0)) {
	    nx = 32;
	}
    } else if ((cname && s_cmp(c2, "HE", (ftnlen)2, (ftnlen)2) == 0)) {
	if (s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nx = 32;
	}
    } else if ((sname && s_cmp(c2, "OR", (ftnlen)2, (ftnlen)2) == 0)) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nx = 128;
	    }
	}
    } else if ((cname && s_cmp(c2, "UN", (ftnlen)2, (ftnlen)2) == 0)) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ",
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nx = 128;
	    }
	}
    }
    ret_val = nx;
    return ret_val;

L400:

/*     ISPEC = 4:  number of shifts (used by xHSEQR) */

    ret_val = 6;
    return ret_val;

L500:

/*     ISPEC = 5:  minimum column dimension (not used) */

    ret_val = 2;
    return ret_val;

L600:

/*     ISPEC = 6:  crossover point for SVD (used by xGELSS and xGESVD) */

    ret_val = (integer) ((real) min(*n1,*n2) * 1.6f);
    return ret_val;

L700:

/*     ISPEC = 7:  number of processors (not used) */

    ret_val = 1;
    return ret_val;

L800:

/*     ISPEC = 8:  crossover point for multishift (used by xHSEQR) */

    ret_val = 50;
    return ret_val;

L900:

/*
       ISPEC = 9:  maximum size of the subproblems at the bottom of the
                   computation tree in the divide-and-conquer algorithm
                   (used by xGELSD and xGESDD)
*/

    ret_val = 25;
    return ret_val;

L1000:

/*
       ISPEC = 10: ieee NaN arithmetic can be trusted not to trap

       ILAENV = 0
*/
    ret_val = 1;
    if (ret_val == 1) {
	ret_val = ieeeck_(&c__0, &c_b3825, &c_b3826);
    }
    return ret_val;

L1100:

/*
       ISPEC = 11: infinity arithmetic can be trusted not to trap

       ILAENV = 0
*/
    ret_val = 1;
    if (ret_val == 1) {
	ret_val = ieeeck_(&c__1, &c_b3825, &c_b3826);
    }
    return ret_val;

/*     End of ILAENV */

} /* ilaenv_ */

