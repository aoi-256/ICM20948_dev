// sensor.h
#ifndef SENSOR_H
#define SENSOR_H

#include <cstdint>

class Sensor {

public:

	//センサーの起動と初期化
	void setup();
	//setup()の中で使用される関数
	bool identify();
	void activation();
	void calibration();

	void change_bank(uint8_t number);
	void get_accelvalue();
	void get_gyrovalue();
	void sensor_setting();

	void gyro_LPF();
	void accel_LPF();

	void estimate_angle();
	void store_value();

private:

	/*角速度関係について表記が揺れてわかりにくい
	 * 値の取得までgyro_OOOO
	 * 値の取得後 angle_OOOO
	 *
	 */

	const uint8_t ICM20948 =  0x68 << 1;

	//bank_1
	const uint8_t WHO_AM_I   = 0x00;
	const uint8_t RWR_MGMT_1 = 0x06;
	const uint8_t GYRO		 = 0x33;
	const uint8_t ACCEL      = 0x2D;
	const uint8_t BANK       = 0xFF;

	//bank_2
	const uint8_t GYRO_CONFIG = 0x01;

	//data_buffer
	uint8_t device_number      = {};
	uint8_t accel_buffer[6]    = {};
	uint8_t gyro_buffer[6]     = {};

	//生のデータ
	float   accel_value_x[5]   = {};
	float   accel_value_y[5]   = {};
	float   accel_value_z[5]   = {};
	float   gyro_value_x[5]    = {};
	float   gyro_value_y[5]    = {};
	float   gyro_value_z[5]    = {};

	//補正後のデータ
	float accel_fliter_x[2]          = {};
	float accel_fliter_y[2]          = {};
	float accel_fliter_z[2]          = {};
	float gyro_fliter_x[2]           = {};
	float gyro_fliter_y[2]           = {};
	float gyro_fliter_z[2]           = {};

	//積分した値
	float integral_gyro[3]           = {};
	float arctan_accel[3]			 = {};
	float est_agnle[3]			     = {};

	//定数
	float time = 1.0 / 1100;

	float   accel_offset[3]    = {};
	float   gyro_offset[3]     = {};

	float   accel_sensitivity  = 16384.0;
	float   gyro_sensitivity   = 32768.0/250;

	uint32_t loop_count        = 0;
	uint16_t debug_count       = 0;
};


#endif // SENSOR_H
