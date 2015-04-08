
#include <stdio.h>

#include <sphinxbase/qsort_ext.h>

/* it's modified stdlib qsort */

#include <sys/types.h>
#include <stdlib.h>
static __inline char* med3(char *, char *, char *, int (*)(const void *, const void *));
static __inline void swapfunc(char *, char *, int, int);
#ifndef min
#define min(a, b) (a) < (b) ? a : b
#endif
/*
* Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
*/
#define swapcode(TYPE, parmi, parmj, n) { \
    long i = (n) / sizeof (TYPE); \
    TYPE *pi = (TYPE *) (parmi); \
    TYPE *pj = (TYPE *) (parmj); \
    do { \
    TYPE t = *pi;    \
    *pi++ = *pj;    \
    *pj++ = t;    \
    } while (--i > 0);    \
}

#define SWAPINIT1(a, as) swaptype1 = ((char *)a - (char *)0) % sizeof(long) || \
    as % sizeof(long) ? 2 : as == sizeof(long)? 0 : 1;

#define PTR_TO_IDX(ptr) ((ptr - (char *)aa)/as)

#define IDX_TO_PTR2(idx) ((char *)bb + bs*idx)

#define IDX_TO_PTR3(idx) ((char *)cc + cs*idx)

#define SWAPINIT2(a, b, as, bs) swaptype1 = ((char *)a - (char *)0) % sizeof(long) || \
    as % sizeof(long) ? 2 : as == sizeof(long)? 0 : 1; \
    swaptype2 = ((char *)b - (char *)0) % sizeof(long) || \
    bs % sizeof(long) ? 2 : bs == sizeof(long)? 0 : 1;

#define SWAPINIT3(a, b, c, as, bs, cs) swaptype1 = ((char *)a - (char *)0) % sizeof(long) || \
    as % sizeof(long) ? 2 : as == sizeof(long)? 0 : 1; \
    swaptype2 = ((char *)b - (char *)0) % sizeof(long) || \
    bs % sizeof(long) ? 2 : bs == sizeof(long)? 0 : 1; \
    swaptype3 = ((char *)c - (char *)0) % sizeof(long) || \
    cs % sizeof(long) ? 2 : cs == sizeof(long)? 0 : 1;

static __inline void
swapfunc(char *a, char *b, int n, int swaptype)
{
    if (swaptype <= 1)
        swapcode(long, a, b, n)
    else
    swapcode(char, a, b, n)
}

#define swap1(a, b)    \
    if (swaptype1 == 0) {    \
    long t = *(long *)(a);    \
    *(long *)(a) = *(long *)(b);    \
    *(long *)(b) = t;    \
    } else    \
    swapfunc(a, b, as, swaptype1)

#define swap2(a, b)    \
    if (swaptype2 == 0) {    \
    long t = *(long *)(a);    \
    *(long *)(a) = *(long *)(b);    \
    *(long *)(b) = t;    \
    } else    \
    swapfunc(a, b, bs, swaptype2)

#define swap3(a, b)    \
    if (swaptype3 == 0) {    \
    long t = *(long *)(a);    \
    *(long *)(a) = *(long *)(b);    \
    *(long *)(b) = t;    \
    } else    \
    swapfunc(a, b, cs, swaptype3)

#define vecswap1(a, b, n) if ((n) > 0) swapfunc(a, b, n, swaptype1)

#define vecswap2(a, b, n) if ((n) > 0) swapfunc(a, b, n, swaptype2)

#define vecswap3(a, b, n) if ((n) > 0) swapfunc(a, b, n, swaptype3)

static __inline char *
med3(char *a, char *b, char *c, int (*cmp)(const void *, const void *))
{
    return cmp(a, b) < 0 ?
        (cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a ))
        :(cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c ));
}

void
qsort1(void *aa, size_t n, size_t as, int (*cmp)(const void *, const void *))
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int d, r, swaptype1, swap_cnt;
    char *a = aa;
