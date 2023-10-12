#include "error_detect.h"
//负载判断测量相关文件放在这里。



//全部拉低
void Close_All(void)
{
	E2_L;
	E3_L;
	E4_L;
	E5_L;
	E6_L;
	C13_L;
	Delay_Us(100);
}
void gpio_length(void)
{
	E2_L;
	E3_H;
	Delay_Us(100);
}
void gpio_res(void)
{
	E2_H;
	E3_L;
	Delay_Us(100);
}
void gpio_cap(void)
{
	E2_L;
	E3_H;
	Delay_Us(100);
}




//
float Mea_Scan_DC(void)
{
		float OutputDC = 0;
		for (u16 temp = 0; temp< 16;temp++)
		{
			comp[temp] = adc1_data[temp];
		}
		QuickSort(comp,0,16);
		OutputDC = (comp[7])*3300/65536;		
		return OutputDC;
}
//测量电阻值
float Cal_Res(void)
{
	float result = 0;
	printf("adc1\r\n");
	adc_saopin();
	for (u16 temp = 0; temp< 16;temp++)
	{
		comp[temp] = (float)(adc1_data[temp])/65536*3300;
	}	
	QuickSort(comp,0,15);
	Res_low = comp[6];
	printf("%f\r\n", Res_low);
	printf("adc3\r\n");
	adc3_init();
	for (u16 temp = 0; temp< 16;temp++)
	{
		comp[temp] = (float)(adc3_data[temp])/65536*3300;
	}	
	QuickSort(comp,0,15);
	Res_high = comp[6];
	printf("%f\r\n", Res_high);
	result = 20.12/(Res_high-Res_low)*Res_low-0.1;
	printf("%f\r\n", result);
	return result;
}
//判断是否是电阻负载
void Check_Res(void)
{
//	printf("adc1\r\n");
	adc_saopin();
	for (u16 temp = 0; temp< 16;temp++)
	{
		comp[temp] = (float)(adc1_data[temp])/65536*3300;
	}	
	QuickSort(comp,0,15);
	Res_low = comp[6];
//	printf("%f\r\n", Res_low);
//	printf("adc3\r\n");
	adc3_init();
	for (u16 temp = 0; temp< 16;temp++)
	{
		comp[temp] = (float)(adc3_data[temp])/65536*3300;
	}	
	QuickSort(comp,0,15);
	Res_high = comp[6];
//	printf("%f\r\n", Res_high);
	if((Res_high-Res_low)<300)
	{
		Res_calib_flag = 0;
	}
	else
	{
		Res_calib_flag = 1;
	}
	if(Res_calib_flag == 1)
	{
		Res_data = 20.12/(Res_high-Res_low)*Res_low-0.1;
		if(Mlength<=10)
		{
			Mlength = 10;
		}
		Res_length = Res_10M+Res_10M_K*(Mlength-10)*(Mlength-10)/10+Res_20M_K*(20-Mlength)*(Mlength-10)/10;
		Res_data = Res_data-Res_length;
		printf("%f\r\n", Res_length);
		printf("%f\r\n", Res_data);	
	}
}


void Scan_cap(void)
{
	float data = 0;
	Init_ad9910();
	Write_Amplitude(100);
	y_tmp = 60000;
	Freq_convert(y_tmp);
	Delay_Us(500);
	x_tmp = 1;
	while(x_tmp)
	{
		for (u16 temp = 0; temp< 8;temp++)
		{
			comp[temp] = ADS1256GetCap();
		}
		QuickSort(comp,0,7);
		data = comp[4];
//		printf("%f\r\n",data);
		if((data <= 441.5)&&(data >= 435.5))
		{
			x_tmp = 0;
			printf("\r\n\n\n%d\r\n",y_tmp);
			break;
		}
		y_tmp = y_tmp+100;
		Freq_convert(y_tmp);
		Delay_Us(10);		
		if(y_tmp >= 110000)
		{
			printf("failed");
			x_tmp = 0;
		}
	}
}

void Cap_value_get(void)
{
//	for (u16 temp = 0; temp< 8;temp++)
//	{
//		comp[temp] = ADS1256GetCap();
//	}
//	QuickSort(comp,0,7);
//	Cap_data = comp[4];
//	Cap_data = 1000000000000*((1/(2*3.1415926*y_tmp))/(752*tan((Cap_data/10)/180*3.1415926)))/2-Cap_length;
//	Cap_data = (Cap_data-6.75)/(Cap_ref/Cap_data);

//	printf("%lf\r\n",Cap_data);	
	M1_Unknown = length_scope();
	scope_judge();	
	printf("%d\r\n",scale_flag);
	cal_length();		
}