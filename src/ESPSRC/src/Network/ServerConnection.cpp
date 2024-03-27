#include <WiFi.h>
#include <HTTPClient.h>
#include "Debugger.h"
#include <esp_wifi.h>
#include "Network/ServerConnection.h"

namespace ServerConnection
{
    struct RequestResponse
    {
        String data;
        int code;
        bool success;
    };

    const char* root = "http://0.0.0.0";

    String GetMacAddress()
    {
        uint8_t baseMac[6];
        esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
        char baseMacChr[13] = {0};
        snprintf(baseMacChr, sizeof(baseMacChr), "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
        return String(baseMacChr);
    }

    RequestResponse GETRequest(String path)
    {
        HTTPClient client;
        if(!client.begin(root + path))
        {
            DebugError("Failed to connect to server");
            return { "", 0, false };
        }

        int code = client.GET();

        if(code <= 0)
        if(!client.begin(root + path))
        {
            DebugError("Failed to connect to server");
            return { "", 0, false };
        }

        RequestResponse resp;

        resp.success = true;

        resp.code = code;
        resp.data = client.getString();

        return resp;
    }



    EntryLogResponse NewEntrylog(uint8_t idbytes[3])
    {
        EntryLogResponse resp;

        uint id = idbytes[0] << 16 | idbytes[1] << 8 | idbytes[2];

        String path = "/logs/new?mac=" + GetMacAddress() + "&userid=" + id;

        RequestResponse response = GETRequest(path);

        resp.code = response.code;
        resp.success = response.success;

        resp.isEntry = response.code == 200;
        resp.namesurname = response.data;

        return resp;
    }

    
}