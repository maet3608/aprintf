# Readme

A printf function for Arduino. For details see
[How To Print To Serial Monitor On Arduino](https://www.makerguides.com/how-to-print-to-serial-monitor-on-arduino/)


## Example

include "aprintf.h"

void setup()
{
  Serial.begin(9600);
  aprintf("%10s\n", "test");
  aprintf("counter: %03d\n", 1);
  aprintf("pi = %.2f\n", 3.145);
}

void loop() {}

