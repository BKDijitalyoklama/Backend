#pragma once
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

namespace LCD
{

    bool Initialize();

    void ClearRow(uint8_t row);

    void Clear();


    
    void Refresh();

    void PrintCenterRow(String str, uint8_t row = 0, bool clear = true);
    void PrintCenterRow(char* str, uint8_t row = 0, bool clear = true);
    void PrintCenterRow(const char* str, uint8_t row = 0, bool clear = true);

    void PrintCenter(String string, bool clear = true);
    void PrintCenter(char* str, bool clear = true);
    void PrintCenter(const char* str, bool clear = true);

}