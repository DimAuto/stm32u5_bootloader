/*
/******************************************************************************
 ** @file          : app_freertos.c
 * @brief          : Freertos main body
/******************************************************************************
 * Created on 14/03/2024 by Dimitris Kalaitzakis
 *
 * Copyright (c) 2024 Theon
 */


/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
#include "uart.h"
#include "gpio.h"
#include <stdint.h>
#include "circular_buffer.h"
#include "message_handler.h"
#include "adc.h"

osEventFlagsId_t ack_rcvd;
osEventFlagsId_t wait_for_ack;

extern IWDG_HandleTypeDef hiwdg;

typedef enum{
  LASER_ON,
  LASER_OFF,
  LASER_LATCH,
}laser_pointer_states;

typedef enum{
  LASER_CONNECTED,
  LASER_DISCONNECTED
}laser_connection_states;

typedef struct{
  laser_pointer_states pointer_state;
  laser_pointer_states prev_pointer_state;
  laser_connection_states connection_state;
  laser_connection_states prev_connection_state;
}laser_state_t;

static laser_state_t laser_state;

/* TASKS */
osThreadId_t ButtonEventHandlerTaskHandle;
const osThreadAttr_t ButtonEventHandlerTask_attributes = {
  .name = "ButtonEventHandlerTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 128 * 8
};

osThreadId_t uart1MessageHandle;
const osThreadAttr_t uart1MessageHandle_attributes = {
  .name = "uart1MessageHandle",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 8
};

osThreadId_t uart2MessageHandle;
const osThreadAttr_t uart2MessageHandle_attributes = {
  .name = "uart2MessageHandle",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 8
};

osThreadId_t uartOutMessageHandle;
const osThreadAttr_t uartOutMessageHandle_attributes = {
  .name = "uartOutMessageHandle",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 8
};

osThreadId_t cableDetectHandle;
const osThreadAttr_t cableDetectHandle_attributes = {
  .name = "cableDetectHandle",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 8
};

/* QUEUES */
osMessageQueueId_t eventQueueHandle;
const osMessageQueueAttr_t eventQueue_attributes = {
  .name = "eventQueue"
};

osMessageQueueId_t uart1messageQueueHandle;
const osMessageQueueAttr_t uart1messageQueue_attributes = {
  .name = "uart1messageQueue"
};

osMessageQueueId_t uart2messageQueueHandle;
const osMessageQueueAttr_t uart2messageQueue_attributes = {
  .name = "uart2messageQueue"
};

osMessageQueueId_t interTaskQueueHandle;
const osMessageQueueAttr_t interTaskQueue_attributes = {
  .name = "interTaskMessageQueue"
};

/* MUTECES*/
osMutexId_t messageHandler_mutex;
const osMutexAttr_t messageHandler_Mutex_attr = {
  "messageHandlerMutex",  
  osMutexPrioInherit,
  NULL,                
  0U                   
};

osMutexId_t laserState_mutex;
const osMutexAttr_t laserState_Mutex_attr = {
  "laserStateMutex",  
  osMutexPrioInherit,
  NULL,                
  0U                   
};

/* RTOS Tasks*/
void ButtonEventHandler(void *argument);
void uart1MessageHandleTask(void *argument);
void uart2MessageHandleTask(void *argument);
void uartOutMessageHandleTask(void *argument);
void cableDetectHandleTask(void *argument);

/* Simple Functions*/
static laser_state_t get_laser_state(void);
static void set_laser_state(laser_state_t state);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {

 // QUEUES
  eventQueueHandle = osMessageQueueNew (14, sizeof(button_event_t), &eventQueue_attributes);

  uart1messageQueueHandle = osMessageQueueNew (14, CB_SIZE, &uart1messageQueue_attributes);

  uart2messageQueueHandle = osMessageQueueNew (14, CB_SIZE, &uart2messageQueue_attributes);

  interTaskQueueHandle = osMessageQueueNew (14, sizeof(uart_package_t), &interTaskQueue_attributes);

  // TASKS
  ButtonEventHandlerTaskHandle = osThreadNew(ButtonEventHandler, NULL, &ButtonEventHandlerTask_attributes);

  uart1MessageHandle = osThreadNew(uart1MessageHandleTask, NULL, &uart1MessageHandle_attributes);

  uart2MessageHandle = osThreadNew(uart2MessageHandleTask, NULL, &uart2MessageHandle_attributes);

  uartOutMessageHandle = osThreadNew(uartOutMessageHandleTask, NULL, &uartOutMessageHandle_attributes);

  cableDetectHandle = osThreadNew(cableDetectHandleTask, NULL, &cableDetectHandle_attributes);
}

