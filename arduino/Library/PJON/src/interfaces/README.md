
### What is an interface?
PJON uses interfaces to handle low level system calls using the pre-processor, abstracting all architecture related implementation and enhancing portability without affecting the memory footprint. Thanks to the interfaces abstraction the PJON source is released in one unified implementation that compiles in all supported devices, architectures or operative systems. An interface is simply a file containing a set of common macros pointing to dedicated system calls. Interfaces provide users with an easy way to port PJON on virtually every architecture able to execute a compiled C++ program, simply filling a list of method definitions.

Here as an example, the Arduino interface already present in PJON:
```cpp
// Use an architecture related constant to conditionally include the interface
#if defined(ARDUINO)
  #include <Arduino.h>
  #include "PJON_IO.h"
  // Architecture related necessary files inclusion

  /* IO methods definition ------------------------------------------------- */

  #if !defined(PJON_ANALOG_READ)
    #define PJON_ANALOG_READ analogRead
  #endif

  #if !defined(PJON_IO_WRITE)
    #define PJON_IO_WRITE digitalWrite
  #endif

  #if !defined(PJON_IO_READ)
    #define PJON_IO_READ digitalRead
  #endif

  #if !defined(PJON_IO_MODE)
    #define PJON_IO_MODE pinMode
  #endif

  #if !defined(PJON_IO_PULL_DOWN)
    #define PJON_IO_PULL_DOWN(P) \
      do { \
        digitalWrite(P, LOW); \
        pinMode(P, INPUT); \
      } while(0)
  #endif

  /* Random ----------------------------------------------------------------- */

  #ifndef PJON_RANDOM
    #define PJON_RANDOM random
  #endif

  #ifndef PJON_RANDOM_SEED
    #define PJON_RANDOM_SEED randomSeed
  #endif

  /* Serial ----------------------------------------------------------------- */

  #ifndef PJON_SERIAL_AVAILABLE
    #define PJON_SERIAL_AVAILABLE(S) S->available()
  #endif

  #ifndef PJON_SERIAL_WRITE
    #define PJON_SERIAL_WRITE(S, C) S->write(C)
  #endif

  #ifndef PJON_SERIAL_READ
    #define PJON_SERIAL_READ(S) S->read()
  #endif

  #ifndef PJON_SERIAL_FLUSH
    #define PJON_SERIAL_FLUSH(S) S->flush()
  #endif

  /* Timing ----------------------------------------------------------------- */

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif
#endif
```
