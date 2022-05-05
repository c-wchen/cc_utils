#ifndef __POINT_ARRAY_H__
#define __POINT_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif
void* createPointArray(int size);
void setPointValue(void *rowPtr, uint32_t index, void *value);
void* getPointValue(void *rowPtr, uint32_t index);

#ifdef __cplusplus
}
#endif
#endif