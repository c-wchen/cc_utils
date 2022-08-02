//
// Created by 忘尘 on 2022/8/2.
//

#ifndef WCHEN_CC_UTILS_CHECK_PARAM_H
#define WCHEN_CC_UTILS_CHECK_PARAM_H
#ifdef __cplusplus
extern "C" {
#endif
#define PARAM_ONE     1
#define PARAM_TWO     2
#define PARAM_THREE   3
#define PARAM_FOUR    4
#define PARAM_FIVE    5
#define PARAM_SIX     6
#define PARAM_SEVEN   7
#define PARAM_EIGHT   8
#define PARAM_NINE    9
bool CheckParamFromAddr(const char *func, uint32_t lineNum, int32_t num, ...);
#define CHECK_ADDR(num, ...) CheckParamFromAddr(__FUNCTION__, __LINE__, num, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif //WCHEN_CC_UTILS_CHECK_PARAM_H
