#ifndef _LOOP_H
#define _LOOP_H

#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

#include "DAC8552.h"
#include "ADS1220.h"
#include "SEGGER_RTT.h"


#define DEBUG1_IN HAL_GPIO_WritePin(TEST1_GPIO_Port, TEST1_Pin, GPIO_PIN_SET)
#define DEBUG1_OUT HAL_GPIO_WritePin(TEST1_GPIO_Port, TEST1_Pin, GPIO_PIN_RESET)

#define DEBUG2_IN HAL_GPIO_WritePin(TEST2_GPIO_Port, TEST2_Pin, GPIO_PIN_SET)
#define DEBUG2_OUT HAL_GPIO_WritePin(TEST2_GPIO_Port, TEST2_Pin, GPIO_PIN_RESET)

#define RunNormal GPIO_PIN_RESET
#define StandBy   GPIO_PIN_SET

typedef enum
{
    V_IN = 0,
    V_OUT,
    I_IN,
    I_OUT
}ADC_CHANNELS;

typedef enum
{
    VOLTAGE = 0,
    CURRENT
}DAC_CHANNELS;

typedef struct
{
    float a0;   // y = a0 + a1 * x
    float a1;
} DataConverCoeff;

typedef struct
{
    float V_IN;
    float V_OUT;
    float I_IN;
    float I_OUT;
    float DAC_voltage_ref;
    float DAC_current_ref;
    float DAC_voltage_now;
    float DAC_current_now;
    uint32_t adc_value[4]; // 0:V_IN 1:V_OUT 2:I_IN 3:I_OUT
    DataConverCoeff ADCdataConver[4];   // ADC值转实际电压电流值
    DataConverCoeff DACdataConver[4];   // 设定值转DAC值，为了保持一致性，虽然只有两个通道，但是和上面一样定义为4个
    uint16_t phase;   // 运行的相数
    uint8_t Vtimer_A_ARR;
    uint8_t Vtimer_B_ARR;
    uint16_t system_status;
}Device;

extern Device device;

void MY_Init();
void MY_Loop();

void converDataInit();
void UVLO_ALL_Close();
void moduleClockInit();
void fanPWMInit();
float Slew_Func(float *slewVal, float refVal, float slewRate);
void updateDAC();
int limiteCurrent();

#endif /* _LOOP_H */