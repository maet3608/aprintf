#ifndef aprintf_h
#define aprintf_h

#include <Arduino.h>

#define APRINTF_VERSION 1.0.5

/**
 * @brief Maximum length of the formatted string buffer (in bytes).
 *
 * Defines the internal buffer size used by both aprintf() and fmt().
 * Output exceeding MAX_ASTR - 1 is silently truncated.
 *
 * @note Override this value BEFORE including aprintf.h if you need a larger
 *       buffer:
 * @code
 *   #define MAX_ASTR 256
 *   #include "aprintf.h"
 * @endcode
 */
#define MAX_ASTR 128

/**
 * @brief Formatted print to the Serial port.
 *
 * Formats a string using standard printf-style format specifiers and prints
 * the result directly to `Serial`. Behaves like `printf()` but with a
 * fixed internal buffer of MAX_ASTR bytes.
 *
 * **Platform-specific behavior:**
 * - **ESP32 / ESP8266:** Uses native `Serial.vprintf()` for efficient,
 *   full-featured formatting.
 * - **AVR (ATmega328P, ATmega2560, etc.):** Uses a custom float-aware
 *   formatter built on `vsnprintf()` + `dtostrf()` for `%f`/`%e`/`%g`.
 * - **Other platforms (ARM, SAMD, etc.):** Falls back to `vsnprintf()` +
 *   `Serial.print()`.
 *
 * @param format  printf-style format string.
 *                Supports: `%%d`, `%%i`, `%%u`, `%%x`, `%%X`, `%%o`,
 *                `%%f`/`%%F`, `%%e`/`%%E`, `%%g`/`%%G`, `%%c`, `%%s`,
 *                `%%p`, `%%n`, `%%%%` (literal percent).
 *                Flags: `-`, `+`, ` ` (space), `0`, `#`.
 *                Width & precision: fixed (e.g. `%%5d`, `%%.3f`) or
 *                dynamic (`%%*d`, `%%.*f`).
 *                Length modifiers: `l`, `ll`, `h`, `hh`.
 * @param ...     Variadic arguments matching the format string.
 *
 * @warning
 * - **Shared buffer:** aprintf() and fmt() use the same static buffer.
 *   Calling either function overwrites the previous result.
 * - **Not re-entrant:** Do NOT call from an ISR or multiple concurrent
 *   contexts.
 * - **Truncation:** Output longer than MAX_ASTR - 1 is clipped. Increase
 *   MAX_ASTR before inclusion if needed.
 *
 * @code
 *   aprintf("Temperature: %.1f °C\n", 23.7);
 *   aprintf("Counter: %03d\n", 42);
 *   aprintf("100%% complete\n");
 * @endcode
 */
void aprintf(const char *format, ...);

/**
 * @brief Format a string into an internal buffer.
 *
 * Formats a string using standard printf-style format specifiers into a
 * static character buffer and returns a pointer to it. Use this function
 * when you need the formatted string for destinations other than `Serial`
 * (e.g. LCD / OLED displays, log strings, network output).
 *
 * @param format  printf-style format string (same specifiers as aprintf).
 * @param ...     Variadic arguments matching the format string.
 * @return        `const char*` pointer to the internal buffer containing
 *                the newly formatted, NUL-terminated string.
 *
 * @warning
 * - **Ephemeral pointer:** The returned pointer is only valid until the
 *   next call to aprintf() or fmt() — both share the same static buffer.
 * - **Not re-entrant:** Do NOT call from an ISR or multiple concurrent
 *   contexts.
 *
 * @code
 *   // Print to an OLED display
 *   disp.print(fmt("Speed: %03d rpm", 1200));
 *
 *   // Store for later use (copy immediately!)
 *   const char *s = fmt("Value: %d", analogRead(A0));
 *   char copy[32];
 *   strncpy(copy, s, sizeof(copy));
 * @endcode
 */
const char *fmt(const char *format, ...);

#endif
