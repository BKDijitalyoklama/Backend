#include <Arduino.h>
#include "Debugger.h"
#include "Feedback/LCD.h"
#include "AsyncDelay.h"
#include "ESPAsyncWebServer.h"
#include "Feedback/Buzzer.h"
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

        LCD::PrintCenter("Lutfen Bekleyin...");

        ServerConnection::EntryLogResult result = ServerConnection::NewEntrylog(cardid);

        if(result == ServerConnection::EntryLogResult::ERROR)
        {
            DebugError("Couldn't save user entry to server");
            return;
        }

        if(result == ServerConnection::EntryLogResult::USERNOTFOUND)
        {
            LCD::PrintCenter("Gecersiz Kart");
            delay(1000);
            return;
        }

        if(result == ServerConnection::EntryLogResult::ENTRYSUCCESS)
        {
            LCD::Clear();
            LCD::PrintCenterRow("Kaydedildi", 0);
            Buzzer::Play_UserEntrySound();
            delay(2000);
            return;
        }
        else
        {
            LCD::PrintCenter("Hata Olustu, Tekrar deneyin");
        }
        
        delay(2000);
    }
}