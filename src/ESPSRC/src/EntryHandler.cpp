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

namespace EntryHandler
{


    void HandleEntry(uint8_t cardid[3])
    {
        #ifdef DEBUGINFO
        uint8_t manufacturerID = cardid[0];
        uint16_t UID = (cardid[1] << 8) | cardid[2];
        DebugInfo("ManID: " + String(manufacturerID) + " UID: " + String(UID));
        #endif

        User user = User();

        if(!Users::GetUserByID(cardid, user))
        {
            DevLogs::Create("Invalid Card: " + String((int)UID));
            LCD::PrintCenter("Gecersiz Kart");
            delay(1000);
            return;
        }

        if(!EntryLogs::UserEligibleForEntry(cardid))
        {
            LCD::PrintCenter("Zaten Giris veya Cikis Yapildi");
            delay(4000);
            return;
        }

        bool suc = EntryLogs::NewEntryLog(EntryLog(cardid, TimeModule::GetEpoch()));

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