# aprintf

*A printf-like function for Arduino (and other MCUs) based on `Serial.print`.*

[![Version](https://img.shields.io/badge/version-1.0.5-blue)](https://github.com/maet3608/aprintf)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

## Description

Arduino's `Serial.print` does not support printf-style formatted printing on
AVR hardware. This library reimplements a `printf`-like function — called
**`aprintf`** to avoid name clashes with the standard `printf` — that works
everywhere `Serial.print` works.

It also provides a companion function **`fmt()`** that formats into a character
buffer instead of printing directly, useful for displaying formatted text on
LCDs, OLEDs, or any other output device.

For more details see
[How To Print To Serial Monitor On Arduino](https://www.makerguides.com/how-to-print-to-serial-monitor-on-arduino/).

## Installation

### Arduino Library Manager

The library is registered in the Arduino Library Manager. Open the Arduino IDE,
go to **Sketch → Include Library → Manage Libraries...**, search for
**aprintf**, and click **Install**.

### Manual ZIP import

1. Download this repository as a `.zip` from GitHub via **Code → Download ZIP**.
2. In the Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library...**
   and select the downloaded file.

### PlatformIO

Add the library to your `platformio.ini`:

```ini
lib_deps =
    maet3608/aprintf
```

Or install via CLI:

```bash
pio pkg install --library "maet3608/aprintf"
```

## Functions

### `aprintf(const char *format, ...)`

Prints a formatted string directly to `Serial`.

### `fmt(const char *format, ...)`

Formats a string into an internal character buffer and returns a pointer to it
(`const char *`). Use this when you need the formatted string for something
other than `Serial` (e.g. displaying on an LCD or OLED).

```cpp
// Print to Serial
aprintf("Temperature: %.1f °C\n", 23.7);

// Format into a buffer (e.g. for a display)
disp.print(fmt("Speed: %03d rpm", 1200));
```

> **⚠️ Important:** Both `aprintf()` and `fmt()` share the same internal
> character buffer and are **not re-entrant**. They must not be called
> concurrently (e.g. from an ISR while the main loop is also using them).

## Supported Format Specifiers

| Specifier | Type | Example | Output |
|-----------|------|---------|--------|
| `%d`, `%i` | signed decimal integer | `aprintf("%d", -42)` | `-42` |
| `%u` | unsigned decimal | `aprintf("%u", 99)` | `99` |
| `%x` | lowercase hex | `aprintf("%x", 255)` | `ff` |
| `%X` | uppercase hex | `aprintf("%X", 255)` | `FF` |
| `%o` | octal | `aprintf("%o", 64)` | `100` |
| `%f`, `%F` | float (fixed-point) | `aprintf("%.2f", 3.14)` | `3.14` |
| `%e`, `%E` | float (scientific) | `aprintf("%e", 1000.0)` | `1.000000e+03` |
| `%g`, `%G` | float (shortest) | `aprintf("%g", 3.14)` | `3.14` |
| `%c` | character | `aprintf("%c", 'A')` | `A` |
| `%s` | string | `aprintf("%s", "hi")` | `hi` |
| `%p` | pointer | `aprintf("%p", &var)` | `0x3ffb...` |
| `%%` | literal `%` | `aprintf("100%%")` | `100%` |

### Flags

| Flag | Description | Example | Output |
|------|-------------|---------|--------|
| `-` | left-align | `aprintf("%-5d", 12)` | `12`   |
| `+` | always show sign | `aprintf("%+d", 42)` | `+42` |
| ` ` (space) | leading space for positive | `aprintf("% d", 42)` | ` 42` |
| `0` | zero-pad | `aprintf("%05d", 42)` | `00042` |
| `#` | alternate form (0x/0/X prefix, octal 0, decimal point) | `aprintf("%#x", 255)` | `0xff` |

### Width & Precision

| Feature | Example | Output |
|---------|---------|--------|
| min. width | `aprintf("%5d", 42)` | `   42` |
| precision | `aprintf("%.3f", 3.14)` | `3.140` |
| dynamic width | `aprintf("%*d", 5, 42)` | `   42` |
| dynamic precision | `aprintf("%.*f", 2, 3.145)` | `3.15` |

For general formatting reference, see
[Format specifiers (cplusplus.com)](https://cplusplus.com/reference/cstdio/printf/).

## Configuration

The maximum length of the formatted string is defined by `MAX_ASTR` in
`aprintf.h` (default: **128** bytes). You can override it by defining your own
value **before** including the library:

```cpp
#define MAX_ASTR 256  // Increase buffer to 256 bytes
#include "aprintf.h"
```

## Examples

### Example 1: Serial Monitor output via `aprintf()`

```cpp
#include "aprintf.h"

void setup() {
  Serial.begin(9600);
  aprintf("%10s\n", "test");
  aprintf("counter: %03d\n", 1);
  aprintf("pi = %.2f\n", 3.145);
}

void loop() {}
```

### Example 2: OLED display via `fmt()`

```cpp
#include "aprintf.h"
#include "Adafruit_SSD1306.h"

Adafruit_SSD1306 disp(128, 64, &Wire, -1);

void setup() {
  disp.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  disp.setTextSize(2);
  disp.setTextColor(WHITE);
}

void loop() {
  disp.clearDisplay();
  disp.setCursor(16, 16);
  disp.print(fmt("pi=%.2f", 3.145));  // <-- fmt formats into a buffer
  disp.display();
  delay(3000);
}
```

You can also open the built-in example from the Arduino IDE:
**File → Examples → aprintf → aprintf_example**.

## Limitations

- **Shared buffer:** `aprintf()` and `fmt()` use the same static buffer
  (`buf_astr`). Calling either function overwrites the previous result.
- **Not re-entrant:** Do not call these functions from an ISR or multiple
  threads while another call is in progress.
- **Fixed buffer size:** The default buffer is 128 bytes. Output exceeding
  `MAX_ASTR - 1` bytes is truncated. Increase `MAX_ASTR` if needed (see
  [Configuration](#configuration) above).
- **Float support:** Relies on the platform's `dtostrf()` implementation. Some
  platforms may not support scientific notation (`%e`/`%E`).
- **No `%n` counting:** Although `%n` is implemented internally, results may
  differ from standard `printf` behavior due to buffering differences.

## License

This library is licensed under the MIT License. See [LICENSE](./LICENSE) for
details.

## Contributing

Bug reports, feature requests, and pull requests are welcome. Please open an
issue or PR at [github.com/maet3608/aprintf](https://github.com/maet3608/aprintf).