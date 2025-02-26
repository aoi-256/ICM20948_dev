/*
 * ICM20948_HAL.h
 *
 *  Created on: Feb 26, 2025
 *      Author: Sezakiaoi
 */

#ifndef INC_ICM20948_HAL_H_
#define INC_ICM20948_HAL_H_

#include <cstdint>
#include "i2c.h"

class ICM20948_HAL{

	public:

	enum class REGISTER: uint8_t{

		//BANK0
		WHO_AM_I = 0x00,
		PWR_MGMT_1 = 0x06,
		ACCEL_XOUT_H = 0x2D,
		REG_BANK_SEL = 0x7F,

		//BANK2
		GYRO_CONFIG_1 = 0x01,
		GYRO_CONFIG_2 = 0x02,
		ACCEL_CONFIG_1 = 0x14,
		ACCEL_CONFIG_2 = 0x15
	};

	ICM20948_HAL(I2C_HandleTypeDef *I2C_Pin);

	uint8_t ICM20948_Write(ICM20948_HAL::REGISTER REG_ADDR, uint8_t* Command, uint8_t len);
	uint8_t ICM20948_Read(ICM20948_HAL::REGISTER REG_ADDR, uint8_t* Buffer, uint8_t len);

	private:

	uint8_t I2C_ADDR = 0b1101000;
	I2C_HandleTypeDef* I2C_Pin;
};

#endif /* INC_ICM20948_HAL_H_ */
