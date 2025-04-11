/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell.h"
#include "shell_port.h"
#include "SEGGER_RTT.h"

Shell rttShell;
char rttShellBuffer[512];

/**
 * @brief rtt shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长度
 */
short rttShellWrite(char *data, unsigned short len)
{
    static int blocked = 0;
    unsigned int time  = HAL_GetTick();
    short avail        = 0;
    short write        = 0;
    short wrote        = 0;
    do {
        avail = SEGGER_RTT_GetAvailWriteSpace(0);
        if (avail <= 0) {
            if (blocked > 10) {
                return 0;
            }
            HAL_Delay(1);
            blocked++;
        } else {
            blocked = 0;
            write   = avail >= len ? len : avail;
            SEGGER_RTT_Write(0, data, write);
            data += write;
            len -= write;
            wrote += write;
        }
    } while (len > 0 && HAL_GetTick() - time < 10);
    return wrote;
}

/**
 * @brief rtt shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取的数据长度
 */
short rttShellRead(char *data, unsigned short len)
{
    return SEGGER_RTT_Read(0, data, len);
}

/**
 * @brief 用户shell初始化
 *
 */
void rttShellInit(void)
{
    rttShell.write = rttShellWrite;
    rttShell.read  = rttShellRead;
    shellInit(&rttShell, rttShellBuffer, 512);
}
