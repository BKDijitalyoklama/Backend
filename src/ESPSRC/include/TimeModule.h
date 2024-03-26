#pragma once

namespace TimeModule
{
    bool Initialize();
    
    tm ConvertEpoch(const time_t epoch);

    String GetDateTimeString();
    time_t GetEpoch();
    time_t GetEpoch(tm base);

    
    bool ForceUpdate();

    tm Now();
}