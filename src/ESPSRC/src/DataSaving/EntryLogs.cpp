#include "DataSaving/DataSaving.h"
#include "DataSaving/Paths.h"
#include "DataSaving/Subjects.h"
#include "Debugger.h"
#include "TimeModule.h"
#include "DataSaving/EntryLogs.h"

namespace EntryLogs // 11 BYTES // 3: USER ID // 8: TIME
{
    String GetTodaysFileName()
    {
        String str = Paths::DPATH_ENTRYLOGS + ("/" + TimeModule::GetDateTimeString().substring(0, 10) + ".bin");
        DebugInfo(str);
        return str;
    }

    bool NewEntryLog(EntryLog log)
    {
        uint8_t bytes[11];
        log.ConvertToBytes(bytes);

        return DataSaving::AppendFile(GetTodaysFileName().c_str(), bytes, 11);
    }

    bool UserEligibleForEntry(uint8_t userID[3])
    {

        time_t now = TimeModule::GetEpoch();

        File file;
        if(!DataSaving::OpenRead(GetTodaysFileName().c_str(), file)) return false;

        int entrylogcount = file.size() / 11;

        for (int i = 0; i < entrylogcount; i++)
        {
            if(!file.seek(i * 11)) continue;

            uint8_t buf[11];
            if(file.read(buf, 11) != 11) continue;
            EntryLog log = EntryLog(buf);
            

            if(memcmp(log.userID, userID, 3) == 0 && log.IsWithinXMinutes(1))
            {
                DebugInfo("User is not eligible for entry");
                file.close();
                return false;
            }
        }

        return true;
        
    }

    int GetEntryLogCount()
    {
        File file;
        if(!DataSaving::OpenRead(GetTodaysFileName().c_str(), file)) return -1;

        int count = file.size() / 11;

        file.close();

        return count;
    }

    bool OpenWriteTodaysFile(File& file)
    {
        return DataSaving::OpenWrite(GetTodaysFileName().c_str(), file);
    }

    /// @brief WARNING: DYNAMIC ALLOCATION
    /// @return 
    EntryLog* GetTodaysEntryLogs(int& count)
    {
        count = 0;

        File file;
        if(!DataSaving::OpenRead(GetTodaysFileName().c_str(), file, true)) return nullptr;

        int entrylogcount = file.size() / 11;

        EntryLog* logs = new EntryLog[entrylogcount]; // WARNING: DYNAMIC ALLOCATION

        for(int i = 0; i<entrylogcount; i++)
        {
            uint32_t offset = i * 11;
            if(!file.seek(offset)) continue;

            uint8_t buf[11];
            if(file.read(buf, 11) != 11) continue;

            logs[count] = EntryLog(buf);

            count++;
        }
        file.close();

        return logs;
    }
}