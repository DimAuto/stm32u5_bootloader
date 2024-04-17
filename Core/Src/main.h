

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "stm32u5xx.h"
#include "stdio.h"
#include "stdint.h"

#define FW_VERSION 0x01

// #define SW_DEBOUNCE

void Error_Handler(void);


/* Private defines -----------------------------------------------------------*/
#define OPTO_IN_2_Pin GPIO_PIN_13
#define OPTO_IN_2_GPIO_Port GPIOC
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define I2C3_SCL_Pin GPIO_PIN_0
#define I2C3_SCL_GPIO_Port GPIOC
#define I2C3_SDA_Pin GPIO_PIN_1
#define I2C3_SDA_GPIO_Port GPIOC
#define LSM303_INT_1_XL_Pin GPIO_PIN_2
#define LSM303_INT_1_XL_GPIO_Port GPIOC
#define LSM303_INT_2_XL_Pin GPIO_PIN_3
#define LSM303_INT_2_XL_GPIO_Port GPIOC
#define USART2_FORCE_Pin GPIO_PIN_0
#define USART2_FORCE_GPIO_Port GPIOA
#define USART2_READY_Pin GPIO_PIN_1
#define USART2_READY_GPIO_Port GPIOA
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define USART3_FORCE_Pin GPIO_PIN_4
#define USART3_FORCE_GPIO_Port GPIOA
#define USART3_RX_Pin GPIO_PIN_5
#define USART3_RX_GPIO_Port GPIOA
#define USART3_READY_Pin GPIO_PIN_6
#define USART3_READY_GPIO_Port GPIOA
#define USART3_TX_Pin GPIO_PIN_7
#define USART3_TX_GPIO_Port GPIOA
#define LSM6_INT1_Pin GPIO_PIN_0
#define LSM6_INT1_GPIO_Port GPIOB
#define LSM6_INT2_Pin GPIO_PIN_1
#define LSM6_INT2_GPIO_Port GPIOB
#define PB2_NC_Pin GPIO_PIN_2
#define PB2_NC_GPIO_Port GPIOB
#define USART1_OE_Pin GPIO_PIN_10
#define USART1_OE_GPIO_Port GPIOB
#define USART3_RS232_SEL_Pin GPIO_PIN_13
#define USART3_RS232_SEL_GPIO_Port GPIOB
#define USART3_OE_Pin GPIO_PIN_14
#define USART3_OE_GPIO_Port GPIOB
#define LSM303_INT_MAG_Pin GPIO_PIN_15
#define LSM303_INT_MAG_GPIO_Port GPIOB
#define FLT_BUTTON_1_Pin GPIO_PIN_6
#define FLT_BUTTON_1_GPIO_Port GPIOC
#define FLT_BUTTON_1_EXTI_IRQn EXTI6_IRQn
#define FLT_BUTTON_2_Pin GPIO_PIN_7
#define FLT_BUTTON_2_GPIO_Port GPIOC
#define FLT_BUTTON_2_EXTI_IRQn EXTI7_IRQn
#define FLT_BUTTON_3_Pin GPIO_PIN_8
#define FLT_BUTTON_3_GPIO_Port GPIOC
#define FLT_BUTTON_3_EXTI_IRQn EXTI8_IRQn
#define FLT_BUTTON_4_Pin GPIO_PIN_9
#define FLT_BUTTON_4_GPIO_Port GPIOC
#define FLT_BUTTON_4_EXTI_IRQn EXTI9_IRQn
#define USART1_FORCE_Pin GPIO_PIN_8
#define USART1_FORCE_GPIO_Port GPIOA
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define USART1_READY_Pin GPIO_PIN_11
#define USART1_READY_GPIO_Port GPIOA
#define PA12_NC_Pin GPIO_PIN_12
#define PA12_NC_GPIO_Port GPIOA
#define OPTO_OUT_1_Pin GPIO_PIN_10
#define OPTO_OUT_1_GPIO_Port GPIOC
#define OPTO_OUT_2_Pin GPIO_PIN_11
#define OPTO_OUT_2_GPIO_Port GPIOC
#define OPTO_IN_1_Pin GPIO_PIN_12
#define OPTO_IN_1_GPIO_Port GPIOC
#define PD2_NC_Pin GPIO_PIN_2
#define PD2_NC_GPIO_Port GPIOD
#define I2C1_EN_Pin GPIO_PIN_5
#define I2C1_EN_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_6
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define USART1_RS232_SEL_Pin GPIO_PIN_8
#define USART1_RS232_SEL_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
