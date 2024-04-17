/**
 * @file circular_buffer.h
 * @author Dimitris Kalaitzakis (dkalaitzakis@theon.com)
 * @brief
 * @version 0.1
 * @date 2022-06-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdint.h>
#include <stdio.h>

#define EOF -1
#define CB_SIZE 40


/**
 * Ring buffer structure.
 */
typedef struct {
  uint8_t buffer[CB_SIZE];
  volatile int rdpos;
  volatile int wrpos;
  uint8_t size;
} CB_t;


uint8_t CB_init(CB_t* cb, uint8_t size);


void CB_deinit(CB_t* cb);


void CB_push(CB_t* cb, uint8_t byte);


uint8_t CB_pop(CB_t* cb);


void CB_pushFront(CB_t* cb, uint8_t byte);


uint8_t CB_popBack(CB_t* cb);


uint8_t CB_capacity(CB_t* cb);


uint8_t CB_size(CB_t* cb);


uint8_t CB_isEmpty(CB_t* cb);


uint8_t CB_isFull(CB_t* cb);


void CB_clear(CB_t* cb);

#endif
