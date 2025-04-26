#include "loop.h"
#include "string.h"
#include "shell_port.h"
#include "stdio.h"
#include "cmd.h"
#include "delayus.h" 
#include "eeprom.h"
#include "least_squares.h"
#include "state_machine.h"

#define QUEUE_SIZE 16

Device device;

char rttLogWriteBuffer[512];
char rttLogReadBuffer[512];

ADS1220_regs ADS1220_default_regs = {
    .cfg_reg0 = 0x00,                       // 关闭PGA
    .cfg_reg1 = ADS1220_DR_90SPS | _BV(2),  // 采样率20 SPS，连续模式
    .cfg_reg2 = (0x01 << 6) | (0x01 << 4),  // 外部参考电压，FIR为50Hz
    .cfg_reg3 = 0x00                        // 关闭IDAC
};



void MY_Init(void)
{

    delay_init();

    SEGGER_RTT_Init();
    //配置通道一的缓冲区
    SEGGER_RTT_ConfigUpBuffer(1, "Log", rttLogWriteBuffer, 
                            512, SEGGER_RTT_MODE_NO_BLOCK_SKIP); // 配置写缓冲
    SEGGER_RTT_ConfigDownBuffer(1, "Log", rttLogReadBuffer, 
                            512, SEGGER_RTT_MODE_NO_BLOCK_SKIP); // 配置读缓冲

    UVLO_ALL_Close();       // 关闭所有模块
    moduleClockInit();      // 初始化时钟

    fanPWMInit();           // 初始化风扇PWM
    SetClockPhases(0);      // 设置相数
    device.phase = 0;       // 保存相数

    HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET);    // ADC片选直接下拉
    HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_SET);      // DAC片选上拉

    ADS1220_init(&hspi2, &ADS1220_default_regs);
    ADS1220_set_conv_mode_continuous(&hspi2, &ADS1220_default_regs);
    ADS1220_set_mode(&hspi2, ADS1220_MODE_Trubo, &ADS1220_default_regs);

    rttShellInit(); // 初始化RTT Shell

    device.DACdataConver[V_OUT].a1 = 902.119202f;
    device.DACdataConver[V_OUT].a0 = -143.923431f;
    device.DACdataConver[I_IN].a1  = 485.702087f;
    device.DACdataConver[I_IN].a0  = 3586.960938f;

    device.ADCdataConver[V_OUT].a1 = 0.008689f/1000.0f;
    device.ADCdataConver[V_OUT].a0 = 11.676025f/1000.0f;

    // SetVoltageOrCurrent(VOLTAGE,14.0f); // 直接设置输出电压
    // SetVoltageOrCurrent(CURRENT,0.1f);  // 直接设置输入电流
    // device.DAC_voltage_ref = 15.0f;
    // device.DAC_current_ref = 0.6f;

    DAC8552_WriteA(&hspi1,0.512f);  // 15.0V
    DAC8552_WriteB(&hspi1,0.14f); // 0.5A

    SetVoltageOrCurrent(VOLTAGE,15.0f); // 直接设置输出电压
    SetVoltageOrCurrent(CURRENT,1.0f);  // 直接设置输入电流

    eepromWrite(0x00,"",0x00,0x00);

    HAL_Delay(100);

    Alpha_State_Ptr = &A0; // 初始化状态机
    A_Task_Ptr = &A1;
    B_Task_Ptr = &B1;
    device.Vtimer_B_CCR = 10;   // 初始化虚拟定时器B的CCR

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
    LL_TIM_CC_EnableChannel(TIM15, LL_TIM_CHANNEL_CH2); // 使能TIM15_CH2，1KHz
    LL_TIM_EnableCounter(TIM15);
    LL_TIM_EnableIT_UPDATE(TIM15);  // 使能TIM15的更新中断，在中断里面对任务B进行调度
    LL_TIM_EnableAllOutputs(TIM15);
    TIM15->CCR2 = 0;
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
    Slew_Func(&device.DAC_voltage_now, device.DAC_voltage_ref, 0.05f);
    SetVoltageOrCurrent(VOLTAGE,device.DAC_voltage_now);
    
    Slew_Func(&device.DAC_current_now, device.DAC_current_ref, 0.05f);
    SetVoltageOrCurrent(CURRENT,device.DAC_current_now);
    
}

