/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

#include "stm32g4xx_ll_crc.h"
#include "stm32g4xx_ll_tim.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_exti.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_dma.h"

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
#define ADC1_Pin LL_GPIO_PIN_0
#define ADC1_GPIO_Port GPIOA
#define ADC2_Pin LL_GPIO_PIN_1
#define ADC2_GPIO_Port GPIOA
#define DAC1_Pin LL_GPIO_PIN_4
#define DAC1_GPIO_Port GPIOA
#define LED1_Pin LL_GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED2_Pin LL_GPIO_PIN_1
#define LED2_GPIO_Port GPIOB
#define LCD_D4_Pin LL_GPIO_PIN_7
#define LCD_D4_GPIO_Port GPIOE
#define LCD_D5_Pin LL_GPIO_PIN_8
#define LCD_D5_GPIO_Port GPIOE
#define LCD_D6_Pin LL_GPIO_PIN_9
#define LCD_D6_GPIO_Port GPIOE
#define LCD_D7_Pin LL_GPIO_PIN_10
#define LCD_D7_GPIO_Port GPIOE
#define LCD_D8_Pin LL_GPIO_PIN_11
#define LCD_D8_GPIO_Port GPIOE
#define LCD_D9_Pin LL_GPIO_PIN_12
#define LCD_D9_GPIO_Port GPIOE
#define LCD_D10_Pin LL_GPIO_PIN_13
#define LCD_D10_GPIO_Port GPIOE
#define LCD_D11_Pin LL_GPIO_PIN_14
#define LCD_D11_GPIO_Port GPIOE
#define LCD_D12_Pin LL_GPIO_PIN_15
#define LCD_D12_GPIO_Port GPIOE
#define FLASH_CS_Pin LL_GPIO_PIN_12
#define FLASH_CS_GPIO_Port GPIOB
#define FLASH_SCK_Pin LL_GPIO_PIN_13
#define FLASH_SCK_GPIO_Port GPIOB
#define FLASH_MISO_Pin LL_GPIO_PIN_14
#define FLASH_MISO_GPIO_Port GPIOB
#define FLASH_MOSI_Pin LL_GPIO_PIN_15
#define FLASH_MOSI_GPIO_Port GPIOB
#define LCD_D13_Pin LL_GPIO_PIN_8
#define LCD_D13_GPIO_Port GPIOD
#define LCD_D14_Pin LL_GPIO_PIN_9
#define LCD_D14_GPIO_Port GPIOD
#define LCD_D15_Pin LL_GPIO_PIN_10
#define LCD_D15_GPIO_Port GPIOD
#define LCD_RS_Pin LL_GPIO_PIN_13
#define LCD_RS_GPIO_Port GPIOD
#define LCD_D0_Pin LL_GPIO_PIN_14
#define LCD_D0_GPIO_Port GPIOD
#define LCD_D1_Pin LL_GPIO_PIN_15
#define LCD_D1_GPIO_Port GPIOD
#define TSCL_Pin LL_GPIO_PIN_6
#define TSCL_GPIO_Port GPIOC
#define TSDA_Pin LL_GPIO_PIN_7
#define TSDA_GPIO_Port GPIOC
#define TPEN_Pin LL_GPIO_PIN_8
#define TPEN_GPIO_Port GPIOC
#define TRST_Pin LL_GPIO_PIN_9
#define TRST_GPIO_Port GPIOC
#define LCD_D2_Pin LL_GPIO_PIN_0
#define LCD_D2_GPIO_Port GPIOD
#define LCD_D3_Pin LL_GPIO_PIN_1
#define LCD_D3_GPIO_Port GPIOD
#define LCD_RD_Pin LL_GPIO_PIN_4
#define LCD_RD_GPIO_Port GPIOD
#define LCD_WR_Pin LL_GPIO_PIN_5
#define LCD_WR_GPIO_Port GPIOD
#define LCD_RST_Pin LL_GPIO_PIN_6
#define LCD_RST_GPIO_Port GPIOD
#define LCD_CS_Pin LL_GPIO_PIN_7
#define LCD_CS_GPIO_Port GPIOD
#define LCD_BL_Pin LL_GPIO_PIN_4
#define LCD_BL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
