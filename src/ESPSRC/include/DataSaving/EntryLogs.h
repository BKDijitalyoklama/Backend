#pragma once

#include "DataSaving/DataSaving.h"
#include "DataSaving/Paths.h"
#include "DataSaving/Users.h"
#include "Debugger.h"
#include "TimeModule.h"


struct EntryLog // 11 BYTES // 3: USER ID // 8: TIME
{
    uint8_t userID[3];
    uint64_t time;

    EntryLog(uint8_t _userid[3], uint64_t _time)
    {
        memcpy(userID, _userid, 3);

        time = _time;
    }

    EntryLog(uint8_t buf[11])
    {
        memcpy(userID, &buf[0], 3);
        memcpy(&time, &buf[3], 8);
    }

    EntryLog()
    {

    }

    bool IsWithinXMinutes(uint16_t minutes)
    {
        time_t now = TimeModule::GetEpoch();

        return (now - time) <= minutes * 60;
    }

    void ConvertToBytes(uint8_t* buf)
    {
        memcpy(&buf[0], userID, 3);
        memcpy(&buf[3], &time, 8);
    }
    


};

namespace EntryLogs
{
    bool NewEntryLog(EntryLog log);
    int GetEntryLogCount();
    bool OpenWriteTodaysFile(File& file);
    
    bool UserEligibleForEntry(uint8_t userID[3]);
    
    EntryLog* GetTodaysEntryLogs(int& count);
}