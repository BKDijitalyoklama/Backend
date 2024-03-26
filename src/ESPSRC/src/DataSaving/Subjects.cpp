#include "DataSaving/DataSaving.h"
#include "DataSaving/Paths.h"
#include "DataSaving/Subjects.h"
#include "Debugger.h"
namespace Subjects // SUBJECT: 11 BYTES // 1: ID // 10: NAME
{
    // NAME MUST BE ASCII

    void ConvertSubjectToBytes(Subject subject, uint8_t* buffer)
    {
        memcpy(buffer, subject.ID, 1);
        memcpy(buffer + 1, subject.Name, strlen(subject.Name));
    }

    bool isSubjectDeleted(uint32_t offset, File& file)
    {
        file.seek(offset);
        uint8_t subID = 0;
        if(!file.read(&subID, 1)) return false;
        if(subID == 0) return true;

        return false;
    }

    bool SubjectExists(uint8_t ID)
    {
        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_SUBJECTS, file)) return false;

        for (uint16_t i = 0; i < file.size() / 11; i++)
        {
            uint16_t offset = i * 11;

            if(isSubjectDeleted(offset, file)) continue;

            file.seek(offset);
            uint8_t buffer[1];

            uint8_t idreadres = file.read(buffer, 1);
            if(idreadres != 1) continue;

            if(ID == buffer[0]) 
            {
                file.close();
                return true;
            }
        }

        file.close();

        return false;
    }
    
    bool SubjectExists(char* Name)
    {
        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_SUBJECTS, file)) return false;

        for (uint16_t i = 0; i < file.size() / 11; i++)
        {
            uint16_t offset = (i * 11);
            
            if(isSubjectDeleted(offset, file)) continue;

            file.seek(offset + 1);
            char buffer[10];

            uint8_t readres = file.read((uint8_t*)buffer, 10);

            if(readres != 10) continue;

            
            if(strcmp(Name, buffer) == 0)
            {
                file.close();
                return true;
            }
        }

        file.close();

        return false;
    }


    uint8_t GetNextID()
    {
        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_SUBJECTS, file)) return -1;

        int8_t maxID = 0;

        for (uint16_t i = 0; i < file.size() / 11; i++)
        {
            uint16_t offset = i * 11;
            if(isSubjectDeleted(offset, file)) continue;

            if(!file.seek(offset)) continue;

            uint8_t subID = 0;
            if(!file.read(&subID, 1)) continue;

            if(maxID < subID) maxID = subID;
        }

        file.close();

        return maxID + 1;
    }

    bool GetSubjectByID(uint8_t ID, Subject & subject)
    {
        
        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_SUBJECTS, file)) return false;

        for (uint16_t i = 0; i < file.size() / 11; i++)
        {
            uint16_t offset = i * 11;
            if(isSubjectDeleted(offset, file)) continue;

            file.seek(offset);
            uint8_t subID;

            uint8_t idreadres = file.read(&subID, 1);
            if(idreadres != 1) continue;

            if(ID == subID)
            {
                file.seek(offset + 1);

                char name[10];
                uint8_t namereadres = file.read((uint8_t*)name, 10);
                if(namereadres != 10) continue;

                subject = Subject(name, subID);
                file.close();
                return true;
            }
            
        }

        file.close();

        return false;
        
    }
    
    
    String GetSubjectName(uint8_t subjectID, bool& suc)
    {
        Subject sub;
        suc = GetSubjectByID(subjectID, sub);
        if(!suc) return String();

        return sub.Name;
    }

    bool AddSubject(Subject subject)
    {
        File file;
        if(!DataSaving::OpenWrite(Paths::FPATH_SUBJECTS, file)) return false;

        uint8_t buffer[11];
        memset(buffer, 0, sizeof(buffer));

        ConvertSubjectToBytes(subject, buffer);

        bool seekSuc = file.seek(file.size());
        if(!seekSuc) return false;

        uint8_t written = file.write(buffer, 11);

        file.close();
        return written == 11;
    }

    File GetSubjectsFile(bool& suc)
    {
        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_SUBJECTS, file)){suc = false; return file;}

        file.seek(0);
        suc = true;
        return file;
    }

    bool DeleteSubjectByID(uint8_t ID)
    {
        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_SUBJECTS, file)) return false;

        for (uint16_t i = 0; i < file.size() / 11; i++)
        {
            uint16_t offset = (i * 11);
            
            if(isSubjectDeleted(offset, file))
            {
                DebugInfo("A - Subject already deleted");
                continue;
            }

            if(!file.seek(offset))
            {
                DebugInfo("A - File seek err");
                continue;
            }

            uint8_t subID = 0;
            if(file.read(&subID, 1) != 1)
            {
                DebugInfo("A - Couldn't read ID");
                continue;
            }
            
            if (ID == subID)
            {
                file.close();
                if(!DataSaving::OpenWrite(Paths::FPATH_SUBJECTS, file)) return false;

                if(!file.seek(offset))
                {
                    
                    DebugInfo("A - File seek err2");
                    return false;
                }
                uint8_t written = file.write(0);
                file.close();
                DebugInfo("A - Written: " + String(written));
                return written == 1;
            }
            
        }

        file.close();
        DebugInfo("A- Default false");
        return false;
    }
    /// @brief WARNING: DYNAMIC ALLOCATION
    /// @param count 
    /// @return 
    Subject* GetAllSubjects(int& count)
    {
        File file;
        if(!DataSaving::OpenRead(Paths::FPATH_SUBJECTS, file)) return nullptr;

        int subjectCount = file.size() / 11;

        Subject* buf = new Subject[subjectCount]; // WARNING: DYNAMIC ALLOCATION

        count = 0;
        for (int i = 0; i < subjectCount; i++)
        {
            uint32_t offset = i*11;
            if(isSubjectDeleted(offset, file)) continue;
            if(!file.seek(offset)) continue;

            uint8_t subjectbuf[11];

            int readres = file.read(subjectbuf, 11);
            if(readres != 11) continue;

            buf[count] = Subject((char*)(subjectbuf + 1), subjectbuf[0]);

            count++;
        }

        file.close();
        return buf;
        
    }
}