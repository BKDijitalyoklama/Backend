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


        switch(result.state)
        {
            case ServerConnection::EntryLogResult::ENTRYSUCCESS:
                LCD::PrintCenter("Kaydedildi");
                Buzzer::Play_UserEntrySound();
                break;

            case ServerConnection::EntryLogResult::USERNOTFOUND:
                LCD::PrintCenter("Gecersiz Kart");
                break;

            case ServerConnection::EntryLogResult::COOLDOWN:
                LCD::PrintCenter("Zaten kart basildi");
            break;

            case ServerConnection::EntryLogResult::ERROR:
                DebugError("Couldn't save user entry to server");
                LCD::PrintCenter("Hata Olustu, Tekrar deneyin");
                break;

            default: return;
        }
        
        delay(2000);
    }
}