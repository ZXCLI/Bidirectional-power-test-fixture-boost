/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_i2c.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_dma.h"

#include "stm32g0xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TEST1_Pin GPIO_PIN_13
#define TEST1_GPIO_Port GPIOC
#define TEST2_Pin GPIO_PIN_14
#define TEST2_GPIO_Port GPIOC
#define UVLO1_Pin GPIO_PIN_15
#define UVLO1_GPIO_Port GPIOC
#define TEMP_Pin GPIO_PIN_0
#define TEMP_GPIO_Port GPIOA
#define UVLO2_Pin GPIO_PIN_1
#define UVLO2_GPIO_Port GPIOA
#define UVLO3_Pin GPIO_PIN_2
#define UVLO3_GPIO_Port GPIOA
#define UVLO4_Pin GPIO_PIN_3
#define UVLO4_GPIO_Port GPIOA
#define UVLO5_Pin GPIO_PIN_4
#define UVLO5_GPIO_Port GPIOA
#define DAC_CLK_Pin GPIO_PIN_5
#define DAC_CLK_GPIO_Port GPIOA
#define DAC_CS_Pin GPIO_PIN_6
#define DAC_CS_GPIO_Port GPIOA
#define DAC_MOSI_Pin GPIO_PIN_7
#define DAC_MOSI_GPIO_Port GPIOA
#define UVLO6_Pin GPIO_PIN_0
#define UVLO6_GPIO_Port GPIOB
#define ADC_CS_Pin GPIO_PIN_1
#define ADC_CS_GPIO_Port GPIOB
#define ADC_MISO_Pin GPIO_PIN_2
#define ADC_MISO_GPIO_Port GPIOB
#define ADC_CLK_Pin GPIO_PIN_10
#define ADC_CLK_GPIO_Port GPIOB
#define ADC_MOSI_Pin GPIO_PIN_11
#define ADC_MOSI_GPIO_Port GPIOB
#define ADC_DRDY_Pin GPIO_PIN_12
#define ADC_DRDY_GPIO_Port GPIOB
#define FAN_PWM_Pin GPIO_PIN_15
#define FAN_PWM_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_15
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_0
#define IN2_GPIO_Port GPIOD
#define IN3_Pin GPIO_PIN_1
#define IN3_GPIO_Port GPIOD
#define IN4_Pin GPIO_PIN_2
#define IN4_GPIO_Port GPIOD
#define IN5_Pin GPIO_PIN_3
#define IN5_GPIO_Port GPIOD
#define IN6_Pin GPIO_PIN_3
#define IN6_GPIO_Port GPIOB
#define IN7_Pin GPIO_PIN_4
#define IN7_GPIO_Port GPIOB
#define IN8_Pin GPIO_PIN_5
#define IN8_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