int limiteCurrent(void)
{
    if(device.phase != 0)
    {
        if (device.DAC_current_ref > device.phase * 20.0f) {
            device.DAC_current_ref = device.phase * 20.0f;
            return 1;
        }
    }// 限制每相最高输入电流为20A
    return 0;
}

// // 异步写入EEPROM，每次最多写一页
// void eepromWrite(uint16_t len, uint8_t *data, uint8_t PageAddr, uint8_t ByteAddr)
// {
//     static dataQueue data_queue[16] = {0};  // 队列,最长支持16个任务
//     static uint8_t len_queue = 0;           // 队列长度

//     if(len_queue == 0){return;}     // 队列为空，直接返回

//     if(len == 0)// 按页写效率最高
//     {
//         if((data_queue[len_queue].len + data_queue[len_queue].ByteAddr) 
//             > ONE_PAGE_BYTE) // 如果当前任务中的数据长度加上当前地址大于一页，则分页写
//         {
//             at24_PageWrite(&hi2c1,data_queue[len_queue].PageAddr,
//                            data_queue[len_queue].ByteAddr,
//                            ONE_PAGE_BYTE - data_queue[len_queue].ByteAddr,
//                            data_queue[len_queue].data);
//             data_queue[len_queue].len -= (ONE_PAGE_BYTE - data_queue[len_queue].ByteAddr);
//             data_queue[len_queue].ByteAddr = 0;
//             data_queue[len_queue].PageAddr ++;
//         }else{// 最后一页或者数据长度小于等于一页
//             at24_PageWrite(&hi2c1,data_queue[len_queue].PageAddr,
//                            data_queue[len_queue].ByteAddr,
//                            data_queue[len_queue].len,
//                            data_queue[len_queue].data);
            
//             data_queue[len_queue].len -= (data_queue[len_queue].len <= ONE_PAGE_BYTE) \
//                                          ? data_queue[len_queue].len : ONE_PAGE_BYTE ;

//             if(data_queue[len_queue].len == 0)
//             {
//                 len_queue --;   // 任务完成，从队列中删除
//             }
//         }
//     }else{// 往队列中添加任务
//         len_queue ++;
//         data_queue[len_queue].len = len;
//         data_queue[len_queue].PageAddr = PageAddr;
//         data_queue[len_queue].ByteAddr = ByteAddr;
//         for(int i = 0; i < len; i++)
//         {
//             data_queue[len_queue].data[i] = data[i];
//         }
//     }    
// }

// 异步写入EEPROM，每次最多写一页
void eepromWrite(uint16_t len, uint8_t *data, uint8_t PageAddr, uint8_t ByteAddr)
{
    static dataQueue queue[QUEUE_SIZE]; // 循环队列
    static uint8_t head  = 0;           // 队列头部索引
    static uint8_t tail  = 0;           // 队列尾部索引
    static uint8_t count = 0;           // 当前任务数

    if (len == 0) {
        // 处理队列中的任务（len=0表示触发写入）
        if (count == 0) return; // 队列为空，直接返回

        dataQueue *current = &queue[head];
        uint16_t remaining = current->len - current->offset; // 剩余待写入数据长度

        // 计算本次写入长度（不超过页边界）
        uint16_t write_len;
        if ((current->ByteAddr + remaining) > ONE_PAGE_BYTE) {
            write_len = ONE_PAGE_BYTE - current->ByteAddr;
        } else {
            write_len = remaining;
        }

        // 执行页写入
        at24_PageWrite(
            &hi2c1,
            current->PageAddr,
            current->ByteAddr,
            write_len,
            current->data + current->offset // 直接定位到未写入的数据位置
        );

        // 更新任务状态
        current->offset += write_len;
        current->ByteAddr += write_len;

        // 检查是否需要翻页
        if (current->ByteAddr >= ONE_PAGE_BYTE) {
            current->PageAddr++;
            current->ByteAddr = 0;
        }

        // 如果任务完成，移除队列头部
        if (current->offset >= current->len) {
            head = (head + 1) % QUEUE_SIZE;
            count--;
        }
    } else {
        // 添加新任务到队列（len>0）
        if (count >= QUEUE_SIZE) return; // 队列已满

        dataQueue *new_task = &queue[tail];
        new_task->len       = len;
        new_task->offset    = 0; // 初始偏移量为0
        new_task->PageAddr  = PageAddr;
        new_task->ByteAddr  = ByteAddr;
        //new_task->data      = data; // 直接引用用户数据指针，无需拷贝

        tail = (tail + 1) % QUEUE_SIZE;
        count++;
    }
}
