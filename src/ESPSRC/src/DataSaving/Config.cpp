#include "DataSaving/DataSaving.h"
#include "DataSaving/Config.h"
#include "DataSaving/Paths.h"
#include "Debugger.h"

namespace Config
{
    String MasterKey;
    NetworkCredentials NetworkCreds;

    uint8_t SCard_IP[3] = {87, 0x52, 0xA1};
    uint8_t SCard_CreateReport[3] = {210, 0x38, 0x39};
    uint8_t SCard_Restart[3] = {87, 47, 197};

    bool SetNetworkCreds()
    {
        const char splitter = ';';

        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_NETWORKCONFIG, file)) return false;
        String str = file.readString();

        int splitterIndex = str.indexOf(splitter);
        if(splitterIndex == -1) return false;

        NetworkCreds.SSID = str.substring(0, splitterIndex);
        NetworkCreds.PASSWORD = str.substring(splitterIndex + 1);
        return true;

    }

    bool SetMasterKey()
    {
        File file;

        if(!DataSaving::OpenRead(Paths::FPATH_MASTERKEY, file)) return false;
        MasterKey = file.readString();
        if(MasterKey == "") return false;

        return true;
    }

    bool Initialize()
    {
        DebugInfo("Initializing Config");
        return SetMasterKey() && SetNetworkCreds();
    }




}