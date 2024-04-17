/*
/******************************************************************************
 ** @file          : gpio.c
 * @brief          : 
/******************************************************************************
 * Created on Thu Apr 11 2024 by Dimitris Kalaitzakis
 *
 * Copyright (c) 2024 Theon
 */


#include "gpio.h"
#include "cmsis_os2.h"
#include "stm32u5xx.h"


extern osMessageQueueId_t eventQueueHandle;

static volatile button_event_t button_event;

void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USART2_FORCE_GPIO_Port, USART2_FORCE_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USART3_RS232_SEL_GPIO_Port, USART3_RS232_SEL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USART3_OE_GPIO_Port, USART3_OE_Pin, GPIO_PIN_RESET);   //FOR ADC

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OPTO_OUT_1_GPIO_Port, OPTO_OUT_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OPTO_IN_2_Pin OSC32_IN_Pin OSC32_OUT_Pin OPTO_OUT_2_Pin
                           OPTO_IN_1_Pin */
  GPIO_InitStruct.Pin = OPTO_IN_2_Pin|OSC32_IN_Pin|OSC32_OUT_Pin|OPTO_OUT_2_Pin
                          |OPTO_IN_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C3_SCL_Pin I2C3_SDA_Pin */
  GPIO_InitStruct.Pin = I2C3_SCL_Pin|I2C3_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LSM303_INT_1_XL_Pin LSM303_INT_2_XL_Pin */
  GPIO_InitStruct.Pin = LSM303_INT_1_XL_Pin|LSM303_INT_2_XL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : USART2_FORCE_Pin */
  GPIO_InitStruct.Pin = USART2_FORCE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USART2_FORCE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART2_READY_Pin USART3_READY_Pin USART1_FORCE_Pin USART1_READY_Pin */
  GPIO_InitStruct.Pin = USART2_READY_Pin|USART3_READY_Pin|USART1_FORCE_Pin|USART1_READY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : USART3_FORCE_Pin USART3_RX_Pin PA12_NC_Pin */
  GPIO_InitStruct.Pin = USART3_FORCE_Pin|USART3_RX_Pin|PA12_NC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LSM6_INT1_Pin LSM6_INT2_Pin LSM303_INT_MAG_Pin */
  GPIO_InitStruct.Pin = LSM6_INT1_Pin|LSM6_INT2_Pin|LSM303_INT_MAG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2_NC_Pin I2C1_EN_Pin USART1_RS232_SEL_Pin */
  GPIO_InitStruct.Pin = PB2_NC_Pin|I2C1_EN_Pin|USART1_RS232_SEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : USART1_OE_Pin I2C1_SCL_Pin I2C1_SDA_Pin */
  GPIO_InitStruct.Pin = USART1_OE_Pin|I2C1_SCL_Pin|I2C1_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : USART3_RS232_SEL_Pin USART3_OE_Pin */
  GPIO_InitStruct.Pin = USART3_RS232_SEL_Pin|USART3_OE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : FLT_BUTTON_1_Pin FLT_BUTTON_2_Pin FLT_BUTTON_3_Pin FLT_BUTTON_4_Pin */
  GPIO_InitStruct.Pin = FLT_BUTTON_1_Pin|FLT_BUTTON_2_Pin|FLT_BUTTON_3_Pin|FLT_BUTTON_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : OPTO_OUT_1_Pin */
  GPIO_InitStruct.Pin = OPTO_OUT_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OPTO_OUT_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2_NC_Pin */
  GPIO_InitStruct.Pin = PD2_NC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PD2_NC_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI6_IRQn);

  HAL_NVIC_SetPriority(EXTI7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI7_IRQn);

  HAL_NVIC_SetPriority(EXTI8_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI8_IRQn);

  HAL_NVIC_SetPriority(EXTI9_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_IRQn);
}


void setLaserOutput(GPIO_PinState state){
    HAL_GPIO_WritePin(OPTO_OUT_1_GPIO_Port, OPTO_OUT_1_Pin, state);
}

GPIO_PinState getLaserOutput(void){
    return HAL_GPIO_ReadPin(OPTO_OUT_1_GPIO_Port, OPTO_OUT_1_Pin);
}

void toggleLaserOutput(void){
    if (HAL_GPIO_ReadPin(OPTO_OUT_1_GPIO_Port, OPTO_OUT_1_Pin) == GPIO_PIN_SET){
        HAL_GPIO_WritePin(OPTO_OUT_1_GPIO_Port, OPTO_OUT_1_Pin, GPIO_PIN_RESET);
    }
    else{
        HAL_GPIO_WritePin(OPTO_OUT_1_GPIO_Port, OPTO_OUT_1_Pin, GPIO_PIN_SET);
    }
}

void enableADCinput(void){
    HAL_GPIO_WritePin(USART3_OE_GPIO_Port, USART3_OE_Pin, GPIO_PIN_SET);
}

void disableADCinput(void){
    HAL_GPIO_WritePin(USART3_OE_GPIO_Port, USART3_OE_Pin, GPIO_PIN_RESET);
}





/* ISR Callbacks*/
HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin == FLT_BUTTON_1_Pin){
        if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_1_Pin) == GPIO_PIN_SET){
            button_event.button = BUTTON_RECT_REL;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
    else if(GPIO_Pin == FLT_BUTTON_2_Pin){
        if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_2_Pin) == GPIO_PIN_SET){
            button_event.button = BUTTON_CIRCLE_REL;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
    else if(GPIO_Pin == FLT_BUTTON_4_Pin){
        if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_4_Pin) == GPIO_PIN_SET){
            button_event.button = BUTTON_UP_REL;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
    else if(GPIO_Pin == FLT_BUTTON_3_Pin){
       if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_3_Pin) == GPIO_PIN_SET){
            button_event.button = BUTTON_DOWN_REL;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
}


HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin == FLT_BUTTON_1_Pin){
        if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_1_Pin) == GPIO_PIN_RESET){
            button_event.button = BUTTON_RECT_PRESS;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
    else if(GPIO_Pin == FLT_BUTTON_2_Pin){
        if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_2_Pin) == GPIO_PIN_RESET){
            button_event.button = BUTTON_CIRCLE_PRESS;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
    else if(GPIO_Pin == FLT_BUTTON_4_Pin){
       if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_4_Pin) == GPIO_PIN_RESET){
            button_event.button = BUTTON_UP_PRESS;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
    else if(GPIO_Pin == FLT_BUTTON_3_Pin){
        if(HAL_GPIO_ReadPin(GPIOC, FLT_BUTTON_3_Pin) == GPIO_PIN_RESET){
            button_event.button = BUTTON_DOWN_PRESS;
            button_event.timestamp = osKernelGetTickCount();
            osMessageQueuePut(eventQueueHandle, &button_event, 0U, 0U);
        }
    }
}