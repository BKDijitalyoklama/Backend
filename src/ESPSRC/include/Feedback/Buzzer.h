#include <Arduino.h>
#include "Pinout.h"
#include "Debugger.h"

namespace Buzzer
{
    bool Initialize();

    void Note();
    void NoNote();

    void Note(int durationMS);
    void Play_UserEntrySound();

}