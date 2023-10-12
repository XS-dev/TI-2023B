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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
int length_flag = 0;
int load_flag = 0;
char screen_message[30 ] = {0};
uint32_t frequency = 25000000;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
ALIGN_32BYTES (uint16_t adc1_data[1000])  __attribute__((section(".ARM.__at_0x30020000")));
ALIGN_32BYTES (uint16_t adc3_data[1000]) 	__attribute__((section(".ARM.__at_0x30000000")));

//存放单个频点100次测量的结果
float M_single[100] = {0};
//临时存放，用于数据的排序
float comp[100] = {0};
//表示adc采集完成__at_0x24000000
__IO uint8_t AdcConvEnd = 0;
//全局临时变量，用于各种for循环
uint32_t x_tmp = 0;
uint32_t y_tmp = 0;
uint32_t z_tmp = 0;
//标志位，表示初始化设计分段相关的数据
uint32_t check_K_flag = 0;

//用于存放dac输出的值
uint16_t Dat[20]={2084};
//////////////////////////////////////////////////////
//以下数据用于在判断完成线长的范围后进行具体线长的测量
//////////////////////////////////////////////////////
//三个测量区间的相位检测起始频率
float M10_fre_start = 6000000;//6M
float M14_fre_start = 5000000;//5M
float M18_fre_start = 4000000;//4M

float M5_fre_start 		= 36000000;//4M
float CM250_fre_start = 93000000;//93M
float CM100_fre_start = 37000000;//37M
float CM50_fre_start 	= 75000000;//75M
float CM20_fre_start 	= 113000000;//113M
float CM2_fre_start 	= 182000000;//182M

//存放初始化过程中测量得到的四个长度下的值

float M5_START[10] 		= {154,153,152,151,150,149,148,147,146,145};
float CM250_START[10] = {147,145,144,143,142,141,140,139,137,136};
float CM100_START[10] = {147,145,144,143,142,141,140,139,137,136};
float CM50_START[10] 	= {166,166,166,165,165,165,165,165,165,165};
float CM20_START[10] 	= {151,151,151,151,151,151,151,151,151,151};
float CM2_START[10] 	= {147,145,144,143,142,141,140,139,137,136};


float M5_END[10] 		= {58,56,55,52,52,50,50,48,47,45};
float CM250_END[10] = {57,56,54,52,51,50,49,47,46,44};
float CM100_END[10] = {57,56,54,52,51,50,49,47,46,44};
float CM50_END[10] 	= {57,56,54,52,51,50,49,47,46,44};
float CM20_END[10] 	= {57,57,57,57,57,57,57,56,56,56};
float CM2_END[10] 	= {21,21,21,21,21,21,21,21,21,21};


float M10_START[10] = {147,145,144,143,142,141,140,139,137,136};
float M14_START[10] = {109,108,108,107,107,105,105,104,104,104};
float M18_START[10] = {103,102,101,101,100,100,99,98,98,97};
float M10_END[10] = {57,56,54,52,51,50,49,47,46,44};
float M14_END[10] = {38,37,35,33,31,29,27,25,24,22};
float M18_END[10] = {73,71,69,67,65,63,60,58,56,54};
//存放三个测量区间的数据的斜率
float M10_K[10] = {0.224775,0.222278, 0.224775, 0.227273, 0.227273, 0.224775, 0.227273, 0.227273, 0.227273, 0.229770};
float M14_K[10] = {0.182848, 0.182848, 0.187999, 0.190574, 0.195725, 0.195725, 0.200876, 0.200876, 0.206026, 0.211177};
float M18_K[10] = {0.15345, 0.15857, 0.16368, 0.17391, 0.17903, 0.18926, 0.19949, 0.20460, 0.21483, 0.21995};

float M5_K[10] 		= {0.224775,0.222278, 0.224775, 0.227273, 0.227273, 0.224775, 0.227273, 0.227273, 0.227273, 0.229770};
float CM250_K[10] = {0.182848, 0.182848, 0.187999, 0.190574, 0.195725, 0.195725, 0.200876, 0.200876, 0.206026, 0.211177};
float CM100_K[10] 	= {0.15345, 0.15857, 0.16368, 0.17391, 0.17903, 0.18926, 0.19949, 0.20460, 0.21483, 0.21995};
float CM50_K[10]	 = {0.224775,0.222278, 0.224775, 0.227273, 0.227273, 0.224775, 0.227273, 0.227273, 0.227273, 0.229770};
float CM20_K[10] 	= {0.182848, 0.182848, 0.187999, 0.190574, 0.195725, 0.195725, 0.200876, 0.200876, 0.206026, 0.211177};
float CM2_K[10] 	= {0.15345, 0.15857, 0.16368, 0.17391, 0.17903, 0.18926, 0.19949, 0.20460, 0.21483, 0.21995};



