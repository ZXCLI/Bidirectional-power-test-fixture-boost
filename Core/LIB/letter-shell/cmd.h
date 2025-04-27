#ifndef CMD_H
#define CMD_H

#include "shell.h"
#include "loop.h"

int DAC_A_test(float value);
int DAC_B_test(float value);
int16_t SetClockPhases(uint8_t phases);
void SetVoltageOrCurrent(DAC_CHANNELS channel, float value);
void setOutputVoltage(float voltage);
void setIutputCurrent(float current);
int writeDacConverData(char *ChannelType, float a0, float a1);
int writeAdcConverData(char *ChannelType, float a0, float a1);

#endif
