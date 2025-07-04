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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VIDEO_P_Pin GPIO_PIN_4
#define VIDEO_P_GPIO_Port GPIOA
#define LASER_PWR_Pin GPIO_PIN_6
#define LASER_PWR_GPIO_Port GPIOA
#define LASER_ON_Pin GPIO_PIN_7
#define LASER_ON_GPIO_Port GPIOA
#define DATA_RDY_Pin GPIO_PIN_0
#define DATA_RDY_GPIO_Port GPIOB
#define SHUTTER_Pin GPIO_PIN_1
#define SHUTTER_GPIO_Port GPIOB
#define READ_Pin GPIO_PIN_2
#define READ_GPIO_Port GPIOB
#define CLR_PIX_Pin GPIO_PIN_13
#define CLR_PIX_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_10
#define LED3_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
