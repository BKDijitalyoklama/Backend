#include "DataSaving/DataSaving.h"
#include "DataSaving/Users.h"
#include "DataSaving/Paths.h"
#include "Debugger.h"
#include "Feedback/LCD.h"

namespace Users // USER: 24 BYTES // 1: SUBJECT // 3: ID // 20: NAME
{
    // NAME MUST BE ASCII

    bool AutoAdd::autoAdd_Active = false;
    bool AutoAdd::autoAdd_operationSuccessful = false;
    bool AutoAdd::autoAdd_operationTimedOut = false;
    String AutoAdd::autoAdd_Namesurname = "";
    uint8_t AutoAdd::autoAdd_SubjectID = 0;
    AsyncDelay AutoAdd::autoAdd_Expire = AsyncDelay();

    void AutoAdd::HandleAutoAdd(uint8_t cardID[3])
    {
        if (Users::UserExists(cardID))
        {
            LCD::PrintCenter("Kullanici Zaten Kayitli");
            delay(1000);
            LCD::PrintCenterRow(autoAdd_Namesurname, 0);
            LCD::PrintCenterRow("Kart Bekleniyor", 1);
            return;
        }

        User user = User((char *)autoAdd_Namesurname.c_str(), cardID, autoAdd_SubjectID);
        if (!Users::AddUser(user))
        {
            LCD::PrintCenter("Kullanici Eklenemedi");
            delay(1000);
            LCD::PrintCenterRow(autoAdd_Namesurname, 0);
            LCD::PrintCenterRow("Kart Bekleniyor", 1);
            return;
        }

        autoAdd_operationSuccessful = true;

        LCD::PrintCenterRow(user.Name, 0);
        LCD::PrintCenterRow("Kaydedildi", 1);

        delay(2000);

        autoAdd_Active = false;
    }

    void AutoAdd::AutoAddPeriodic()
    {
        if (autoAdd_Expire.isExpired())
        {
            autoAdd_Active = false;
            autoAdd_operationTimedOut = true;
            LCD::PrintCenter("Otomatik Ekleme Zaman Asimi");
            delay(1000);
            return;
        }

        LCD::PrintCenterRow(autoAdd_Namesurname, 0);
        LCD::PrintCenterRow("Kart Bekleniyor", 1);
    }
    
    void AutoAdd::BeginAutoAdd(String namesurname, uint8_t subjectID)
    {
        Users::AutoAdd::autoAdd_Active = true;
        Users::AutoAdd::autoAdd_operationSuccessful = false;
        Users::AutoAdd::autoAdd_operationTimedOut = false;
        Users::AutoAdd::autoAdd_Namesurname = namesurname;
        Users::AutoAdd::autoAdd_SubjectID = subjectID;
        Users::AutoAdd::autoAdd_Expire.start(20000, AsyncDelay::MILLIS);
    }


    void ConvertUserToBytes(User user, uint8_t *buffer)
    {
        memset(buffer, 0, 24); // 24 bytes
        memcpy(buffer, &user.SubjectID, 1);
        memcpy(buffer + 1, user.ID, 3);
        memcpy(buffer + 4, user.Name, strlen(user.Name));
    }

    User ConvertBytesToUser(uint8_t data[24])
    {
        uint8_t subjectID = 0;
        uint8_t ID[3];
        char name[20];

        memset(ID, 0, 3);
        memset(name, 0, 20);

        subjectID = data[0];
        memcpy(ID, &data[1], 3);
        memcpy(name, &data[4], 20);

        User user = User(name, ID, subjectID);

        return user;
    }

    String ConvertUserIDToHEX(uint8_t ID[3])
    {
        String hexString = "";
        for (int i = 0; i < 3; ++i)
        {
            char hex[3];
            sprintf(hex, "%02X", ID[i]);
            hexString += hex;
        }
        return hexString;
    }

    uint8_t *ConvertHEXToUserID(String hex)
    {
        static uint8_t result[3];
        for (int i = 0; i < 3; ++i)
        {
            char hexByte[3];
            hex.substring(i * 2, (i + 1) * 2).toCharArray(hexByte, 3);
            result[i] = strtoul(hexByte, NULL, 16);
        }
        return result;
    }

