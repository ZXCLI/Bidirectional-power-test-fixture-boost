#include "state_machine.h"
#include "loop.h"
#include "least_squares.h"
#include "shell_port.h"
#include "data_management.h"

bool A_Task_Flag = false;      // A分支任务标志
bool B_Task_Flag = false;      // B分支任务标志

void (*Alpha_State_Ptr)(void); // 基态状态机指针
void (*A_Task_Ptr)(void);      // A分支任务指针
void (*B_Task_Ptr)(void);      // B分支任务指针

void A0(void)       // A分支1KHz
{
    if(A_Task_Flag)
    {   A_Task_Flag = false;
        DEBUG2_IN;
        (*A_Task_Ptr)();    // 执行A分支任务
        DEBUG2_OUT;
    }
    Alpha_State_Ptr = &B0;  // 转换到B分支
}

void B0(void)       // B分支66Hz
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
    // A分支任务2，使用斜坡函数更新DAC输出
    limiteCurrent();
    updateDAC();
    A_Task_Ptr = &A1;
}

void B1(void)
{
    // B分支任务1，读取ADC数据

    static uint8_t ADC_CHANNEL = 0; 
    // 使用O1和Og优化时，下面的代码在读取ADC数据的时候会跑飞，很奇怪  
    
    ADC_CHANNEL++;
    if (ADC_CHANNEL == 4) {ADC_CHANNEL = 0;}
    
    uint8_t Forward_data_index = (ADC_CHANNEL + 3) % 4; // 读取上一轮触发的ADC数据
    device.adc_value[Forward_data_index] = ADS1220_read_blocking(&hspi2,
                                                                 ADC_DRDY_GPIO_Port,
                                                                 ADC_DRDY_Pin, 100);

    ADS1220_select_mux_config(&hspi2, ADS1220_MUX_AIN0_AVSS + ADC_CHANNEL * 16,
                              &ADS1220_default_regs);
    ADS1220_start_conversion(&hspi2);
    // 触发本轮的ADC转换，经过状态机切换的延时，ADC转换完成，在下一轮开头读取数据

    device.V_OUT = device.adc_value[V_OUT] * device.ADCdataConver[V_OUT].a1 
                    + device.ADCdataConver[V_OUT].a0;
    device.I_OUT = device.adc_value[I_OUT] * device.ADCdataConver[I_OUT].a1 
                    + device.ADCdataConver[I_OUT].a0;
    device.V_IN = device.adc_value[V_IN] * device.ADCdataConver[V_IN].a1 
                    + device.ADCdataConver[V_IN].a0;
    device.I_IN = device.adc_value[I_IN] * device.ADCdataConver[I_IN].a1
                    + device.ADCdataConver[I_IN].a0;
    
    SEGGER_RTT_printf(1, "%d,%d,%d,%d\n\r", device.adc_value[V_IN],
                                            device.adc_value[V_OUT],
                                            device.adc_value[I_IN],
                                            device.adc_value[I_OUT]);
    
    // Toggle the LED
    static uint16_t Vtimer_B1 = 0;
    Vtimer_B1++;
    if (Vtimer_B1 >= device.Vtimer_B_ARR) {
        Vtimer_B1 = 0;
        HAL_GPIO_TogglePin(TEST1_GPIO_Port, TEST1_Pin);
    }

    eepromWrite(0x00,NULL,0x00,0x00);   // 异步按页写入EEPROM

    B_Task_Ptr = &B1;
}
