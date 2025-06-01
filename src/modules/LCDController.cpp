// file:	LCDController.cpp
//
// summary:	Implements the LCD controller class
// Copyright (C) 2019 Alex Goris
// This file is part of FlyballETS-Software
// FlyballETS-Software is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>

#include "config.h"
#include "LCDController.h"
#include <Arduino.h>
#include "config.h"
#include "Structs.h"

/// <summary>
///   Initialises LCD.
/// </summary>

void LCDControllerClass::initI2C()
{
    lcd1_i2c.begin(20, 4);           // 20 columns, 4 rows
    lcd1_i2c.setBacklight(255);      // Full brightness
    lcd1_i2c.clear();
    lcd1_i2c.setCursor(0, 0);
    lcd1_i2c.print("Init LCD...");
    vTaskDelay(2000);
    _DisplayInit();
}


void LCDControllerClass::_DisplayInit()
{
// Initial splash (aligned visually)
    _UpdateLCD(1, 0, String("   Flyball ETS   "), 20);
    _UpdateLCD(2, 0, String(" Welcome Titans  "), 20);
    _UpdateLCD(3, 0, String("   FW: ") + String(FW_VER), 20);
    _UpdateLCD(4, 0, String(" Initialising... "), 20);
    vTaskDelay(3500);

    // Clear screen to start live fields
    lcd1_i2c.clear();

    // Live display layout: D#:  time  +cross
    _UpdateLCD(1, 0, String("D1:  0.000 +0.000"), 20);
    _UpdateLCD(2, 0, String("D2:  0.000 +0.000"), 20);
    _UpdateLCD(3, 0, String("D3:  0.000 +0.000"), 20);
    _UpdateLCD(4, 0, String("D4:  0.000 +0.000"), 20);

    // Field definitions: column spacing matches above
    _SlcdfieldFields[D1Time]       = {true, 1, 5, 6, String("0.000")};
    _SlcdfieldFields[D1CrossTime]  = {true, 1, 13, 6, String("0.000")};

    _SlcdfieldFields[D2Time]       = {true, 2, 5, 6, String("0.000")};
    _SlcdfieldFields[D2CrossTime]  = {true, 2, 13, 6, String("0.000")};

    _SlcdfieldFields[D3Time]       = {true, 3, 5, 6, String("0.000")};
    _SlcdfieldFields[D3CrossTime]  = {true, 3, 13, 6, String("0.000")};

    _SlcdfieldFields[D4Time]       = {true, 4, 5, 6, String("0.000")};
    _SlcdfieldFields[D4CrossTime]  = {true, 4, 13, 6, String("0.000")};
   //_SlcdfieldFields[D1RerunInfo] = {true, 1, 22, 2, String("  ")};
   //_SlcdfieldFields[D2RerunInfo] = {true, 2, 22, 2, String("  ")};
   //_SlcdfieldFields[D3RerunInfo] = {true, 3, 22, 2, String("  ")};
   //_SlcdfieldFields[D4RerunInfo] = {true, 4, 22, 2, String("  ")};
   //_SlcdfieldFields[TeamTime] = {true, 2, 33, 7, String("  0.000")};
   //_SlcdfieldFields[CleanTime] = {true, 3, 33, 7, String("  0.000")};
   //_SlcdfieldFields[RaceState] = {true, 1, 28, 7, String(" READY ")};
   //_SlcdfieldFields[RaceID] = {true, 1, 37, 3, String("  1")};
//#if BatteryCalibration
   //_SlcdfieldFields[BattLevel] = {true, 4, 27, 4, String("0000")};
//#else
  // _SlcdfieldFields[BattLevel] = {true, 4, 27, 3, String("UNK")};
//#endif
   //_SlcdfieldFields[WifiState] = {true, 4, 32, 1, String("W")};
   //_SlcdfieldFields[GpsState] = {true, 4, 34, 1, String(" ")};
   //_SlcdfieldFields[SDcardState] = {true, 4, 36, 2, String("  ")};
   //_SlcdfieldFields[BoxDirection] = {true, 4, 39, 1, String(">")};
}

