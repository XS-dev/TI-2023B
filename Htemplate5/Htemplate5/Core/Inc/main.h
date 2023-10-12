/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//extern uint32_t FFT_LENGTH; 
#define FFT_LENGTH 100
#define ADC_BUFFER_SIZE FFT_LENGTH


#define Relay1_H 	HAL_GPIO_WritePin(relay_1_GPIO_Port,	relay_1_Pin,	GPIO_PIN_SET);
#define Relay2_H 	HAL_GPIO_WritePin(relay_2_GPIO_Port,	relay_2_Pin,	GPIO_PIN_SET);

#define Relay1_L	HAL_GPIO_WritePin(relay_1_GPIO_Port,	relay_1_Pin,	GPIO_PIN_RESET);
#define Relay2_L	HAL_GPIO_WritePin(relay_2_GPIO_Port,	relay_2_Pin,	GPIO_PIN_RESET);

#define E2_H 	HAL_GPIO_WritePin(GPIOE,	relay_1_Pin,	GPIO_PIN_SET);
#define E3_H 	HAL_GPIO_WritePin(GPIOE,	relay_2_Pin,	GPIO_PIN_SET);
#define E4_H 	HAL_GPIO_WritePin(GPIOE,	GPIO_PIN_4,		GPIO_PIN_SET);
#define E5_H 	HAL_GPIO_WritePin(GPIOE,	GPIO_PIN_5,		GPIO_PIN_SET);
#define E6_H 	HAL_GPIO_WritePin(GPIOE,	GPIO_PIN_6,		GPIO_PIN_SET);
#define C13_H HAL_GPIO_WritePin(GPIOC,	GPIO_PIN_13,	GPIO_PIN_SET);

#define E2_L 	HAL_GPIO_WritePin(GPIOE,	relay_1_Pin,	GPIO_PIN_RESET);
#define E3_L 	HAL_GPIO_WritePin(GPIOE,	relay_2_Pin,	GPIO_PIN_RESET);
#define E4_L 	HAL_GPIO_WritePin(GPIOE,	GPIO_PIN_4,		GPIO_PIN_RESET);
#define E5_L 	HAL_GPIO_WritePin(GPIOE,	GPIO_PIN_5,		GPIO_PIN_RESET);
#define E6_L 	HAL_GPIO_WritePin(GPIOE,	GPIO_PIN_6,		GPIO_PIN_RESET);
#define C13_L HAL_GPIO_WritePin(GPIOC,	GPIO_PIN_13,	GPIO_PIN_RESET);



float get1256(void);

extern int length_flag;
extern int load_flag;

extern ALIGN_32BYTES (uint16_t adc1_data[1000]) 	__attribute__((section(".ARM.__at_0x30020000")));
extern ALIGN_32BYTES (uint16_t adc3_data[1000]) 	__attribute__((section(".ARM.__at_0x30000000")));

extern float M_single[100];
extern float comp[100];
extern char  screen_message[30];

extern uint32_t x_tmp;
extern uint32_t y_tmp;
extern uint32_t z_tmp;

extern uint32_t check_K_flag;

extern float M5_START[10] ;		
extern float CM250_START[10] ;
extern float CM100_START[10] ;
extern float CM50_START[10] ;	
extern float CM20_START[10] ;	
extern float CM2_START[10] 	;

extern float M5_END[10] 	 ;	
extern float CM250_END[10] ;
extern float CM100_END[10] ;
extern float CM50_END[10]  ;
extern float CM20_END[10]  ;
extern float CM2_END[10] 	 ;


extern float M10_START[10];
extern float M14_START[10];
extern float M18_START[10];
extern float M10_END[10];
extern float M14_END[10];
extern float M18_END[10];

extern float M10_K[10];
extern float M14_K[10];
extern float M18_K[10];

extern float M5_K[10] ;		
extern float CM250_K[10] ;
extern float CM100_K[10] ;
extern float CM50_K[10]	;
extern float CM20_K[10] ;
extern float CM2_K[10] 	;

extern float M10_fre_start;
extern float M14_fre_start;
extern float M18_fre_start;

extern float M5_fre_start ;		
extern float CM250_fre_start; 
extern float CM100_fre_start ;
extern float CM50_fre_start ;
extern float CM20_fre_start ;	
extern float CM2_fre_start;

extern float MUnknown[10];
extern float Mlength;

extern float M2_CM2; 	
extern float M2_CM20 ;
extern float M2_CM50	;
extern float M2_CM100;
extern float M2_CM250;
extern float M2_M5 	;

extern float M1_5M ;
extern float M1_10M;
extern float M1_14M;
extern float M1_18M;
extern float M1_20M;
extern float M1_length[10]; 
extern float M1_fre[10];
extern float M1_Unknown;
extern float M2_Unknown;




extern uint32_t scale_flag;


extern float Res_data;
extern float Cap_data;
extern uint32_t Load_type;
extern float calib1256;
extern float Res_low ;	
extern float Res_high; 	
extern uint32_t Res_calib_flag;

extern float Res_10M;
extern float Res_20M;
extern float Res_10M_K;
extern float Res_20M_K;

extern float Res_length;

extern float Cap_length;
extern float Cap_data ;
extern float Cap_ref;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

#include <stdio.h>
#include "math.h"
#include "stdlib.h"	
#include <stdbool.h>

#include "arm_math.h"
#include "arm_const_structs.h"
#include "ADF4351.h"
#include "ADS1256.h"

#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "AD9910.h"


#include "Measure.h"
#include "display.h"
#include "error_detect.h"


typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;  
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
void adc_saopin(void);
void adc3_init(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define relay_1_Pin GPIO_PIN_2
#define relay_1_GPIO_Port GPIOE
#define relay_2_Pin GPIO_PIN_3
#define relay_2_GPIO_Port GPIOE
#define ADF4351_CE_Pin GPIO_PIN_4
#define ADF4351_CE_GPIO_Port GPIOD
#define ADF4351_LE_Pin GPIO_PIN_3
#define ADF4351_LE_GPIO_Port GPIOB
#define ADF4351_DAT_Pin GPIO_PIN_7
#define ADF4351_DAT_GPIO_Port GPIOB
#define ADF4351_CLK_Pin GPIO_PIN_1
#define ADF4351_CLK_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
