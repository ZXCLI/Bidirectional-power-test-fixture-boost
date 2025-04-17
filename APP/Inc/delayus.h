/* 微秒延时模块头文件 (delay.h) */
#pragma once
#include <stdint.h>

// 平台适配接口（需用户根据硬件实现）
uint32_t get_tick(void);    // 获取当前毫秒计数（对接HAL_GetTick等）
void delay_ms(uint32_t ms); // 毫秒延时（对接HAL_Delay等）

// 初始化函数
void delay_init(void); // 模块初始化

// 延时函数
void delay_us(uint32_t us);      // 混合延时（避免在中断中使用）
void delay_us_busy(uint32_t us); // 纯忙等待（中断安全）

/* 微秒延时模块实现 (delay.c) */
#include <math.h>

/******************** 核心变量 ********************/
static volatile uint32_t _delay_base = 0; // 每微秒所需循环次数

/******************** 防优化宏 ********************/
#define DELAY_NOP() __asm__ volatile("nop")
