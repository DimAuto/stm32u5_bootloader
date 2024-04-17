/*
/******************************************************************************
 ** @file          : message_handler.h
 * @brief          : 
/******************************************************************************
 * Created on Thu Apr 11 2024 by Dimitris Kalaitzakis
 *
 * Copyright (c) 2024 Theon
 */


#ifndef SRC_MESSAGE_HANDLER_H_
#define SRC_MESSAGE_HANDLER_H_

#include <stdint.h>
#include <stdlib.h>
#include "uart.h"
#include "circular_buffer.h"

#define SBP_S_ID            0x32
#define COMM_PROTOCOL_REV   0x01
#define PROTOCOL_VER        0xFE
#define MAX_RETRIES         0x03

#define STX 	0x02
#define ETX 	0x03
#define ESC 	0x1B
#define NACK 	0x15
#define ACK		0x06

#define THERMIS_ADDR 0x53

#define FRAME_FIXED_LEN 9

// THERMIS COMMANDS
#define CMD_HEARTBEAT           0x51
#define CMD_BUTTON_STATUS       0x00
#define CMD_BUTTON_STATUS_REQ   0x42
#define CMD_BUTTON_STATUS_REPLY 0x01
#define CMD_FW_VERSION_REQ      0x56
#define CMD_FW_VERSION_REPLY    0x55
#define CMD_FW_TEST_REQ         0x58
#define CMD_FW_TEST_REPLY       0x57
#define CMD_FW_UPDATE_REQ       0x4e

#define CMD_UID_REQ             0x6a

#define DATA_BUTTON_NO          0x00
#define DATA_BUTTON_UP          0x10
#define DATA_BUTTON_DOWN        0x20
#define DATA_BUTTON_CIRCLE      0x40
#define DATA_BUTTON_RECT        0x80



#define MESSAGE_RETRANSMIT_INTERVAL 300


typedef struct{
    uint8_t senderID;
    uint8_t cmd;
    uint8_t data[4];
    uint8_t checksum;
    uint8_t status;
}message_t;

typedef struct{
    uint8_t mess_buffer[14];
    UART_select uart_device; 
}uart_package_t;

/**
 * Ticks the message handler
 */
void tick_Handler(uint8_t *data, UART_select device);

/**
 * Sends a two byte NACK
 * @param device: the desired uart interface
 */
uint8_t sendNack(UART_select device);

/**
 * Sends a two byte ACK
 * @param device: the desired uart interface
 */
uint8_t sendAck(UART_select device);

/**
 * Transmits a message of the required form.
 * @param msg: pointer to the message_t struct.
 * @param data: pointer to the data to be sent
 * @param data_len: the length of the data to be send.
 * @param device: the desired uart interface.
 */
uint8_t transmitMessage(uint8_t *data, uint8_t cmd, UART_select device);

void reportFW(uint8_t cmd, UART_select device);

uint16_t get_heartbeat_timeout(void);

uint8_t send_heartbeat(UART_select device);

#endif /* SRC_MESSAGE_HANDLER_H_ */
