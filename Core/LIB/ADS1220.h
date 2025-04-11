/*
 * Single-header pure C ADS1220 driver for STM32 with HAL.
 * You probably want to add error checking, since there's none
 *
 * Chip select twiddling is not implemented - pull to ground(1 device only) or implement your own
 *
 * Bits taken from various examples, mostly GH@ Protocentral/Protocentral_ADS1220
 * License: MIT(http://opensource.org/licenses/MIT)
 */

#ifndef INC_ADS1220_H_
#define INC_ADS1220_H_

#include "main.h" // Your project's main should include HAL

// //Commands
// #define ADS1220_RESET 0x06 // Good idea to reset on power-up
// #define ADS1220_START 0x08 // Both single-shot and continuous conversion must be started via 0x08
// #define ADS1220_WREG  0x40
// #define ADS1220_RREG  0x20

// //Registers
// #define ADS1220_CONFIG_REG0_ADDRESS 0x00
// #define ADS1220_CONFIG_REG1_ADDRESS 0x01
// #define ADS1220_CONFIG_REG2_ADDRESS 0x02
// #define ADS1220_CONFIG_REG3_ADDRESS 0x03

// //Masks
// #define ADS1220_REG_CONFIG1_DR_MASK       0xE0
// #define ADS1220_REG_CONFIG0_PGA_GAIN_MASK 0x0E
// #define ADS1220_REG_CONFIG0_MUX_MASK      0xF0

// //Sample rate
// #define ADS1220_DR_20SPS   0x00
// #define ADS1220_DR_45SPS   0x20
// #define ADS1220_DR_90SPS   0x40
// #define ADS1220_DR_175SPS  0x60
// #define ADS1220_DR_330SPS  0x80
// #define ADS1220_DR_600SPS  0xA0
// #define ADS1220_DR_1000SPS 0xC0

// //PGA gain settings
// #define ADS1220_PGA_GAIN_1   0x00
// #define ADS1220_PGA_GAIN_2   0x02
// #define ADS1220_PGA_GAIN_4   0x04
// #define ADS1220_PGA_GAIN_8   0x06
// #define ADS1220_PGA_GAIN_16  0x08
// #define ADS1220_PGA_GAIN_32  0x0A
// #define ADS1220_PGA_GAIN_64  0x0C
// #define ADS1220_PGA_GAIN_128 0x0E

// //Input mux
// #define ADS1220_MUX_AIN0_AIN1 0x00
// #define ADS1220_MUX_AIN0_AIN2 0x10
// #define ADS1220_MUX_AIN0_AIN3 0x20
// #define ADS1220_MUX_AIN1_AIN2 0x30
// #define ADS1220_MUX_AIN1_AIN3 0x40
// #define ADS1220_MUX_AIN2_AIN3 0x50
// #define ADS1220_MUX_AIN1_AIN0 0x60
// #define ADS1220_MUX_AIN3_AIN2 0x70
// #define ADS1220_MUX_AIN0_AVSS 0x80
// #define ADS1220_MUX_AIN1_AVSS 0x90
// #define ADS1220_MUX_AIN2_AVSS 0xA0
// #define ADS1220_MUX_AIN3_AVSS 0xB0

// #define _BV(bit) (1<<(bit))

// struct ADS1220_regs_s
// {
// 	uint8_t cfg_reg0; // = 0x00;   //AINP=AIN0, AINN=AIN1, gain=1, PGA is enabled
// 	uint8_t cfg_reg1; // = 0x04;   //20SPS, Mode=Normal, Conversion=Continuous, Temp mode disabled, Current source disabled
// 	uint8_t cfg_reg2; // = 0x10;   //Internal 2.048V VREF, 50/60Hz filter, transistors open, IDAC off
// 	uint8_t cfg_reg3; // = 0x00;   //IDAC1&2 are disabled, only DRDY signals conversion completion
// } ADS1220_default_regs =
// { 0x00, 0x04, 0x10, 0x00 };

// typedef struct ADS1220_regs_s ADS1220_regs; // Default init


// Commands
#define ADS1220_RESET 0x06
#define ADS1220_START 0x08
#define ADS1220_WREG  0x40
#define ADS1220_RREG  0x20

// Registers
#define ADS1220_CONFIG_REG0_ADDRESS 0x00
#define ADS1220_CONFIG_REG1_ADDRESS 0x01
#define ADS1220_CONFIG_REG2_ADDRESS 0x02
#define ADS1220_CONFIG_REG3_ADDRESS 0x03

