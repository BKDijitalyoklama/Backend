#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Debugger.h"
#include "DataSaving/Config.h"
#include "DataSaving/Subjects.h"
#include "DataSaving/Users.h"
#include "EntryHandler.h"
#include "Feedback/LCD.h"
#include <esp_task_wdt.h>

namespace HTTPServer
{
    AsyncWebServer server(80);


    

    void REQ_ValidateMasterKey(AsyncWebServerRequest* request);
    void REQ_Ping(AsyncWebServerRequest* request);
    void REQ_Restart(AsyncWebServerRequest* request);
    void REQ_Subject_Add(AsyncWebServerRequest* request);
    void REQ_Subject_List(AsyncWebServerRequest* request);
    void REQ_Subject_Delete(AsyncWebServerRequest* request);
    void REQ_Users_AddByID(AsyncWebServerRequest* request);
    void REQ_Users_AutoAdd(AsyncWebServerRequest* request);
    void REQ_Users_CancelAutoAdd(AsyncWebServerRequest* request);
    void REQ_Users_List(AsyncWebServerRequest* request);
    void REQ_Users_Delete(AsyncWebServerRequest* request);
    void REQ_Users_AutoAddState(AsyncWebServerRequest* request);
    
    bool Initialize()
    {
        DebugInfo("Initializing HTTPServer");

        server.on("/ping", HTTP_GET, REQ_Ping);

        server.on("/validatemasterkey", HTTP_GET, REQ_ValidateMasterKey);

        server.on("/restart", HTTP_GET, REQ_Restart);

        server.on("/subjects/add", HTTP_GET, REQ_Subject_Add);
        server.on("/subjects/list", HTTP_GET, REQ_Subject_List);
        server.on("/subjects/delete", HTTP_GET, REQ_Subject_Delete);

        server.on("/users/addbyid", HTTP_GET, REQ_Users_AddByID);
        server.on("/users/autoadd", HTTP_GET, REQ_Users_AutoAdd);
        server.on("/users/cancelautoadd", HTTP_GET, REQ_Users_CancelAutoAdd);
        server.on("/users/getautoaddstate", HTTP_GET, REQ_Users_AutoAddState);
        server.on("/users/list", HTTP_GET, REQ_Users_List);
        server.on("/users/delete", HTTP_GET, REQ_Users_Delete);

        server.begin();

        return true;
    }

    inline void FailRequest(AsyncWebServerRequest* request)
    {
        request->send(200, "text/plain", "fail");
    }

    bool IsMasterKeyValid(AsyncWebServerRequest* request)
    {
        if(!request->hasHeader("masterkey") || request->header("masterkey") != Config::MasterKey)
        {
            request->send(200, "text/plain", "authfail");
            DebugInfo("master key not valid");
            return false;
        }

        return true;
    }
    
    void REQ_ValidateMasterKey(AsyncWebServerRequest* request)
    {
        DebugInfo("HTTP validatemasterkey");
        if(!IsMasterKeyValid(request)) return;
        request->send(200, "text/plain", "ok");
    }

    void REQ_Ping(AsyncWebServerRequest* request)
    {
        request->send(200, "text/plain", "pong " + String(millis()));
    }

    void REQ_Restart(AsyncWebServerRequest* request)
    {
        DebugInfo("HTTP restart");
        if(!IsMasterKeyValid(request)) return;
        request->send(200, "text/plain", "ok");
        delay(1000);
        ESP.restart();
    }


    void REQ_Subject_Add(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        if(!request->hasArg("subjectname"))
        {
            FailRequest(request);
            return;
        }

        String subjectname = request->arg("subjectname");
        subjectname.toUpperCase();

        if(subjectname.length() > 9)
        {
            FailRequest(request);
            return;
        }

        if(Subjects::SubjectExists((char*)subjectname.c_str()))
        {
            FailRequest(request);
            return;
        }
        uint8_t nextID = Subjects::GetNextID();

        bool res = Subjects::AddSubject(Subject((char*)subjectname.c_str(), nextID));

        request->send(200, "text/plain", res ? String(nextID) : "fail");
    }

    void REQ_Subject_List(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        bool suc = false;
        File str = Subjects::GetSubjectsFile(suc);

        if(!suc)
        {
            FailRequest(request);
            return;
        }

        request->send(str,"text/plain", str.size());

        str.close();
    }

    void REQ_Subject_Delete(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        FailRequest(request); // DISABLED, TODO: ENABLE IT ONCE THE FEATURE IS READY
        return;

        if(!request->hasArg("subjectid"))
        {
            FailRequest(request);
            return;
        }

        long __subjectID = request->arg("subjectid").toInt();

        if(__subjectID < 1 || __subjectID > 255)
        {
            FailRequest(request);
            return;
        }
        

        bool suc = Subjects::DeleteSubjectByID(__subjectID);

        request->send(200, "text/plain", suc ? "ok" : "fail");
    }


    void REQ_Users_List(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        //request->send(200, "text/plain", "");
        //return; /// TODO: FIX WDT ISSUE

        int count = 0;

        User* users = Users::GetAllUsers(count); // !!!

        DebugInfo("COUNT: ");
        DebugInfo(count);

        if(users == NULL || count == -1)
        {
            DebugInfo("USERLISTFAIL");
            DebugInfo(users == NULL);
            FailRequest(request);
            delete[] users;
            return;
        }

        uint32_t bufLength = 24 * count;

        uint8_t buf[bufLength];

        for(int i = 0; i<count; i++)
        {
            esp_task_wdt_reset();
            User usr = users[i];
            DebugInfo("UserInfo:");
            DebugInfo(usr.Name);
            DebugInfo((char*)String(usr.ID[0]).c_str());
            DebugInfo((char*)String(usr.ID[1]).c_str());
            DebugInfo((char*)String(usr.ID[2]).c_str());
            DebugInfo(usr.SubjectID);

            uint8_t* userbuf = usr.ConvertToBytes();

            memcpy(&buf[i*24], userbuf, 24);
        }

        delete[] users;
        
        AsyncResponseStream* response = request->beginResponseStream("text/plain", bufLength);

        if(response->write(buf, bufLength) != bufLength)
        {
            DebugWarning("REQ_Users_List RESPONSE CREATION FAILED");
            FailRequest(request);
            return;
        }

        request->send(response);
    }

