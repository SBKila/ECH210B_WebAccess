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

#define ADDR_TP_WATERIN 1135
#define ADDR_TP_WATEROUT 1137
#define ADDR_TP_CONDENSOR 1139
#define ADDR_TP_OUTDOOR 1141
#define TP_1 1205
#define ADDR_2 1188
#define ADDR_3 3351

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

#define MODBUS_MESSAGE_DELAY 200

#include <Arduino.h>
#include <Ticker.h>
#include "config.h"
#include "DataMgr.h"
#include "ECH210BModbusMaster232.h"

ECH210BModbusMaster232 eCH210BModbus;
boolean performECHAnalyse = false;
Ticker echMgrTicker;
int echMgrTickerPeriod = 15;

void onECHMgrTicker()
{
    performECHAnalyse = true;
}

void setupECH()
{
    DEBUG_ECH_PRINTLN("setupECH");

    // Initialize Ech210b Modbus brearer
    eCH210BModbus.init(MODBUS_RX_PIN, MODBUS_TX_PIN, ECH210BD_ADRESS);

    // Initialize Modbus communication baud rate
    eCH210BModbus.begin(MODBUS_BAUDRATE);

    // Initalize echModule sensor value reading
    echMgrTicker.attach(echMgrTickerPeriod, onECHMgrTicker);
}

// Read ECH Module sensor values
void read_EchSensors()
{

    sint16 value;

    DEBUG_ECH_PRINTLN("ECH Reading Analog Input ");
    int result = eCH210BModbus.readHoldingRegisters(ADDR_TP_WATERIN, 1);
    if (result != 0)
    {
        DataMgr::setEchComStatus(false);
        DEBUG_ECH_PRINTLN(" ECH Error reading ADDR_TP_WATERIN");
        BLINK
        return;
    }
    value = eCH210BModbus.getResponseBuffer(0);
    DataMgr::setSD1(value);
    eCH210BModbus.clearResponseBuffer();
    DEBUG_ECH_PRINT("sd1:");
    DEBUG_ECH_PRINTLN(value);
    delay(MODBUS_MESSAGE_DELAY);

    result = eCH210BModbus.readHoldingRegisters(ADDR_TP_WATEROUT, 1);
    if (result != 0)
    {
        DataMgr::setEchComStatus(false);
        DEBUG_ECH_PRINTLN(" ECH Error reading ADDR_TP_WATEROUT");
        BLINK
        return;
    }
    value = eCH210BModbus.getResponseBuffer(0);
    DataMgr::setSD2(value);
    eCH210BModbus.clearResponseBuffer();
    DEBUG_ECH_PRINT("sd2:");
    DEBUG_ECH_PRINTLN(value);
    delay(MODBUS_MESSAGE_DELAY);

    result = eCH210BModbus.readHoldingRegisters(ADDR_TP_CONDENSOR, 1);
    if (result != 0)
    {
        DataMgr::setEchComStatus(false);
        DEBUG_ECH_PRINTLN(" ECH Error reading ADDR_TP_CONDENSOR");
        BLINK
        return;
    }
    value = eCH210BModbus.getResponseBuffer(0);
    DataMgr::setSD3(value);
    eCH210BModbus.clearResponseBuffer();
    DEBUG_ECH_PRINT("sd3:");
    DEBUG_ECH_PRINTLN(value);
    delay(MODBUS_MESSAGE_DELAY);

    result = eCH210BModbus.readHoldingRegisters(ADDR_TP_OUTDOOR, 1);
    if (result != 0)
    {
        DataMgr::setEchComStatus(false);
        DEBUG_ECH_PRINTLN(" ECH Error reading ADDR_TP_OUTDOOR");
        BLINK
        return;
    }
    value = eCH210BModbus.getResponseBuffer(0);
    DataMgr::setSD4(value);
    eCH210BModbus.clearResponseBuffer();
    delay(MODBUS_MESSAGE_DELAY);
    DEBUG_ECH_PRINT("sd4:");
    DEBUG_ECH_PRINTLN(value);

    DEBUG_ECH_PRINTLN("ECH Reading DigitalInput");
    result = eCH210BModbus.readHoldingRegisters(ADDR_DIGITAL_INPUT, 1);
    if (result != 0)
    {
        DEBUG_ECH_PRINTLN(" ECH Error reading ECHSensors");
        DataMgr::setEchComStatus(false);
        BLINK
        return;
    }
    DataMgr::setEchComStatus(true);

    value = eCH210BModbus.getResponseBuffer(0);
    DataMgr::setDigitalInput(value);
    eCH210BModbus.clearResponseBuffer();
    delay(MODBUS_MESSAGE_DELAY);

    DEBUG_ECH_PRINT("compressorIn:");
    DEBUG_ECH_PRINT((0 != (DataMgr::getDigitalInput() & MASK_DI_COMPRESSOR)));
    DEBUG_ECH_PRINT(" boilerIn:");
    DEBUG_ECH_PRINT((0 != (DataMgr::getDigitalInput() & MASK_DI_BOILER)));
    DEBUG_ECH_PRINT(" pumpIn:");
    DEBUG_ECH_PRINT((0 != (DataMgr::getDigitalInput() & MASK_DI_PUMP)));
    DEBUG_ECH_PRINT(" warmcoolIn:");
    DEBUG_ECH_PRINT((0 != (DataMgr::getDigitalInput() & MASK_DI_WARMCOOL)));
    DEBUG_ECH_PRINT(" onstandbyIn:");
    DEBUG_ECH_PRINTLN((0 != (DataMgr::getDigitalInput() & MASK_DO_REVERSAL)));

    DEBUG_ECH_PRINTLN("ECH Reading DigitalOutput ");
    result = eCH210BModbus.readHoldingRegisters(ADDR_DIGITAL_OUTPUT, 1);
    if (result != 0)
    {
        DEBUG_ECH_PRINTLN("ECH Error reading ECHSensors");
        BLINK
        return;
    }
    value = eCH210BModbus.getResponseBuffer(0);
    DataMgr::setDigitalOutput(value);
    eCH210BModbus.clearResponseBuffer();
    delay(MODBUS_MESSAGE_DELAY);

    DEBUG_ECH_PRINT("compressorOut:");
    DEBUG_ECH_PRINT((0 != (value & MASK_DO_COMPRESSOR)));
    DEBUG_ECH_PRINT("pumpOut:");
    DEBUG_ECH_PRINT((0 != (value & MASK_DO_PUMP)));
    DEBUG_ECH_PRINT("valveOut:");
    DEBUG_ECH_PRINT((0 != (value & MASK_DO_REVERSAL)));
    DEBUG_ECH_PRINT("valveOut:");
    DEBUG_ECH_PRINTLN((0 != (value & MASK_DO_COMPRESSOR)));
    DEBUG_ECH_PRINT("alarmOut:");
    DEBUG_ECH_PRINTLN((0 != (value & MASK_DO_ALARM)));
}

uint8_t readRegisters(uint16_t addr, uint16_t &value)
{
    uint8_t result = eCH210BModbus.readHoldingRegisters(addr, 1);
    if (result == 0)
    {
        value = eCH210BModbus.getResponseBuffer(0);
        eCH210BModbus.clearResponseBuffer();
    }

    return result;
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