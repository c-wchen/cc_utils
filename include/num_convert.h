#ifndef W_UTILS_H
#define W_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
int str_to_int(const char *buf, int length);
int float_to_str(float val, int preserve, char *output, int outputSize);
#ifdef __cplusplus
}
#endif
#endif