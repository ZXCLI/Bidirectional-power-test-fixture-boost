#include "loop.h"
#include "shell_port.h"
#include "stdio.h"
#include "cmd.h"

Device device;

char rttLogWriteBuffer[512];
char rttLogReadBuffer[512];

void (*Alpha_State_Ptr)(void); // 基态状态机指针
void (*A_Task_Ptr)(void);      // A分支任务指针
void (*B_Task_Ptr)(void);      // B分支任务指针
bool A_Task_Flag = false;      // A分支任务标志
bool B_Task_Flag = false;      // B分支任务标志

ADS1220_regs ADS1220_default_regs = {
    .cfg_reg0 = ADS1220_PGA_GAIN_1,         // 1x增益
    .cfg_reg1 = ADS1220_DR_45SPS | _BV(2),  // 采样率20 SPS，连续模式
    .cfg_reg2 = (0x01 << 6) | (0x01 << 4),  // 外部参考电压，FIR为50Hz
    .cfg_reg3 = 0x00                        // 关闭IDAC
};

void MY_Init(void)
{
    SEGGER_RTT_Init();
    //配置通道一的缓冲区
    SEGGER_RTT_ConfigUpBuffer(1, "Log", rttLogWriteBuffer, 512, SEGGER_RTT_MODE_NO_BLOCK_SKIP); // 配置写缓冲
    SEGGER_RTT_ConfigDownBuffer(1, "Log", rttLogReadBuffer, 512, SEGGER_RTT_MODE_NO_BLOCK_SKIP); // 配置读缓冲

    UVLO_ALL_Close();       // 关闭所有模块
    moduleClockInit();      // 初始化时钟
    fanPWMInit();           // 初始化风扇PWM
    SetClockPhases(0);      // 设置相数

    HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET);    // ADC片选直接下拉
    HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_SET);      // DAC片选上拉

    ADS1220_init(&hspi2, &ADS1220_default_regs);
    ADS1220_set_conv_mode_continuous(&hspi2, &ADS1220_default_regs);
    ADS1220_set_mode(&hspi2, ADS1220_MODE_Trubo, &ADS1220_default_regs);

    rttShellInit(); // 初始化RTT Shell

    SetVoltageOrCurrent(VOLTAGE,14.0f); // 直接设置输出电压
    SetVoltageOrCurrent(CURRENT,0.1f);  // 直接设置输入电流
    device.DAC_voltage_ref = 15.0f;
    device.DAC_voltage_ref = 0.6f;

    HAL_Delay(100);

    Alpha_State_Ptr = &A0; // 初始化状态机
    A_Task_Ptr = &A1;
    B_Task_Ptr = &B1;

}

void MY_Loop(void)
{
    (*Alpha_State_Ptr)(); // 调用状态机
}

void UVLO_ALL_Close()
{
    HAL_GPIO_WritePin(UVLO1_GPIO_Port, UVLO1_Pin, StandBy);
    HAL_GPIO_WritePin(UVLO2_GPIO_Port, UVLO2_Pin, StandBy);
    HAL_GPIO_WritePin(UVLO3_GPIO_Port, UVLO3_Pin, StandBy);
    HAL_GPIO_WritePin(UVLO4_GPIO_Port, UVLO4_Pin, StandBy);
    HAL_GPIO_WritePin(UVLO5_GPIO_Port, UVLO5_Pin, StandBy);
    HAL_GPIO_WritePin(UVLO6_GPIO_Port, UVLO6_Pin, StandBy);
}

void moduleClockInit()
{
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1); // 使能TIM3_CH1
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2); // 使能TIM3_CH2

    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1); // 使能TIM1_CH1
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2); // 使能TIM1_CH2
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3); // 使能TIM1_CH3
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH4); // 使能TIM1_CH4

    TIM1->CCR1 = 1;
    TIM1->CCR2 = 24+1;
    TIM1->CCR3 = 48+1;
    TIM1->CCR4 = 72+1;
    TIM3->CCR1 = 96+1;
    TIM3->CCR2 = 1;

    LL_TIM_EnableAllOutputs(TIM1);

    LL_TIM_EnableCounter(TIM1);

}

void fanPWMInit()
{
    LL_TIM_CC_EnableChannel(TIM15, LL_TIM_CHANNEL_CH1); // 使能TIM15_CH1
    TIM15->CCR1 = 0;
    LL_TIM_EnableCounter(TIM15);
    LL_TIM_EnableIT_UPDATE(TIM15);//使能TIM15的更新中断，在终端里面运行终端
}

float Slew_Func(float *slewVal, float refVal, float slewRate)
{
    static float diff = 0;
    diff = refVal - *slewVal;
    if (diff >= slewRate) {
        *slewVal += slewRate;
        return (1);
    } else if (-diff >= slewRate) {
        *slewVal -= slewRate;
        return (-1);
    } else {
        *slewVal = refVal;
        return (0);
    }
}

// 通过斜坡函数更新DAC输出
void updateDAC()
{
    if(Slew_Func(&device.DAC_voltage_now, device.DAC_voltage_ref, 0.1f) != 0)
    {
        SetVoltageOrCurrent(VOLTAGE,device.DAC_voltage_now);
    }
    if(Slew_Func(&device.DAC_current_now, device.DAC_current_ref, 0.1f) != 0)
    {
        SetVoltageOrCurrent(CURRENT,device.DAC_current_now);
    }
}

void A0(void)
{
    if(A_Task_Flag)
    {   A_Task_Flag = false;
        (*A_Task_Ptr)();    // 执行A分支任务
    }
    Alpha_State_Ptr = &B0;  // 转换到B分支
}

void B0(void)
{
    if(B_Task_Flag)
    {   B_Task_Flag = false;
        (*B_Task_Ptr)();    // 执行B分支任务
    }
    Alpha_State_Ptr = &A0;  // 转换到A分支
}

void A1(void)
{
    // A分支任务1，读取RTT输入，运行Shell
    char data[128] = {0};
    uint16_t len = 0;
    len = rttShellRead(data, 128);

    for (uint16_t i = 0; i < len; i++) {
        shellHandler(&rttShell, data[i]);
    }

    A_Task_Ptr = &A2;
}

void A2(void)
{
    // A分支任务2，读取ADC数据
    static uint8_t ADC_CHANNEL = 0;
    device.adc_value[ADC_CHANNEL] = ADS1220_read_singleshot_channel(&hspi2, 
                                                                    ADS1220_MUX_AIN0_AVSS + ADC_CHANNEL * 16
                                                                    , &ADS1220_default_regs,
                                                                    ADC_DRDY_GPIO_Port, ADC_DRDY_Pin, 100);
    ADC_CHANNEL++;
    if(ADC_CHANNEL == 4){
        ADC_CHANNEL = 0;
    }
    SEGGER_RTT_printf(1,"%d,%d,%d,%d\n\r",device.adc_value[V_IN],
                                          device.adc_value[V_OUT],
                                          device.adc_value[I_IN],
                                          device.adc_value[I_OUT]);
    A_Task_Ptr = &A1;
}

void B1(void)
{
    // B分支任务1，使用斜坡函数更新DAC输出
    updateDAC();
    B_Task_Ptr = &B1;
}