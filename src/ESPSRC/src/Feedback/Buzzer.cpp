#include <Arduino.h>
#include "Pinout.h"
#include "Debugger.h"

namespace Buzzer
{
    bool Initialize()
    {
        pinMode(PIN_BUZZER, OUTPUT);

        return true;
    }

    void Note()
    {
        digitalWrite(PIN_BUZZER, HIGH);
    }

    void NoNote()
    {
        digitalWrite(PIN_BUZZER, LOW);
    }

    void Note(int durationMS)
    {
        Note();
        delay(durationMS);
        NoNote();
    }

    void Play_UserEntrySound()
    {
        Note(100);
        delay(50);
        Note(100);
    }
}