#include "aprintf.h"

void setup()
{
  Serial.begin(9600);
  aprintf("This is a %s pi = %.2f\n", "test", 3.145);
}

void loop() {}
