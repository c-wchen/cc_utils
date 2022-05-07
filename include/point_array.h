#ifndef __POINT_ARRAY_H__
#define __POINT_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    int32_t length;
    int32_t data[0];
} FlexArray;
void* CreatePointArray(int size);
void SetPointValue(void *rowPtr, uint32_t index, void *value);
void* GetPointValue(void *rowPtr, uint32_t index);

#ifdef __cplusplus
}
#endif
#endif