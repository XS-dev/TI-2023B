#include "error_detect.h"

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
