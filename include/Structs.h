#pragma once
#include <Arduino.h>  // This brings in uint8_t, uint16_t, etc.
#include <stdint.h>
#include <rom/rtc.h>

struct stLightsState {
  uint8_t State[5];
};

// ✅ ADD THIS STRUCT FOR LIGHT CONFIGURATION
struct SNeoPixelConfig {
  uint8_t iPixelNumber;
  uint32_t iColor;
};