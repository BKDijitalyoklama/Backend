#include <WiFi.h>
#include "Debugger.h"

#include "DataSaving/Config.h"
#include "Feedback/LCD.h"
#include "ElegantOTA.h"

#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <AsyncTCP.h>

namespace OTA
{

    AsyncWebServer server(80);
    unsigned long ota_progress_millis = 0;

    void onOTAStart()
    {
        Serial.println("OTA update started!");
    }

    void onOTAProgress(size_t current, size_t final)
    {
        if (millis() - ota_progress_millis > 1000)
        {
            ota_progress_millis = millis();
            DebugInfo("OTA Progress: " + String(map(current, 0, final, 0, 100)));
        }
    }

    void onOTAEnd(bool success)
    {
        if (success) DebugInfo("OTA update finished successfully");
        else DebugInfo("There was an error during OTA update!");
    }

    bool Initialize()
    {
        ElegantOTA.begin(&server);
        ElegantOTA.onStart(onOTAStart);
        ElegantOTA.onProgress(onOTAProgress);
        ElegantOTA.onEnd(onOTAEnd);

        server.begin();
        return true;
    }
}