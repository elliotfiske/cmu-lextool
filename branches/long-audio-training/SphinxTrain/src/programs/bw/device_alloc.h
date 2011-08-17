
void *
device_alloc_3d(size_t d1, size_t d2, size_t d3, size_t elemsize);

void *
device_alloc_4d(size_t d1, size_t d2, size_t d3, size_t d4, size_t elemsize);

void
device_free_3d(void *inptr);

void
device_free_4d(void *inptr);

