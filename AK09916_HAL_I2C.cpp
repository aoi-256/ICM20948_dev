/*
 * AK09916HALI2C.cpp
 *
 *  Created on: Feb 17, 2024
 *      Author: heila
 */

#ifdef USE_HAL_DRIVER

#include <AK09916_HAL_I2C.h>
#include <array>

AK09916_HAL_I2C::AK09916_HAL_I2C(I2C_HandleTypeDef *hi2c, I2C_ADDRESS address)
:hi2c(hi2c),i2cAddress((uint8_t)address){
	AK09916();
}

void AK09916_HAL_I2C::selectUserBank(uint8_t bank){
	//if(bank>3){
		//return;
	//}
	bank=bank<<4;
	HAL_I2C_Mem_Write(hi2c, i2cAddress<<1, 0x7F, 1, &bank, 1, 100);
}

uint8_t AK09916_HAL_I2C::readByte(REGISTER reg){
	return readByte((uint8_t)reg);
}

uint8_t AK09916_HAL_I2C::readByte(uint8_t reg){
	uint8_t res=0;
	std::array<uint8_t, 3> ub3s={};
	ub3s.at(0)= (0b1<<7) + 0x0C;
	ub3s.at(1)=(uint8_t)reg;
	ub3s.at(2)=0b10000001;
	selectUserBank(3);
	HAL_I2C_Mem_Write(hi2c, i2cAddress<<1, (uint8_t)ub3::I2C_SLV0_ADDR, 1, ub3s.data(), 3, 100);

	//std::array<uint8_t, 3> tmp={};
	//HAL_I2C_Mem_Read(hi2c, i2cAddress<<1, (uint8_t)ub3::I2C_SLV0_ADDR, 1, tmp.data(), 3, 100);

	HAL_Delay(1);
	selectUserBank(0);
	HAL_I2C_Mem_Read(hi2c, i2cAddress<<1, 0x3B, 1, &res, 1, 100);
	return res;
}

//I tried to write code of writeByte
void AK09916_HAL_I2C::writeByte(REGISTER reg, uint8_t value){
	std::array<uint8_t, 4> ub3s={};
	ub3s.at(0)= 0x0C | 0x00;	//addr
	ub3s.at(1)=(uint8_t)reg;	//reg
	ub3s.at(2)=0x81;			//ctrl
	ub3s.at(3)=value;			//data out
	selectUserBank(3);
	HAL_I2C_Mem_Write(hi2c, i2cAddress<<1, (uint8_t)ub3::I2C_SLV0_ADDR, 1, ub3s.data(), 2, 100);
	HAL_I2C_Mem_Write(hi2c, i2cAddress<<1, (uint8_t)ub3::I2C_SLV0_DO, 1, ub3s.data()+3, 1,100);
	HAL_I2C_Mem_Write(hi2c, i2cAddress<<1, (uint8_t)ub3::I2C_SLV0_CTRL, 1, ub3s.data()+2, 1,100);
	HAL_Delay(1);
	selectUserBank(0);
}

void AK09916_HAL_I2C::readBytes(REGISTER reg, uint8_t *pData, uint8_t size){
	std::array<uint8_t, 3> ub3s={};
	ub3s.at(0)=(0x01<<7)+0x0C;
	ub3s.at(1)=(uint8_t)reg;
	ub3s.at(2)=0b10000000+size;
	selectUserBank(3);
	HAL_I2C_Mem_Write(hi2c, i2cAddress<<1, (uint8_t)ub3::I2C_SLV0_ADDR, 1, ub3s.data(), 3, 100);
	HAL_Delay(1);
	selectUserBank(0);
	HAL_I2C_Mem_Read(hi2c, i2cAddress<<1, 0x3B, 1, pData, size, 100);
}

#endif
