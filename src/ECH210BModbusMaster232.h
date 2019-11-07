#ifndef ECH210BMODBUSMASTER232_H
#define ECH210BMODBUSMASTER232_H

#include <Arduino.h>
#include <Stream.h>
#include "SoftwareSerial8E1.h"

class ECH210BModbusMaster232 {
    public:
        void init(uint8_t u8RxPin, uint8_t u8TxPin, uint8_t u8MBSlave);
        void begin(unsigned long BaudRate);
        uint8_t readHoldingRegisters(uint16_t u16ReadAddress,uint16_t u16ReadQty);
        uint16_t getResponseBuffer(uint8_t u8Index);
        void clearResponseBuffer();
    private:
        uint8_t ModbusMasterTransaction(uint8_t u8MBFunction);
        uint16_t _crc16_update2 (uint16_t crc, uint8_t a);
        SoftwareSerial8E1* pSwSerial;
};

#endif
