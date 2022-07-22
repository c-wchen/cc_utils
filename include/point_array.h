#ifndef __POINT_ARRAY_H__
#define __POINT_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    int32_t length;
    int32_t data[0];
} FlexArray;
void* create_point_array(int size);
void set_point_value(void *rowPtr, uint32_t index, void *value);
void* get_point_value(void *rowPtr, uint32_t index);
void free_point_array(void *ptr, int size);
#ifdef __cplusplus
}
#endif
#endif