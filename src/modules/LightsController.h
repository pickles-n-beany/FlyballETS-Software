// LightsController.h
// Copyright (C) 2019 Alex Goris
// Licensed under the GNU GPL v3

#ifndef _LIGHTSCONTROLLER_H
#define _LIGHTSCONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "Structs.h"

class LightsControllerClass {
   friend class WebHandlerClass;

public:
   enum LightStates { OFF = 0, ON = 1 };

   enum Lights {
      WHITE0, RED0,      // pixel 0
      YELLOW1, RED1,     // pixel 1
      YELLOW2, BLUE2,    // pixel 2
      YELLOW3, GREEN4    // pixels 3 and 4
   };

   enum OverallStates { WARNING, INITIATED, RESET, STARTING, STARTED };
   OverallStates byOverallState = RESET;

   void init(Adafruit_NeoPixel* LightsStrip);
   void Main();

   void HandleStartSequence();
   void InitiateStartSequence();
   void WarningStartSequence();

   void ToggleLightState(uint8_t pixelIndex, uint32_t color);           // legacy direct pixel control
   void ToggleLightState(Lights byLight, LightStates state);           // main mapped LED control

   void ResetLights();
   void DeleteSchedules();
   void ToggleFaultLight(uint8_t iDogNumber, LightStates state);
   void RaceReadyFault(LightStates state);

   stLightsState GetLightsState();
   LightStates CheckLightState(Lights byLight);

   // Public control flags
   volatile bool bS1ExecuteRaceReadyFaultON = false;
   volatile bool bS2ExecuteRaceReadyFaultON = false;
   volatile bool bS1ExecuteRaceReadyFaultOFF = false;
   volatile bool bS2ExecuteRaceReadyFaultOFF = false;
   volatile bool bExecuteResetLights = false;

private:
   struct SNeoPixelConfig {
      uint8_t iPixelNumber;
      uint32_t iColor;
   };

   SNeoPixelConfig _GetNeoPixelConfig(Lights byLight);

   Adafruit_NeoPixel* _LightsStrip = nullptr;

   byte _byCurrentLightsState = 255;
   byte _byNewLightsState = 0;

   bool _bS1RaceReadyFaultActive = false;
   bool _bS2RaceReadyFaultActive = false;

   unsigned long _lLightsOnSchedule[8];
   unsigned long _lLightsOutSchedule[8];

   Lights _byLightsArray[8] = {
      WHITE0, RED0, YELLOW1, RED1,
      YELLOW2, BLUE2, YELLOW3, GREEN4
   };

   Lights _byDogErrorLights[4] = {
      RED1, BLUE2, YELLOW3, GREEN4
   };
};

extern LightsControllerClass LightsController;

#endif // _LIGHTSCONTROLLER_H