// Masks
#define ADS1220_REG_CONFIG1_DR_MASK       0xE0
#define ADS1220_REG_CONFIG0_PGA_GAIN_MASK 0x0E
#define ADS1220_REG_CONFIG0_MUX_MASK      0xF0

// Data Rates
#define ADS1220_DR_20SPS   0x00
#define ADS1220_DR_45SPS   0x20
#define ADS1220_DR_90SPS   0x40
#define ADS1220_DR_175SPS  0x60
#define ADS1220_DR_330SPS  0x80
#define ADS1220_DR_600SPS  0xA0
#define ADS1220_DR_1000SPS 0xC0

// PGA Gains
#define ADS1220_PGA_GAIN_1   0x00
#define ADS1220_PGA_GAIN_2   0x02
#define ADS1220_PGA_GAIN_4   0x04
#define ADS1220_PGA_GAIN_8   0x06
#define ADS1220_PGA_GAIN_16  0x08
#define ADS1220_PGA_GAIN_32  0x0A
#define ADS1220_PGA_GAIN_64  0x0C
#define ADS1220_PGA_GAIN_128 0x0E

// MUX Configurations
#define ADS1220_MUX_AIN0_AIN1 0x00
#define ADS1220_MUX_AIN0_AIN2 0x10
#define ADS1220_MUX_AIN0_AIN3 0x20
#define ADS1220_MUX_AIN1_AIN2 0x30
#define ADS1220_MUX_AIN1_AIN3 0x40
#define ADS1220_MUX_AIN2_AIN3 0x50
#define ADS1220_MUX_AIN1_AIN0 0x60
#define ADS1220_MUX_AIN3_AIN2 0x70
#define ADS1220_MUX_AIN0_AVSS 0x80
#define ADS1220_MUX_AIN1_AVSS 0x90
#define ADS1220_MUX_AIN2_AVSS 0xA0
#define ADS1220_MUX_AIN3_AVSS 0xB0

#define _BV(bit) (1<<(bit))

// 结构体定义
typedef struct {
    uint8_t cfg_reg0;   // 控制AINP和AINN的引脚，设置增益，启用PGA
    uint8_t cfg_reg1;   // 控制采样率和转换模式
    uint8_t cfg_reg2;   // 控制参考电压，滤波器，IDAC电流源
    uint8_t cfg_reg3;   // 控制IDAC电流源，DRDY信号
} ADS1220_regs;

// 默认配置（声明为extern）
extern ADS1220_regs ADS1220_default_regs;

// 函数声明
void ADS1220_writeRegister(SPI_HandleTypeDef *hspi, uint8_t address, uint8_t value);
uint8_t ADS1220_readRegister(SPI_HandleTypeDef *hspi, uint8_t address);
void ADS1220_reset(SPI_HandleTypeDef *hspi);
uint8_t ADS1220_init(SPI_HandleTypeDef *hspi, ADS1220_regs *r);
void ADS1220_start_conversion(SPI_HandleTypeDef *hspi);
void ADS1220_enable_PGA(SPI_HandleTypeDef *hspi, ADS1220_regs *r);
void ADS1220_disable_PGA(SPI_HandleTypeDef *hspi, ADS1220_regs *r);
void ADS1220_set_conv_mode_continuous(SPI_HandleTypeDef *hspi, ADS1220_regs *r);
void ADS1220_set_conv_mode_single_shot(SPI_HandleTypeDef *hspi, ADS1220_regs *r);
void ADS1220_set_data_rate(SPI_HandleTypeDef *hspi, int datarate, ADS1220_regs *r);
void ADS1220_select_mux_config(SPI_HandleTypeDef *hspi, int channels_conf, ADS1220_regs *r);
void ADS1220_set_pga_gain(SPI_HandleTypeDef *hspi, int pgagain, ADS1220_regs *r);
uint8_t* ADS1220_get_config(SPI_HandleTypeDef *hspi, ADS1220_regs *r);
int32_t ADS1220_read_blocking(SPI_HandleTypeDef *hspi, GPIO_TypeDef *DRDY_PORT, uint16_t DRDY_PIN, uint16_t timeout);
int32_t ADS1220_read_singleshot(SPI_HandleTypeDef *hspi, GPIO_TypeDef *DRDY_PORT, uint16_t DRDY_PIN, uint16_t timeout);
int32_t ADS1220_read_singleshot_channel(SPI_HandleTypeDef *hspi, uint8_t channel_num, ADS1220_regs *r, GPIO_TypeDef *DRDY_PORT, uint16_t DRDY_PIN, uint16_t timeout);

#endif /* INC_ADS1220_H_ */
