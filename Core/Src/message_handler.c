/*
/******************************************************************************
 ** @file          : message_handler.c
 * @brief          : 
/******************************************************************************
 * Created on Thu Apr 11 2024 by Dimitris Kalaitzakis
 *
 * Copyright (c) 2024 Theon
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message_handler.h"
#include "uart.h"
#include "main.h"
#include "cmsis_os2.h"

static message_t init_message(message_t msg);
static message_t parseMessage(uint8_t *data);
static uint8_t calcChecksum(message_t msg);
static message_t init_message(message_t msg);
static void handler(message_t msg, UART_select device);

extern osEventFlagsId_t ack_rcvd;
extern osEventFlagsId_t wait_for_ack;

// uart output queue
extern osMessageQueueId_t interTaskQueueHandle;

uart_package_t output_message;

uint8_t flag_connected_toIris = 0;

// static message_t msg;

void tick_Handler(uint8_t *data, UART_select device){
    message_t msg;
    init_message(msg);
    msg = parseMessage(data);
    if (!msg.status){
		handler(msg, device);
	}
}

uint8_t send_heartbeat(UART_select device){
    uint8_t data[1] = {0};
    return transmitMessage(data, CMD_HEARTBEAT, device);
}

uint8_t transmitMessage(uint8_t *data, uint8_t cmd, UART_select device){
    uint8_t message[14];
    message_t msg;
    uint8_t i,index;

    msg.cmd = cmd;
    memcpy(msg.data, data, 4);
    msg.senderID = THERMIS_ADDR;
    msg.checksum = calcChecksum(msg);
    msg.status = 0;
    //CREATE MESSAGE
    index = 0;
    message[index++] = STX;
    message[index++] = msg.senderID;
    message[index++] = msg.cmd;
    for(i=0; i<4; i++){
        message[index++] = msg.data[i];
    }
    message[index++] = msg.checksum;
    message[index++] = ETX;
    //Load message to output buffer
    memcpy(output_message.mess_buffer, message, sizeof(message));
    output_message.uart_device = device;
    osMessageQueuePut(interTaskQueueHandle, &output_message, 0, 100);
    // uart_write(message, FRAME_FIXED_LEN, device, 20);
    return 1;
}

static uint8_t calcChecksum(message_t msg){
    msg.checksum ^= msg.senderID;

    msg.checksum ^= msg.cmd;
    uint8_t i;
    for (i=0; i < 4; i++)
    {
       msg.checksum ^= msg.data[i];
    }
    return msg.checksum;
}


static message_t parseMessage(uint8_t *data){
    message_t msg;
    uint8_t chsum = 0;
    uint8_t len = data[0];
    if (len < 7){
        // sendNack(device);
        msg.status = 1;
        return msg;
    }
    msg.senderID = data[1];
    msg.cmd = data[2];
    uint8_t i;
    for (i=0; i<4; i++){
        msg.data[i] = data[3+i];
    }

    msg.checksum = calcChecksum(msg);
    chsum = data[7];
    // if(msg.checksum == chsum){
    //     msg.status = 0;
    // }
    msg.status = 0;
    return msg;                  //Note that after the parsing the escape chars remains in payload.
}



static void handler(message_t msg, UART_select device){
    switch (msg.cmd){
        case CMD_BUTTON_STATUS_REQ:
            break;
        case CMD_FW_VERSION_REQ:
            reportFW(CMD_FW_VERSION_REPLY, device);
            break;
        case CMD_UID_REQ:
            sendMCUuID(CMD_UID_REQ, device);
            break;
        default:
            break;
    }
}

static message_t init_message(message_t msg){
    msg.senderID = 0;
    msg.cmd = 0;
    memset(msg.data, 0, sizeof(msg.data));
    msg.checksum = 0;
    msg.status = 0;
    return msg;
}

void reportFW(uint8_t cmd, UART_select device){
    uint8_t fwv[4] = {0,0,0,FW_VERSION};
    transmitMessage(fwv, cmd, device);
}

void sendMCUuID(uint8_t cmd, UART_select device){
    uint32_t uid[3] = {
        HAL_GetUIDw0(),
        HAL_GetUIDw1(),
        HAL_GetUIDw2()
    }; 
    uint8_t i;   
    for (i=0; i<3; i++){
       uint8_t mes[4] = {((uid[i] & 0xFF000000) >> 24),
            ((uid[i] & 0x00FF0000) >> 16),
            ((uid[i] & 0x0000FF00) >> 8),
            (uid[i] & 0x000000FF)
        };
        transmitMessage(mes, cmd, device);
    }
}





