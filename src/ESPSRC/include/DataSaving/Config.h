#pragma once
#include <Arduino.h>

struct NetworkCredentials
{
    String SSID;
    String PASSWORD;

};

namespace Config
{
    bool Initialize();

    extern String MasterKey;
    extern NetworkCredentials NetworkCreds;

    extern uint8_t SCard_IP[3];
    extern uint8_t SCard_CreateReport[3];
    extern uint8_t SCard_Restart[3];
}