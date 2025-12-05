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
#include "stm32g0xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define S2_Pin GPIO_PIN_2
#define S2_GPIO_Port GPIOA
#define LM_Pin GPIO_PIN_3
#define LM_GPIO_Port GPIOA
#define LM_RESET_Pin GPIO_PIN_4
#define LM_RESET_GPIO_Port GPIOA
#define FF_PIN_Pin GPIO_PIN_5
#define FF_PIN_GPIO_Port GPIOA
#define FFA_POWER_EN_Pin GPIO_PIN_6
#define FFA_POWER_EN_GPIO_Port GPIOA
#define S5_Pin GPIO_PIN_2
#define S5_GPIO_Port GPIOB
#define SOL1_Pin GPIO_PIN_8
#define SOL1_GPIO_Port GPIOA
#define SOL2_Pin GPIO_PIN_9
#define SOL2_GPIO_Port GPIOA
#define S3_Pin GPIO_PIN_6
#define S3_GPIO_Port GPIOC
#define MOTOR_Pin GPIO_PIN_10
#define MOTOR_GPIO_Port GPIOA
#define S1F_FBW_Pin GPIO_PIN_11
#define S1F_FBW_GPIO_Port GPIOA
#define S9_Pin GPIO_PIN_12
#define S9_GPIO_Port GPIOA
#define S8_Pin GPIO_PIN_15
#define S8_GPIO_Port GPIOA
#define S1T_Pin GPIO_PIN_3
#define S1T_GPIO_Port GPIOB
#define S1F_Pin GPIO_PIN_4
#define S1F_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_8
#define LED1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
