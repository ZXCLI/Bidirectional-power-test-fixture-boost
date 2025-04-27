#include "data_management.h"

#define QUEUE_SIZE 16

/*
 * 储存结构：
 *    DAC_CONVER             ADC_CONVER         其余数据
 *        |                      |
 *  VIN VOUT IIN IOUT     VIN VOUT IIN IOUT
 *   |
 *   |__a0 a1
 */

// 异步写入EEPROM，每次最多写一页，使用环形队列管理多个任务
void eepromWrite(uint16_t len, uint8_t *data, uint8_t PageAddr, uint8_t ByteAddr)
{
    static dataQueue queue[QUEUE_SIZE]; // 循环队列
    static uint8_t head  = 0;           // 队列头部索引
    static uint8_t tail  = 0;           // 队列尾部索引
    static uint8_t count = 0;           // 当前任务数

    if (len == 0) {
        // 处理队列中的任务（len=0表示进行写入操作）
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
        for (int i = 0; i < len; i++) {
            new_task->data[i] = data[i];
        }

        tail = (tail + 1) % QUEUE_SIZE; // 更新尾部索引
        count++;
    }
}
