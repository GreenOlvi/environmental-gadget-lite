#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

uint32_t calculateCRC32(const uint8_t *data, size_t length);
float roundTwoDigits(float value);

#endif