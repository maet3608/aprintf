# Readme

A printf function for Arduino.

Serial.print does not support formatted printing
on AVR hardware. This reimplementation of a printf-like
function fixes this.

For more details see 
[How To Print To Serial Monitor On Arduino](https://www.makerguides.com/how-to-print-to-serial-monitor-on-arduino/)


There are two functions:

aprintf(const char *format, ...) - which prints to Serial
fmt(const char *format, ...) - which prints to a char buffer and returns it

Note that these functions share the same char buffer
and cannot be called concurrently.


## Formatting options

```
aprintf("text);
aprintf(123)
aprintf("counter = %d", 123)
aprintf("counter = %03d", 123)
aprintf("speed = %.2f", 20.5)
```

For more formatting options see [Format specifiers](https://cplusplus.com/reference/cstdio/printf/)


## Example aprintf()

A simple example that includes the library
and uses the aprintf() function to print
data formatted to the Serial Monitor.

```
include "aprintf.h"

void setup()
{
  Serial.begin(9600);
  aprintf("%10s\n", "test");
  aprintf("counter: %03d\n", 1);
  aprintf("pi = %.2f\n", 3.145);
}

void loop() {}
```


## Example fmt()

Using fmt() to format a string that is displayed
on a 128x64 OLED using the Adafruit_SSD1306 library.

```
#include "aprintf.h"
#include "Adafruit_SSD1306.h"

Adafruit_SSD1306 disp(128,64, &Wire, -1);

void setup() {
  disp.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  disp.setTextSize(2);
  disp.setTextColor(WHITE);   
}

void loop() {
  disp.clearDisplay();
  disp.setCursor(16, 16);
  disp.print(fmt("pi=%.2f", 3.145));  // <-- fmt
  disp.display();
  delay(3000);
}
```