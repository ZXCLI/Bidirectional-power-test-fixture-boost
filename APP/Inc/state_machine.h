#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "main.h"
#include "stdbool.h"

extern bool A_Task_Flag; // A分支任务标志
extern bool B_Task_Flag; // B分支任务标志

extern void (*Alpha_State_Ptr)(void); // 基态状态机指针
extern void (*A_Task_Ptr)(void);      // A分支任务指针
extern void (*B_Task_Ptr)(void);      // B分支任务指针

void A0(void);
void A1(void);
void A2(void);

void B0(void);
void B1(void);
void B2(void);

#endif /* STATE_MACHINE_H */
