#ifndef log_h
#define log_h

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

#endif