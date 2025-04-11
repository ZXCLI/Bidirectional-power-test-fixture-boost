#include "shell.h"
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

// void BOOST_clock(uint16_t)
// {

// }