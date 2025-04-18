#include "shell.h"
#include "loop.h"
#include "spi.h"
#include "dac8552.h"
#include "shell_port.h"
#include "stdio.h"

//这个文件用于注册shell命令

int DAC_A_test(float value)
{
    DAC8552_WriteA(&hspi1, value);
    return 0;
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    DAC_A_out, DAC_A_test, "DAC_A_out 0.0~2.5"
);


int DAC_B_test(float value)
{
    DAC8552_WriteB(&hspi1, value);
    return 0;
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    DAC_B_out, DAC_B_test, "DAC_B_out 0.0~2.5"
);

int16_t SetClockPhases(uint8_t phases)
{
    uint8_t six_phase[6] = {1,24+1,48+1,72+1,96+1,1};   //六相全部工作
    uint8_t three_phase[3] = {1,60+1,1};                //三相只有1，3，6工作

    static uint8_t last_phases = 6;
    if (phases != last_phases)
    {
        if((phases != 0) && (phases != 1) && (phases != 2) && (phases != 3) && (phases != 6)){
            return -1;
        }    
        switch(last_phases | (phases << 3)){
            case (6|(3<<3))://六相转三相
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                TIM1->CCR3 = three_phase[1];
                break;
            case (6|(2<<3))://六相转两相
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                break;
            case (6|(1<<3))://六相转一相
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (6|(0<<3))://六相转零相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (3|(6<<3))://三相转六相
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                TIM1->CCR3 = six_phase[2];
                break;
            case (3|(2<<3))://三相转两相
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                break;
            case (3|(1<<3))://三相转一相
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (3|(0<<3))://三相转零相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (2|(6<<3))://两相转六相
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                TIM1->CCR3 = six_phase[2];
                break;
            case (2|(3<<3))://两相转三相
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                TIM1->CCR3 = three_phase[1];
                break;
            case (2|(1<<3))://两相转一相
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (2|(0<<3))://两相转零相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,StandBy);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (1|(6<<3))://一相转六相
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                TIM1->CCR3 = six_phase[2];
                break;
            case (1|(3<<3))://一相转三相
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                TIM1->CCR3 = three_phase[1];
                break;
            case (1|(2<<3))://一相转两相
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (1|(0<<3))://一相转零相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,StandBy);
                LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                break;
            case (0|(6<<3))://零相转六相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO2_GPIO_Port,UVLO2_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO4_GPIO_Port,UVLO4_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO5_GPIO_Port,UVLO5_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH4);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                TIM1->CCR3 = six_phase[2];
                break;
            case (0|(3<<3))://零相转三相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO3_GPIO_Port,UVLO3_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                TIM1->CCR3 = three_phase[1];
                break;
            case (0|(2<<3))://零相转两相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,RunNormal);
                HAL_GPIO_WritePin(UVLO6_GPIO_Port,UVLO6_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
                break;
            case (0|(0<<3))://零相转一相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                break;
            default:
                break;
        }
        last_phases = phases;
    }
    return phases;
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    setClockPhases, SetClockPhases, "SetClockPhases 1,2,3 or 6"
);

// 直接设置输出电压和电流
void SetVoltageOrCurrent(DAC_CHANNELS channel, float value)
{
    if (channel == VOLTAGE) {
        //float voltage = value * 0.034275f + 0.000393f;
        uint32_t voltage = (int)(value * device.dataConver[V_OUT].a1 + device.dataConver[V_OUT].a0);
        DAC8552_WriteA_test(&hspi1, voltage); // 设置输出电压
    } else if (channel == CURRENT) {
        float current = value * 0.018614f + 0.131894;
        DAC8552_WriteB(&hspi1, current); // 设置输入电流
    }
}

// 间接设置输出电压，带斜坡
void setOutputVoltage(float voltage)
{
    if(voltage < 12.0f)
    {
        return;
    }else
    {
        device.DAC_voltage_ref = voltage;
    }
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    setOutputVoltage, setOutputVoltage, "SetOutputVoltage"
);

// 间接设置输入电流，带斜坡
void setIutputCurrent(float current)
{
    if(current < 0.5f)
    {
        return;
    }else
    {
        device.DAC_current_ref = current;
    }
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    setIutputCurrent, setIutputCurrent, "SetOutputCurrent"
);
