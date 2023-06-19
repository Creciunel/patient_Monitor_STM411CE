/*
 * bitMap.h
 *
 *  Created on: Apr 6, 2023
 *      Author: ICG
 */

#ifndef INC_BITMAP_H_
#define INC_BITMAP_H_

#include "main.h"

void checkBTN(uint8_t state, uint8_t encoder);
void Display1SHT3X_Init(uint16_t temperature, uint16_t humidity);
void Display1SHT3X(uint16_t temperature, uint16_t humidity);
void Display2Gas_Init(uint16_t MQ3, uint16_t MQ4, uint16_t MQ7, uint16_t MQ135);
void Display2Gas(uint16_t MQ3, uint16_t MQ4, uint16_t MQ7, uint16_t MQ135);
void Display3MLX_Init(float temp);
void Display3MLX(float temp);
#endif /* INC_BITMAP_H_ */
