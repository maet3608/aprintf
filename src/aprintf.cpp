#include "aprintf.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buf_astr[MAX_ASTR];

// ---------------------------------------------------------------------------
// Buffer helpers (operate on buf_astr via position index)
// ---------------------------------------------------------------------------

static inline void buf_add(int &pos, char c) {
  if (pos < MAX_ASTR - 1)
    buf_astr[pos++] = c;
}

static void buf_add_str(int &pos, const char *s) {
  while (*s && pos < MAX_ASTR - 1)
    buf_astr[pos++] = *s++;
}

static void buf_pad(int &pos, char c, int count) {
  while (count-- > 0 && pos < MAX_ASTR - 1)
    buf_astr[pos++] = c;
}

// ---------------------------------------------------------------------------
// Portable integer-to-string conversion (replaces non-standard ltoa/ultoa)
// ---------------------------------------------------------------------------

static int utoa_portable(unsigned long val, char *buf, int base) {
  if (val == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return 1;
  }
  char tmp[33];
  int i = 0;
  while (val > 0) {
    int digit = val % base;
    tmp[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
    val /= base;
  }
  int len = i;
  for (int j = 0; j < len; j++)
    buf[j] = tmp[len - 1 - j];
  buf[len] = '\0';
  return len;
}

static int itoa_portable(long val, char *buf) {
  if (val < 0) {
    buf[0] = '-';
    return utoa_portable((unsigned long)(-val), buf + 1, 10) + 1;
  }
  return utoa_portable((unsigned long)val, buf, 10);
}

// ---------------------------------------------------------------------------
// Check if a format specifier is a floating-point type
// ---------------------------------------------------------------------------

static bool is_float_spec(char c) {
  return c == 'f' || c == 'F' || c == 'e' || c == 'E' || c == 'g' || c == 'G';
}

// ---------------------------------------------------------------------------
// Core formatter – parses the format string, writes into buf_astr
// ---------------------------------------------------------------------------

static void vformat(const char *format, va_list args) {
  int pos = 0;
  const char *p = format;

  while (*p && pos < MAX_ASTR - 1) {
    // -- Literal text ---------------------------------------------------
    if (*p != '%') {
      buf_add(pos, *p++);
      continue;
    }

    p++; // skip '%'

    // -- %% -------------------------------------------------------------
    if (*p == '%') {
      buf_add(pos, '%');
      p++;
      continue;
    }

    // -- Parse flags ----------------------------------------------------
    bool minus = false; // '-'
    bool plus = false;  // '+'
    bool space = false; // ' '
    bool zero = false;  // '0'
    bool hash = false;  // '#'

    bool parsing = true;
    while (parsing) {
      switch (*p) {
      case '-':
        minus = true;
        p++;
        break;
      case '+':
        plus = true;
        p++;
        break;
      case ' ':
        space = true;
        p++;
        break;
      case '#':
        hash = true;
        p++;
        break;
      case '0':
        zero = true;
        p++;
        break;
      default:
        parsing = false;
        break;
      }
    }

    // '-' overrides '0'
    if (minus)
      zero = false;

    // -- Parse width ----------------------------------------------------
    int width = 0;
    if (*p == '*') {
      width = va_arg(args, int);
      if (width < 0) {
        minus = true;
        width = -width;
      }
      p++;
    } else {
      while (*p >= '0' && *p <= '9') {
        width = width * 10 + (*p - '0');
        p++;
      }
    }

    // -- Parse precision ------------------------------------------------
    int precision = -1;
    if (*p == '.') {
      p++;
      if (*p == '*') {
        precision = va_arg(args, int);
        if (precision < 0)
          precision = -1;
        p++;
      } else {
        precision = 0;
        while (*p >= '0' && *p <= '9') {
          precision = precision * 10 + (*p - '0');
          p++;
        }
      }
    }

    // -- Length modifier ------------------------------------------------
    bool long_mod = false;
    if (*p == 'l') {
      long_mod = true;
      p++;
      if (*p == 'l')
        p++;
    } else if (*p == 'h') {
      p++;
      if (*p == 'h')
        p++;
    }

    // -- Specifier character --------------------------------------------
    char spec = *p++;
    if (!spec)
      break;

    // ===================================================================
    //  FLOAT specifiers  (%f %F %e %E %g %G)
    // ===================================================================
    if (is_float_spec(spec)) {
      double val = va_arg(args, double);
      int prec = (precision >= 0) ? precision : 6;

      // Determine sign prefix
      const char *sign = "";
      if (val < 0.0) {
        sign = "-";
        val = -val;
      } else if (plus)
        sign = "+";
      else if (space)
        sign = " ";

      // Convert the absolute value with dtostrf
      // dtostrf(value, min_width, precision, buffer)
      // We use min_width=1 and do padding ourselves
      char num_buf[48];
      dtostrf(val, 1, prec, num_buf);

      int num_len = (int)strlen(num_buf);
      int sign_len = (int)strlen(sign);
      int total_len = num_len + sign_len;
      int pad_len = (width > total_len) ? (width - total_len) : 0;

      // Prefix for alternate form: force decimal point for %f, %e, %g
      // when # flag is set and no decimal point exists.
      if (hash && strchr(num_buf, '.') == NULL && spec != 'E' && spec != 'G') {
        // Append '.' at end
        size_t nlen = strlen(num_buf);
        if (nlen < sizeof(num_buf) - 2) {
          num_buf[nlen] = '.';
          num_buf[nlen + 1] = '\0';
          num_len++;
          total_len++;
          if (width > total_len)
            pad_len = width - total_len;
        }
      }

      // Output
      if (!minus) {
        if (zero && precision < 0) {
          buf_add_str(pos, sign);
          buf_pad(pos, '0', pad_len);
        } else {
          buf_pad(pos, ' ', pad_len);
          buf_add_str(pos, sign);
        }
      } else {
        buf_add_str(pos, sign);
      }
      buf_add_str(pos, num_buf);
      if (minus)
        buf_pad(pos, ' ', pad_len);

      continue;
    }

    // ===================================================================
    //  INTEGER specifiers  (%d %i %u %o %x %X)
    // ===================================================================
    char num_buf[48];
    num_buf[0] = '\0';
    const char *sign = "";

    if (spec == 'd' || spec == 'i') {
      long val = long_mod ? va_arg(args, long)
                          : (long)va_arg(args, int);
      if (val < 0) {
        val = -val;
        sign = "-";
      } else if (plus)
        sign = "+";
      else if (space)
        sign = " ";
      itoa_portable(val, num_buf);
    } else if (spec == 'u') {
      unsigned long val = long_mod ? va_arg(args, unsigned long)
                                   : (unsigned long)va_arg(args, unsigned int);
      utoa_portable(val, num_buf, 10);
    } else if (spec == 'o') {
      unsigned long val = long_mod ? va_arg(args, unsigned long)
                                   : (unsigned long)va_arg(args, unsigned int);
      utoa_portable(val, num_buf, 8);
      if (hash && num_buf[0] != '0') {
        size_t nl = strlen(num_buf);
        memmove(num_buf + 1, num_buf, nl + 1);
        num_buf[0] = '0';
      }
    } else if (spec == 'x' || spec == 'X') {
      unsigned long val = long_mod ? va_arg(args, unsigned long)
                                   : (unsigned long)va_arg(args, unsigned int);
      utoa_portable(val, num_buf, 16);
      if (spec == 'X') {
        for (int i = 0; num_buf[i]; i++)
          num_buf[i] = toupper((unsigned char)num_buf[i]);
      }
      // '#' prefix for hex
      // (we handle this later after measuring lengths)
    } else if (spec == 'c') {
      num_buf[0] = (char)va_arg(args, int);
      num_buf[1] = '\0';
    } else if (spec == 's') {
      const char *s = va_arg(args, const char *);
      if (!s)
        s = "(null)";
      if (precision >= 0) {
        int n = precision;
        int i = 0;
        while (i < n && s[i] && i < (int)sizeof(num_buf) - 1)
          num_buf[i++] = s[i];
        num_buf[i] = '\0';
      } else {
        strncpy(num_buf, s, sizeof(num_buf) - 1);
        num_buf[sizeof(num_buf) - 1] = '\0';
      }
      // no sign for strings
      sign = "";
    } else if (spec == 'p') {
      void *ptr = va_arg(args, void *);
      strcpy(num_buf, "0x");
      utoa_portable((unsigned long)(uintptr_t)ptr, num_buf + 2, 16);
      sign = "";
    } else if (spec == 'n') {
      int *nptr = va_arg(args, int *);
      *nptr = pos;
      continue;
    } else {
      // Unknown specifier – output literally
      buf_add(pos, spec);
      continue;
    }

    // -- Hash prefix for hex (%#x / %#X) -------------------------------
    const char *hex_prefix = "";
    if (hash && (spec == 'x' || spec == 'X') && num_buf[0] != '0') {
      hex_prefix = (spec == 'X') ? "0X" : "0x";
    }

    int num_len = (int)strlen(num_buf);
    int sign_len = (int)strlen(sign) + (int)strlen(hex_prefix);
    int total_len = num_len + sign_len;
    int pad_len = (width > total_len) ? (width - total_len) : 0;

    // -- Handle integer precision (min digits) --------------------------
    if (precision >= 0 && spec != 'c' && spec != 's' && spec != 'p') {
      // Precision specifies minimum number of digits
      zero = true; // precision overrides '0' flag behavior
      int min_digits = precision;
      if (min_digits > num_len) {
        // Insert leading zeros into num_buf
        int shift = min_digits - num_len;
        memmove(num_buf + shift, num_buf, num_len + 1);
        memset(num_buf, '0', shift);
        num_len = min_digits;
        total_len = num_len + sign_len;
        pad_len = (width > total_len) ? (width - total_len) : 0;
      }
    }

    // -- Output -----------------------------------------------
    if (!minus) {
      if (zero) {
        buf_add_str(pos, sign);
        buf_add_str(pos, hex_prefix);
        buf_pad(pos, '0', pad_len);
      } else {
        buf_pad(pos, ' ', pad_len);
        buf_add_str(pos, sign);
        buf_add_str(pos, hex_prefix);
      }
    } else {
      buf_add_str(pos, sign);
      buf_add_str(pos, hex_prefix);
    }
    buf_add_str(pos, num_buf);
    if (minus)
      buf_pad(pos, ' ', pad_len);
  }

  buf_astr[pos] = '\0';
}

// ===========================================================================
//  Public API
// ===========================================================================

const char *fmt(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vformat(format, args);
  va_end(args);
  return buf_astr;
}

void aprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vformat(format, args);
  va_end(args);
  Serial.print(buf_astr);
}