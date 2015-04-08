#ifndef __QSORT_EXT_H__
#define __QSORT_EXT_H__

#include <stdlib.h>

#include <sphinxbase/sphinxbase_export.h>

#ifdef __cplusplus
extern "C" {
#endif
#if 0
/* Fool Emacs. */
}
#endif

SPHINXBASE_EXPORT
void qsort1(void *aa, size_t n, size_t as, int (*cmp)(const void *, const void *));

SPHINXBASE_EXPORT
void qsort2(void *aa, void *bb, size_t n, size_t as, size_t bs, int (*cmp)(const void *, const void *));

SPHINXBASE_EXPORT
void qsort3(void *aa, void *bb, void *cc, size_t n, size_t as, size_t bs, size_t cs, int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif /* __QSORT_EXT_H__ */