#ifndef aprintf_h
#define aprintf_h

#include <Arduino.h>

#define APRINTF_VERSION 1.0.0
#define MAX_ASTR 128

/*
  Print a formatted string to the serial port.
  Same as printf, but with a fixed buffer size.
*/
void aprintf(const char *format, ...);

#endif