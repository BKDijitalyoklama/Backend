#include <Arduino.h>
#include "Debugger.h"
#include "Pinout.h"
#include "DataSaving/Users.h"
#include "EntryHandler.h"
#include "DataSaving/Config.h"
#include "Report/ReportHandler.h"
#include "Network/NetworkConnection.h"

namespace RFIDReader
{

    inline bool CheckSpecialCards(uint8_t cardID[3])
    {
        if(memcmp(cardID, Config::SCard_IP, 3) == 0)
        {
            NetworkConnection::DisplayIPAddr();
        }
        else if(memcmp(cardID, Config::SCard_CreateReport, 3) == 0)
        {
            ReportHandler::CreateReport();
        }
        else if(memcmp(cardID, Config::SCard_Restart, 3) == 0)
        {
            ESP.restart();
        }
        else
        {
            return false;
        }

        return true;
    }

    inline void HandleCard(uint8_t cardID[3])
    {
        DebugInfo("RFID Card Detected");
        DebugInfo("ManID: " + String(cardID[0]) + " / CardID: " + String(cardID[1] << 8 | cardID[2]));

        if(Users::AutoAdd::autoAdd_Active && !Users::AutoAdd::autoAdd_Expire.isExpired())
        {
            Users::AutoAdd::HandleAutoAdd(cardID);
            return;
        }

        if(CheckSpecialCards(cardID)) return;
        
        EntryHandler::HandleEntry(cardID);
    }

    void ClearBuffer()
    {
        DebugWarning("RFID Buffer has bytes left: " + String(Serial2.available()));

        while (Serial2.available() > 0)
        {
            Serial2.read();
        }

        DebugInfo("Cleared RFID Buffer");
    }

    void RFIDCheck()
    {
        if (Serial2.available() < 4)
            return;
        
        if(Serial2.available() > 4) ClearBuffer();

        uint8_t buf[4];

        size_t readres = Serial2.readBytes(buf, 4);

        if (Serial2.available() > 0)
        {
            ClearBuffer();
            return;
        }

        if (readres != 4)
        {
            DebugWarning("RFID readres error, clearing buffer");

            ClearBuffer();
            return;
        }

        if (buf[0] != 0)
        {
            DebugWarning("RFID buffer[0] not 0, clearing buffer");
            
            ClearBuffer();
            
            return;
        }

        uint8_t cardInfo[3];

        memcpy(&cardInfo, &buf[1], 3);

        if(cardInfo[0] == 0 || (cardInfo[1] << 8 | cardInfo[2]) == 0)
        {
            DebugWarning("RFID cardInfo is 0, clearing buffer");
            ClearBuffer();
            return;
        }

        HandleCard(cardInfo);
        if(Serial2.available() > 0)  ClearBuffer();
    }

    bool Initialize()
    {
        DebugInfo("Initializing RFID Reader");

        Serial2.begin(19200);

        return true;
    }

}