#ifndef _LOOP_H
#define _LOOP_H


#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

#include "DAC8552.h"
#include "ADS1220.h"
#include "SEGGER_RTT.h"

#define RunNormal GPIO_PIN_RESET
#define StandBy   GPIO_PIN_SET

void MY_Init();
void MY_Loop();

void UVLO_ALL_Close();
void boostClockInit();
void fanPWMInit();

#endif /* _LOOP_H */