#include "DAC8552.h"
#include "spi.h"

void DAC8552_WriteData(SPI_HandleTypeDef *hspi, uint32_t Data)
{
	uint8_t TXbuff[3];
	Data &= 0xffffff;
	TXbuff[0] = (Data >> 16) & 0xff;
	TXbuff[1] = (Data >> 8) & 0xff;
	TXbuff[2] = (Data >> 0) & 0xff;
	
	HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_RESET);
	
	HAL_SPI_Transmit(hspi, TXbuff, 3, HAL_MAX_DELAY);
	
	HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_SET);
}
//******************************************
//函数名称：DAC8552_VoltageToData
//函数功能：将电压值转化为二进制数据
//参    数：float Vref    ----参考电压值
//          float Voltage ----要转化的电压值
//返还值：  转化后的二进制数据
//******************************************
uint32_t DAC8552_VoltageToData(float Vref, float Voltage)
{
	uint32_t Data=0;
	Data = (uint32_t)(65536 * Voltage / Vref);
    Data &= 0x00ffff;
    return Data;
}

//******************************************
//函数名称：DAC8552_WriteA
//函数功能：写数据给通道A
//参    数：float Voltage----要写入的电压值
//返还值：  无
//******************************************
void DAC8552_WriteA(SPI_HandleTypeDef *hspi,float Voltage)
{
  uint32_t Data=0;
  Voltage = (Voltage-0.00416f)/1.000546f;
  Data = DAC8552_VoltageToData(DAC8552_Vref, Voltage);
  Data = DAC8552_ChannelA|Data;
  DAC8552_WriteData(hspi, Data);
}

// 直接写入16位数据
void DAC8552_WriteA_test(uint32_t Data)
{
  Data = DAC8552_ChannelA|Data;
  DAC8552_WriteData(&hspi1, Data);
}

//******************************************
//函数名称：DAC8552_WriteB
//函数功能：写数据给通道B
//参    数：float Voltage----要写入的电压值
//返还值：  无
//******************************************
void DAC8552_WriteB(SPI_HandleTypeDef *hspi,float Voltage)
{
  uint32_t Data=0;
  Voltage = (Voltage+0.004657f)/1.0000167f;
  Data = DAC8552_VoltageToData(DAC8552_Vref, Voltage);
  Data = DAC8552_ChannelB|Data;
  DAC8552_WriteData(hspi, Data);
}

// 直接写入16位数据
void DAC8552_WriteB_test(uint32_t Data)
{
  Data = DAC8552_ChannelB|Data;
  DAC8552_WriteData(&hspi1, Data);
}