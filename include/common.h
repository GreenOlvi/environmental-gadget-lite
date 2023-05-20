#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <ESP8266WiFi.h>
#include <TZ.h>
#include "log.h"

#define countof(a) (sizeof(a) / sizeof(a[0]))

#define TIMEZONE TZ_Europe_Warsaw

uint32_t calculateCRC32(const uint8_t *data, size_t length);
float roundTwoDigits(float value);
void setClockFromNtp(const char *ntpServer);

#endif