loop:    SWAPINIT1(a, as);
    swap_cnt = 0;
    if (n < 7) {
        for (pm = (char *)a + as; pm < (char *) a + n * as; pm += as)
            for (pl = pm; pl > (char *) a && cmp(pl - as, pl) > 0;
                pl -= as)
                swap1(pl, pl - as);
        return;
    }
    pm = (char *)a + (n / 2) * as;
    if (n > 7) {
        pl = (char *)a;
        pn = (char *)a + (n - 1) * as;
        if (n > 40) {
            d = (n / 8) * as;
            pl = med3(pl, pl + d, pl + 2 * d, cmp);
            pm = med3(pm - d, pm, pm + d, cmp);
            pn = med3(pn - 2 * d, pn - d, pn, cmp);
        }
        pm = med3(pl, pm, pn, cmp);
    }
    swap1(a, pm);
    pa = pb = (char *)a + as;
    pc = pd = (char *)a + (n - 1) * as;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap1(pa, pb);
                pa += as;
            }
            pb += as;
        }
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap1(pc, pd);
                pd -= as;
            }
            pc -= as;
        }
        if (pb > pc)
            break;
        swap1(pb, pc);
        swap_cnt = 1;
        pb += as;
        pc -= as;
    }
    if (swap_cnt == 0) { /* Switch to insertion sort */
        for (pm = (char *) a + as; pm < (char *) a + n * as; pm += as)
            for (pl = pm; pl > (char *) a && cmp(pl - as, pl) > 0;
                pl -= as)
                swap1(pl, pl - as);
        return;
    }
    pn = (char *)a + n * as;
    r = min(pa - (char *)a, pb - pa);
    vecswap1(a, pb - r, r);
    r = min(pd - pc, pn - pd - (int)as);
    vecswap1(pb, pn - r, r);
    if ((r = pb - pa) > (int)as)
        qsort1(a, r / as, as, cmp);
    if ((r = pd - pc) > (int)as) {
        /* Iterate rather than recurse to save stack space */
        a = pn - r;
        n = r / as;
        goto loop;
    }
    /* qsort(pn - r, r / as, as, cmp);*/
}

void qsort2(void *aa, void *bb, size_t n, size_t as, size_t bs, int (*cmp)(const void *, const void *))
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int d, r, r2, swaptype1, swaptype2, swap_cnt;
    char *a = (char *)aa;
    char *b = (char *)bb;
