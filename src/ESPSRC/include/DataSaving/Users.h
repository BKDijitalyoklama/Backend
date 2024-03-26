#pragma once
#include <Arduino.h>
#include "SD.h"
#include "AsyncDelay.h"
struct User  // USER: 24 BYTES // 1: SUBJECT // 3: ID // 20: NAME
{
    char Name[20];
    uint8_t ID[3];
    uint8_t SubjectID;

    User(char _name[20], uint8_t _id[3], uint8_t _subjectid)
    {
        memset(Name, 0, 20);
        memset(ID, 0, 3);
        
        SubjectID = _subjectid;
        memcpy(Name, _name, 20);
        memcpy(ID, _id, 3);
    }
    
    User(char _name[20], uint8_t _manid, uint16_t _cardid, uint8_t _subjectid)
    {
        memset(Name, 0, 20);
        memset(ID, 0, 3);


        SubjectID = _subjectid;

        memcpy(Name, _name, 20);

        ID[0] = _manid;
        ID[1] = _cardid >> 8;
        ID[2] = _cardid & 0xFF;
    }

    User()
    {
        
    }


    uint8_t* ConvertToBytes()
    {
        static uint8_t buf[24];

        buf[0] = SubjectID;
        memcpy(&buf[1], ID, 3);
        memcpy(&buf[4], Name, 20);

        return buf;
    }
};

namespace Users
{

    class AutoAdd
    {
        public:
        static bool autoAdd_Active;
        static bool autoAdd_operationSuccessful;
        static bool autoAdd_operationTimedOut;
        static String autoAdd_Namesurname;
        static uint8_t autoAdd_SubjectID;
        static AsyncDelay autoAdd_Expire;

        static void HandleAutoAdd(uint8_t cardID[3]);

        static void AutoAddPeriodic();

        static void BeginAutoAdd(String namesurname, uint8_t subjectID);

        AutoAdd() = delete;
    };
    
    bool AddUser(User user);

    bool GetUserByID(uint8_t ID[3], User & usr);

    bool UserExists(uint8_t ID[3]);

    User* GetAllUsers(int& count);
    bool GetAllUsers(void (*userCallBack)(User user));

    bool DeleteUserByID(uint8_t ID[3]);

    int GetUsersCount();

}
