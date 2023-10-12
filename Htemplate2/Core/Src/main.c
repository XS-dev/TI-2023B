/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
int length_flag = 0;
int load_flag = 0;
char screen_message[30] = {0};
uint32_t frequency = 25000000;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
ALIGN_32BYTES (uint16_t adc1_data[10000])  __attribute__((section(".ARM.__at_0x30020000")));
ALIGN_32BYTES (uint16_t adc3_data[2000]) 	__attribute__((section(".ARM.__at_0x30000000")));

//��ŵ���Ƶ��100�β����Ľ��
float M_single[100] = {0};
//��ʱ��ţ��������ݵ�����
float comp[100] = {0};
//��ʾadc�ɼ����__at_0x24000000
__IO uint8_t AdcConvEnd = 0;
//ȫ����ʱ���������ڸ���forѭ��
uint32_t x_tmp = 0;
uint32_t y_tmp = 0;
uint32_t z_tmp = 0;
//��־λ����ʾ��ʼ����Ʒֶ���ص�����
uint32_t check_K_flag = 0;

float voltage_load1[16] = {0};
float voltage_load2[16] = {0};
float voltage_load3[16] = {0};
//���ڴ��dac�����ֵ
uint16_t Dat[20]={0};
//////////////////////////////////////////////////////
//���������������ж�����߳��ķ�Χ����о����߳��Ĳ���
//////////////////////////////////////////////////////
//���������������λ�����ʼƵ��
float M10_fre_start = 6500000;//7M
float M14_fre_start = 4500000;//7.5M
float M18_fre_start = 3500000;//6M

//��ų�ʼ�������в����õ����ĸ������µ�ֵ
float M10_START[10] = {138,137,136,135,134,133,132,131,130,129};
float M14_START[10] = {139,139,138,138,137,137,136,136,136,135};
float M18_START[10] = {138,138,137,136,136,135,134,134,133,132};
float M10_END[10] = {45,44,43,41,40,39,38,37,35,34};
float M14_END[10] = {72,70,68,67,65,63,61,58,57,55};
float M18_END[10] = {110,108,104,103,101,99,97,95,93,91};
//�������������������ݵ�б��
float M10_K[10] = {0.232500,0.232500, 0.232500, 0.232500, 0.235000, 0.235000, 0.235000, 0.235000, 0.237500, 0.235000};
float M14_K[10] = {0.165, 0.1675, 0.172500, 0.175000, 0.177500, 0.180000, 0.185000, 0.187500, 0.190000, 0.192500};
float M18_K[10] = {0.14, 0.145000, 0.160000, 0.165000, 0.170000, 0.175000, 0.185000, 0.190000, 0.195000, 0.205000};

//��Ų����õ���10����λֵ
float MUnknown[10] = {0};

//��Ų����õ����ߵĳ���
float Mlength = 0;

////////////////////////////////////////////////////////////
//�������������ж��߳������ĸ���Χ�ѽ��к����Ľ�Ϊ׼ȷ�Ĳ���
////////////////////////////////////////////////////////////
//�����1M-2M������10�������λֵ
float M1_length[10] = {0};
//1M-2M���������ݵĶ�ӦƵ��ֵ
float M1_fre[10] = {0};
//�����1M-2M��Χ�ھ���У׼��õ���б��ֵ����ֵ���Ǻ����жϵĻ���
float M1_1M  = 1;
float M1_10M = 54.908886;
float M1_14M = 83.030113;
float M1_18M = 112.363220;
float M1_20M = 135.332840;
//�����1M-2M��Χ�����յõ���б��
float M1_Unknown = 0;
//��־λ�����ڱ�ʶ���ڵõ��Ĳ�����������ĸ�������Χ
uint32_t scale_flag = 0;



void adc_saopin(void)
{
	MX_ADC1_Init();	//��ʼ�����÷�����, ȷ����MX_DMA_Init()��ʼ������    
	Delay_Us(100);	//�еط�˵������Եȵȵ�ѹ�ȶ�����У׼

	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADCEx_Calibration_Start\r\n");
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_data, 16) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADC_Start_DMA\r\n");
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc1);
}
void SineWave()
{
    uint16_t i;
    for( i=0;i<20;i++)
    {
  		Dat[i]=2048;
    }
}


