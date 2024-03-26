#pragma once
#include <SD.h>


namespace DataSaving
{
    bool Initialize();
    

    bool OpenRead(const char *path, File &file, bool createIfDoesntExist = true);

    bool OpenWrite(const char *path, File &file, bool createIfDoesntExist = true);
    
    bool FileExists(const char* path);

    bool DeleteFile(const char* path);

    int GetFileCountOfDirectory(const char* dirpath);

    bool AppendFile(const char* path, uint8_t* buf, uint32_t length, bool createIfDoesntExist = true);
}