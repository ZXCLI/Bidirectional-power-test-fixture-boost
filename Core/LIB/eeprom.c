/* 2025-04-18	by zxcli
 * Code version: 1.0
 * ad24c02文件管理
 * 24c02地址格式：
 * 32页(PageAddr:0-31)
 *   |___8字节(ByteAddr:0-7)
 * 设备地址
 * +-----+---+---+---+---+---+---+---+-----+
 * | bit | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7   |
 * +-----+---+---+---+---+---+---+---+-----+
 * |     | 1 | 0 | 1 | 0 | 0 | 0 | 0 | R/W |
 * +-----+---+---+---+---+---+---+---+-----+
 * 10100000--->0xA0
 * 
 * 数据地址
 * +-----+-------+-------+-------+-------+-------+----------+----------+----------+
 * | bit | 0     | 1     | 2     | 3     | 4     | 5        | 6        | 7        |
 * +-----+-------+-------+-------+-------+-------+----------+----------+----------+
 * |     | Page1 | Page2 | Page3 | Page4 | page5 | ByteAddr1| ByteAddr2| ByteAddr3|
 * +-----+-------+-------+-------+-------+-------+----------+----------+----------+
 * 
 * 数据帧格式
 * 数据块帧头----数据长度-----数据------数据块帧尾
 * +--------+-------------+--------+--------------+
 * | Header | Data Length |  Data  | End of Frame |
 * +--------+-------------+--------+--------------+
 * | 8bit   | 8bit        | n*8bit | 8bit         |
 * +--------+-------------+--------+--------------+
 * 数据块帧头：8位
 * 数据长度：8位，以字节为单位，大小3+n
 * 数据块帧尾：8位，为(数据块帧头|数据长度)
 * 数据头定义：
 * +-----+---+---+---+---+---+---+---+---+
 * | bit | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 * +-----+---+---+---+---+---+---+---+---+
 * |     |   devices addr    | data addr |
 * +-----+-------------------+-----------+
 * 
 */

#include "eeprom.h"

#define AT24BaseAddr 0xA0

void at24_EraseMemFull()
{
    uint8_t temp[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    for(uint8_t j = 0;j < 32;j++){
        HAL_I2C_Mem_Write(&hi2c1,AT24BaseAddr,j<<3,1,temp,8,0xFF);
        HAL_Delay(2);
    }
}

//字节写
void at24_BitWrite(I2C_HandleTypeDef* I2cHandle,uint8_t PageAddr,
                   uint8_t ByteAddr,uint8_t* RxData)
{
    HAL_I2C_Mem_Write(I2cHandle,AT24BaseAddr,
                      (PageAddr<<3)|ByteAddr,1,RxData,1,0xFF);
}

//按页写
void at24_PageWrite(I2C_HandleTypeDef* I2cHandle,
                    uint8_t PageAddr,uint8_t ByteAddr,uint16_t DataLength,
                    uint8_t* RxData)
{
    if((ByteAddr+DataLength) > ONE_PAGE_BYTE)   {return;}
    HAL_I2C_Mem_Write(I2cHandle,AT24BaseAddr,
                      (PageAddr<<3)|ByteAddr,1,RxData,DataLength,0xFF);
}

//任意地址，任意长度按页写
void at24_RandomWrite(I2C_HandleTypeDef* I2cHandle,
                      uint8_t PageAddr,uint8_t ByteAddr,uint16_t DataLength,
                      uint8_t* RxData)
{
    if((ByteAddr+DataLength) <= ONE_PAGE_BYTE){//数据未超过一页
        at24_PageWrite(I2cHandle,PageAddr,
                       ByteAddr,DataLength,RxData);
    }else{//数据超过一页
        uint8_t ByteAddrOffset = ONE_PAGE_BYTE - ByteAddr;
        uint8_t PageNum = (DataLength + ByteAddr)>>3;
        at24_PageWrite(I2cHandle,PageAddr,
                       ByteAddr,ByteAddrOffset,RxData);//第一页
        HAL_Delay(2);//给上面的操作延时，让EEPROM写入完成
        for(uint8_t i = 0;i < PageNum;i++){
            if(i == (PageNum-1)){//最后一页
                at24_PageWrite(I2cHandle,PageAddr+1+(i%ONE_PAGE_BYTE),
                               0x00,((ByteAddr+DataLength)%ONE_PAGE_BYTE),RxData+ByteAddrOffset+i*ONE_PAGE_BYTE);//最后一页
                HAL_Delay(2);
            }else{//中间的页
                at24_PageWrite(I2cHandle,PageAddr+1+(i%ONE_PAGE_BYTE),
                               0x00,ONE_PAGE_BYTE,RxData+ByteAddrOffset+i*ONE_PAGE_BYTE);
                HAL_Delay(2);
            }
        }
    }
}
//任意地址，任意长度读
void at24_RandomRead(I2C_HandleTypeDef* I2cHandle,
                     uint8_t PageAddr,uint8_t ByteAddr,uint16_t DataLength,uint8_t* RxData)
{
    HAL_I2C_Mem_Read(I2cHandle,AT24BaseAddr+1,(PageAddr<<3)|ByteAddr,
                     1,RxData,DataLength,0xFF);
}

//floa转4个char类型数据
void floatTO4char(float data,uint8_t* buf)
{
    for(uint8_t i = 0;i<4;i++){
       buf[i]=(*((uint8_t*)(&data)+i));
    }
}
//转回来
void charTOfloat(uint8_t* buf,float* data)
{
    uint32_t temp=buf[3];
	for(int8_t i=2;i>=0;i--){
		temp=(temp<<8)|buf[i];
	}
	float *p=(float*)(&temp);//暂存
	*data=*p;
}
//uint16_t转2个char类型数据
void uint16TO2char(uint16_t data,uint8_t* buf)
{
    buf[0]=(data)&0xFF;
    buf[1]=data>>8;
}
//转回来
void charTOuint16(uint8_t* buf,uint16_t* data)
{
    *data = (uint16_t)(buf[0]|(buf[1]<<8));
}



void at24_HAL_write(I2C_HandleTypeDef* I2cHandle,
                    uint8_t PageAddr,uint8_t ByteAddr,uint16_t DataLength,uint8_t* RxData)
{
    if(DataLength == 1){
        at24_BitWrite(I2cHandle,PageAddr,ByteAddr,RxData);
        HAL_Delay(2);
    }else{
        at24_RandomWrite(I2cHandle,PageAddr,ByteAddr,DataLength,RxData);
    }
}

void at24_HAL_read(I2C_HandleTypeDef* I2cHandle,
                   uint8_t PageAddr,uint8_t ByteAddr,uint16_t DataLength,uint8_t* RxData)
{
    at24_RandomRead(I2cHandle,PageAddr,ByteAddr,DataLength,RxData);
}
