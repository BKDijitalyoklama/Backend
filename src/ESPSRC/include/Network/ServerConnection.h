
#pragma once

#include <WiFi.h>
#include <HTTPClient.h>
#include "Debugger.h"
#include <esp_wifi.h>

namespace ServerConnection
{
    struct EntryLogResponse
    {
        bool success;
        int code;

        String namesurname;
        bool isEntry;
    };
    

    EntryLogResponse NewEntrylog(uint8_t id[3]);
    
}