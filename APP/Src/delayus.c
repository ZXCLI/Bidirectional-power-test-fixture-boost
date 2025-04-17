#include "main.h"
#include "delayus.h"

void calibrate_once(void)
{
    volatile uint32_t counter  = 0;
    const uint32_t target_tick = get_tick() + 1;

    // 等待进入精确的毫秒周期
    while (get_tick() != target_tick) { DELAY_NOP(); }

    // 统计1ms内循环次数
    const uint32_t end_tick = target_tick + 1;
    while (get_tick() != end_tick) {
        counter++;
        DELAY_NOP(); // 防止优化
    }

    _delay_base = counter / 1000;
}

void calibrate_delay(uint8_t cycles)
{
    if (cycles == 0) cycles = 1;

    uint32_t total = 0;
    for (uint8_t i = 0; i < cycles; i++) {
        calibrate_once();
        total += _delay_base;
        delay_ms(10); // 校准间隔
    }
    _delay_base = total / cycles;
}

void delay_init(void)
{
    calibrate_delay(5); // 默认5次校准
}

void delay_us_busy(uint32_t us)
{
    volatile uint32_t cycles = us * _delay_base;
    volatile uint32_t i      = 0;

    while (i < cycles) {
        i++;
        DELAY_NOP(); // 关键防优化点
    }
}

void delay_us(uint32_t us)
{
    const uint32_t ms_part = us / 1000;
    const uint32_t us_part = us % 1000;

    if (ms_part > 0) {
        delay_ms(ms_part);
    }

    if (us_part > 0) {
        delay_us_busy(us_part);
    }
}

// 平台适配实现
uint32_t get_tick(void)
{
    return HAL_GetTick();
}
void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
