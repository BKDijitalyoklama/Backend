#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include "AsyncDelay.h"

namespace EntryHandler
{
    void HandleEntry(uint8_t cardid[3]);
}