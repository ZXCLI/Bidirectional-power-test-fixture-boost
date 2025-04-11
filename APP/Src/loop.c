#include "loop.h"
#include "shell_port.h"
#include "stdio.h"

char rttLogWriteBuffer[5120];
char rttLogReadBuffer[5120];

ADS1220_regs ADS1220_default_regs = {
    .cfg_reg0 = ADS1220_PGA_GAIN_1,         // 1x增益
    .cfg_reg1 = ADS1220_DR_20SPS | _BV(2),  // 采样率20 SPS
    .cfg_reg2 = (0x01 << 6) | (0x01 << 4),  // 外部参考电压，FIR为50Hz
    .cfg_reg3 = 0x00                        // 关闭IDAC
};

void MY_Init(void)
{
    SEGGER_RTT_Init();
    //配置通道一的缓冲区
    SEGGER_RTT_ConfigUpBuffer(1, "Log", rttLogWriteBuffer, 512, SEGGER_RTT_MODE_NO_BLOCK_SKIP); // 配置写缓冲
    SEGGER_RTT_ConfigDownBuffer(1, "Log", rttLogReadBuffer, 512, SEGGER_RTT_MODE_NO_BLOCK_SKIP); // 配置读缓冲

    UVLO_ALL_Close();
    boostClockInit();
    fanPWMInit();

    HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET);    // ADC片选直接下拉
    HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_SET);      // DAC片选上拉

    ADS1220_init(&hspi2, &ADS1220_default_regs);
    ADS1220_select_mux_config(&hspi2, ADS1220_MUX_AIN2_AVSS, &ADS1220_default_regs);

    rttShellInit(); // 初始化RTT Shell

    DAC8552_WriteA(&hspi1, 0.51724f);
    DAC8552_WriteB(&hspi1, 0.51724f);

    HAL_Delay(100);

    HAL_GPIO_WritePin(UVLO1_GPIO_Port, UVLO1_Pin, RunNormal);
    HAL_GPIO_WritePin(UVLO2_GPIO_Port, UVLO2_Pin, RunNormal);
    HAL_GPIO_WritePin(UVLO3_GPIO_Port, UVLO3_Pin, RunNormal);
    HAL_GPIO_WritePin(UVLO4_GPIO_Port, UVLO4_Pin, RunNormal);
    HAL_GPIO_WritePin(UVLO5_GPIO_Port, UVLO5_Pin, RunNormal);
    HAL_GPIO_WritePin(UVLO6_GPIO_Port, UVLO6_Pin, RunNormal);
}

void MY_Loop(void)
{
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    int32_t adc_value = ADS1220_read_singleshot(&hspi2, ADC_DRDY_GPIO_Port, ADC_DRDY_Pin, 100);
    
    SEGGER_RTT_printf(1, "adc_value = %d\n\r", (int)(100000.0f*DAC8552_Vref*(float)(adc_value)/(float)(1<<23)));
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

void boostClockInit()
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