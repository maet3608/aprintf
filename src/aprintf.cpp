#include "aprintf.h"

#include <stdio.h>
#include <stdarg.h>

char buf_astr[MAX_ASTR];

const char *fmt(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(buf_astr, MAX_ASTR, format, args);
    return buf_astr;
}

void aprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(buf_astr, MAX_ASTR, format, args);
    Serial.print(buf_astr);
}