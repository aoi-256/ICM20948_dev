/*
 * ICM20948.h
 *
 *  Created on: Feb 26, 2025
 *      Author: Sezakiaoi
 */

#ifndef INC_ICM20948_H_
#define INC_ICM20948_H_

#include "ICM20948_HAL.h"

class ICM20948: private ICM20948_HAL{

	public:

		enum class Accel_Scale: uint8_t{

			scale_2g = 0x00,
			scale_4g,
			scale_8g,
			scale_16g
		};

		enum class Gyro_Scale: uint8_t{

			dps_0250 = 0x00,
			dps_0500,
			dps_1000,
			dps_2000
		};

		enum class FCHOICE: uint8_t{

			Bypass = 0x00,
		    Enable = 0x01
		};

		enum class DLPFCFG: uint8_t{

			Setting_0 = 0x00,
			Setting_1 = 0x01,
			Setting_2 = 0x02,
			Setting_3 = 0x03,
			Setting_4 = 0x04,
			Setting_5 = 0x05,
			Setting_6 = 0x06,
			Setting_7 = 0x07
		};

		enum class Gyro_Ave: uint8_t{

			x1   = 0x00,
			x2   = 0x01,
			x4   = 0x02,
			x8   = 0x03,
			x16  = 0x04,
			x32  = 0x05,
			x64  = 0x06,
			x128 = 0x07
		};

		enum class Accel_Ave: uint8_t{

			x1  = 0x00,
			x4  = 0x0F,
			x8  = 0x01,
			x16 = 0x02,
			x32 = 0x03
		};

		ICM20948(I2C_HandleTypeDef *I2C_Pin);
		uint8_t Connection();
		uint8_t Activation();
		uint8_t Accel_Config(ICM20948::Accel_Scale, ICM20948::FCHOICE, ICM20948::DLPFCFG, ICM20948::Accel_Ave);
		uint8_t Gyro_Config(ICM20948::Gyro_Scale, ICM20948::FCHOICE, ICM20948::DLPFCFG, ICM20948::Gyro_Ave);
		uint8_t Get_Data(int16_t Accel_Data[3], int16_t Gyro_Data[3]);
		uint8_t Select_Bank(uint8_t Bank_Channel);

	private:

		uint8_t Raw_Data[12];

};

#endif /* INC_ICM20948_H_ */
