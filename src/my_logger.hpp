#pragma once

#include <Arduino.h>
#include <TelnetStream.h>
#include <StreamUtils.h>

// Create a new stream that will forward all calls to Serial, and log to Serial.
// Everything will be written twice to the Serial!
static LoggingStream loggingStream(Serial, TelnetStream);
