# Readme

A printf function for Arduino.


## Example

'''
include "aprintf.h"

void setup()
{
  Serial.begin(9600);
  aprintf("%10s\n", "test");
  aprintf("counter: %03d\n", 1);
  aprintf("pi = %.2f\n", 3.145);
}

void loop() {}
'''


