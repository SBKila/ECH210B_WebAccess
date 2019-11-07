#include "DataMgr.h"
#include <Arduino.h>

/*#define DEBUG_DAT*/
#ifdef DEBUG_DAT
#define DEBUG_DAT_PRINT(x) Serial.print(x)
#define DEBUG_DAT_PRINTLN(x) Serial.println(x)
#define DEBUG_DAT_PRINT_DEC(x) Serial.print(x, DEC)
#define DEBUG_DAT_PRINTLN_DEC(x) Serial.println(x, DEC)
#else
#define DEBUG_DAT_PRINT(x)
#define DEBUG_DAT_PRINTLN(x)
#define DEBUG_DAT_PRINT_DEC(x)
#define DEBUG_DAT_PRINTLN_DEC(x)
#endif

#define DELTA_TEMP 5
#define DELTA_HUMIDITY 10
#define UNDEFINED -600

sint16 DataMgr::sd[8] = {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED};

boolean DataMgr::echComStatus = false;

boolean DataMgr::dataUpdated = false;

boolean DataMgr::isDataUpdated()
{
  return dataUpdated;
}
void DataMgr::resetDataUpdated()
{
  dataUpdated = false;
}

void DataMgr::setValue(short index, sint16 value)
{
  DEBUG_DAT_PRINT("SD");
  DEBUG_DAT_PRINT(1 + index);
  if ((UNDEFINED == sd[index]) || (sd[index] != value))
  {
    sd[index] = value;
    dataUpdated = true;

    DEBUG_DAT_PRINT(" updated to ");
    DEBUG_DAT_PRINTLN_DEC(value);
  }
  else
  {
    DEBUG_DAT_PRINTLN(" not updated.");
  }
}

void DataMgr::setSD(short index, sint16 value, int delta = DELTA_TEMP)
{
  DEBUG_DAT_PRINT("SD");
  DEBUG_DAT_PRINT(1 + index);
  if ((UNDEFINED == sd[index]) || ((abs(sd[index] - value) > delta)))
  {
    sd[index] = value;
    dataUpdated = true;

    DEBUG_DAT_PRINT(" updated to ");
    DEBUG_DAT_PRINTLN_DEC(value);
  }
  else
  {
    DEBUG_DAT_PRINTLN(" not updated.");
  }
}

void DataMgr::setSD1(sint16 value)
{
  setSD(0, value);
}
sint16 DataMgr::getSD1()
{
  return sd[0];
}

void DataMgr::setSD2(sint16 value)
{
  setSD(1, value);
}
sint16 DataMgr::getSD2()
{
  return sd[1];
}
void DataMgr::setSD3(sint16 value)
{
  setSD(2, value);
}
sint16 DataMgr::getSD3()
{
  return sd[2];
}
void DataMgr::setSD4(sint16 value)
{
  setSD(3, value);
}
sint16 DataMgr::getSD4()
{
  return sd[3];
}
void DataMgr::setTemperature(sint16 value)
{
  setSD(4, value);
}
sint16 DataMgr::getTemperature()
{
  return sd[4];
}
void DataMgr::setHumidity(sint16 value)
{
  setSD(5, value, DELTA_HUMIDITY);
}
sint16 DataMgr::getHumidity()
{
  return sd[5];
}
void DataMgr::setDigitalOutput(sint16 value)
{
  setValue(6, value);
  //setValue(6,((value&0xFF00)&&(sd[6]&0x00FF)));
}
sint16 DataMgr::getDigitalOutput()
{
  return sd[6];
}
void DataMgr::setDigitalInput(sint16 value)
{
  setValue(7, value);
  //setValue(6,(((value>>8)&0x00FF)&&(sd[6]&0xFF00)));
}
sint16 DataMgr::getDigitalInput()
{
  return sd[7];
}
boolean DataMgr::getEchComStatus()
{
  return echComStatus;
}
void DataMgr::setEchComStatus(boolean status)
{
  if (echComStatus != status)
  {
    echComStatus = status;
    dataUpdated = true;
  }
}