/**
 * @file shell_port.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

/*
    加在.rodata :
 *  _shell_command_start = .;
    KEEP (*(shellCommand))
    _shell_command_end = .;
 */

#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__


#include "shell.h"
#include <stdbool.h>

extern Shell rttShell;

void rttShellInit(void);
short rttShellRead(char *data, unsigned short len);
short rttShellWrite(char *data, unsigned short len);
#endif
