#ifndef ECHMGR_H
#define ECHMGR_H

#define DEBUG_ECH
#ifdef DEBUG_ECH
#define DEBUG_ECH_PRINT(x) Serial.print(x)
#define DEBUG_ECH_PRINTLN(x) Serial.println(x)
#define DEBUG_ECH_PRINT_DEC(x) Serial.print(x, DEC)
#define DEBUG_ECH_PRINTLN_DEC(x) Serial.println(x, DEC)
#else
#define DEBUG_ECH_PRINT(x)
#define DEBUG_ECH_PRINTLN(x)
#define DEBUG_ECH_PRINT_DEC(x)
#define DEBUG_ECH_PRINTLN_DEC(x)
#endif

#define BLINK                            \
    for (int i = 0; i < 5; i++)          \
    {                                    \
        digitalWrite(LED_BUILTIN, LOW);  \
        delay(50);                       \
        digitalWrite(LED_BUILTIN, HIGH); \
        delay(50);                       \
    }

#define ADDR_ECH 0
#define MASK_ECHCOM 0x1

#define ADDR_STATUS 1188
#define MASK_STANDBYWARMCOOL 0x3
#define MASK_POWER 0x4

#define ADDR_TP_WATERIN 1135
#define ADDR_TP_WATEROUT 1137
#define ADDR_TP_CONDENSOR 1139
#define ADDR_TP_OUTDOOR 1141
#define ADDR_FANSPEED 1205
#define ADDR_DIGITAL_INPUT 1124
#define MASK_DI_COMPRESSOR 0x40
#define MASK_DI_BOILER 0x60
#define MASK_DI_PUMP 0x08
#define MASK_DI_WARMCOOL 0x10
#define MASK_DI_ONOFFSTANDBY 0x20

#define ADDR_DIGITAL_OUTPUT 1189
#define MASK_DO_COMPRESSOR 0x01
#define MASK_DO_PUMP 0x02
#define MASK_DO_REVERSAL 0x04
#define MASK_DO_BOILER 0x08
#define MASK_DO_ALARM 0x10
#define MASK_DO_TBD 0x20

#define ADDR1_ALARM_AUTO 1214
#define ADDR1_ALARM_MAN 1217
#define MASK_ALARM_HP 0x02
#define MASK_ALARM_BP 0x04
#define MASK_ALARM_FLUX 0x08
#define MASK_ALARM_BOILER 0x10

#define ADDR2_ALARM_AUTO 1215
#define ADDR2_ALARM_MAN 1218
#define MASK_ALARM_GAZ 0x01
#define MASK_ALARM_CONF 0x02
#define MASK_ALARM_SENSOR1 0x04
#define MASK_ALARM_SENSOR2 0x08
#define MASK_ALARM_SENSOR3 0x10
#define MASK_ALARM_SENSOR4 0x20
#define MASK_ALARM_MAXT 0x040

#define MODBUS_MESSAGE_DELAY 200

#include <Arduino.h>
#include <Ticker.h>
#include "config.h"
#include "ECH210BModbusMaster232.h"

ECH210BModbusMaster232 eCH210BModbus;
modbusRegBank m_EchRegBank;
boolean performECHAnalyse = false;
Ticker echMgrTicker;

uint16_t registers2Read[12] = {ADDR_STATUS, ADDR_DIGITAL_INPUT, ADDR_TP_WATERIN, ADDR_TP_WATEROUT, ADDR_TP_CONDENSOR, ADDR_TP_OUTDOOR, ADDR_FANSPEED, ADDR_DIGITAL_OUTPUT, ADDR1_ALARM_AUTO, ADDR2_ALARM_AUTO, ADDR1_ALARM_MAN, ADDR2_ALARM_MAN};
//,2048,2049,2051,2052,2053,2054,2081,2082,2083,2084,2085,2086,2087,2089,2090,2091,2092,2094,2095,2,2136,2137,2192,2196,3351,3355};