/// <summary>
///   Main entry-point for this application, this function should be called in every main loop
///   cycle. It will check whether the last time we updated the LCD screen is more than the given
///   timeout, and if yes, it will update the LCD screen with the latest data.
/// </summary>
void LCDControllerClass::Main()
{
   // This is the main loop which handles LCD updates
   if (((millis() - _ulLastLCDUpdate) > iLCDUpdateInterval) && !bExecuteLCDUpdate)
      bExecuteLCDUpdate = true;

   if (bExecuteLCDUpdate)
   {
      // Handle LCD data updates
      _HandleLCDUpdates();
      uint8_t iLoopCounter = 0;
      for (const SLCDField &lcdField : _SlcdfieldFields)
      {
         if (lcdField.bUpdateFlag)
         {
            _UpdateLCD(lcdField.iLine, lcdField.iStartingPosition, lcdField.strText, lcdField.iFieldLength);
            _SlcdfieldFields[iLoopCounter].bUpdateFlag = false;
         }
         iLoopCounter++;
      }
      _ulLastLCDUpdate = millis();
      bExecuteLCDUpdate = false;
   }
}

/// <summary>
///   Updates a given pre-defined field on the LCD, with the new value.
/// </summary>
///
/// <param name="lcdfieldField"> The lcdfield identifier for which field should be updated </param>
/// <param name="strNewValue">   The new value. </param>
void LCDControllerClass::UpdateField(LCDFields lcdfieldField, String strNewValue)
{
   if (_SlcdfieldFields[lcdfieldField].iFieldLength < strNewValue.length())
   {
      log_e("[LCD Controller] Field (%i) has %i characters. Received string '%s' has %i characters",
            lcdfieldField, _SlcdfieldFields[lcdfieldField].iFieldLength, strNewValue.c_str(), strNewValue.length());
      return;
   }
   _SlcdfieldFields[lcdfieldField].strText = strNewValue;
   _SlcdfieldFields[lcdfieldField].bUpdateFlag = true;
}

/// <summary>
///   Updates the LCD. This function will update the correct portion of the LCD, based on which line and position we want to update.
/// </summary>
///
/// <param name="iLine">         Zero-based index of the line (1-4). </param>
/// <param name="iPosition">     Zero-based index of the starting position of the text which should be put on the screen. </param>
/// <param name="strText">       The text which should be put at the given position. </param>
/// <param name="iFieldLength">  Length of the field, if the given text is longer than this value, the text will be made scrolling within the given field length. </param>
void LCDControllerClass::_UpdateLCD(uint8_t iLine, uint8_t iPosition, String strText, int iFieldLength)
{
    if (iLine < 1 || iLine > 4) return;
    iLine--;  // Convert to 0-based index

    if (strText.length() > iFieldLength)
        strText = strText.substring(0, iFieldLength);
    else
        while (strText.length() < iFieldLength)
            strText += " ";

    lcd1_i2c.setCursor(iPosition, iLine);
    lcd1_i2c.print(strText);
}

