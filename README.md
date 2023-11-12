# Readme

A printf function for Arduino.

Serial.print does not support formatted printing
on AVR hardware. This reimplementation of a printf-like
function fixes this.

For more details see 
[How To Print To Serial Monitor On Arduino](https://www.makerguides.com/how-to-print-to-serial-monitor-on-arduino/)


## Example

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