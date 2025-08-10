#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define BIT_SUCCESS 1
#define BIT_FAIL -1

typedef struct {
    uint8_t *data;
    int32_t len;
} bitmap;

bitmap *bitmap_alloc(int32_t len);
void bitmap_free(bitmap *bits);
int32_t bitmap_set(bitmap *bits, int32_t offset, uint8_t value);
int32_t bitmap_get(bitmap *bits, int32_t offset);
int32_t bitmap_print(bitmap *bits);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* BITMAP_H */