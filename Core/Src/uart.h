/*
/******************************************************************************
 ** @file          : uart.h
 * @brief          : 
/******************************************************************************
 * Created on Thu Apr 11 2024 by Dimitris Kalaitzakis
 *
 * Copyright (c) 2024 Theon
 */


#ifndef SRC_UART_H_
#define SRC_UART_H_

#include "stm32u5xx_hal.h"
#include "stm32u5xx.h"
#include <stdint.h>

typedef enum{
	UART_DEBUG,
	UART_THERMIS
}UART_select;

#define ACK_FLAG 0x00000001U
#define NACK_FLAG 0x00000000U

void MX_USART1_UART_Init(void);

void MX_USART2_UART_Init(void);

HAL_StatusTypeDef uart_write_debug(uint8_t *pData, uint32_t Timeout);

HAL_StatusTypeDef uart_write(uint8_t *pData, uint8_t len, UART_select device, uint32_t Timeout);



#endif /* SRC_UART_H_ */
