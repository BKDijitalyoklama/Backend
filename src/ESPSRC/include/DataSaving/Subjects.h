#include "DataSaving/DataSaving.h"
#include "DataSaving/Paths.h"

struct Subject
{
    char Name[10];
    uint8_t ID[1];

    Subject(char _name[10], uint8_t _id[1])
    {
        memset(Name, 0, 10);
        memset(ID, 0, 1);

        memcpy(Name, _name, 10);
        memcpy(ID, _id, 1);
    }

    Subject(char _name[10], uint8_t _id)
    {
        memset(Name, 0, 10);
        memset(ID, 0, 1);

        memcpy(Name, _name, 10);
        memcpy(ID, &_id, 1);
    }

    Subject()
    {
        
    }
};

namespace Subjects // SUBJECT: 11 BYTES // 1: ID // 10: NAME
{
    // NAME MUST BE ASCII

    void ConvertSubjectToBytes(Subject subject, uint8_t* buffer);
    File GetSubjectsFile(bool& suc);
    bool SubjectExists(char* Name);
    bool SubjectExists(uint8_t ID);
    bool GetSubjectByID(uint8_t ID, Subject & subject);
    bool AddSubject(Subject subject);
    String GetSubjectName(uint8_t subjectID, bool& suc);
    uint8_t GetNextID();
    bool DeleteSubjectByID(uint8_t ID);

    Subject* GetAllSubjects(int& count);
}