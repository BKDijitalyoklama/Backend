#include <Arduino.h>
#include "DataSaving/EntryLogs.h"
#include "DataSaving/Users.h"
#include "Debugger.h"
#include "Feedback/LCD.h"
#include "AsyncDelay.h"
#include "ESPAsyncWebServer.h"
#include "DataSaving/EntryLogs.h"
#include "Feedback/Buzzer.h"
#include "DataSaving/DevLogs.h"
#include "Network/ServerConnection.h"

namespace EntryHandler
{


    void HandleEntry(uint8_t cardid[3]) // TODO: Complete the function
    {
        #ifdef DEBUGINFO
        uint8_t manufacturerID = cardid[0];
        uint16_t UID = (cardid[1] << 8) | cardid[2];
        DebugInfo("ManID: " + String(manufacturerID) + " UID: " + String(UID));
        #endif

        User user = User();

        ServerConnection::EntryLogResponse result = ServerConnection::NewEntrylog(cardid);

        if(!result.success || result.code == 500)
        {
            DebugError("Couldn't save user entry to server");
            return;
        }

        if(result.code == 404)
        {
            LCD::PrintCenter("Gecersiz Kart");
            delay(1000);
            return;
        }

        if(suc)
        {
            LCD::PrintCenterRow((char*)user.Name, 0);
            LCD::PrintCenterRow("Kaydedildi", 1);
            Buzzer::Play_UserEntrySound();
        }
        else
        {
            
            DevLogs::Create("Error Saving User Entry: " + String((int)UID));
            DebugError("Couldn't save user entry");
        }

        delay(2000);
    }
}