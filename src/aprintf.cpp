#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "aprintf.h"

static char _aprintf_buf[MAX_ASTR];

#if defined(__AVR__)

// Advance a va_list past all arguments consumed by the specifiers in `fmt`.
// `fmt` must contain NO float specifiers (%f/%e/%g) — those are handled
// separately by the caller.
static void avr_skip_args(const char *fmt, va_list &args) {
  const char *p = fmt;
  while (*p) {
    if (*p++ != '%')
      continue;
    if (*p == '%') {
      ++p;
      continue;
    } // literal %%

    // flags
    while (*p && strchr("-+ 0#", *p))
      ++p;

    // width
    if (*p == '*') {
      va_arg(args, int);
      ++p;
    } else
      while (*p >= '0' && *p <= '9')
        ++p;

    // precision
    if (*p == '.') {
      ++p;
      if (*p == '*') {
        va_arg(args, int);
        ++p;
      } else
        while (*p >= '0' && *p <= '9')
          ++p;
    }

    // length modifier
    bool isLong = false;
    bool isLongLong = false;
    if (*p == 'l') {
      ++p;
      isLong = true;
      if (*p == 'l') {
        ++p;
        isLongLong = true;
      }
    } else if (*p == 'h') {
      ++p;
      if (*p == 'h')
        ++p; // hh — still promoted to int
    }

    // specifier
    switch (*p++) {
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    case 'X':
      if (isLongLong)
        va_arg(args, long long);
      else if (isLong)
        va_arg(args, long);
      else
        va_arg(args, int); // char/short promoted
      break;
    case 's':
      va_arg(args, char *);
      break;
    case 'p':
      va_arg(args, void *);
      break;
    case 'c':
      va_arg(args, int);
      break; // promoted
    case 'n':
      va_arg(args, int *);
      break;
    // float specifiers must never appear in the segment passed here
    default:
      break;
    }
  }
}

// Fill _aprintf_buf with the formatted string (AVR float-aware).
static void fmt_avr(const char *format, va_list args) {
  char *outPtr = _aprintf_buf;
  int outLeft = (int)sizeof(_aprintf_buf) - 1;

  const char *segStart = format;
  const char *p = format;

  while (*p) {
    if (*p != '%') {
      ++p;
      continue;
    }

    const char *specStart = p;
    ++p;

    if (*p == '%') {
      ++p;
      continue;
    } // %%

    // flags
    while (*p && strchr("-+ 0#", *p))
      ++p;

    // width
    if (*p == '*')
      ++p;
    else
      while (*p && *p >= '0' && *p <= '9')
        ++p;

    // precision
    int precision = 6;
    if (*p == '.') {
      ++p;
      if (*p == '*') {
        ++p;
      } else {
        precision = 0;
        while (*p && *p >= '0' && *p <= '9') {
          precision = precision * 10 + (*p - '0');
          ++p;
        }
      }
    }

    // length modifier
    if (*p == 'l' || *p == 'h' || *p == 'L')
      ++p;
    if (*p == 'l' || *p == 'h')
      ++p; // ll, hh

    char spec = *p;
    if (spec == 'f' || spec == 'F' ||
        spec == 'e' || spec == 'E' ||
        spec == 'g' || spec == 'G') {

      // --- 1. format the segment before this %f via vsnprintf ---
      size_t segLen = (size_t)(specStart - segStart);
      if (segLen > 0 && outLeft > 0) {
        char segFmt[128];
        if (segLen >= sizeof(segFmt))
          segLen = sizeof(segFmt) - 1;
        memcpy(segFmt, segStart, segLen);
        segFmt[segLen] = '\0';

        int written = vsnprintf(outPtr, (size_t)outLeft + 1, segFmt, args);

        // *** manually advance args past what vsnprintf consumed ***
        avr_skip_args(segFmt, args);

        if (written > 0) {
          int advanced = (written < outLeft) ? written : outLeft;
          outPtr += advanced;
          outLeft -= advanced;
        }
      }

      // --- 2. consume the double and convert with dtostrf ---
      double val = va_arg(args, double);
      if (outLeft > 0) {
        char tmp[32];
        dtostrf(val, 1, precision, tmp);
        int tlen = (int)strlen(tmp);
        if (tlen > outLeft)
          tlen = outLeft;
        memcpy(outPtr, tmp, (size_t)tlen);
        outPtr += tlen;
        outLeft -= tlen;
      }

      ++p;
      segStart = p;

    } else {
      if (*p)
        ++p;
    }
  }

  // --- 3. flush the trailing non-float tail ---
  if (*segStart && outLeft > 0) {
    int written = vsnprintf(outPtr, (size_t)outLeft + 1, segStart, args);
    if (written > 0) {
      int advanced = (written < outLeft) ? written : outLeft;
      outPtr += advanced;
      outLeft -= advanced;
    }
  }

  *outPtr = '\0';
}

static void aprintf_avr(const char *format, va_list args) {
  fmt_avr(format, args);
  Serial.print(_aprintf_buf);
}

#endif // __AVR__

void aprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);

#if defined(ESP32) || defined(ESP8266)
  Serial.vprintf(format, args);

#elif defined(__AVR__)
  aprintf_avr(format, args);

#else
  vsnprintf(_aprintf_buf, sizeof(_aprintf_buf), format, args);
  Serial.print(_aprintf_buf);
#endif

  va_end(args);
}

const char *fmt(const char *format, ...) {
  va_list args;
  va_start(args, format);

#if defined(ESP32) || defined(ESP8266)
  vsnprintf(_aprintf_buf, sizeof(_aprintf_buf), format, args);

#elif defined(__AVR__)
  // Work around missing vsnprintf float support on AVR.
  // Must make a copy of args because fmt_avr relies on non-portable
  // va_list copying — the classic AVR va_list is a simple pointer.
  va_list args_copy;
  va_copy(args_copy, args);
  fmt_avr(format, args_copy);
  va_end(args_copy);

#else
  vsnprintf(_aprintf_buf, sizeof(_aprintf_buf), format, args);
#endif

  va_end(args);
  return _aprintf_buf;
}
