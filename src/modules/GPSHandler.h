// GPSHandler.h
#ifndef _GPSHANDLER_h
#define _GPSHANDLER_h

#include <Arduino.h>
#include "config.h"
#include "Structs.h"
#include <TinyGPS++.h>
#include <Timezone.h>

class GPSHandlerClass {
private:
    bool _bGPSConnected = false;
    int _iCompilationYear;

    TinyGPSPlus _Tgps;
    HardwareSerial GPSSerial = HardwareSerial(1);

    char _cUTCTime[25];
    char _cLocalDateAndTime[25];
    char _cLocalTimestamp[9];
    char _cDate[11];

    unsigned long long llLastGPSRead = 0;

    void _HandleSerialPort();
    void _FormatTime();

public:
    void init(uint8_t rxPin, uint8_t txPin);
    void loop();

    char* GetUtcDateAndTime();
    char* GetLocalTimestamp();
    char* GetDate();

    time_t timeLocal;
};

extern GPSHandlerClass GPSHandler;

#endif