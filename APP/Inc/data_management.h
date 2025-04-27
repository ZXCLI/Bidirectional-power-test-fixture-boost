#ifndef DATA_MANAGEMENT_H
#define DATA_MANAGEMENT_H

#include "main.h"
#include "loop.h"
#include "eeprom.h"

#define ONE_FLOAT_BYTE    sizeof(float)
#define ONE_uint16_BYTE   sizeof(uint16_t)

#define VIN_OFFSET        0
#define VOUT_OFFSET       8
#define IIN_OFFSET        16
#define IOUT_OFFSET       24

#define DAC_CONVER_OFFSET 0x00
#define ADC_CONVER_OFFSET 32

// eeprom异步写入的数据队列
typedef struct
{
    uint8_t len;
    uint8_t index;
    uint8_t data[32]; // 一个float占4个字节，一个uint16_t占2个字节，一个uint8_t占1个字节
    uint8_t offset;   // 已写入的数据偏移量（避免内存拷贝）
    uint8_t PageAddr;
    uint8_t ByteAddr;
} dataQueue;

void eepromWrite(uint16_t len, uint8_t *data, uint8_t PageAddr, uint8_t ByteAddr);

#endif /* DATA_MANAGEMENT_H */
