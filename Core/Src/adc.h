#ifndef __ADC_H
#define __ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32u5xx_hal.h"
#include "stm32u5xx.h"
#include "stdio.h"
#include "stdint.h"

#define ADC_10B_MULT 3.079
#define ADC_14B_MULT 0.192

/**
 * @brief Initialize the ADC1 on portA pin7
 * 
 */
void MX_ADC1_Init(void);

uint32_t adc1_get_value(void);



#ifdef __cplusplus
}
#endif

#endif