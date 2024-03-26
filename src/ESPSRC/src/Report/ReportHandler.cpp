#include <Arduino.h>
#include "DataSaving/DataSaving.h"
#include "DataSaving/Subjects.h"
#include "DataSaving/Users.h"
#include "DataSaving/EntryLogs.h"
#include "Debugger.h"
#include "Feedback/LCD.h"
#include "Network/MailSender.h"

namespace ReportHandler
{
    struct UserEntryLogConnection
    {
        User user;
        EntryLog* entryLog1 = nullptr;
        EntryLog* entryLog2 = nullptr;

        UserEntryLogConnection(User _user)
        {
            user = _user;
        }

        UserEntryLogConnection()
        {

        }

        void AddEntryLog(EntryLog* log)
        {
            if(entryLog1 == nullptr) entryLog1 = log;
            else entryLog2 = log;
        }

        inline String TimeFormat(tm& time)
        {
            char str[6];
            memset(str, 0, 6);

            strftime(str, 6, "%H:%M", &time);

            return String(str);
        }

        String GetCSVLine()
        {
            bool subjectNameSuc = false;

            String line = "";
            line += user.Name;
            line += ',';
            line += Subjects::GetSubjectName(user.SubjectID, subjectNameSuc);
            line += ',';

            if(entryLog1 != NULL && entryLog2 != NULL)
            {
                tm inTimeTM = TimeModule::ConvertEpoch(entryLog1->time < entryLog2->time ? entryLog1->time : entryLog2->time);
                tm outTimeTM = TimeModule::ConvertEpoch(entryLog1->time > entryLog2->time ? entryLog1->time : entryLog2->time);

                String inTime = TimeFormat(inTimeTM);
                String outTime = TimeFormat(outTimeTM);

                line += inTime;
                line += ',';
                line += outTime;
            }
            else if(entryLog1 != nullptr || entryLog2 != nullptr)
            {
                tm timeTM;

                if(entryLog1 != nullptr) timeTM = TimeModule::ConvertEpoch(entryLog1->time);
                else timeTM = TimeModule::ConvertEpoch(entryLog2->time);

                bool isOut = timeTM.tm_hour > 12 || (timeTM.tm_hour == 12 && timeTM.tm_min > 30);

                String inTime = isOut ? "-" : TimeFormat(timeTM);
                String outTime = isOut ?  TimeFormat(timeTM) : "-";

                line += inTime;
                line += ',';
                line += outTime;
            }
            else line += "-,-";

            DebugInfo("LINE:" + line);

            return line;
        }
    };
    


    String GetTodaysFileName()
    {
        String str = Paths::DPATH_CSVRESULTS + ("/" + TimeModule::GetDateTimeString().substring(0, 10) + ".csv");
        DebugInfo(str);
        return str;
    }


    UserEntryLogConnection* connections;
    int connectionIndex = 0;


    UserEntryLogConnection* FindConnectionByUserID(uint8_t userID[3], int usercount)
    {

        DebugInfo("FindConnByUserID");

        for(int i = 0; i<usercount; i++)
        {
            if(memcmp(connections[i].user.ID, userID, 3) == 0)
            {
                
                DebugInfo("UserFound");
                return &connections[i];
            }
        }
        
        DebugInfo("UserNotFound");

        return nullptr;
    }
    
    int usercount = 0;
    bool CreateCSV()
    {
        File csvfile;

        if(!DataSaving::OpenWrite(GetTodaysFileName().c_str(), csvfile, true)){DebugWarning("RA"); return false;}
        if(csvfile.println("AD SOYAD, BRANS, GIRIS, CIKIS") != 31)
        {
            csvfile.close();
            DebugWarning("RB");
            return false;
        }

        int entrylogcount = 0;
        usercount = Users::GetUsersCount();
        
        EntryLog* entrylogs = EntryLogs::GetTodaysEntryLogs(entrylogcount); // WARNING: DYNAMIC ALLOCATION

        if(entrylogs == nullptr ||usercount == -1)
        {
            delete[] entrylogs;
            DebugWarning("RC");
            return String();
        }
        if(usercount == -1)
        {
            delete[] entrylogs;
            DebugWarning("RD");
            return String();
        }

        connections = new UserEntryLogConnection[usercount]; // WARNING: DYNAMIC ALLOCATION
        connectionIndex = 0;

        LCD::PrintCenterRow("1/3");

        Users::GetAllUsers([](User user){
            //LCD::PrintCenterRow("1/3 %" + String(map(connectionIndex + 1, 0, usercount, 0, 100)), 1); // Print Progress
            connections[connectionIndex++] = UserEntryLogConnection(user);
        });
        
        DebugInfo("ELCOUNT: ");
        DebugInfo(entrylogcount);

        LCD::PrintCenterRow("2/3");
        
        for(int i = 0; i<entrylogcount;i++)
        {
            //LCD::PrintCenterRow("2/3 %" + String(map(i + 1, 0, entrylogcount, 0, 100)), 1); // Print Progress


            DebugInfo("ENTRYLOG: " + String((int)entrylogs[i].time));

            FindConnectionByUserID(entrylogs[i].userID, usercount)->AddEntryLog(&entrylogs[i]);
        }

LCD::PrintCenterRow("3/3");
        
        for(int i = 0; i<connectionIndex; i++)
        {
            //LCD::PrintCenterRow("3/3 %" + String(map(i + 1, 0, connectionIndex, 0, 100)), 1); // Print Progress
            String line = connections[i].GetCSVLine();
            if(csvfile.println(line) != line.length() + 2)
            {
                csvfile.close();
                delete[] entrylogs;
                delete[] connections;
                DebugWarning("RE");
                return false;
            }
        }


        delete[] entrylogs;
        delete[] connections;

        csvfile.close();
        
        DebugWarning("RF");
        return true;

    }

    void CreateReport()
    {
        LCD::Clear();
        LCD::PrintCenterRow("Rapor Yaziliyor", 0);

        bool csvsuc = CreateCSV();

        if(!csvsuc)
        {
            LCD::PrintCenter("Hata, CSV olusturulamadi");
            delay(2000);
            return;
        }

        LCD::PrintCenter("Mail Gonderiliyor");

        MailSender::SendMail("AA");

        LCD::PrintCenter("Mail Gonderildi");

        delay(3000);
       
    }
}