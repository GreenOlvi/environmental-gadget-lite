#ifndef log_h
#define log_h

#ifdef DEBUG
#define log(format, ...) Serial.printf(format "\n", ##__VA_ARGS__)
#else
#define log(x, ...)
#endif

#endif