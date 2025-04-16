#ifndef _LOOP_H
#define _LOOP_H

#include "stdbool.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

#include "DAC8552.h"
#include "ADS1220.h"
#include "SEGGER_RTT.h"

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

extern bool A_Task_Flag; // A分支任务标志
extern bool B_Task_Flag; // B分支任务标志

void MY_Init();
void MY_Loop();

void UVLO_ALL_Close();
void boostClockInit();
void fanPWMInit();

void A0(void);
void A1(void);
void A2(void);

void B0(void);
void B1(void);
void B2(void);

#endif /* _LOOP_H */