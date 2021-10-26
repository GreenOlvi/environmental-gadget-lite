#ifndef common_h
#define common_h

#ifdef DEBUG
#define log(x) Serial.print(x)
#define logf1(x, a) Serial.printf(x, a)
#define logf2(x, a, b) Serial.printf(x, a, b)
#define logln(x) Serial.println(x)
#else
#define log(x)
#define logf1(x, a)
#define logf2(x, a, b)
#define logln(x)
#endif

#include <stdio.h>

uint32_t calculateCRC32(const uint8_t *data, size_t length ) {
  uint32_t crc = 0xffffffff;
  while ( length-- ) {
    uint8_t c = *data++;
    for ( uint32_t i = 0x80; i > 0; i >>= 1 ) {
      bool bit = crc & 0x80000000;
      if ( c & i ) {
        bit = !bit;
      }

      crc <<= 1;
      if ( bit ) {
        crc ^= 0x04c11db7;
      }
    }
  }

  return crc;
}

#endif