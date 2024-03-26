#include <WiFi.h>
#include "Debugger.h"

#include "DataSaving/Config.h"
#include "Feedback/LCD.h"

namespace NetworkConnection
{
    bool Initialize()
    {
        NetworkCredentials creds = Config::NetworkCreds;

        DebugInfo("Initializing WiFi // SSID: " + creds.SSID + " // PW: " + creds.PASSWORD);

        if(creds.SSID == nullptr || creds.PASSWORD == nullptr) return false;

        WiFi.begin(creds.SSID.c_str(), creds.PASSWORD.c_str());

        uint8_t seconds = 15;

        for (uint8_t i = 0; i < seconds * 5; i++)
        {
            if(WiFi.status() == WL_CONNECTED || WiFi.status() == WL_CONNECT_FAILED) break;
        
            delay(200);
        }

        
        if(WiFi.status() == WL_CONNECTED)
        {
            DebugInfo("WiFi connected // IP: " + WiFi.localIP().toString());
            return true;
        }
        else
        {
            DebugInfo("WiFi connection failed");
            return false;
        }

        
        
    }

    void EnsureConnection()
    {
        if(WiFi.status() != WL_CONNECTED)
        {
            LCD::PrintCenter("WiFi Baglantisi Kesildi");
            delay(1000);
            LCD::PrintCenter("Yeniden Baslatiliyor");
            delay(1000);
            ESP.restart();
        }
    }

    void DisplayIPAddr()
    {
        LCD::Clear();
        LCD::PrintCenterRow("IP Adresi", 0, false);
        LCD::PrintCenterRow(WiFi.localIP().toString(), 1, false);
        delay(6000);
    }
}