//存放测量得到的10个相位值
float MUnknown[10] = {0};

//存放测量得到的线的长度
float Mlength = 15;

////////////////////////////////////////////////////////////
//以下数据用于判断线长属于哪个范围已进行后续的较为准确的测量
////////////////////////////////////////////////////////////
//存放在1M-2M测量的10个点的相位值
float M1_length[10] = {0};
//1M-2M测量的数据的对应频率值
float M1_fre[10] = {0};
//存放在1M-2M范围内经过校准后得到的斜率值，该值将是后续判断的基础

float M2_CM2 		= 0;
float M2_CM20 	= 33.714287 ;
float M2_CM50		= 49.999992 ;
float M2_CM100	= 80.571388;
float M2_CM250 	= 176.857071;
float M2_M5 		= 0;

float M1_5M  = 48.606060;
float M1_10M = 83.696968;
float M1_14M = 111.757637;
float M1_18M = 143.030426;
float M1_20M = 154.666550;
//存放在1M-2M范围内最终得到的斜率
float M1_Unknown = 0;
float M2_Unknown = 0;
//标志位，用于标识属于得到的测量结果属于哪个测量范围
uint32_t scale_flag = 0;

////////////////////////////////////////////////////////////
//以下数据用于负载判断与计算
////////////////////////////////////////////////////////////
//负载的类型标志位：0：开路;1:电阻;2:电容。
uint32_t Load_type = 0;
//ads1256在300-500mV的校正系数
float calib1256 = 0.000000603;
float Res_low 	= 0;
float Res_high 	= 0;
//用于校准的值
float Res_10M = 1.260830;
float Res_20M = 2.629039;
float Res_10M_K = 0.12608;
float Res_20M_K = 0.13145;
//电阻的大小
float Res_data = 0;
//电阻的校正标志位
uint32_t Res_calib_flag = 0;
//电缆的短路电阻阻值
float Res_length = 0;

//电容的长度测量值
float Cap_length = 0;
//电容的大小
float Cap_data = 0;
//参考的电缆容值与倍数乘积
float Cap_ref  = 1730;


void adc3_init(void)
{
//	HAL_ADC_DeInit(&hadc3);
	MX_ADC3_Init();	//初始化调用放这里, 确保在MX_DMA_Init()初始化后面    
	Delay_Us(100);	//有地方说这里可以等等电压稳定后再校准

	if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADCEx_Calibration_Start\r\n");
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc3_data, 16) != HAL_OK)
	{
			printf("hadc3 error with HAL_ADC_Start_DMA\r\n");
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim1);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc3);

}
void adc_saopin(void)
{
	MX_ADC1_Init();	//初始化调用放这里, 确保在MX_DMA_Init()初始化后面    
	Delay_Us(100);	//有地方说这里可以等等电压稳定后再校准

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
	//系统初始化代码
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
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);  //开启PWM通道1
//	SineWave();//生成正弦数据
//	HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_2,(uint32_t *)Dat,20,DAC_ALIGN_12B_R);//开启DMA-DAC
//	HAL_TIM_Base_Start(&htim6);//打开定时器6
	double data = 0;
	float adata = 0;
	Close_All();
	Init_ad9910();
	Write_Amplitude(100);
	Freq_convert(10000);

	ADF4351Init();
	ADF4351WriteFreq(200);	//设置固定输出频率为400MHZ
	ADS1256_Init(ADS1256_GAIN_1,ADS1256_DRATE_2000SPS);
	workstate(0);
	gpio_length();
	uint16_t scope_tmp = 0;
	uint32_t gap = 100000;
	printf("hello\r\n");
	loadstate(0);
	show_load_data(0);
	