    void REQ_Users_AutoAdd(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        if(!request->hasArg("subjectid") || !request->hasArg("namesurname"))
        {
            FailRequest(request);
            return;
        }

        String namesurname = request->arg("namesurname");
        namesurname.trim();
        namesurname.toUpperCase();
        long __subjectID = request->arg("subjectid").toInt();

        if(namesurname.length() > 19)
        {
            DebugInfo("NAMESURNAMELENGTH > 19");
            FailRequest(request);
            return;
        }

        if(__subjectID < 1 || __subjectID > 255)
        {
            DebugInfo("SUBJECTID OUT OF RANGE");
            FailRequest(request);
            return;
        }

        uint8_t subjectID = (uint8_t)__subjectID;

        if(!Subjects::SubjectExists(subjectID))
        {
            DebugInfo("GIVEN SUBJECT NOT FOUND");
            request->send(200, "text/plain", "subjectdoesnotexist");
            return;
        }

        request->send(200, "text/plain", "ok");

        Users::AutoAdd::BeginAutoAdd(namesurname, subjectID);
    }

    void REQ_Users_AutoAddState(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        if(Users::AutoAdd::autoAdd_Active) request->send(200, "text/plain", "active");
        else if (Users::AutoAdd::autoAdd_operationSuccessful)
        {
            request->send(200, "text/plain", "success");
            Users::AutoAdd::autoAdd_operationSuccessful = false;
            Users::AutoAdd::autoAdd_operationTimedOut = false;
        }
        else if (Users::AutoAdd::autoAdd_operationTimedOut)
        {
            request->send(200, "text/plain", "expired");
            Users::AutoAdd::autoAdd_operationSuccessful = false;
            Users::AutoAdd::autoAdd_operationTimedOut = false;
        }
        else request->send(200, "text/plain", "none");
    }

    void REQ_Users_CancelAutoAdd(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;
        request->send(200, "text/plain", "ok");


        if(Users::AutoAdd::autoAdd_Active && !Users::AutoAdd::autoAdd_Expire.isExpired())
        {
            Users::AutoAdd::autoAdd_Active = false;
            Users::AutoAdd::autoAdd_Expire.expire();
        }
    }

    void REQ_Users_AddByID(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        DebugInfo("AAA");

        if(!request->hasArg("manid") || !request->hasArg("cardid") || !request->hasArg("subjectid") || !request->hasArg("namesurname"))
        {
        DebugInfo("F1");
            FailRequest(request);
            return;
        }

        String namesurname = request->arg("namesurname");
        namesurname.trim();
        namesurname.toUpperCase();
        long __manID = request->arg("manid").toInt();
        long __cardID = request->arg("cardid").toInt();
        long __subjectID = request->arg("subjectid").toInt();

        if(namesurname.length() > 19 || namesurname.length() <= 0)
        {
        DebugInfo("F1");
            FailRequest(request);
            return;
        }

        if(__manID < 0 || __manID > 255)
        {
        DebugInfo("F2");
            FailRequest(request);
            return;
        }

        if(__cardID < 0 || __cardID > 65535)
        {
        DebugInfo("F3");
            FailRequest(request);
            return;
        }

        if(__subjectID < 1 || __subjectID > 255)
        {
        DebugInfo("F4");
            FailRequest(request);
            return;
        }

        uint8_t manID = (uint8_t)__manID;
        uint16_t cardID = (uint16_t)__cardID;
        uint8_t subjectID = (uint8_t)__subjectID;
        

        uint8_t cardIDBytes[3] = {manID, (uint8_t)(cardID >> 8), (uint8_t)(cardID & 0xFF)};

        DebugInfo("A");
        
        if(Users::UserExists(cardIDBytes))
        {
            DebugInfo("B");
            request->send(200, "text/plain", "cardexists");
            return;
        }

        if(!Subjects::SubjectExists(subjectID))
        {
        DebugInfo("C");
            request->send(200, "text/plain", "subjectdoesnotexist");
            return;
        }
        
        bool suc = Users::AddUser(User((char*)namesurname.c_str(), cardIDBytes, subjectID));

        DebugInfo("E");

        request->send(200, "text/plain", suc ? "ok" : "fail");
        DebugInfo("F");
    }

    void REQ_Users_Delete(AsyncWebServerRequest* request)
    {
        if(!IsMasterKeyValid(request)) return;

        if(!request->hasArg("manid") || !request->hasArg("cardid"))
        {
            FailRequest(request);
            return;
        }


        long __manID = request->arg("manid").toInt();
        long __cardID = request->arg("cardid").toInt();

        
        if(__manID < 0 || __manID > 255)
        {
            FailRequest(request);
            return;
        }

        if(__cardID < 0 || __cardID > 65535)
        {
            FailRequest(request);
            return;
        }

        uint8_t manID = (uint8_t)__manID;
        uint16_t cardID = (uint16_t)__cardID;
        

        uint8_t cardIDBytes[3] = {manID, (uint8_t)(cardID >> 8), (uint8_t)(cardID & 0xFF)};


        bool suc = Users::DeleteUserByID(cardIDBytes);

        request->send(200, "text/plain", suc ? "ok" : "fail");
        

    }

}