    String getUserFilePath(uint8_t ID[3])
    {
        String path = Paths::DPATH_USERS + ('/' + ConvertUserIDToHEX(ID));
        DebugInfo(path);
        return path;
    }

    bool UserExists(uint8_t ID[3])
    {
        return DataSaving::FileExists(getUserFilePath(ID).c_str());
    }

    int GetUsersCount()
    {
        return DataSaving::GetFileCountOfDirectory(Paths::DPATH_USERS);
    }

    /// @brief WARNING!!! DYNAMIC ALLOCATION!!!
    /// @param count
    /// @return
    User *GetAllUsers(int &count)
    {
        count = 0;

        int userCount = GetUsersCount();

        if (userCount == -1)
        {
            DebugInfo("GETALLUSERS: USERCOUNT -1");
            count = -1;
            return NULL;
        }

        File dir;

        if (!DataSaving::OpenRead(Paths::DPATH_USERS, dir))
        {
            DebugInfo("GETALLUSERS: COULDN'T OPEN DIR");
            count = -1;
            return NULL;
        }

        User *buf = new User[userCount]; // WARNING: DYNAMIC ALLOCATION

        for (int i = 0; i < userCount; i++)
        {
            File file = dir.openNextFile();
            if (!file)
                break;
            if (file.isDirectory())
            {
                i--;
                file.close();
                continue;
            }

            DebugInfo(file.name());

            uint8_t userbuf[24];

            file.seek(0);
            int readres = file.read(userbuf, 24);
            if (readres != 24)
            {
                DebugWarning("Missing user bytes / error");
                i--;
                file.close();
                continue;
            }
            file.close();

            User user = ConvertBytesToUser(userbuf);

            buf[i] = user;
            count++;
        }

        DebugInfo("GETALLUSERS: Returning buf");
        return buf;
    }

    bool GetAllUsers(void (*userCallBack)(User user))
    {

        int userCount = DataSaving::GetFileCountOfDirectory(Paths::DPATH_USERS);

        if (userCount == -1)
        {
            DebugInfo("GETALLUSERS: USERCOUNT -1");
            return false;
        }

        File dir;

        if (!DataSaving::OpenRead(Paths::DPATH_USERS, dir))
        {
            DebugInfo("GETALLUSERS: COULDN'T OPEN DIR");
            return false;
        }

        for (int i = 0; i < userCount; i++)
        {
            File file = dir.openNextFile();
            if (!file)
                break;
            if (file.isDirectory())
            {
                i--;
                file.close();
                continue;
            }

            DebugInfo(file.name());

            uint8_t userbuf[24];

            file.seek(0);
            int readres = file.read(userbuf, 24);
            if (readres != 24)
            {
                DebugWarning("Missing user bytes / error");
                i--;
                file.close();
                continue;
            }
            file.close();

            User user = ConvertBytesToUser(userbuf);
            
            userCallBack(user);
        }

        return true;
        
    }

    bool AddUser(User user)
    {
        File file;

        String filepath = getUserFilePath(user.ID);

        if (!DataSaving::OpenWrite(filepath.c_str(), file, true))
            return false;
        uint8_t buf[24];

        ConvertUserToBytes(user, buf);

        DebugInfo("CHECK NS");
        DebugInfo(user.Name);
        DebugInfo(ConvertBytesToUser(buf).Name);

        file.seek(0);
        int res = file.write(buf, 24);

        return res == 24;
    }

    bool GetUserByID(uint8_t ID[3], User &usr)
    {
        if (!UserExists(ID))
            return false;

        String path = getUserFilePath(ID);

        File file;

        if (!DataSaving::OpenRead(path.c_str(), file))
            return false;

        uint8_t buffer[24];

        int readres = file.read(buffer, 24);

        if (readres != 24)
            return false;

        usr = ConvertBytesToUser(buffer);

        return true;
    }

    bool DeleteUserByID(uint8_t ID[3])
    {
        if (!UserExists(ID))
            return false;

        String path = getUserFilePath(ID);

        DataSaving::DeleteFile(path.c_str());

        return true;
    }
}