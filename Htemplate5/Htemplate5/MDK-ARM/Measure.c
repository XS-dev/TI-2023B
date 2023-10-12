#include "Measure.h"
// 交换数组中两个元素的位置
void swap(float *a, float *b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

// 快速排序算法
void QuickSort(float arr[], int low, int high) 
{
    if (low < high) {
        // 选择最右边的元素作为基准值
        float pivot = arr[high];

        // 将小于等于基准值的元素放在左侧，大于基准值的元素放在右侧
        int i = low - 1;
        for (int j = low; j <= high - 1; j++) {
            if (arr[j] <= pivot) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        swap(&arr[i + 1], &arr[high]);

        // 递归对左右两侧进行排序
        QuickSort(arr, low, i);
        QuickSort(arr, i + 2, high);
    }
}
// 计算斜率函数
float calculate_slope(float x[], float y[], int n) 
{
    float sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x_squared = 0.0;
    float slope, mean_x, mean_y;

    // Calculate the sums of x, y, xy, and x^2
    for (int i = 0; i < n; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x_squared += x[i] * x[i];
    }

    // Calculate the means of x and y
    mean_x = sum_x / n;
    mean_y = sum_y / n;

    // Calculate the slope using the formula: m = (n * Σxy - Σx * Σy) / (n * Σx^2 - (Σx)^2)
    slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x_squared - sum_x * sum_x);

    return slope;
}

// 计算斜率函数
float length_scope(void)
{
	Init_ad9910();
	Delay_Us(50);
	uint16_t scope_tmp = 0;
	float result = 0;
	for(scope_tmp = 0;scope_tmp < 5;scope_tmp++)
	{
		M1_fre[scope_tmp] = 1+0.2*scope_tmp;
		x_tmp = 500000+300000*scope_tmp;
		Freq_convert(x_tmp);
		Delay_Us(50);
		for(x_tmp = 0;x_tmp<10;x_tmp++)
		{
			comp[x_tmp] = get1256();
		}
		QuickSort(comp,0,9);
		M1_length[scope_tmp] =comp[9]; 
	}
	for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
	{
		M1_fre[scope_tmp] = 0.05*scope_tmp;
		x_tmp = 1000000+100000*scope_tmp;
		Freq_convert(x_tmp);
		Delay_Us(100);
		for(x_tmp = 0;x_tmp<10;x_tmp++)
		{
			comp[x_tmp] = get1256();
		}
		QuickSort(comp,0,9);
		M1_length[scope_tmp] =comp[9]; 
	}
	result = calculate_slope(M1_fre,M1_length,10);
	return result;
}
// 计算斜率函数_1M
float length_scope_1M(void)
{
	Init_ad9910();
	Delay_Us(200);
	uint16_t scope_tmp = 0;
	float result = 0;
	for(scope_tmp = 0;scope_tmp < 6;scope_tmp++)
	{
		M1_fre[scope_tmp] = 0.1*scope_tmp;
		x_tmp = 3000000+2000000*scope_tmp;
		Freq_convert(x_tmp);
		Delay_Us(200);
		for(x_tmp = 0;x_tmp<10;x_tmp++)
		{
			comp[x_tmp] = get1256();
		}
		QuickSort(comp,0,9);
		M1_length[scope_tmp] =comp[9]; 
	}
	for(scope_tmp = 0;scope_tmp < 6;scope_tmp++)
	{
		M1_fre[scope_tmp] = 0.1*scope_tmp;
		x_tmp = 3000000+2000000*scope_tmp;
		Freq_convert(x_tmp);
		Delay_Us(200);
		for(x_tmp = 0;x_tmp<10;x_tmp++)
		{
			comp[x_tmp] = get1256();
		}
		QuickSort(comp,0,9);
		M1_length[scope_tmp] =comp[9]; 
	}	
	result = calculate_slope(M1_fre,M1_length,6);
	return result;
}
//分段识别
void scope_judge(void)
{
	M1_Unknown = length_scope();
	if(M1_Unknown>=M1_20M)
	{
		scale_flag = 18;
	}
	else if(M1_Unknown>=M1_18M-1)
	{
		scale_flag = 18;
	}
	else if(M1_Unknown>=M1_14M-1)
	{
		scale_flag = 14;
	}
	else if(M1_Unknown>=M1_10M-5)
	{
		scale_flag = 10;
	}
	else if(M1_Unknown>=M1_5M)
	{
		scale_flag = 5;
	}	
	else 
	{
		M1_Unknown = length_scope_1M();
		if(M1_Unknown>=M2_CM250-1)
		{
			scale_flag = 250;
		}
		else if(M1_Unknown>=M2_CM100)
		{
			scale_flag = 100;
		}
		else if(M1_Unknown>=M2_CM50)
		{
			scale_flag = 50;
		}
		else if(M1_Unknown>=M2_CM20)
		{
			scale_flag = 20;
		}
		else 
		{
			scale_flag = 2;
		}	
	}
}


void cal_length_step1(float fre)
{
	uint16_t scope_tmp = 0;
	for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
	{
		x_tmp = fre+30000*scope_tmp;
		Freq_convert(x_tmp);
		Delay_Us(100);
		for(x_tmp = 0;x_tmp<20;x_tmp++)
		{
			comp[x_tmp] = get1256();
		}
		QuickSort(comp,0,19);
		MUnknown[scope_tmp] =comp[10]; 
//			printf("%f\r\n",MUnknown[scope_tmp]);
	}

}
//计算长度
void cal_length(void)
{
	uint16_t scope_tmp = 0;
	if(scale_flag == 2)
	{
		cal_length_step1(CM2_fre_start);
		Mlength = 0;		
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (CM2_START[scope_tmp]-MUnknown[scope_tmp])/CM2_K[scope_tmp]+3.5;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;		
	}
	if(scale_flag == 20)
	{
		cal_length_step1(CM20_fre_start);
		Mlength = 0;		
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (CM20_START[scope_tmp]-MUnknown[scope_tmp])/CM20_K[scope_tmp]+22;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;		
	}	
	if(scale_flag == 50)
	{
		cal_length_step1(CM50_fre_start);
		Mlength = 0;		
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (CM50_START[scope_tmp]-MUnknown[scope_tmp])/CM50_K[scope_tmp]+45.7;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;		
	}
	if(scale_flag == 100)
	{
		cal_length_step1(CM100_fre_start);
		Mlength = 0;		
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (CM100_START[scope_tmp]-MUnknown[scope_tmp])/CM100_K[scope_tmp]+99.4;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;		
	}
	if(scale_flag == 250)
	{
		cal_length_step1(CM250_fre_start);
		Mlength = 0;		
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (CM250_START[scope_tmp]-MUnknown[scope_tmp])/CM250_K[scope_tmp]+260.4;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;		
	}
	if(scale_flag == 5)
	{
		cal_length_step1(M5_fre_start);
		Mlength = 0;		
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (M5_START[scope_tmp]-MUnknown[scope_tmp])/CM2_K[scope_tmp]+520.8;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;		
	}	
	else if(scale_flag == 10)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M10_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			MUnknown[scope_tmp] =comp[10]; 
//			printf("%f\r\n",MUnknown[scope_tmp]);
		}
		Mlength = 0;		
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (M10_START[scope_tmp]-MUnknown[scope_tmp])/M10_K[scope_tmp]+1000.3;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;
	}
	else if(scale_flag == 14)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M14_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			MUnknown[scope_tmp] =comp[10]; 
			//printf("%f\r\n",MUnknown[scope_tmp]);
		}
		Mlength = 0;
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (M14_START[scope_tmp]-MUnknown[scope_tmp])/M14_K[scope_tmp]+1400.7;
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;
	}
	else if(scale_flag == 18)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M18_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			MUnknown[scope_tmp] =comp[10];
			printf("%f\r\n",MUnknown[scope_tmp]);														
		}
		Mlength = 0;
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (M18_START[scope_tmp]-MUnknown[scope_tmp])/M18_K[scope_tmp]+1789;
			//printf("%f\r\n", MUnknown[scope_tmp]);	
		}
		QuickSort(MUnknown,0,9);
		for(scope_tmp = 7;scope_tmp < 8;scope_tmp++)
		{
			Mlength = Mlength+MUnknown[scope_tmp];
		}		
		Mlength = Mlength;
