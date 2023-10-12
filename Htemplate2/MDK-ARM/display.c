#include "display.h"

void workstate(uint8_t workstate)
{
	if(workstate == 1)
	{
		for(int i = 0;i<5;i++)
		{
			USART7PutString("t6.txt=\"正在检测\"\xff\xff\xff");
		}
	}
	else
	{
		for(int i = 0;i<5;i++)
		{
			USART7PutString("t6.txt=\"结果保持\"\xff\xff\xff");
		}
	}
}
void calibration_show(float Length)
{
	sprintf(screen_message,"x0.val=%d\xff\xff\xff",(uint32_t)(Length*100));
	USART7PutString(screen_message);
}

void show_length(float Length)
{
	sprintf(screen_message,"x0.val=%d\xff\xff\xff",(uint32_t)(Length*1000));
	USART7PutString(screen_message);
}

void show_load_data(float load_data)
{
	sprintf(screen_message,"x3.val=%d\xff\xff\xff",(uint32_t)(load_data*100));
	USART7PutString(screen_message);
}
void loadstate(uint8_t loadstate)
{
	if(loadstate == 0)
	{
		for(int i = 0;i<5;i++)
		{
			USART7PutString("t3.txt=\"开路\"\xff\xff\xff");
		}
	}
	else if(loadstate == 1)
	{
		for(int i = 0;i<5;i++)
		{
			USART7PutString("t3.txt=\"电阻\"\xff\xff\xff");
		}	
		for(int i = 0;i<5;i++)
		{
			USART7PutString("t4.txt=\"Ω\"\xff\xff\xff");
		}	
	}
	else if(loadstate == 2)
	{
		for(int i = 0;i<5;i++)
		{
			USART7PutString("t3.txt=\"电容\"\xff\xff\xff");
		}
		for(int i = 0;i<5;i++)
		{
			USART7PutString("t4.txt=\"pF\"\xff\xff\xff");
		}			
	}
}