#ifndef POINT_ARRAY_H
#define POINT_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    int32_t length;
    int32_t data[0];
} flex_array_t;
void *create_point_array(int size);
void set_point_value(void *row_ptr, uint32_t index, void *value);
void *get_point_value(void *row_ptr, uint32_t index);
void free_point_array(void *ptr, int size);
#ifdef __cplusplus
}
#endif
#endif