void LCDControllerClass::_HandleLCDUpdates()
{
   // Clean Time
   /*
   if (bUpdateThisLCDField[CleanTime] || bUpdateTimerLCDdata)
   {
      String sReadCleanTime = RaceHandler.GetCleanTime();
      UpdateField(CleanTime, sReadCleanTime);
      bUpdateThisLCDField[CleanTime] = false;
      log_v("LCD CleanTime updated with string '%s'", sReadCleanTime.c_str());
   }
   */

   // Team Time
   /*
   if (bUpdateThisLCDField[TeamTime] || bUpdateTimerLCDdata)
   {
      String sReadRaceTime = RaceHandler.GetRaceTime();
      UpdateField(TeamTime, sReadRaceTime);
      bUpdateThisLCDField[TeamTime] = false;
      log_v("LCD TeamTime updated with string '%s'", sReadRaceTime.c_str());
   }
   */

   // Dog-specific fields
   for (int i = 0; i < RaceHandler.iNumberOfRacingDogs; i++)
   {
      int iRunNumber;
      if (RaceHandler.iDogRunCounters[i] > 0 && !RaceHandler.bRerunsOff)
         iRunNumber = RaceHandler.SelectRunNumber(i);
      else
         iRunNumber = 0;

      // Dog Time
      if (bUpdateThisLCDField[i] || bUpdateTimerLCDdata)
      {
         String sReadDogTime = RaceHandler.GetDogTime(i, iRunNumber);
         UpdateField(LCDFields(i), sReadDogTime);
         bUpdateThisLCDField[i] = false;
         log_v("LCD Dog %i time updated with string '%s'", i + 1, sReadDogTime.c_str());
      }

      // Cross Time
      /*
      if (bUpdateThisLCDField[i + 4] || bUpdateTimerLCDdata || (RaceHandler.iDogRunCounters[i] > 0 && !RaceHandler.bRerunsOff))
      {
         String sReadCrossingTime = RaceHandler.GetCrossingTime(i, iRunNumber);
         UpdateField(LCDFields(i + 4), sReadCrossingTime);
         bUpdateThisLCDField[i + 4] = false;
         log_v("LCD Dog %i Crossing time updated with string '%s'", i + 1, sReadCrossingTime.c_str(), iRunNumber, RaceHandler.iDogRunCounters[i]);
      }
      */

      // Rerun Info
      /*
      if (bUpdateThisLCDField[i + 8] || bUpdateTimerLCDdata || (RaceHandler.iDogRunCounters[i] > 0 && !RaceHandler.bRerunsOff))
      {
         String sReadRerunInfo = RaceHandler.GetRerunInfo(i, iRunNumber);
         UpdateField(LCDFields(i + 8), sReadRerunInfo);
         bUpdateThisLCDField[i + 8] = false;
         log_v("LCD Dog %i Re-run Info updated with string '%s'", i + 1, sReadRerunInfo.c_str(), iRunNumber, RaceHandler.iDogRunCounters[i]);
      }
      */
   }

   bUpdateTimerLCDdata = false;

   // Secondary screen fields (RaceState, RaceID, etc.)
   /*
   if (bUpdateNonTimerLCDdata)
   {
      bUpdateNonTimerLCDdata = false;
      for (uint8_t i = 14; i < 21; i++)
         _SlcdfieldFields[i].bUpdateFlag = true;
   }
   */

   // CRITICAL Battery warning (shortened for 16x4)
   if (_bCriticalBattery)
   {
      _UpdateLCD(1, 0, String(" LOW BATTERY!!! "), 16);
      _UpdateLCD(2, 0, String("  PLEASE CHARGE "), 16);
      _UpdateLCD(3, 0, String("      NOW       "), 16);
      _UpdateLCD(4, 0, String("                "), 16);
      LightsController.bExecuteResetLights = true;
      vTaskDelay(3000);
      esp_deep_sleep_start();
   }

   // Normal battery display (we tuck it in for now)
   else if ((millis() < 2000 || ((millis() - llLastBatteryLCDupdate) > 30000)) &&
            (RaceHandler.RaceState == RaceHandler.STOPPED || RaceHandler.RaceState == RaceHandler.RESET))
   {
      uint16_t iBatteryPercentage = BatterySensor.GetBatteryPercentage();
      String sBatteryPercentage;

      if (iBatteryPercentage == 9999)
      {
         sBatteryPercentage = "!!!";
         UpdateField(BattLevel, sBatteryPercentage);
         _bCriticalBattery = true;
      }
      else if (iBatteryPercentage == 9911)
         sBatteryPercentage = "USB";
      else if (iBatteryPercentage == 0)
         sBatteryPercentage = "LOW";
      else
         sBatteryPercentage = String(iBatteryPercentage);

      while (sBatteryPercentage.length() < 3)
         sBatteryPercentage = " " + sBatteryPercentage;

      // Optional: Battery corner label
      _UpdateLCD(4, 12, "B:" + sBatteryPercentage, 4);
      llLastBatteryLCDupdate = millis();
   }

}

