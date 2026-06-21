// ---------------------------------------------------------------------------
// aprintf example sketch
// Demonstrates all supported format specifiers for the aprintf library.
// ---------------------------------------------------------------------------
#include "aprintf.h"

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(1000); // wait for Serial to stabilise on some boards

  // -----------------------------------------------------------------------
  // Mixed example
  // -----------------------------------------------------------------------

  aprintf("=== Mixed example ===\n");
  aprintf("The first %d digits of '%s' are %5.4f\n", 5, "PI", PI);
  aprintf("\n");

  // -----------------------------------------------------------------------
  // Integer formatting
  // -----------------------------------------------------------------------
  aprintf("=== Integer Formatting ===\n");
  aprintf("  %%d     -> [%d]\n", 42);
  aprintf("  %%i     -> [%i]\n", -99);
  aprintf("  %%u     -> [%u]\n", 65535);
  aprintf("  %%03d   -> [%03d]\n", 7);
  aprintf("  %%+d    -> [%+d]\n", 42);
  aprintf("  %% d    -> [% d]\n", 42);
  aprintf("  %%8d    -> [%8d]\n", 42);
  aprintf("  %%-8d   -> [%-8d]  (left-justified)\n", 42);
  aprintf("  %%ld    -> [%ld]\n", 123456L);
  aprintf("\n");

  // -----------------------------------------------------------------------
  // Hex / Octal formatting
  // -----------------------------------------------------------------------
  aprintf("=== Hex & Octal ===\n");
  aprintf("  %%x     -> [%x]\n", 255);
  aprintf("  %%X     -> [%X]\n", 255);
  aprintf("  %%#x    -> [%#x]\n", 255);
  aprintf("  %%#X    -> [%#X]\n", 255);
  aprintf("  %%o     -> [%o]\n", 255);
  aprintf("  %%#o    -> [%#o]\n", 255);
  aprintf("  %%04x   -> [%04x]\n", 0x0F);
  aprintf("\n");

  // -----------------------------------------------------------------------
  // Floating-point formatting
  // -----------------------------------------------------------------------
  aprintf("=== Floating Point (dtostrf-based) ===\n");
  aprintf("  %%f      -> [%f]\n", 3.14159);
  aprintf("  %%.2f    -> [%.2f]\n", 3.14159);
  aprintf("  %%.4f    -> [%.4f]\n", 3.14159);
  aprintf("  %%8.2f   -> [%8.2f]\n", 3.14);
  aprintf("  %%-8.2f  -> [%-8.2f]\n", 3.14);
  aprintf("  %%e      -> [%e]\n", 3.14159);
  aprintf("  %%E      -> [%E]\n", 3.14159);
  aprintf("  %%g      -> [%g]\n", 0.001234);
  aprintf("  %%G      -> [%G]\n", 0.001234);
  aprintf("  negative -> [%.2f]\n", -123.456);
  aprintf("  zero     -> [%.2f]\n", 0.0);
  aprintf("\n");

  // -----------------------------------------------------------------------
  // String and character formatting
  // -----------------------------------------------------------------------
  aprintf("=== Strings & Characters ===\n");
  aprintf("  %%s       -> [%s]\n", "hello");
  aprintf("  %%10s     -> [%10s]\n", "hi");
  aprintf("  %%-10s    -> [%-10s]\n", "hi");
  aprintf("  %%.3s     -> [%.3s]\n", "arduino");
  aprintf("  %%c       -> [%c]\n", 'X');
  aprintf("\n");

  // -----------------------------------------------------------------------
  // Pointer formatting
  // -----------------------------------------------------------------------
  aprintf("=== Pointer ===\n");
  int dummy = 0;
  aprintf("  %%p       -> [%p]\n", &dummy);
  aprintf("\n");

  // -----------------------------------------------------------------------
  // fmt() — buffer version
  // -----------------------------------------------------------------------
  aprintf("=== fmt() Buffer Version ===\n");
  const char *buf = fmt("  buf: %04X | %s | %.2f", 0x3F, "done", 9.81);
  aprintf("  %s\n", buf);
  aprintf("\n");

  aprintf("=== End of examples ===\n\n");
}

// ---------------------------------------------------------------------------
void loop() {
  // Nothing to do — all examples run once in setup()
}