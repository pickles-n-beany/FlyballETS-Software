// file: LightsController.cpp
// summary: Implements the lights controller class

#include "LightsController.h"
#include "RaceHandler.h"
#include <Arduino.h>
#include "config.h"
#include "Structs.h"
#include "WebHandler.h"
#include <Adafruit_NeoPixel.h>

void LightsControllerClass::init(Adafruit_NeoPixel *LightsStrip) {
   _LightsStrip = LightsStrip;
   _LightsStrip->begin();
   _LightsStrip->show();
   log_i("LightsController initialized.");
}

void LightsControllerClass::Main() {
   // Handle race ready fault logic
   if (bS1ExecuteRaceReadyFaultON && !_bS2RaceReadyFaultActive) {
      RaceReadyFault(ON); _bS1RaceReadyFaultActive = true;
   }
   if (bS2ExecuteRaceReadyFaultON && !_bS1RaceReadyFaultActive) {
      RaceReadyFault(ON); _bS2RaceReadyFaultActive = true;
   }
   if (bS1ExecuteRaceReadyFaultOFF && !_bS2RaceReadyFaultActive) {
      RaceReadyFault(OFF); _bS1RaceReadyFaultActive = false;
   }
   if (bS2ExecuteRaceReadyFaultOFF && !_bS1RaceReadyFaultActive) {
      RaceReadyFault(OFF); _bS2RaceReadyFaultActive = false;
   }

   bS1ExecuteRaceReadyFaultON = false;
   bS2ExecuteRaceReadyFaultON = false;
   bS1ExecuteRaceReadyFaultOFF = false;
   bS2ExecuteRaceReadyFaultOFF = false;

   if (bExecuteResetLights) {
      ResetLights();
      bExecuteResetLights = false;
   }

   // Handle light schedules
   for (int i = 0; i < 8; i++) {
      if (_lLightsOnSchedule[i] && millis() > _lLightsOnSchedule[i]) {
         ToggleLightState(_byLightsArray[i], ON);
         _lLightsOnSchedule[i] = 0;
      }
      if (_lLightsOutSchedule[i] && millis() > _lLightsOutSchedule[i]) {
         ToggleLightState(_byLightsArray[i], OFF);
         _lLightsOutSchedule[i] = 0;
#ifdef WiFiON
         if (i < 2) WebHandler.bSendLightsAndRaceData = true;
#endif
      }
   }

   _LightsStrip->show();

   if (_byCurrentLightsState != _byNewLightsState) {
      _byCurrentLightsState = _byNewLightsState;
#ifdef WiFiON
      WebHandler.bSendLightsAndRaceData = true;
#endif
   }
}

void LightsControllerClass::InitiateStartSequence() {
   uint32_t now = millis();
   uint16_t step = 1000;
   for (int i = 0; i < 4; i++) {
   _lLightsOnSchedule[i * 2]     = now + (i * step);
   _lLightsOutSchedule[i * 2 + 1] = now + ((i + 1) * step);
   }
}

void LightsControllerClass::ResetLights() {
   byOverallState = RESET;
   for (uint16_t i = 0; i < _LightsStrip->numPixels(); i++)
      _LightsStrip->setPixelColor(i, 0);
   _byNewLightsState = 0;
   DeleteSchedules();
}

void LightsControllerClass::DeleteSchedules() {
   for (int i = 0; i < 8; i++) {
      _lLightsOnSchedule[i] = 0;
      _lLightsOutSchedule[i] = 0;
   }
   _lLightsOnSchedule[0] = millis();
   _lLightsOutSchedule[0] = millis() + 100;
}

void LightsControllerClass::ToggleLightState(Lights byLight, LightStates byLightState) {
   bool current = CheckLightState(byLight);

   // Convert enum to pixel index and color directly
   uint8_t pixelNumber = static_cast<uint8_t>(byLight);
   uint32_t color = (byLightState == OFF) ? _LightsStrip->Color(0, 0, 0) : _LightsStrip->Color(255, 255, 255); // Example: white when ON

   // Trigger state changes for START logic
   if (byLightState == ON && byOverallState == INITIATED && pixelNumber == 1) {
      RaceHandler.bExecuteStartRaceTimer = true;
      byOverallState = STARTING;
   } else if (byOverallState == STARTING && pixelNumber == 4) {
      byOverallState = STARTED;
   }

   for (int chain = 0; chain < LIGHTSCHAINS; chain++) {
      uint8_t pixelIndex = pixelNumber + 5 * chain;
      _LightsStrip->setPixelColor(pixelIndex, color);
      log_d("Light %d -> %d", pixelIndex, byLightState);
   }

   if (current != byLightState)
      _byNewLightsState += (byLightState == ON) ? byLight : -byLight;
}

void LightsControllerClass::ToggleFaultLight(uint8_t DogNumber, LightStates state) {
   if (DogNumber >= 4) return;
   _lLightsOnSchedule[1] = millis();
   _lLightsOutSchedule[1] = millis() + 1000;
   ToggleLightState(_byDogErrorLights[DogNumber], state);
}

void LightsControllerClass::RaceReadyFault(LightStates state) {
   ToggleLightState(_byLightsArray[0], state);
}

stLightsState LightsControllerClass::GetLightsState() {
   stLightsState state;
   state.State[0] = CheckLightState(WHITE0) ? 1 : (CheckLightState(RED0) ? 2 : 0);
   state.State[1] = CheckLightState(YELLOW1) ? 1 : (CheckLightState(RED1) ? 2 : 0);
   state.State[2] = CheckLightState(YELLOW2) ? 1 : (CheckLightState(BLUE2) ? 2 : 0);
   state.State[3] = CheckLightState(YELLOW3) ? 1 : 0;
   state.State[4] = CheckLightState(GREEN4) ? 1 : 0;
   return state;
}

void Core1Lights(void* pvParameters) {
   while (true) {
      LightsController.Main();
      vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}

LightsControllerClass::LightStates LightsControllerClass::CheckLightState(Lights byLight) {
   SNeoPixelConfig config = _GetNeoPixelConfig(byLight);
   return (_LightsStrip->getPixelColor(config.iPixelNumber) == config.iColor) ? ON : OFF;
}

LightsControllerClass::SNeoPixelConfig LightsControllerClass::_GetNeoPixelConfig(Lights byLight) {
   SNeoPixelConfig cfg;
   switch (byLight) {
      case WHITE0:  cfg = {0, _LightsStrip->Color(255, 255, 255)}; break;
      case RED0:    cfg = {0, _LightsStrip->Color(255, 0, 0)};     break;
      case YELLOW1: cfg = {1, _LightsStrip->Color(255, 100, 0)};   break;
      case RED1:    cfg = {1, _LightsStrip->Color(255, 0, 0)};     break;
      case YELLOW2: cfg = {2, _LightsStrip->Color(255, 100, 0)};   break;
      case BLUE2:   cfg = {2, _LightsStrip->Color(0, 0, 255)};     break;
      case YELLOW3: cfg = {3, _LightsStrip->Color(255, 100, 0)};   break;
      case GREEN4:  cfg = {4, _LightsStrip->Color(0, 255, 0)};     break;
   }
   return cfg;
}

LightsControllerClass LightsController;
