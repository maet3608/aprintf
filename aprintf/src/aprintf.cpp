#include "aprintf.h"

#include <stdio.h>
#include <stdarg.h>

void aprintf(const char *format, ...)
{
    char buf[MAX_ASTR];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, MAX_ASTR, format, args);
    Serial.print(buf);
}