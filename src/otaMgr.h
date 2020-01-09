#ifndef OTAMGR_H
#define OTAMGR_H

#include <ArduinoOTA.h>
#include "config.h"

#define DEBUG_OTAMGR
#ifdef DEBUG_OTAMGR
#define DEBUG_OTA_PRINT(x)  Serial.print (x)
#define DEBUG_OTA_PRINTLN(x)  Serial.println (x)
#define DEBUG_OTA_PRINTLN_DEC(x)  Serial.println (x,DEC)
#define DEBUG_OTA_PRINTF(x,y)  Serial.printf (x,y)
#else
#define DEBUG_OTA_PRINT(x)
#define DEBUG_OTA_PRINTLN(x)
#define DEBUG_OTA_PRINTLN_DEC(x)
#define DEBUG_OTA_PRINTF(x,y)
#endif

bool isOTAUpdate = false;

void setupOTAUpdate() {
  DEBUG_OTA_PRINT("setupOTAUpdate");

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(mySettings.hostname);

  // No authentication by default
  ArduinoOTA.setPasswordHash(OTAPASSWORD);


  ArduinoOTA.onStart([]() {
    isOTAUpdate = true;
#ifdef DEBUG_OTAMGR
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
#endif
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_OTA_PRINTLN("\nEnd");
    isOTAUpdate = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_OTA_PRINTF("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
#ifdef DEBUG_OTAMGR
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
#endif
    isOTAUpdate = false;
  });
  ArduinoOTA.begin();
}

void loopOTAUpdate() {
   //DEBUG_OTA_PRINTLN("loopOTAUpdate");
    ArduinoOTA.handle();
}
#endif