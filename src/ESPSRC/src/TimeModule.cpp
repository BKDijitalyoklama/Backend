#include "ESPDateTime.h"
#include "DateTimeTZ.h"
#include "Debugger.h"
#include "time.h"
#include "ctime"

namespace TimeModule
{
    bool initialized = false;

    bool Initialize()
    {
        DebugInfo("Initializing TimeModule");
        
        DateTime.setTimeZone(TZ_Europe_Istanbul);

        DateTime.begin(10000);
        
        return initialized = DateTime.isTimeValid();
    }

    tm ConvertEpoch(const time_t epoch)
    {
        return *localtime(&epoch);
    }

    bool ForceUpdate()
    {
        return DateTime.forceUpdate(10000);
    }
    

    String GetDateTimeString()
    {
        if(!initialized) return "???";

        return DateTime.toString();
    }

    time_t GetEpoch()
    {
        return DateTime.getTime();
    }

    time_t GetEpoch(tm base)
    {
        return mktime(&base);
    }

    tm Now()
    {
        return ConvertEpoch(GetEpoch());
    }
}