## FlyballETS-Software by simonttp78

This project is a fork of Flyball ETS project by vyruz1986 and Simonttp
ETS stands for Electronic Training System and it's kind of Electronic Judging System (EJS - used during official Flyball competitions) adjusted to regular Flyball trainings needs.

This changelog will be altered in future.

## Change log

#### Check [CHANGELOG.md](https://github.com/simonttp78/FlyballETS-Software/blob/maintain/CHANGELOG.md) file

## Source files

The source files are divided into 2 subprojects:

### Firmware

Main folder contains the sources for compiling the firmware which is to be flashed onto the ESP32 MCU.
It uses [PlatformIO](https://platformio.org/) to manage the board library, as well as other libraries needed.

### WebUI

This folder contains the angular sources for building the web interface used to control the system.
They need to be built first because they output a header file to the `include` folder which is needed to compile the firmware.
Make sure you have [NodeJS](https://nodejs.org/en/download/) installed, then run the following commands in the `WebUI` folder:

```bash
npm install
npm run build
```

## Thanks

This project was made possible by the following awesome libraries/projects and their developers:

- [TinyGPS++](http://arduiniana.org/libraries/tinygpsplus/) for interacting with the GPS module
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) for easily building JSON objects in Arduino
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) fork by me-no-def of very powerful base and featured webserver & WebSocket interface
- [Arduino core SDK for ESP32](https://github.com/espressif/arduino-esp32) for being able to use the ESP32 platform at all
- [Embedis](https://github.com/thingSoC/embedis) for providing an easy-to-use EEPROM interface to store settings
- [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) for reliably controlling WS2812 LEDs using ESP32
- [Timezone](https://github.com/JChristensen/Timezone) used for local time adjustments: time zones and daylight-saving time change

