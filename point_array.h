#ifndef __POINT_ARRAY_H__
#define __POINT_ARRAY_H__
void* createPointArray(int size);
void setPointValue(void *rowPtr, uint32_t index, void *value);
void* getPointValue(void *rowPtr, uint32_t index);

#endif