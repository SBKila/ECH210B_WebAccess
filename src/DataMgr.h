#ifndef DATAMGR_H
#define DATAMGR_H

#include <Arduino.h>

#define UNDEFINED -600

class DataMgr
{
public:
    static sint16 getSD1();
    static sint16 getSD2();
    static sint16 getSD3();
    static sint16 getSD4();
    static sint16 getTemperature();
    static sint16 getHumidity();
    static sint16 getDigitalOutput();
    static sint16 getDigitalInput();
    static boolean getEchComStatus();
    
    static void setSD1(sint16 value);
    static void setSD2(sint16 value);
    static void setSD3(sint16 value);
    static void setSD4(sint16 value);
    static void setTemperature(sint16 value);
    static void setHumidity(sint16 value);
    static void setDigitalOutput(sint16 value);
    static void setDigitalInput(sint16 value);
    static void setEchComStatus(boolean);

private:
    static sint16 sd[8];// = {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED};
    static boolean dataUpdated;// = false;
    static boolean echComStatus;
    static boolean isDataUpdated();
    static void resetDataUpdated();
    static void setValue(short index, sint16 value);
    static void setSD(short index, sint16 value, int delta);
};

#endif
