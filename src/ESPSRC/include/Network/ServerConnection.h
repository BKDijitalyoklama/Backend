
#pragma once

#include <WiFi.h>
#include <HTTPClient.h>
#include "Debugger.h"
#include <esp_wifi.h>

namespace ServerConnection
{

    enum EntryLogResult
    {
        ENTRYSUCCESS,
        USERNOTFOUND,
        COOLDOWN,
        ERROR
    };

    EntryLogResult NewEntrylog(uint8_t id[3]);
    
}