/**
 * @file circular_buffer.c
 * @author Dimitris Kalaitzakis (dkalaitzakis@theon.com)
 * @brief
 * @version 0.1
 * @date 2022-06-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "circular_buffer.h"
#include <string.h>
#include "assert.h"

uint8_t CB_init(CB_t* cb, uint8_t size) {
  memset(cb->buffer, 0, sizeof(uint8_t)*size);
//  cb->buffer = malloc(size * sizeof(char));
//
  if (!cb->buffer) {
    return 0;
  }
  cb->rdpos = 0;
  cb->wrpos = 0;
  cb->size = size;

  return 1;
}

void CB_deinit(CB_t* cb) {
  cb->size = 0;
  cb->rdpos = 0;
  cb->wrpos = 0;
  memset(cb->buffer, 0, sizeof(cb->buffer));
}

void CB_push(CB_t* cb, uint8_t byte) {
  if ((cb->wrpos + 1) % cb->size == cb->rdpos) {
    return;
  }

  cb->buffer[cb->wrpos] = byte;
  cb->wrpos = (cb->wrpos + 1) % cb->size;
}

uint8_t CB_pop(CB_t* cb) {
  if (cb->rdpos == cb->wrpos) return EOF;

  uint8_t byte = cb->buffer[cb->rdpos];
  cb->rdpos = (cb->rdpos + 1) % cb->size;

  return byte;
}

void CB_pushFront(CB_t* cb, uint8_t byte) {
  uint8_t newrpos = (cb->rdpos - 1) < 0 ? (cb->size - 1) : cb->rdpos - 1;

  if (newrpos == cb->wrpos) {
    return;
  }

  cb->rdpos = newrpos;
  cb->buffer[cb->rdpos] = byte;
}

uint8_t CB_popBack(CB_t* cb) {
  if (cb->rdpos == cb->wrpos) return EOF;

  cb->wrpos = (cb->wrpos - 1) < 0 ? (cb->size - 1) : (cb->wrpos - 1);
  return cb->buffer[cb->wrpos];
}

uint8_t CB_capacity(CB_t* cb) { return cb->size; }

uint8_t CB_size(CB_t* cb) {
  return (cb->wrpos + cb->size - cb->rdpos) % cb->size;
}

uint8_t CB_isEmpty(CB_t* cb) { return (cb->rdpos == cb->wrpos); }

uint8_t CB_isFull(CB_t* cb) {
  return ((cb->wrpos + 1) % cb->size == cb->rdpos);
}

void CB_clear(CB_t* cb) {
  memset(cb->buffer, 0, sizeof(cb->buffer));
  cb->rdpos = 0;
  cb->wrpos = 0;
}
