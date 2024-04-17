/*
 * flash_memory.h
 *
 *  Created on: Aug 1, 2023
 *      Author: dkalaitzakis
 */

#include "main.h"

#ifndef SRC_FLASH_MEMORY_H_
#define SRC_FLASH_MEMORY_H_

#define FLASH_DATA_LEN 128

/**
 * @brief 3x3 matrix in row-major order.
 */
typedef union {
    float array[3][3];

    struct {
        float xx;
        float xy;
        float xz;
        float yx;
        float yy;
        float yz;
        float zx;
        float zy;
        float zz;
    } element;
} FusionMatrix;

/**
 * @brief 3D vector.
 * 
 */
typedef union {
    float array[3];

    struct {
        float x;
        float y;
        float z;
    } axis;
} FusionVector;

void MX_FLASH_Init(void);

void FlashReadData(uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);

void Flash_Write_NUM(uint32_t StartSectorAddress, float Num);

float Flash_Read_NUM(uint32_t StartSectorAddress);

uint32_t FlashWriteData(uint32_t StartPageAddress, uint32_t *QuadW_Data, uint16_t numberofwords);

uint32_t Flash_Write_Vector(uint32_t StartSectorAddress, FusionVector *gyro_data);

uint32_t Flash_isWritten(uint32_t StartSectorAddress);

uint32_t FlashErase(uint32_t StartPageAddress, uint32_t numberofpages);

uint32_t Flash_Read_Vector(uint32_t StartSectorAddress, FusionVector *data);

uint32_t Flash_Write_Matrix(uint32_t StartSectorAddress, FusionMatrix *data);

uint32_t Flash_Read_Matrix(uint32_t StartSectorAddress, FusionMatrix *data);

void Flash_Write_Double(uint32_t StartSectorAddress, double Num);

double Flash_Read_Double (uint32_t StartSectorAddress);

#endif /* SRC_FLASH_MEMORY_H_ */
