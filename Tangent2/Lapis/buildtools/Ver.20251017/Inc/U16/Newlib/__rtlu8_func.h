/* malloc.h internal header */

void free_n(void __near *);
void free_f(void __far *);
void __near * malloc_n(size_t);
void __far  * malloc_f(size_t);
void __near * calloc_n(size_t nelem, size_t);
void __far  * calloc_f(size_t nelem, size_t);
void __near * realloc_n(void __near *, size_t);
void __far  * realloc_f(void __far  *, size_t);
void __near * memset_n(void __near *, int, size_t);
void __far  * memset_f(void __far *, int, size_t);
void __near * memcpy_nn(void __near *, const void __near *, size_t);
void __far * memcpy_ff(void __far *, const void __far *, size_t);