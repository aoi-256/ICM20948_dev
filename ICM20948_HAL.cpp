/*
 * ICM20948_HAL.cpp
 *
 *  Created on: Feb 26, 2025
 *      Author: Sezakiaoi
 */

#include <ICM20948_HAL.h>

ICM20948_HAL::ICM20948_HAL(I2C_HandleTypeDef *I2C_Pin){

	this->I2C_Pin = I2C_Pin;
}

uint8_t ICM20948_HAL::ICM20948_Write(ICM20948_HAL::REGISTER REG_ADDR, uint8_t* Command, uint8_t len){

	return HAL_I2C_Mem_Write(I2C_Pin, I2C_ADDR << 1, uint8_t(REG_ADDR), 1, Command, len, 100);
}

uint8_t ICM20948_HAL::ICM20948_Read(ICM20948_HAL::REGISTER REG_ADDR, uint8_t* Buffer, uint8_t len){

	return HAL_I2C_Mem_Read(I2C_Pin, I2C_ADDR << 1, uint8_t(REG_ADDR), 1, Buffer, len, 100);
}