void LCDControllerClass::DisplayReInit()
{
   // Clear all 4 lines (16 chars each)
   _UpdateLCD(1, 0, String("                "), 16);
   _UpdateLCD(2, 0, String("     Mode:      "), 16);
   _UpdateLCD(3, 0, String("                "), 16);
   _UpdateLCD(4, 0, String("                "), 16);

   // Minimal restart layout
   _UpdateLCD(1, 0, String("D1: 0.000+0.000"), 16);
   _UpdateLCD(2, 0, String("D2: 0.000+0.000"), 16);
   _UpdateLCD(3, 0, String("D3: 0.000+0.000"), 16);
   _UpdateLCD(4, 0, String("D4: 0.000+0.000"), 16);

   // Re-enable LCD data updates
   bUpdateNonTimerLCDdata = true;
   bUpdateTimerLCDdata = true;
   bExecuteLCDUpdate = true;
}

void LCDControllerClass::UpdateNumberOfDogsOnLCD(uint8_t iNumberOfDogs)
{
   // Clear all rows first
   _UpdateLCD(1, 0, String("                "), 16);
   _UpdateLCD(2, 0, String("                "), 16);
   _UpdateLCD(3, 0, String("                "), 16);
   _UpdateLCD(4, 0, String("                "), 16);

   // Draw only what we need
   if (iNumberOfDogs >= 1)
      _UpdateLCD(1, 0, String("D1: 0.000+0.000"), 16);
   if (iNumberOfDogs >= 2)
      _UpdateLCD(2, 0, String("D2: 0.000+0.000"), 16);
   if (iNumberOfDogs >= 3)
      _UpdateLCD(3, 0, String("D3: 0.000+0.000"), 16);
   if (iNumberOfDogs >= 4)
      _UpdateLCD(4, 0, String("D4: 0.000+0.000"), 16);

   // Wipe unused fields
   if (iNumberOfDogs < 4)
      _SlcdfieldFields[D4Time].strText = "       ";
   if (iNumberOfDogs < 3)
      _SlcdfieldFields[D3Time].strText = "       ";
   if (iNumberOfDogs < 2)
      _SlcdfieldFields[D2Time].strText = "       ";

   // Optional: Clear rerun info if needed
   for (int i = iNumberOfDogs; i < 4; i++)
      _SlcdfieldFields[D1RerunInfo + i].strText = "  ";

   // Mark LCD for refresh
   bUpdateNonTimerLCDdata = true;
   bUpdateTimerLCDdata = true;
   bExecuteLCDUpdate = true;
}

void LCDControllerClass::FirmwareUpdateInit()
{
   lcd1_i2c.clear();
   _UpdateLCD(1, 0, String(" Firmware Update "), 16);
   _UpdateLCD(2, 0, String("   Progress: 0%   "), 16);
}

void LCDControllerClass::FirmwareUpdateProgress(String strNewValue)
{
   String progressText = "   Progress: " + strNewValue + "%";
   // Pad or trim to 16 characters
   while (progressText.length() < 16) progressText += " ";
   if (progressText.length() > 16) progressText = progressText.substring(0, 16);

   _UpdateLCD(2, 0, progressText, 16);
}


void LCDControllerClass::FirmwareUpdateSuccess()
{
   _UpdateLCD(4, 0, String("  Update SUCCESS!   "), 16);
}

void LCDControllerClass::FirmwareUpdateError()
{
   _UpdateLCD(4, 0, String("  Update ERROR!  "), 16);
}

/// <summary>
///   The LCD controller.
/// </summary>
LCDControllerClass LCDController;