//		Mlength = MUnknown[0];
	}
}
//获取校准值2cm-20m
void get_K(void)
{
	uint16_t scope_tmp = 0;
	if(check_K_flag == 86)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM2_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM2_START[scope_tmp] =comp[10];
			printf("%f\r\n", CM2_START[scope_tmp]);			
		}	
	}	
	else if(check_K_flag == 85)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM2_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM2_END[scope_tmp] =comp[10]; 
			printf("%f\r\n", CM2_END[scope_tmp]);			
		}			
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM20_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM20_START[scope_tmp] =comp[10];
			printf("%f\r\n", CM20_START[scope_tmp]);			
		}	
	}	
	else if(check_K_flag == 84)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM20_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM20_END[scope_tmp] =comp[10]; 
			printf("%f\r\n", CM20_END[scope_tmp]);			
		}			
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM50_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM50_START[scope_tmp] =comp[10];
			printf("%f\r\n", CM50_START[scope_tmp]);			
		}	
	}	
	else if(check_K_flag == 83)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM50_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM50_END[scope_tmp] =comp[10]; 
			printf("%f\r\n", M10_END[scope_tmp]);			
		}			
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM100_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM100_START[scope_tmp] =comp[10];
			printf("%f\r\n", M10_START[scope_tmp]);			
		}	
	}		
	else if(check_K_flag == 82)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM100_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM100_END[scope_tmp] =comp[10]; 
			printf("%f\r\n", M10_END[scope_tmp]);			
		}			
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM250_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM250_START[scope_tmp] =comp[10];
			printf("%f\r\n", M10_START[scope_tmp]);			
		}	
	}	
	else if(check_K_flag == 81)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = CM250_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			CM250_END[scope_tmp] =comp[10]; 
			printf("%f\r\n", M10_END[scope_tmp]);			
		}			
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M5_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M5_START[scope_tmp] =comp[10];
			printf("%f\r\n", M10_START[scope_tmp]);			
		}	
	}	
	
	else if(check_K_flag == 10)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M5_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M5_END[scope_tmp] =comp[10]; 
			printf("%f\r\n", M10_END[scope_tmp]);			
		}			
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M10_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M10_START[scope_tmp] =comp[10];
			printf("%f\r\n", M10_START[scope_tmp]);			
		}	
	}
	else if(check_K_flag == 14)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M10_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M10_END[scope_tmp] =comp[10]; 
			//printf("%f\r\n", M10_END[scope_tmp]);			
		}			
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M14_fre_start+10000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M14_START[scope_tmp] =comp[10]; 
			//printf("%f\r\n", M14_START[scope_tmp]);			
		}	
	}
	else if(check_K_flag == 18)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M14_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M14_END[scope_tmp] =comp[10]; 
			//printf("%f\r\n", M14_END[scope_tmp]);									
		}	
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M18_fre_start+10000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M18_START[scope_tmp] =comp[10];
			//printf("%f\r\n", M18_START[scope_tmp]);						
		}	
	}
	else if(check_K_flag == 20)
	{
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			x_tmp = M18_fre_start+30000*scope_tmp;
			Freq_convert(x_tmp);
			Delay_Us(100);
			for(x_tmp = 0;x_tmp<20;x_tmp++)
			{
				comp[x_tmp] = get1256();
			}
			QuickSort(comp,0,19);
			M18_END[scope_tmp] =comp[10]; 
			printf("%f\r\n", M18_END[scope_tmp]);									
		}	
	}
	else if(check_K_flag == 24)
	{
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
	}
}