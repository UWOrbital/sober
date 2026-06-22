#include "app_math.h"

uint32_t AppClampU32(uint32_t value, uint32_t lowerBound, uint32_t upperBound)
{
    if (lowerBound > upperBound) {
        return value;
    }

    if (value < lowerBound) {
        return lowerBound;
    }

    if (value > upperBound) {
        return upperBound;
    }

    return value;
}

bool appIsWithinU32Range(uint32_t value, uint32_t lowerBound, uint32_t upperBound)
{
    if (lowerBound > upperBound) {
        return false;
    }

    return (value >= lowerBound) && (value <= upperBound);
}