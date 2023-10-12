#ifndef __Error_detect_H__
#define __Error_detect_H__

#include "main.h"


void Close_All(void);

void gpio_length(void);

void gpio_res(void);

void gpio_cap(void);

float Mea_Scan_DC(void);

void Check_Res(void);

float Cal_Res(void);

void Scan_cap(void);

void Cap_value_get(void);

#endif