void onECHMgrTicker()
{
    performECHAnalyse = true;
}

String registerProcessor(const String &var)
{
    char tempBuffer[5];
    word value;

    if (var == "COM")
        return m_EchRegBank.get(ADDR_ECH, value) ? ((value & MASK_ECHCOM) ? "OK" : "KO") : "UNKNOWN";
    if (var == "POWER")
        return m_EchRegBank.get(ADDR_STATUS, value) ? ((value & MASK_POWER) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "STATE")
    {
        if (m_EchRegBank.get(ADDR_STATUS, value))
        {
            switch (value & MASK_STANDBYWARMCOOL)
            {
            case 0:
                return "STANDBY";
            case 1:
                return "WARM";
            case 3:
                return "COOL";
            }
        }
        return "UNKNOWN";
    }

    if (var == "SD1")
        return m_EchRegBank.get(ADDR_TP_WATERIN, value) ? dtostrf(((float)((sint16_t)value)) / 10, 5, 1, tempBuffer) : "";
    if (var == "SD2")
        return m_EchRegBank.get(ADDR_TP_WATEROUT, value) ? dtostrf(((float)((sint16_t)value)) / 10, 5, 1, tempBuffer) : "";
    if (var == "SD3")
        return m_EchRegBank.get(ADDR_TP_CONDENSOR, value) ? dtostrf(((float)((sint16_t)value)) / 10, 5, 1, tempBuffer) : "";
    if (var == "SD4")
        return m_EchRegBank.get(ADDR_TP_OUTDOOR, value) ? dtostrf(((float)((sint16_t)value)) / 10, 5, 1, tempBuffer) : "";

    if (var == "DO_COMPRESSOR")
        return m_EchRegBank.get(ADDR_DIGITAL_OUTPUT, value) ? ((value & MASK_DO_COMPRESSOR) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "DO_PUMP")
        return m_EchRegBank.get(ADDR_DIGITAL_OUTPUT, value) ? ((value & MASK_DO_PUMP) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "DO_REVERSAL")
        return m_EchRegBank.get(ADDR_DIGITAL_OUTPUT, value) ? ((value & MASK_DO_REVERSAL) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "DO_BOILER")
        return m_EchRegBank.get(ADDR_DIGITAL_OUTPUT, value) ? ((value & MASK_DO_BOILER) ? "ON" : "OFF") : "UNKNOWN";

    if (var == "DI_COMPRESSOR")
        return m_EchRegBank.get(ADDR_DIGITAL_INPUT, value) ? ((value & MASK_DI_COMPRESSOR) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "DI_BOILER")
        return m_EchRegBank.get(ADDR_DIGITAL_INPUT, value) ? ((value & MASK_DI_BOILER) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "DI_PUMP")
        return m_EchRegBank.get(ADDR_DIGITAL_INPUT, value) ? ((value & MASK_DI_PUMP) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "DI_WARMCOOL")
        return m_EchRegBank.get(ADDR_DIGITAL_INPUT, value) ? ((value & MASK_DI_WARMCOOL) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "DI_ONOFFSTANDBY")
        return m_EchRegBank.get(ADDR_DIGITAL_INPUT, value) ? ((value & MASK_DI_ONOFFSTANDBY) ? "ON" : "OFF") : "UNKNOWN";

    if (var == "ALAU_HP")
        return m_EchRegBank.get(ADDR1_ALARM_AUTO, value) ? ((value & MASK_ALARM_HP) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_BP")
        return m_EchRegBank.get(ADDR1_ALARM_AUTO, value) ? ((value & MASK_ALARM_BP) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_PUMP")
        return m_EchRegBank.get(ADDR1_ALARM_AUTO, value) ? ((value & MASK_ALARM_FLUX) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_BOILER")
        return m_EchRegBank.get(ADDR1_ALARM_AUTO, value) ? ((value & MASK_ALARM_BOILER) ? "ON" : "OFF") : "UNKNOWN";

    if (var == "ALAU_GAZ")
        return m_EchRegBank.get(ADDR2_ALARM_AUTO, value) ? ((value & MASK_ALARM_GAZ) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_CONF")
        return m_EchRegBank.get(ADDR2_ALARM_AUTO, value) ? ((value & MASK_ALARM_CONF) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_SENSOR1")
        return m_EchRegBank.get(ADDR2_ALARM_AUTO, value) ? ((value & MASK_ALARM_SENSOR1) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_SENSOR2")
        return m_EchRegBank.get(ADDR2_ALARM_AUTO, value) ? ((value & MASK_ALARM_SENSOR2) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_SENSOR3")
        return m_EchRegBank.get(ADDR2_ALARM_AUTO, value) ? ((value & MASK_ALARM_SENSOR3) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_SENSOR4")
        return m_EchRegBank.get(ADDR2_ALARM_AUTO, value) ? ((value & MASK_ALARM_SENSOR4) ? "ON" : "OFF") : "UNKNOWN";
    if (var == "ALAU_MAXT")
        return m_EchRegBank.get(ADDR2_ALARM_AUTO, value) ? ((value & MASK_ALARM_MAXT) ? "ON" : "OFF") : "UNKNOWN";
    if(var ==" FANSPEED")
        return m_EchRegBank.get(ADDR_FANSPEED,value)?dtostrf(((float)(value)), 5, 0, tempBuffer) : "";


    return "UNKNOWN";
}

void setupECH()
{
    DEBUG_ECH_PRINTLN("setupECH");

    // Initialize Ech210b Modbus brearer
    eCH210BModbus.init(MODBUS_RX_PIN, MODBUS_TX_PIN, mySettings.ech_Modbus_Id);

    // Initialize Modbus communication baud rate
    eCH210BModbus.begin(MODBUS_BAUDRATE);

    // Initalize echModule sensor value reading
    if (0 != mySettings.ech_Read_Period)
    {
        echMgrTicker.attach(mySettings.ech_Read_Period, onECHMgrTicker);
    }

    onECHMgrTicker();
}

uint8_t readRegisters(uint16_t addr, uint16_t &value)
{
    DEBUG_ECH_PRINT(" readRegisters ");
    DEBUG_ECH_PRINTLN(addr);

    uint8_t result = eCH210BModbus.readHoldingRegisters(addr, 1);
    if (result != 0)
    {
        m_EchRegBank.set(ADDR_ECH,0);
        DEBUG_ECH_PRINT(" ECH Error reading ");
        DEBUG_ECH_PRINTLN(addr);
        BLINK
    }
    else
    {
        m_EchRegBank.set(ADDR_ECH,1);
        value = eCH210BModbus.getResponseBuffer(0);
        DEBUG_ECH_PRINT(" ECH reading ");
        DEBUG_ECH_PRINT(addr);
        DEBUG_ECH_PRINT(" : ");
        DEBUG_ECH_PRINTLN(value);
        eCH210BModbus.clearResponseBuffer();
    }

    return result;
}

void read_EchSensors()
{
    size_t NumberOfElements = sizeof(registers2Read) / sizeof(registers2Read[0]);
    for (size_t index = 0; index < NumberOfElements; index++)
    {
        uint16_t value;
        int result = readRegisters(registers2Read[index], value);
        if (result == 0)
        {
            m_EchRegBank.set(registers2Read[index], value);
            DEBUG_ECH_PRINTLN("register saved");
        }
        else
        {
            m_EchRegBank.remove(registers2Read[index]);
            DEBUG_ECH_PRINTLN("register removed");
        }
        delay(MODBUS_MESSAGE_DELAY);
    }
}

void loopECH()
{
    if (performECHAnalyse)
    {
        read_EchSensors();
        performECHAnalyse = false;
    }
}

#endif