#ifndef DAC8552_H
#define DAC8552_H

#include "main.h"

#define DAC8552_POWERDOWN_NORMAL   0
#define DAC8552_POWERDOWN_1K       1
#define DAC8552_POWERDOWN_100K     2
#define DAC8552_POWERDOWN_HIGH_IMP 3

// 参考电压
#define DAC8552_Vref 2.4991f
// 通道
#define DAC8552_ChannelA 0x100000
#define DAC8552_ChannelB 0x240000

// 函数
void DAC8552_WriteData(SPI_HandleTypeDef *hspi, uint32_t Data);
void DAC8552_WriteA(SPI_HandleTypeDef *hspi, float Voltage);
void DAC8552_WriteB(SPI_HandleTypeDef *hspi, float Voltage);

void DAC8552_WriteA_test(SPI_HandleTypeDef *hspi, uint32_t Data);

#endif