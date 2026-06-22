#ifndef APP_MATH_H
#define APP_MATH_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t appClampU32(uint32_t value, uint32_t lowerBound, uint32_t upperBound);
bool appIsWithinU32Range(uint32_t value, uint32_t lowerBound, uint32_t upperBound);

#ifdef __cplusplus
}
#endif

#endif