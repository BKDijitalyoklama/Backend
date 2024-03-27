
#pragma once

#include <WiFi.h>
#include <HTTPClient.h>
#include "Debugger.h"
#include <esp_wifi.h>

namespace ServerConnection
{

    struct EntryLogResult
    {
        enum EntryLogState
        {
            ENTRYSUCCESS,
            USERNOTFOUND,
            COOLDOWN,
            ERROR
        } state;

        String namesurname;


        EntryLogResult(EntryLogState state, String namesurname)
        {
            this->state = state;
            this->namesurname = namesurname;
        }
        
    };

    EntryLogResult NewEntrylog(uint8_t id[3]);

}