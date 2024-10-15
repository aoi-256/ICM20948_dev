/*
 * ICM20948.h
 *
 *  Created on: Oct 12, 2024
 *      Author: Sezakiaoi
 */

#ifndef ICM20948_H
#define ICM20948_H

#include <cstdint>
#include "i2c.h"

class ICM20948 {

public:

	//センサーの起動と初期化
	void setup();
	//setup()の中で使用される関数
	bool identify();
	void activation();
	void calibration();
	void sensor_setting();

	//値の取得に使用する関数 floatでオイラー角を出力
	void get_value(float* accel_buffer, float* gyro_buffer);

	//bankの変更
	void change_bank(uint8_t bank);

private:

	const uint8_t ICM20948 =  0x68 << 1;

	//bank_1
	const uint8_t WHO_AM_I   = 0x00;
	const uint8_t RWR_MGMT_1 = 0x06;
	const uint8_t GYRO		 = 0x33;
	const uint8_t ACCEL      = 0x2D;
	const uint8_t BANK       = 0x7F;

	//bank_2
	const uint8_t GYRO_CONFIG = 0x01;
	const uint8_t ACEEL_CONFIG = 0x14;

	//data_buffer
	uint8_t data_buffer[12]; //ICM20948から取得した値を入れる(0~5: 加速度/ 6~11: 角速度)
	uint8_t pre_data[3] = {};//直前の値を保存する

	float   accel_offset[3]    = {};
	float   gyro_offset[3]     = {};

	float   accel_sensitivity  = 8192.0;
	float   gyro_sensitivity   = 32768.0 / 2000;
};


#endif /* SRC_ICM20948_H_ */
