// Copyright (C) 2019 Alex Goris
// This file is part of FlyballETS-Software
// FlyballETS-Software is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>

#ifndef GLOBALCONST_H
#define GLOBALCONST_H

#define MICROS esp_timer_get_time()  // ✅ Great — wraps the ESP32 microsecond timer

// ❗ These ESP config macros are okay to redefine if needed, 
// but you may want to verify these match sdkconfig if you're using IDF stuff.
// Here, it just forces UART dump:
#define CONFIG_ESP_COREDUMP_ENABLE_TO_UART 1

// ✅ Firmware version
#define FW_VER "0.0.1"

// ✅ Simulation flags
#define Simulate false
#define NumSimulatedRaces 10

// ✅ Trigger queue size (used for timing events?)
#define TRIGGER_QUEUE_LENGTH 110

// ✅ LED config
#define LIGHTSCHAINS 2  // 2 strips = 10 LEDs total if each chain has 5

// ✅ WiFi feature toggle
#define WiFiON  // Can be commented out to disable OTA/WebServer/etc.
//#define WebUIonSDcard  // Optional override
#define BatteryCalibration false

// ✅ EEPROM and flash settings
#define EEPROM_SIZE 4096
#define SPI_FLASH_SEC_SIZE 4096
#define U_PART U_SPIFFS  // Logical name for flash partition

// ✅ Websocket timeout
#define WS_TICKET_BUFFER_SIZE 8
#define WS_TIMEOUT 1800000  // 30 minutes in milliseconds

// ✅ Web UI version
#define APP_VER "1.2.0"

#endif
