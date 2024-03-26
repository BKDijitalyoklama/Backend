#include <Pinout.h>
#include "SD.h"
#include "Debugger.h"

namespace DataSaving
{
    bool Initialize()
    {
        DebugInfo("Initializing SD");

        if (!SD.begin(PIN_SD_CS))
        {
            DebugError("SD Init failed");
            return false;
        }

        return true;
    }

    bool CreateFile(const char *path)
    {
        File file = SD.open(path, FILE_WRITE);
        file.close();
        return true;
    }

    bool FileExists(const char *path)
    {
        return SD.exists(path);
    }
    
    bool OpenRead(const char *path, File &file, bool createIfDoesntExist = true)
    {
        if (!SD.exists(path))
        {
            if (createIfDoesntExist)
            {
                bool createFileSuc = CreateFile(path);
                if (!createFileSuc)
                {
                    DebugError("Couldn't create the file " + String(path));
                    return false;
                }
            }
            else
            {
                DebugWarning("File does not exist");
                return false;
            }
        }

        file = SD.open(path, FILE_READ);
        return true;
    }


    int GetFileCountOfDirectory(const char *dirpath)
    {
        int count = 0;
        File dir;

        if (!OpenRead(dirpath, dir))
            return -1;

        while (true)
        {
            File file = dir.openNextFile();
            if (!file)
                break;
            if (!file.isDirectory())
                count++;
            file.close();
        }

        return count;
    }

    bool DeleteFile(const char *path)
    {
        return SD.remove(path);
    }

    bool OpenWrite(const char *path, File &file, bool createIfDoesntExist = true)
    {
        if (!SD.exists(path))
        {
            if (createIfDoesntExist)
            {
                bool createFileSuc = CreateFile(path);
                if (!createFileSuc)
                {
                    DebugError("Couldn't create the file " + String(path));
                    return false;
                }
            }
            else
            {
                DebugWarning("File does not exist");
                return false;
            }
        }

        file = SD.open(path, FILE_WRITE);
        return true;
    }

    bool AppendFile(const char* path, uint8_t* buf, uint32_t length, bool createIfDoesntExist = true)
    {
        if (!SD.exists(path))
        {
            if (createIfDoesntExist)
            {
                bool createFileSuc = CreateFile(path);
                if (!createFileSuc)
                {
                    DebugError("Couldn't create the file " + String(path));
                    return false;
                }
            }
            else
            {
                DebugWarning("File does not exist");
                return false;
            }
        }

        File file = SD.open(path, FILE_APPEND);
        if(!file.seek(file.size())) return false;
        bool suc = file.write(buf, length) == length;

        file.close();

        return suc;
    }

}
