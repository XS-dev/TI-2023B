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

float length_scope(void)
{
	Init_ad9910();
	Write_Amplitude(100);
	Delay_Us(500);
	uint16_t scope_tmp = 0;
	float result = 0;
	for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
	{
		M1_fre[scope_tmp] = 1+0.1*scope_tmp;
		x_tmp = 500000+150000*scope_tmp;
		Freq_convert(x_tmp);
		Delay_Us(100);
		for(x_tmp = 0;x_tmp<20;x_tmp++)
		{
			comp[x_tmp] = get1256();
		}
		QuickSort(comp,0,19);
		M1_length[scope_tmp] =comp[10]; 
	}
	for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
	{
		M1_fre[scope_tmp] = 1+0.1*scope_tmp;
		x_tmp = 500000+150000*scope_tmp;
		Freq_convert(x_tmp);
		Delay_Us(100);
		for(x_tmp = 0;x_tmp<20;x_tmp++)
		{
			comp[x_tmp] = get1256();
		}
		QuickSort(comp,0,19);
		M1_length[scope_tmp] =comp[10]; 
//		printf("%f\r\n", M1_length[scope_tmp]);
	}	
	result = calculate_slope(M1_fre,M1_length,10);
	return result;
}

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
	else if(M1_Unknown>=M1_10M-1)
	{
		scale_flag = 10;
	}
	else if(M1_Unknown>=M1_1M)
	{
		scale_flag = 10;
	}	
	else 
	{
		scale_flag = 1;
	}
}

void cal_length(void)
{
	uint16_t scope_tmp = 0;

	if(scale_flag == 10)
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
		}
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (M10_START[scope_tmp]-MUnknown[scope_tmp])/M10_K[scope_tmp]+1000;
		}
		QuickSort(MUnknown,0,9);
		Mlength = MUnknown[0];
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
		}
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (M14_START[scope_tmp]-MUnknown[scope_tmp])/M14_K[scope_tmp]+1400;
		}
		QuickSort(MUnknown,0,9);
		Mlength = MUnknown[0];
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
														
		}
		for(scope_tmp = 0;scope_tmp < 10;scope_tmp++)
		{
			MUnknown[scope_tmp] = (M18_START[scope_tmp]-MUnknown[scope_tmp])/M18_K[scope_tmp]+1800;
			printf("%f\r\n", MUnknown[scope_tmp]);	
		}
		QuickSort(MUnknown,0,9);
		Mlength = MUnknown[0];
	}	
}
void get_K(void)
{
	uint16_t scope_tmp = 0;
	if(check_K_flag == 10)
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
			printf("%f\r\n", M10_END[scope_tmp]);			
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
			printf("%f\r\n", M14_START[scope_tmp]);			
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
			printf("%f\r\n", M14_END[scope_tmp]);									
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
			printf("%f\r\n", M18_START[scope_tmp]);						
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
			M10_K[scope_tmp] = (M10_START[scope_tmp]-M10_END[scope_tmp])/400;
			M14_K[scope_tmp] = (M14_START[scope_tmp]-M14_END[scope_tmp])/400;
			M18_K[scope_tmp] = (M18_START[scope_tmp]-M18_END[scope_tmp])/200;
		}				
	}
}