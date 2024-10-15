/*
 * AK09916HALI2C.h
 *
 *  Created on: Feb 17, 2024
 *      Author: heila
 */

#ifdef USE_HAL_DRIVER

#ifndef INC_AK09916_HAL_I2C_H_
#define INC_AK09916_HAL_I2C_H_

#include "AK09916.h"
#include "i2c.h"

class AK09916_HAL_I2C : public AK09916{
public:
	enum class ub3{
		I2C_SLV0_ADDR=0x03,
		I2C_SLV0_REG,
		I2C_SLV0_CTRL,
		I2C_SLV0_DO
	};

    AK09916_HAL_I2C(I2C_HandleTypeDef *hi2c, I2C_ADDRESS address);

    uint8_t readByte(REGISTER reg);
    uint8_t readByte(uint8_t reg);
    void writeByte(REGISTER reg, uint8_t value);
    void readBytes(REGISTER reg, uint8_t *pData, uint8_t size=1);

private:
    I2C_HandleTypeDef *hi2c;
    uint8_t i2cAddress;
    void selectUserBank(uint8_t bank);
};

#endif /* INC_AK09916_HAL_I2C_H_ */

#endif /* USE_HAL_DRIVER */
