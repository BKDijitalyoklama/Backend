#include <Arduino.h>
#include "Debugger.h"
#include "RFIDReader.h"
#include "Network/NetworkConnection.h"
#include "Feedback/LCD.h"
#include "EntryHandler.h"
#include "AsyncDelay.h"
#include "DataSaving/Config.h"
#include "Feedback/Buzzer.h"
#include "Network/OTA.h"

void InitSingle(bool (*initFunc)(), const char* loadingText, const char* errorText)
{
  LCD::PrintCenter(loadingText);
  bool suc = initFunc();
  if (!suc)
  {

    LCD::PrintCenter(errorText);
    delay(3000);
    LCD::PrintCenter("Yeniden Baslatiliyor");
    delay(1500);
    ESP.restart();
  }
}

//AsyncDelay timeDisplayTimer = AsyncDelay();
AsyncDelay connectionCheckTimer = AsyncDelay();
AsyncDelay LCDRefreshTimer = AsyncDelay();

//AsyncDelay datetimeUpdateTimer = AsyncDelay();

void setup()
{
  Serial.begin(9600);
  DebugInfo("BEGIN");

  if(!LCD::Initialize()) ESP.restart();

  //InitSingle(DataSaving::Initialize, "SD Kart Yukleniyor", "SD Kart Hatasi");


  InitSingle(Config::Initialize, "Konfigrasyon Aliniyor", "Konfigrasyon Hatasi");

  InitSingle(NetworkConnection::Initialize, "WiFi Baglantisi Kuruluyor", "WiFi Baglanti Hatasi");

  InitSingle(OTA::Initialize, "OTA Hazirlaniyor", "OTA Hatasi");
  
  //InitSingle(HTTPServer::Initialize, "Web Sunucusu Olusturuluyor", "Web Sunucu Hatasi");

  InitSingle(RFIDReader::Initialize, "RFID Okuyucu Yukleniyor", "RFID Okuyucu Hatasi");

  //InitSingle(TimeModule::Initialize, "Zaman bilgisi aliniyor", "Zaman bilgisi alinamadi");

  InitSingle(Buzzer::Initialize, "Buzzer Ayarlaniyor", "Buzzer Ayarlanamadi");

  LCD::PrintCenter("Baslatiliyor");

  //timeDisplayTimer.start(1000, AsyncDelay::MILLIS);
  connectionCheckTimer.start(1000, AsyncDelay::MILLIS);
  
  LCDRefreshTimer.start(1800000, AsyncDelay::MILLIS);
  //datetimeUpdateTimer.start(1000 * 60 * 60, AsyncDelay::MILLIS);
  
  DebugInfo("Started");
  Buzzer::Note(1000);
  LCD::PrintCenter("Kart Bekleniyor");
}


void loop()
{
  RFIDReader::RFIDCheck();
  /*
  if(Users::AutoAdd::autoAdd_Active)
  {
    Users::AutoAdd::AutoAddPeriodic();
    return;
  }
  */

  /*
  if(timeDisplayTimer.isExpired())
  {
    LCD::Refresh();
    LCD::Clear();
    LCD::PrintCenter(TimeModule::GetDateTimeString());
    timeDisplayTimer.start(1000, AsyncDelay::MILLIS);
  }
  */

  if(connectionCheckTimer.isExpired())
  {
    NetworkConnection::EnsureConnection();
    connectionCheckTimer.start(1000, AsyncDelay::MILLIS);
  }

  if(LCDRefreshTimer.isExpired())
  {
    LCD::Refresh();
    LCDRefreshTimer.start(1800000, AsyncDelay::MILLIS);
  }

  /*
  if(datetimeUpdateTimer.isExpired())
  {
    InitSingle(TimeModule::ForceUpdate, "Zaman bilgisi guncelleniyor", "Zaman bilgisi guncellenemedi");
    datetimeUpdateTimer.start(1000 * 60 * 60, AsyncDelay::MILLIS);
  }
  */

}