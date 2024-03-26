#pragma once

#include <Arduino.h>
#include "DataSaving/DataSaving.h"
#include "DataSaving/Subjects.h"
#include "DataSaving/Users.h"
#include "DataSaving/EntryLogs.h"
#include "Debugger.h"
#include "Feedback/LCD.h"

namespace ReportHandler
{

    void CreateReport();
    
    String GetTodaysFileName();
}