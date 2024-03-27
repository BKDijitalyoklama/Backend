#include "DataSaving/Config.h"
#include "Debugger.h"

namespace Config
{
    String MasterKey = "12345";
    NetworkCredentials NetworkCreds = NetworkCredentials();

    uint8_t SCard_IP[3] = {87, 0x52, 0xA1};
    uint8_t SCard_CreateReport[3] = {210, 0x38, 0x39};
    uint8_t SCard_Restart[3] = {87, 47, 197};

    bool Initialize()
    {
        NetworkCreds.PASSWORD = "8K-$cH0oL!";
        NetworkCreds.SSID = "BK_School";
        DebugInfo("Initializing Config");
        return true;
    
    }




}