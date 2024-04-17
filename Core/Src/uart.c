/*
/******************************************************************************
 ** @file          : uart.c
 * @brief          : 
/******************************************************************************
 * Created on Thu Apr 11 2024 by Dimitris Kalaitzakis
 *
 * Copyright (c) 2024 Theon
 */

#include "uart.h"
#include "message_handler.h"
#include "cmsis_os2.h"
#include "circular_buffer.h"


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

extern osEventFlagsId_t ack_rcvd;
extern osEventFlagsId_t wait_for_ack;

//uart message queue
extern osMessageQueueId_t uart1messageQueueHandle;
extern osMessageQueueId_t uart2messageQueueHandle;

CB_t uart1RXrb;
CB_t uart2RXrb;

/* for uart1*/
volatile uint8_t rxChar = 0x00;
volatile uint8_t buffer_enable = 0;
volatile uint8_t byte_count = 0;

/* for uart2*/
volatile uint8_t rxChar_2 = 0x00;
volatile uint8_t buffer_enable_2 = 0;
volatile uint8_t byte_count_2 = 0;

volatile uint8_t ack_rcv_flag = 0;

// extern osMessageQueueId_t messageQueueHandle;
// extern osEventFlagsId_t ack_rcvd;


/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}


HAL_StatusTypeDef uart_receive_it(UART_select device){
	switch (device){
	case UART_DEBUG:
    return HAL_UART_Receive_IT(&huart2, &rxChar_2, 1);
	case UART_THERMIS:
    return HAL_UART_Receive_IT(&huart1, &rxChar, 1);
  default:
    break;
	}
  return HAL_ERROR;
}


HAL_StatusTypeDef uart_write_debug(uint8_t *pData, uint32_t Timeout){
	return HAL_UART_Transmit(&huart1,pData,strlen(pData),Timeout);// Sending in normal mode
}

HAL_StatusTypeDef uart_write(uint8_t *pData, uint8_t len, UART_select device, uint32_t Timeout){
	UART_HandleTypeDef *huart;
	switch (device){
	case UART_DEBUG:
		huart = &huart2;
		break;
	case UART_THERMIS:
		huart = &huart1;
		break;
	}
	if (len == 0){
		return HAL_UART_Transmit(huart,pData,strlen(pData),Timeout);
	}
	return HAL_UART_Transmit(huart,pData,len,Timeout);// Sending in normal mode
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
    if (byte_count >= 8){
      if(rxChar == ETX){
        uint8_t start_ch = 0;
        start_ch = CB_pop(&uart1RXrb);
        if (start_ch == STX){
          uint8_t cb_len = CB_size(&uart1RXrb);
          CB_pushFront(&uart1RXrb, cb_len);
          osMessageQueuePut(uart1messageQueueHandle, uart1RXrb.buffer, 0U, 0U);
          CB_clear(&uart1RXrb);
          buffer_enable = 0;
          byte_count = 0;
        }
      }
      CB_clear(&uart1RXrb);
      buffer_enable = 0;
      byte_count = 0;
    }
    if((rxChar == STX) && (byte_count == 0)){
      buffer_enable = 1;
      CB_push(&uart1RXrb, rxChar);
      byte_count++;
    }
    else {
      if(buffer_enable == 1){
        CB_push(&uart1RXrb, rxChar);
        byte_count++;
      }
    }
		HAL_UART_Receive_IT(&huart1, &rxChar, 1);
  }
  else if (huart->Instance == USART2)
	{

    if (byte_count_2 >= 8){
      if(rxChar_2 == ETX){
        uint8_t start_ch_2 = 0;
        start_ch_2 = CB_pop(&uart2RXrb);
        if (start_ch_2 == STX){
          uint8_t cb_len_2 = CB_size(&uart2RXrb);
          CB_pushFront(&uart2RXrb, cb_len_2);
          osMessageQueuePut(uart2messageQueueHandle, uart2RXrb.buffer, 0U, 0U);
          CB_clear(&uart2RXrb);
          buffer_enable_2 = 0;
          byte_count_2 = 0;
        }
      }
      CB_clear(&uart2RXrb);
      buffer_enable_2 = 0;
      byte_count_2 = 0;
    }
    if((rxChar_2 == STX) && (byte_count_2 == 0)){
      buffer_enable_2 = 1;
      CB_push(&uart2RXrb, rxChar_2);
      byte_count_2++;
    }
    else {
      if(buffer_enable_2 == 1){
        CB_push(&uart2RXrb, rxChar_2);
        byte_count_2++;
      }
    }
		HAL_UART_Receive_IT(&huart2, &rxChar_2, 1);
  }
}
