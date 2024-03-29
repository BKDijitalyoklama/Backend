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

    const char* root = "http://10.134.100.56:80";

    String GetMacAddress()
    {
        uint8_t baseMac[6];
        esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
        char baseMacChr[13] = {0};
        snprintf(baseMacChr, sizeof(baseMacChr), "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
        return String(baseMacChr);
    }

    RequestResponse GETRequest(String path, byte timeoutSeconds = 5)
    {
        HTTPClient client;
        if(!client.begin(root + path))
        {
            DebugError("Failed to connect to server");
            return { "", 0, false };
        }

        client.setTimeout(timeoutSeconds * 1000);

        int code = client.GET();

        if(code <= 0)
        {
            DebugError("Code: " + String(code));
            return { "", 0, false };
        }

        RequestResponse resp;

        resp.success = true;

        resp.code = code;
        resp.data = client.getString();

        client.end();


        return resp;
    }



    EntryLogResult NewEntrylog(uint8_t idbytes[3])
    {

        uint id = idbytes[0] << 16 | idbytes[1] << 8 | idbytes[2];

        String path = "/logs/new?mac=" + GetMacAddress() + "&userid=" + id;

        RequestResponse response = GETRequest(path);

        if(response.code == 500 || response.code <= 0 || !response.success) return EntryLogResult(EntryLogResult::ERROR, "");

        if(response.code == 404) return EntryLogResult(EntryLogResult::USERNOTFOUND, "");

        if(response.code == 210) return EntryLogResult(EntryLogResult::COOLDOWN, "");

        if(response.code == 200) return EntryLogResult(EntryLogResult::ENTRYSUCCESS, response.data);
        

        return EntryLogResult(EntryLogResult::ERROR, "");
    }

    
}