float get1256(void)
{
	float get_data = 0;
	get_data = ADS1256GetVolt(ADS1256_MUXP_AIN0,ADS1256_MUXN_AINCOM,ADS1256_GAIN_1);
	return get_data;
}


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//ϵͳ��ʼ������
//	{
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	uint32_t saopin = 0;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM7_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_ADC3_Init();
  MX_TIM15_Init();
  MX_TIM1_Init();
  MX_UART7_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_DAC1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);  //����PWMͨ��1
	SineWave();//������������
	HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_2,(uint32_t *)Dat,20,DAC_ALIGN_12B_R);//����DMA-DAC
	HAL_TIM_Base_Start(&htim6);//�򿪶�ʱ��6
	double data = 0;
	Init_ad9910();
	Write_Amplitude(100);
	Freq_convert(20000000);
	float adata = 0;
	ADF4351Init();
	ADF4351WriteFreq(200);	//���ù̶����Ƶ��Ϊ400MHZ
	ADS1256_Init(ADS1256_GAIN_1,ADS1256_DRATE_2000SPS);
	workstate(0);
	while(1)
	{
		if(length_flag == 1)
		{
			workstate(1);
			length_flag = 0;
			M1_Unknown = length_scope();
			scope_judge();	
			cal_length();
			printf("%d\r\n",scale_flag);
			show_length(Mlength);
			workstate(0);
		}
		if(load_flag == 1)
		{
			workstate(1);
			load_flag = 0;

			workstate(0);
		}
		if(check_K_flag == 10)
		{
			workstate(1);
			M1_10M = length_scope();
			calibration_show(M1_10M);
//			printf("%f\r\n\r\n",M1_10M);			
			get_K();
			workstate(0);
			check_K_flag = 0;
		}
		else if(check_K_flag == 14)
		{
			workstate(1);
			M1_14M = length_scope();
//			printf("%f\r\n\r\n",M1_14M);
			calibration_show(M1_14M);
			get_K();
			workstate(0);
			check_K_flag = 0;
		}
		else if(check_K_flag == 18)
		{
			workstate(1);
			M1_18M = length_scope();
//			printf("%f\r\n\r\n",M1_18M);
			calibration_show(M1_18M);			
			get_K();
			workstate(0);
			check_K_flag = 0;
		}
		else if(check_K_flag == 20)
		{
			workstate(1);
			M1_20M = length_scope();
//			printf("%f\r\n\r\n",M1_20M);
			calibration_show(M1_20M);						
			get_K();
			workstate(0);
			check_K_flag = 0;
		}
		else if(check_K_flag == 101)
		{
			workstate(1);
			M1_1M = length_scope();
//			printf("%f\r\n\r\n",M1_1M);
			calibration_show(M1_1M);	
			workstate(0);
			check_K_flag = 0;
		}			
		else if(check_K_flag == 24)
		{
			workstate(1);
			printf("\r\n");
			get_K();
			workstate(0);
			check_K_flag = 0;
			for(x_tmp = 0;x_tmp<10;x_tmp++)
			{
				printf("M10_K= %f\r\n\r\n",M10_K[x_tmp]);
			}
			for(x_tmp = 0;x_tmp<10;x_tmp++)
			{
				printf("M14_K= %f\r\n\r\n",M14_K[x_tmp]);
			}			
			for(x_tmp = 0;x_tmp<10;x_tmp++)
			{
				printf("M18_K= %f\r\n\r\n",M18_K[x_tmp]);
			}			
		}				
	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	adc_saopin();
	for (u16 temp = 0; temp< 16;temp++)
	{
		adata = (float)(adc1_data[temp])/65536*3300;
		printf("%f\r\n", adata);
	}	

	
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 25;
  PeriphClkInitStruct.PLL2.PLL2N = 360;
  PeriphClkInitStruct.PLL2.PLL2P = 5;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   if(hadc->Instance == ADC1) 
	{
      //SCB_InvalidateDCache_by_Addr((uint32_t *) &adc1_data[0], ADC1_BUFFER_SIZE);
   }
	 AdcConvEnd = 1;
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30020000;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
