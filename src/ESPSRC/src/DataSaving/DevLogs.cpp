#include <Arduino.h>
#include "DataSaving/DataSaving.h"
#include "TimeModule.h"
#include "Debugger.h"
#include "DataSaving/Paths.h"

namespace DevLogs
{
    bool Create(String str)
    {
        File file;
        if(!DataSaving::OpenWrite(Paths::FPATH_DEVLOGS, file)) return false;

        if(!file.seek(file.size())) return false;
        file.println('[' + TimeModule::GetDateTimeString() + "]: " + str);
        return true;
    }
    
}