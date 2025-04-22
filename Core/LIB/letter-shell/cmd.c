#include "shell.h"
#include "loop.h"
#include "spi.h"
#include "dac8552.h"
#include "shell_port.h"
#include "stdio.h"
#include "least_squares.h"

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
            case (0|(1<<3))://零相转一相
                HAL_GPIO_WritePin(UVLO1_GPIO_Port,UVLO1_Pin,RunNormal);
                LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
                break;
            default:
                break;
        }
        last_phases = phases;
        device.phase = phases;  // 保存当前相位
        if(limiteCurrent() == 1){
            SEGGER_RTT_printf(0, "ERROR:The current is too high and has been adjusted to %f\r\n", device.DAC_current_ref);
        }
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
        // float voltage = value * 0.034275f + 0.000393f;<---> 映射关系：DAC_A输出电压 = 0.034275*输入电压V + 0.000393
        uint32_t voltage = (int)(value * device.DACdataConver[V_OUT].a1 + device.DACdataConver[V_OUT].a0);
        // 现在改为直接用输出电压映射到DAC的16位数值
        //DAC8552_WriteA(&hspi1,voltage); // 设置输出电压
        DAC8552_WriteA_test(voltage);
    } else if (channel == CURRENT) {
        // float current = value * 0.018614f + 0.131894;<---> 映射关系：DAC_B输出电压 = 0.018614*输入电流A + 0.131894
        uint32_t current = (int)(value * device.DACdataConver[I_IN].a1 + device.DACdataConver[I_IN].a0);
        // 现在改为直接用输入电流映射到DAC的16位数值
        //DAC8552_WriteB(&hspi1, current); // 设置输入电流
        DAC8552_WriteB_test(current);
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
        if (limiteCurrent() == 1) {
            SEGGER_RTT_printf(0, "ERROR:The current is too high and has been adjusted to %f\r\n", device.DAC_current_ref);
        }
    }
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    setIutputCurrent, setIutputCurrent, "SetOutputCurrent"
);

// 在运行下面的校准命令前，请先运行StartCalibration命令
void StartCalibration(void)
{
    DAC8552_WriteA_test(13474); // 15V
    DAC8552_WriteB_test(3942);  // 1A
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    StartCalibration, StartCalibration, "StartCalibration"
);

// 校准电压
int CalibrationVoltage(float realVoltage)
{
    static uint8_t count = 0;
    static float RealVolt[5] = {0}; //x
    static float ADC_date[5] = {0}; //x

    float DAC_input[5] = {13474.0f,17969.0f,22462.0f,26954.0f,31447.0f};//15V,20V,25V,30V,35V,y
   
    RealVolt[count] = realVoltage;
    ADC_date[count] = (float)(device.adc_value[V_OUT]);
    count++;
    
    if(count == 5)
    {
        count = 0;
        LinearFitResult result_voltTOdac;
        LinearFitResult result_adcTOvolt;
        if ((Linear_LeastSquares_Fit(RealVolt,DAC_input,5,&result_voltTOdac) == LS_OK) &&
            (Linear_LeastSquares_Fit(ADC_date,RealVolt,5,&result_adcTOvolt) == LS_OK)){
            SEGGER_RTT_printf(0, "Linear_LeastSquares_Fit OK\n\r");
            SEGGER_RTT_printf(0,"Voltage to DAC a1:%f,a0:%f,r:%f\n\r",
                                result_voltTOdac.slope,result_voltTOdac.intercept,result_voltTOdac.r_squared);
            SEGGER_RTT_printf(0,"ADC to Voltage (x1000) a1:%f,(x1000) a0:%f,r:%f\n\r",
                                result_adcTOvolt.slope*1000.0f,result_adcTOvolt.intercept*1000.0f,result_adcTOvolt.r_squared);
            return 0; 
        }else{
            SEGGER_RTT_printf(0,"Linear_LeastSquares_Fit error\n\r");
            return -1;
        }
    }
    
    DAC8552_WriteA_test((int)(DAC_input[count]));
    return count;
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    CalibrationVoltage, CalibrationVoltage, "CalibrationVoltage realVoltage"
);

// 校准电流
int CalibrationCurrent(float realCurrent)
{
    static uint8_t count     = 0;
    static float RealCur[5] = {0}; // x
    static float ADC_date[5] = {0}; // x

    float DAC_input[5] = {3942.0f, 4430.0f, 5400.0f, 6370.0f, 7340.0f}; // 1A,2A,4A,6A,8A,y

    RealCur[count] = realCurrent;
    ADC_date[count] = (float)(device.adc_value[I_IN]);
    count++;

    if (count == 5) {
        count = 0;
        LinearFitResult result_curTOdac;
        LinearFitResult result_adcTOcur;
        if ((Linear_LeastSquares_Fit(RealCur, DAC_input, 5, &result_curTOdac) == LS_OK) && 
            (Linear_LeastSquares_Fit(ADC_date, RealCur, 5, &result_adcTOcur) == LS_OK)) {
            SEGGER_RTT_printf(0, "Linear_LeastSquares_Fit OK\n\r");
            SEGGER_RTT_printf(0, "Current to DAC a1:%f,a0:%f,r:%f\n\r", 
                                 result_curTOdac.slope, result_curTOdac.intercept, result_curTOdac.r_squared);
            SEGGER_RTT_printf(0, "ADC to Current (x1000) a1:%f,(x1000) a0:%f,r:%f\n\r",
                                 result_adcTOcur.slope*1000.0f, result_adcTOcur.intercept*1000.0f, result_adcTOcur.r_squared);
            return 0;
        } else {
            SEGGER_RTT_printf(0, "Linear_LeastSquares_Fit error\n\r");
            return -1;
        }
    }

    DAC8552_WriteB_test((int)(DAC_input[count]));
    return count;
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    CalibrationCurrent, CalibrationCurrent, "CalibrationCurrent realCurrent"
);

void sendCruent(void)
{
    SEGGER_RTT_printf(0, "Current:%f\n\r", device.DAC_current_ref);
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    sendCruent, sendCruent, "sendCruent"
);

void sendVoltage(void) 
{
    SEGGER_RTT_printf(0, "Voltage:%f\n\r", device.DAC_voltage_ref);
}

SHELL_EXPORT_CMD(   
    SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
    sendVoltage, sendVoltage, "sendVoltage"
);