//	for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
//	{
//		x_tmp = 1000000+gap*scope_tmp;
//		Freq_convert(x_tmp);
//		printf("x_tmp = %d\r\n",x_tmp/100000);
//		Delay_Us(100);
//		for(x_tmp = 0;x_tmp<10;x_tmp++)
//		{
//			comp[x_tmp] = get1256();
//		}
//		QuickSort(comp,0,9);
//		MUnknown[0] =comp[4]; 		
//		printf("%f\r\n",MUnknown[0]);
//	}
//	MUnknown[0] = length_scope();
//	printf("MUnknown[0]=%f\r\n",MUnknown[0]);


	Close_All();
	workstate(1);
	for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
	{
		CM2_K[scope_tmp]  	= (CM2_START[scope_tmp] 	-CM2_END[scope_tmp])	/18.5;
		CM20_K[scope_tmp]  	= (CM20_START[scope_tmp] 	-CM20_END[scope_tmp])	/23.7;
		CM50_K[scope_tmp]  	= (CM50_START[scope_tmp] 	-CM50_END[scope_tmp])	/54.7;
		CM100_K[scope_tmp]  = (CM100_START[scope_tmp] -CM100_END[scope_tmp])/161;			
		CM250_K[scope_tmp]  = (CM250_START[scope_tmp] -CM250_END[scope_tmp])/260.4;
		M5_K[scope_tmp]  = (M5_START[scope_tmp] -M5_END[scope_tmp])/479.5;
		M10_K[scope_tmp] = (M10_START[scope_tmp]-M10_END[scope_tmp])/400.4;
		M14_K[scope_tmp] = (M14_START[scope_tmp]-M14_END[scope_tmp])/388.3;
		M18_K[scope_tmp] = (M18_START[scope_tmp]-M18_END[scope_tmp])/195.5;
	}	
	workstate(0);	
	while(1)
	{
		if(length_flag == 1)
		{
			gpio_length();
			workstate(1);
			length_flag = 0;
			M1_Unknown = length_scope();
			scope_judge();	
		  printf("%d\r\n",scale_flag);
			cal_length();			
			show_length(Mlength);		
			workstate(0);				
			loadstate(0);
			show_load_data(0);
			Cap_length = Mlength;			
			Close_All();
		}
		if(load_flag == 1)
		{
			gpio_cap();		
			workstate(1);
			load_flag = 0;			
			Cap_value_get();
//			show_length(Mlength);		
			Cap_data = (Mlength-Cap_length-5)*96.45/100;
			if(Cap_data<60)
			{
				loadstate(0);	
				gpio_res();
				Init_ad9910();
				Write_Amplitude(100);
				Freq_convert(1000000);
				Delay_Us(100);				
				for(x_tmp = 0;x_tmp<10;x_tmp++)
				{
					comp[x_tmp] = get1256();
				}
				QuickSort(comp,0,9);
				Res_low =comp[4];
				printf("%f\r\n",Res_low);					
				if(Res_low < 168)
				{
					loadstate(0);		
					show_load_data(0);					
				}
				else
				{
					if(Cap_length>1370)
					{
						Res_data = -0.002870*Cap_length+0.389264*Res_low-53.506447;		
						if(Res_data>27)
						{
							Res_data = Res_data-2;
						}
						if(Res_data<12.4)
						{
							Res_data = Res_data-1.45;
						}
					}
					else 
					{
						Res_data = -0.001472*Cap_length+0.360192*Res_low-49.938362;	
						if(Res_data <13.5)
						{
							Res_data = Res_data-1.8;
						}
						if((Res_data > 15.5)&&(Res_data<25))
						{
							Res_data = Res_data+0.5;
						}
					}
					loadstate(1);
					show_load_data(Res_data);					
				}					
			}
		
			else 
			{	//小于16m的时候认为会测到偏大的
				if((Cap_length <1650)&&(Cap_length>9000))
				{
					if(Cap_data<200)
					{
						Cap_data = Cap_data-(1650-Cap_length)/1650*18;
						//跨段修正
						if((Cap_length<1400)&&(Mlength>1400))
						{
							Cap_data = Cap_data-25;
						}						
					}
					else
					{
						Cap_data = Cap_data-(1650-Cap_length)/1650*15;
						//跨段修正
						if((Cap_length<1400)&&(Mlength>1400))
						{
							Cap_data = Cap_data-15;
						}
					}
				}
				//大于16m的时候认为会测到偏大的
				else if(Cap_length>=1650)
				{
					if(Cap_data<200)
					{
						if(Cap_data<150)
						{
							Cap_data = 100+Cap_data/100;
						}
						else
						{
							Cap_data = 150+Cap_data/100;
						}
					}	
					else
					{
						Cap_data = Cap_data-5;
					}					
				}
				loadstate(2);
				show_load_data(Cap_data);
			}
			
			Close_All();
			workstate(0);
		}
		if(check_K_flag == 10)
		{
			gpio_length();
			workstate(1);
			M1_10M = length_scope();
			calibration_show(M1_10M);
			printf("%f\r\n\r\n",M1_10M);			
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();
		}
		else if(check_K_flag == 14)
		{
			gpio_length();
			workstate(1);
			M1_14M = length_scope();
			printf("%f\r\n\r\n",M1_14M);
			calibration_show(M1_14M);
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}
		else if(check_K_flag == 18)
		{
			gpio_length();
			workstate(1);
			M1_18M = length_scope();
			printf("%f\r\n\r\n",M1_18M);
			calibration_show(M1_18M);			
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}
		else if(check_K_flag == 20)
		{
			gpio_length();
			workstate(1);
			M1_20M = length_scope();
			printf("%f\r\n\r\n",M1_20M);
			calibration_show(M1_20M);						
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}	
		else if(check_K_flag == 81)
		{
			gpio_length();
			workstate(1);
			M1_5M = length_scope();
			printf("%f\r\n\r\n",M1_5M);
			calibration_show(M1_5M);	
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}			
		else if(check_K_flag == 82)
		{
			gpio_length();
			workstate(1);
			M2_CM250 = length_scope_1M();
			printf("%f\r\n\r\n",M2_CM250);
			calibration_show(M2_CM250);	
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}		
		else if(check_K_flag == 83)
		{
			gpio_length();
			workstate(1);
			M2_CM100 = length_scope_1M();
			printf("%f\r\n\r\n",M2_CM100);
			calibration_show(M2_CM100);	
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}				
		else if(check_K_flag == 84)
		{
			gpio_length();
			workstate(1);
			M2_CM50 = length_scope_1M();
			printf("%f\r\n\r\n",M2_CM50);
			calibration_show(M2_CM50);	
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}		
		else if(check_K_flag == 85)
		{
			gpio_length();
			workstate(1);
			M2_CM20 = length_scope_1M();
			printf("%f\r\n\r\n",M2_CM20);
			calibration_show(M2_CM20);	
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}		
		else if(check_K_flag == 86)
		{
			gpio_length();
			workstate(1);
			M2_CM2 = length_scope_1M();
			printf("%f\r\n\r\n",M2_CM2);
			calibration_show(M2_CM2);	
			get_K();
			workstate(0);
			check_K_flag = 0;
			Close_All();			
		}					
		else if(check_K_flag == 24)
		{
			gpio_length();
			workstate(1);
			//printf("\r\n");
			get_K();
			workstate(0);
			check_K_flag = 0;
			for(x_tmp = 0;x_tmp<10;x_tmp++)
			{
				//printf("M10_K= %f\r\n\r\n",M10_K[x_tmp]);
			}
			for(x_tmp = 0;x_tmp<10;x_tmp++)
			{
				//printf("M14_K= %f\r\n\r\n",M14_K[x_tmp]);
			}			
			for(x_tmp = 0;x_tmp<10;x_tmp++)
			{
				//printf("M18_K= %f\r\n\r\n",M18_K[x_tmp]);
			}
			Close_All();			
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
	Scan_cap();
	for (u16 temp = 0; temp< 20;temp++)
	{
		comp[temp] = ADS1256GetCap();
	}
	QuickSort(comp,0,19);
	data = comp[10];
	data=roundf(data);
	adata = 1000000000000*((1/(2*3.1415926*y_tmp))/(752*tan(data/10/180*3.1415926)))/2;
	printf("%lf\r\n",adata);
	Delay_Ms(1000);
	while(1)
	{
		for (u16 temp = 0; temp< 8;temp++)
		{
			comp[temp] = ADS1256GetCap();
		}
		QuickSort(comp,0,7);
		data = comp[4];
		data=roundf(data);
		data = 1000000000000*((1/(2*3.1415926*y_tmp))/(752*tan((data)/10/180*3.1415926)))/2-adata;
		data = (data-6.75)/(Cap_ref/adata);
		printf("%lf\r\n",data);
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