loop:    SWAPINIT2(a, b, as, bs);
    swap_cnt = 0;
    if (n < 7) {
        for (pm = (char *)a + as; pm < (char *) a + n * as; pm += as) {
            for (pl = pm; pl > (char *) a && cmp(pl - as, pl) > 0; pl -= as) {
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pl)), IDX_TO_PTR2(PTR_TO_IDX(pl - as)));
                swap1(pl, pl - as);
            }
        }
        return;
    }
    pm = (char *)a + (n / 2) * as;
    if (n > 7) {
        pl = (char *)a;
        pn = (char *)a + (n - 1) * as;
        if (n > 40) {
            d = (n / 8) * as;
            pl = med3(pl, pl + d, pl + 2 * d, cmp);
            pm = med3(pm - d, pm, pm + d, cmp);
            pn = med3(pn - 2 * d, pn - d, pn, cmp);
        }
        pm = med3(pl, pm, pn, cmp);
    }
    swap2(IDX_TO_PTR2(PTR_TO_IDX(a)), IDX_TO_PTR2(PTR_TO_IDX(pm)));
    swap1(a, pm);
    pa = pb = (char *)a + as;
    pc = pd = (char *)a + (n - 1) * as;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pa)), IDX_TO_PTR2(PTR_TO_IDX(pb)));
                swap1(pa, pb);
                pa += as;
            }
            pb += as;
        }
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pc)), IDX_TO_PTR2(PTR_TO_IDX(pd)));
                swap1(pc, pd);
                pd -= as;
            }
            pc -= as;
        }
        if (pb > pc)
            break;
        swap2(IDX_TO_PTR2(PTR_TO_IDX(pb)), IDX_TO_PTR2(PTR_TO_IDX(pc)));
        swap1(pb, pc);
        swap_cnt = 1;
        pb += as;
        pc -= as;
    }
    if (swap_cnt == 0) { /* Switch to insertion sort */
        for (pm = (char *) a + as; pm < (char *) a + n * as; pm += as)
            for (pl = pm; pl > (char *) a && cmp(pl - as, pl) > 0;
                pl -= as) {
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pl)), IDX_TO_PTR2(PTR_TO_IDX(pl - as)));
                swap1(pl, pl - as);
            }
        return;
    }
    pn = (char *)a + n * as;
    r = min(pa - (char *)a, pb - pa);
    r2 = min(IDX_TO_PTR2(PTR_TO_IDX(pa)) - IDX_TO_PTR2(PTR_TO_IDX((char *)a)), IDX_TO_PTR2(PTR_TO_IDX(pb)) - IDX_TO_PTR2(PTR_TO_IDX(pa)));
    vecswap2(IDX_TO_PTR2(PTR_TO_IDX(a)), IDX_TO_PTR2(PTR_TO_IDX(pb - r)), r2);
    vecswap1(a, pb - r, r);
    r = min(pd - pc, pn - pd - (int)as);
    r2 = min(IDX_TO_PTR2(PTR_TO_IDX(pd)) - IDX_TO_PTR2(PTR_TO_IDX(pc)), IDX_TO_PTR2(PTR_TO_IDX(pn)) - IDX_TO_PTR2(PTR_TO_IDX(pd)) - (int)bs);
    vecswap2(IDX_TO_PTR2(PTR_TO_IDX(pb)), IDX_TO_PTR2(PTR_TO_IDX(pn - r)), r2);
    vecswap1(pb, pn - r, r);
    if ((r = pb - pa) > (int)as)
        qsort2(a, IDX_TO_PTR2(PTR_TO_IDX(a)), r / as, as, bs, cmp);
    if ((r = pd - pc) > (int)as) {
        /* Iterate rather than recurse to save stack space */
        a = pn - r;
        n = r / as;
        goto loop;
    }
    /* qsort(pn - r, r / as, as, cmp);*/
}

void qsort3(void *aa, void *bb, void *cc, size_t n, size_t as, size_t bs, size_t cs, int (*cmp)(const void *, const void *))
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int d, r, r2, r3, swaptype1, swaptype2, swaptype3, swap_cnt;
    char *a = (char *)aa;
    char *b = (char *)bb;
    char *c = (char *)cc;
