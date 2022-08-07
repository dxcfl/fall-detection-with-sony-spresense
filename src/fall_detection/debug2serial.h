/*
  debug2serial.h

    Macros to switch off/on debug output to serial interface.

    Read more: https://github.com/dxcfl/fall-detection-with-sony-spresense#readme
    Git: https://github.com/dxcfl/fall-detection-with-sony-spresense
    (c) 2022 by dxcfl
*/

#ifndef DEBUG2SERIAL_H
#define DEBUG2SERIAL_H

#ifdef DEBUG
    #ifdef DEBUG_VERBOSE
        #define DEBUG_SERIAL_INIT(baud) \
            Serial.begin(baud); \
            Serial.println("DEBUG ENABLED")
        #define DEBUG_SERIAL_FLUSH() Serial.flush() 
        #define DEBUG_SERIAL_PRINT(...)  \
            Serial.print(millis());     \
            Serial.print(": ");    \
            Serial.print(__PRETTY_FUNCTION__); \
            Serial.print(' ');      \
            Serial.print(__LINE__);     \
            Serial.print(' ');      \
            Serial.print(__VA_ARGS__)
        #define DEBUG_SERIAL_PRINTLN(...)  \
            Serial.print(millis());     \
            Serial.print(": ");    \
            Serial.print(__PRETTY_FUNCTION__); \
            Serial.print(' ');      \
            Serial.print(__LINE__);     \
            Serial.print(' ');      \
            Serial.println(__VA_ARGS__)
        #define DEBUG_SERIAL_PRINTF(...)  \
            Serial.print(millis());     \
            Serial.print(": ");    \
            Serial.print(__PRETTY_FUNCTION__); \
            Serial.print(' ');      \
            Serial.print(__LINE__);     \
            Serial.print(' ');      \
            Serial.printf(__VA_ARGS__)
    #else
        #define DEBUG_SERIAL_INIT(baud) Serial.begin(baud)
        #define DEBUG_SERIAL_PRINT(...) Serial.print(__VA_ARGS__)
        #define DEBUG_SERIAL_PRINTLN(...) Serial.println(__VA_ARGS__)
        #define DEBUG_SERIAL_PRINTF(...) Serial.printf(__VA_ARGS__)
    #endif
#else
    #define DEBUG_SERIAL_INIT(baud)
    #define DEBUG_SERIAL_FLUSH()
    #define DEBUG_SERIAL_PRINT(...)
    #define DEBUG_SERIAL_PRINTLN(...)
    #define DEBUG_SERIAL_PRINTF(...)
#endif

#endif