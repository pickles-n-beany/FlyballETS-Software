#include <Arduino.h>
#include "config.h"
#include "Structs.h"
#include "SettingsManager.h"
#include "LightsController.h"
#include "RaceHandler.h"
#include "LCDController.h"
// #include "WebHandler.h"     // Uncomment if using web features
// #include "BatterySensor.h" // Uncomment if using battery features
// #include "SDcardController.h" // Uncomment if using SD

// Simulation support
#if Simulate
#include "Simulator.h"
#endif

// Function Prototypes
void IRAM_ATTR Sensor1Wrapper();
void IRAM_ATTR Sensor2Wrapper();
void ResetRace();
void StartStopRace();
void StartRaceMain();
void StopRaceMain();
void mdnsServerSetup();
void serialEvent();
void HandleSerialCommands();
void HandleRemoteAndButtons();
void ToggleWifi();
void FactoryReset();
void Core1Race(void* parameter);
void Core1Lights(void* parameter);
void Core1LCD(void* parameter);
String GetButtonString(uint8_t _iActiveBit);

// Pin assignments
static const uint8_t iS1Pin = 34;
static const uint8_t iS2Pin = 33;
static const uint8_t iLCDSdaPin = 47;
static const uint8_t iLCDSclPin = 48;
static const uint8_t iGPStxPin = 22;
static const uint8_t iGPSrxPin = 39;
static const uint8_t iGPSppsPin = 36;
static const uint8_t iLightsDataPin = 18;

// Global state
bool bCheckWsClinetStatus = false;
IPAddress ipTocheck;

unsigned int uiLastProgress = 0;
uint16_t iLaserOnTime = 180;
bool bLaserActive = false;

unsigned long long llLastDebounceTime = 0;
unsigned long long llPressedTime[8] = {0};
unsigned long long llReleasedTime[8] = {0};
uint8_t iLastActiveBit = 0;
byte byDataIn = 0;
byte byLastStadyState = 0;
byte byLastFlickerableState = 0;

const uint16_t DEBOUNCE_DELAY = 30;
const uint16_t SHORT_PRESS_TIME = 700;
const uint16_t VERYLONG_PRESS_TIME = 10000;

String strSerialData;
byte bySerialIndex = 0;
bool bSerialStringComplete = false;

TaskHandle_t taskRace;
TaskHandle_t taskLights;
TaskHandle_t taskLCD;