loop:    SWAPINIT3(a, b, c, as, bs, cs);
    swap_cnt = 0;
    if (n < 7) {
        for (pm = (char *)a + as; pm < (char *) a + n * as; pm += as) {
            for (pl = pm; pl > (char *) a && cmp(pl - as, pl) > 0; pl -= as) {
                swap3(IDX_TO_PTR3(PTR_TO_IDX(pl)), IDX_TO_PTR3(PTR_TO_IDX(pl - as)));
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pl)), IDX_TO_PTR2(PTR_TO_IDX(pl - as)));
                swap1(pl, pl - as);
            }
        }
        return;
    }
    pm = (char *)a + (n / 2) * as;
    if (n > 7) {
        pl = (char *)a;
        pn = (char *)a + (n - 1) * as;
        if (n > 40) {
            d = (n / 8) * as;
            pl = med3(pl, pl + d, pl + 2 * d, cmp);
            pm = med3(pm - d, pm, pm + d, cmp);
            pn = med3(pn - 2 * d, pn - d, pn, cmp);
        }
        pm = med3(pl, pm, pn, cmp);
    }
    swap3(IDX_TO_PTR3(PTR_TO_IDX(a)), IDX_TO_PTR3(PTR_TO_IDX(pm)));
    swap2(IDX_TO_PTR2(PTR_TO_IDX(a)), IDX_TO_PTR2(PTR_TO_IDX(pm)));
    swap1(a, pm);
    pa = pb = (char *)a + as;
    pc = pd = (char *)a + (n - 1) * as;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap3(IDX_TO_PTR3(PTR_TO_IDX(pa)), IDX_TO_PTR3(PTR_TO_IDX(pb)));
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pa)), IDX_TO_PTR2(PTR_TO_IDX(pb)));
                swap1(pa, pb);
                pa += as;
            }
            pb += as;
        }
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap3(IDX_TO_PTR3(PTR_TO_IDX(pc)), IDX_TO_PTR3(PTR_TO_IDX(pd)));
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pc)), IDX_TO_PTR2(PTR_TO_IDX(pd)));
                swap1(pc, pd);
                pd -= as;
            }
            pc -= as;
        }
        if (pb > pc)
            break;
        swap3(IDX_TO_PTR3(PTR_TO_IDX(pb)), IDX_TO_PTR3(PTR_TO_IDX(pc)));
        swap2(IDX_TO_PTR2(PTR_TO_IDX(pb)), IDX_TO_PTR2(PTR_TO_IDX(pc)));
        swap1(pb, pc);
        swap_cnt = 1;
        pb += as;
        pc -= as;
    }
    if (swap_cnt == 0) { /* Switch to insertion sort */
        for (pm = (char *) a + as; pm < (char *) a + n * as; pm += as)
            for (pl = pm; pl > (char *) a && cmp(pl - as, pl) > 0;
                pl -= as) {
                swap3(IDX_TO_PTR3(PTR_TO_IDX(pl)), IDX_TO_PTR3(PTR_TO_IDX(pl - as)));
                swap2(IDX_TO_PTR2(PTR_TO_IDX(pl)), IDX_TO_PTR2(PTR_TO_IDX(pl - as)));
                swap1(pl, pl - as);
            }
        return;
    }
    pn = (char *)a + n * as;
    r = min(pa - (char *)a, pb - pa);
    r2 = min(IDX_TO_PTR2(PTR_TO_IDX(pa)) - IDX_TO_PTR2(PTR_TO_IDX((char *)a)), IDX_TO_PTR2(PTR_TO_IDX(pb)) - IDX_TO_PTR2(PTR_TO_IDX(pa)));
    r3 = min(IDX_TO_PTR3(PTR_TO_IDX(pa)) - IDX_TO_PTR3(PTR_TO_IDX((char *)a)), IDX_TO_PTR3(PTR_TO_IDX(pb)) - IDX_TO_PTR3(PTR_TO_IDX(pa)));
    vecswap3(IDX_TO_PTR3(PTR_TO_IDX(a)), IDX_TO_PTR3(PTR_TO_IDX(pb - r)), r3);
    vecswap2(IDX_TO_PTR2(PTR_TO_IDX(a)), IDX_TO_PTR2(PTR_TO_IDX(pb - r)), r2);
    vecswap1(a, pb - r, r);
    r = min(pd - pc, pn - pd - (int)as);
    r2 = min(IDX_TO_PTR2(PTR_TO_IDX(pd)) - IDX_TO_PTR2(PTR_TO_IDX(pc)), IDX_TO_PTR2(PTR_TO_IDX(pn)) - IDX_TO_PTR2(PTR_TO_IDX(pd)) - (int)bs);
    r3 = min(IDX_TO_PTR3(PTR_TO_IDX(pd)) - IDX_TO_PTR3(PTR_TO_IDX(pc)), IDX_TO_PTR3(PTR_TO_IDX(pn)) - IDX_TO_PTR3(PTR_TO_IDX(pd)) - (int)cs);
    vecswap3(IDX_TO_PTR3(PTR_TO_IDX(pb)), IDX_TO_PTR3(PTR_TO_IDX(pn - r)), r3);
    vecswap2(IDX_TO_PTR2(PTR_TO_IDX(pb)), IDX_TO_PTR2(PTR_TO_IDX(pn - r)), r2);
    vecswap1(pb, pn - r, r);
    if ((r = pb - pa) > (int)as)
        qsort3(a, IDX_TO_PTR2(PTR_TO_IDX(a)), IDX_TO_PTR3(PTR_TO_IDX(a)), r / as, as, bs, cs, cmp);
    if ((r = pd - pc) > (int)as) {
        /* Iterate rather than recurse to save stack space */
        a = pn - r;
        n = r / as;
        goto loop;
    }
    /* qsort(pn - r, r / as, as, cmp);*/
}