#ifndef SETTINGMGR_H
#define SETTINGMGR_H

#include <Arduino.h>
#include <EEPROM.h>
#include "settings.h"

settings mySettings;

#define DEBUG_SETTING
#ifdef DEBUG_SETTING
#define DEBUG_SETTING_PRINT(x) Serial.print(x)
#define DEBUG_SETTING_PRINTLN(x) Serial.println(x)
#define DEBUG_SETTING_PRINT_DEC(x) Serial.print(x, DEC)
#define DEBUG_SETTING_PRINTLN_DEC(x) Serial.println(x, DEC)
#else
#define DEBUG_SETTING_PRINT(x)
#define DEBUG_SETTING_PRINTLN(x)
#define DEBUG_SETTING_PRINT_DEC(x)
#define DEBUG_SETTING_PRINTLN_DEC(x)
#endif

void loadSettings()
{
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, mySettings);
  EEPROM.end();
  DEBUG_SETTING_PRINT("loaded:");
  DEBUG_SETTING_PRINT(mySettings.tag);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ssid_name);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ssid_key);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ech_Modbus_Id);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ech_Read_Period);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINTLN(mySettings.hostname);

  EEPROM.put(0, mySettings);
  DEBUG_SETTING_PRINTLN("Settings loaded");
}
void storeSettings()
{
  EEPROM.begin(sizeof(struct settings));
  DEBUG_SETTING_PRINT("store:");
  DEBUG_SETTING_PRINT(mySettings.tag);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ssid_name);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ssid_key);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ech_Modbus_Id);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINT(mySettings.ech_Read_Period);
  DEBUG_SETTING_PRINT(",");
  DEBUG_SETTING_PRINTLN(mySettings.hostname);
  EEPROM.put(0, mySettings);
  DEBUG_SETTING_PRINTLN("Settings stored");
  EEPROM.end();
}

String settingsProcessor(const String &var)
{
  if (var == "SSIDNAME")
    return mySettings.ssid_name;
  if (var == "MODBUSID")
    return String(mySettings.ech_Modbus_Id);
  if (var == "REFRESHPERIOD")
    return String(mySettings.ech_Read_Period);
  if (var == "HOSTNAME")

    return mySettings.hostname;
  return "";
}

#endif