/* USER CODE BEGIN Header_EventHandler */
/**
* @brief Function implementing the button event handling.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_EventHandler */
void ButtonEventHandler(void *argument)
{
  osStatus_t status;
  laser_state_t laser_state_temp;
  HAL_StatusTypeDef serial_status;
  static button_event_t prev_event;
  button_event_t button_event;
  uint8_t data[4] = {0};
  for(;;)
  {
    status = osMessageQueueGet(eventQueueHandle, &button_event, NULL, osWaitForever);   // wait for message
    if (status == osOK) 
      osMutexAcquire(laserState_mutex, osWaitForever);
      laser_state_temp = get_laser_state();
      osMutexRelease(laserState_mutex);
      switch (button_event.button)
      {
      case BUTTON_RECT_PRESS:
#ifdef SW_DEBOUNCE
        if(((button_event.timestamp - prev_event.timestamp) < 10)
         && (button_event.button == prev_event.button)){
          break;
         }
#endif
        data[0]=DATA_BUTTON_RECT;
        setLaserOutput(GPIO_PIN_SET);               //LASER SWITCH ON
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      case BUTTON_RECT_REL:
#ifdef SW_DEBOUNCE
        if(((button_event.timestamp - prev_event.timestamp) < 10)
         && (button_event.button == prev_event.button)){
          break;
         }
#endif
        data[0]=DATA_BUTTON_NO;
        setLaserOutput(GPIO_PIN_RESET);             //LASER SWITCH OFF
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      case BUTTON_CIRCLE_PRESS:
        data[0]=DATA_BUTTON_CIRCLE;
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      case BUTTON_CIRCLE_REL:
        data[0]=DATA_BUTTON_NO;
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      case BUTTON_UP_PRESS:
        data[0]=DATA_BUTTON_UP;
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      case BUTTON_UP_REL:
        data[0]=DATA_BUTTON_NO;
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      case BUTTON_DOWN_PRESS:
        data[0]=DATA_BUTTON_DOWN;
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      case BUTTON_DOWN_REL:
        data[0]=DATA_BUTTON_NO;
        transmitMessage(data, 0x00, UART_THERMIS);
        break;
      default:
        break;
      prev_event = button_event;
      }
    }
    // osDelay(20);
}


/// @brief Task to handle uart thermis communication
/// @param argument 
void uart1MessageHandleTask(void *argument){
	osStatus_t status;
	// uint32_t ack_flag, wait_flag;
	uint8_t message_buffer[CB_SIZE] = {0};
  uart_receive_it(UART_THERMIS);
	for(;;){
		status = osMessageQueueGet(uart1messageQueueHandle, message_buffer, NULL, osWaitForever);
		if (status == osOK) {
      tick_Handler(message_buffer, UART_THERMIS);
		}
		// osDelay(200);
	}
}

/// @brief Task to handle uart debug communication
/// @param argument 
void uart2MessageHandleTask(void *argument){
	osStatus_t status;
	// uint32_t ack_flag, wait_flag;
	uint8_t message_buffer[CB_SIZE] = {0};
  uart_receive_it(UART_DEBUG);
	for(;;){
		status = osMessageQueueGet(uart2messageQueueHandle, message_buffer, NULL, osWaitForever);
		if (status == osOK) {
      tick_Handler(message_buffer, UART_DEBUG);
		}
		// osDelay(200);
	}
}

/// @brief Gatekeeper Task to handle uart output
/// @param argument 
void uartOutMessageHandleTask(void *argument){
  osStatus_t status;
  uart_package_t message;
  for(;;){
    status = osMessageQueueGet(interTaskQueueHandle, &message, NULL, osWaitForever);
    if (status == osOK) {
      uart_write(message.mess_buffer, FRAME_FIXED_LEN, message.uart_device, 30);
		}
    // osDelay(100);
  }
}

/// @brief Task to detect cable connection
/// @param argument 
void cableDetectHandleTask(void *argument){
  laser_state_t laser_state_temp;
  uint32_t cur_val=0;
  uint32_t prev_val=0;
  // uint8_t text[50]= {0};
  for(;;){
    enableADCinput(); 
    osDelay(5);
    cur_val = adc1_get_value(); 
    int diff = cur_val - prev_val;
    disableADCinput();
    if ((diff) > 300){
      osMutexAcquire(laserState_mutex, 100);
      laser_state.connection_state = LASER_CONNECTED;
      osMutexRelease(laserState_mutex);
    }
    if ((diff) < -300){
      osMutexAcquire(laserState_mutex, 100);
      laser_state.connection_state = LASER_DISCONNECTED;
      osMutexRelease(laserState_mutex);
    }
    prev_val = cur_val;
    // sprintf(text, "Cable State (0 Conn / 1 Disconn): %d\r\n", laser_state.connection_state);
    // uart_write_debug(text, 20);
    // memset(text, "0", sizeof(text));
    // sprintf(text, "ADC val: %f \r\n", cur_val * ADC_10B_MULT);
    // uart_write_debug(text, 20);
    osDelay(2000);
  }
}

/// @brief HOOK for IDLE TASK
/// @param  used to kick watchdog
void vApplicationIdleHook(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

/* Simple function to get the laser state from the global variable*/
static laser_state_t get_laser_state(void){
  return laser_state;
}

/* Simple function to set the laser state from the global variable*/
static void set_laser_state(laser_state_t state){
  laser_state = state;
}