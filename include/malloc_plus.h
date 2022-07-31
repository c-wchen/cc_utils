#ifndef MALLOC_PLUS_H
#define MALLOC_PLUS_H

#ifdef __cplusplus
extern "C" {
#endif
void *malloc_plus(int size, const char *func, uint32_t lineNum);
void free_plus(void **ptr, const char *func, uint32_t lineNum);
#define MALLOC_PLUS(size) malloc_plus(size, __FUNCTION__, __LINE__)
#define FREE_PLUS(ptr) free_plus((void **)&(ptr), __FUNCTION__, __LINE__)
#ifdef __cplusplus
}
#endif
#endif