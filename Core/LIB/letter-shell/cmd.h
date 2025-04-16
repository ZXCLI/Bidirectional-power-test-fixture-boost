#ifndef CMD_H
#define CMD_H

#include "shell.h"

int DAC_A_test(float value);
int DAC_B_test(float value);
int16_t SetClockPhases(uint8_t phases);
void SetVoltageOrCurrent(DAC_CHANNELS channel, float value);
void setOutputVoltage(float voltage);
void setIutputCurrent(float current);

#endif
