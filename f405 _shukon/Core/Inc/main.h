/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define SC_PSTOP_Pin GPIO_PIN_0
#define SC_PSTOP_GPIO_Port GPIOC
#define SC_CE_Pin GPIO_PIN_1
#define SC_CE_GPIO_Port GPIOC
#define NTC_ADC_Pin GPIO_PIN_2
#define NTC_ADC_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_4
#define LCD_RST_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_5
#define LCD_DC_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOB
#define LCD_RSTB2_Pin GPIO_PIN_2
#define LCD_RSTB2_GPIO_Port GPIOB
#define RELEASE_Pin GPIO_PIN_13
#define RELEASE_GPIO_Port GPIOB
#define K1_Pin GPIO_PIN_15
#define K1_GPIO_Port GPIOB
#define K2_Pin GPIO_PIN_6
#define K2_GPIO_Port GPIOC
#define K3_Pin GPIO_PIN_7
#define K3_GPIO_Port GPIOC
#define K4_Pin GPIO_PIN_8
#define K4_GPIO_Port GPIOC
#define SC_INT_Pin GPIO_PIN_9
#define SC_INT_GPIO_Port GPIOA
#define LCD_DCA10_Pin GPIO_PIN_10
#define LCD_DCA10_GPIO_Port GPIOA
#define KEY_LEFT_Pin GPIO_PIN_11
#define KEY_LEFT_GPIO_Port GPIOC
#define KEY_DOWN_Pin GPIO_PIN_12
#define KEY_DOWN_GPIO_Port GPIOC
#define KEY_RIGHT_Pin GPIO_PIN_2
#define KEY_RIGHT_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
