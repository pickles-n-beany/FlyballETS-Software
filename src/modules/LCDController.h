// LCDController.h
// Copyright (C) 2019 Alex Goris
// This file is part of FlyballETS-Software, licensed under the GNU GPL v3

#ifndef _LCDCONTROLLER_H
#define _LCDCONTROLLER_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include "LightsController.h"
#include "RaceHandler.h"
#include "BatterySensor.h"

class LCDControllerClass {
public:
    // LCD update timing
    uint16_t iLCDUpdateInterval = 451; // in ms
    bool bUpdateTimerLCDdata = false;
    bool bUpdateNonTimerLCDdata = false;
    bool bExecuteLCDUpdate = false;

    void initI2C();
    void DisplayReInit();
    void _DisplayInit();

    // Firmware update display messages
    void FirmwareUpdateInit();
    void FirmwareUpdateProgress(String strNewValue);
    void FirmwareUpdateSuccess();
    void FirmwareUpdateError();

    // Race-related UI methods
    void UpdateNumberOfDogsOnLCD(uint8_t iNumberOfDogs);
    void Main();

    // Fields that can be updated
    enum LCDFields {
        D1Time, D2Time, D3Time, D4Time,
        D1CrossTime, D2CrossTime, D3CrossTime, D4CrossTime,
        D1RerunInfo, D2RerunInfo, D3RerunInfo, D4RerunInfo,
        TeamTime, CleanTime, RaceState, RaceID,
        BattLevel, WifiState, GpsState, SDcardState, BoxDirection
    };

    volatile bool bUpdateThisLCDField[21];  // Update flags per field
    void UpdateField(LCDFields lcdField, String strNewValue);

private:
    void _UpdateLCD(uint8_t iLine, uint8_t iPosition, String strText, int iFieldLength);
    void _HandleLCDUpdates();

    LiquidCrystal_PCF8574 lcd1_i2c;

    unsigned long _ulLastLCDUpdate = 0;
    unsigned long _ulLastLCDUpdateWithTimes = 0;
    long long llLastBatteryLCDupdate = -25000;

    bool _bLCDRefresh = false;
    bool _bCriticalBattery = false;

    struct SLCDField {
        bool bUpdateFlag;
        uint8_t iLine;
        uint8_t iStartingPosition;
        uint8_t iFieldLength;
        String strText;
    };

    SLCDField _SlcdfieldFields[21];
};

extern LCDControllerClass LCDController;

#endif // _LCDCONTROLLER_H
