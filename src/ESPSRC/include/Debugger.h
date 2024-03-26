#pragma once

#define DEBUGINFO 1
#define DEBUGWARNING 1
#define DEBUGERROR 1


#if DEBUGINFO == 1
#define DebugInfo(x) Serial.println("[I] " + String(x))
#else
#define DebugInfo(x)
#endif

#if DEBUGWARNING == 1
#define DebugWarning(x) Serial.println("[W] " + String(x))
#else
#define DebugWarning(x)
#endif


#if DEBUGERROR == 1
#define DebugError(x) Serial.println("[E] " + String(x))
#else
#define DebugError(x)
#endif