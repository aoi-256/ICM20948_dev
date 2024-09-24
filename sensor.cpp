// sensor.cpp
#include "sensor.h"
#include "stm32f7xx_hal.h"
#include "i2c.h"
#include <cstdint>
#include <cmath>


/*----------------------------------------------------------------------------------------*/
// 初期設定
// 下3つの関数を順番に実行して初期設定をする
void Sensor::setup(){

	if(identify()){

		activation();
		calibration();
		sensor_setting();
	}
}

//ICM20948との通信ができるかをチェックする
bool  Sensor::identify(){

	while(device_number != 0xEA){

		HAL_I2C_Mem_Read(&hi2c2, ICM20948, WHO_AM_I, 1,&device_number , 1, 10);
	}

	return true;
}

//sleepモードを解除する
void Sensor::activation(){

	uint8_t PWR_ON = 0x01;
	HAL_I2C_Mem_Write(&hi2c2, ICM20948, RWR_MGMT_1, 1,&PWR_ON, 1, 10);
}

//初期値を設定する
void Sensor::calibration(){

	float accel_calibration[3] = {};
	float gyro_calibration[3]  = {};

	for(uint16_t i=0; i < 10000; i++){

		get_accelvalue();
		get_gyrovalue();

		accel_calibration[0] += accel_value_x[0];
		accel_calibration[1] += accel_value_y[0];
		accel_calibration[2] += accel_value_z[0] - 9.8;

		gyro_calibration[0] += gyro_value_x[0];
		gyro_calibration[1] += gyro_value_y[0];
		gyro_calibration[2] += gyro_value_z[0];

	}

	accel_offset[0] = accel_calibration[0] / 10000;
	accel_offset[1] = accel_calibration[1] / 10000;
	accel_offset[2] = accel_calibration[2] / 10000;

	gyro_offset[0] = gyro_calibration[0] / 10000;
	gyro_offset[1] = gyro_calibration[1] / 10000;
	gyro_offset[2] = gyro_calibration[2] / 10000;

}

//bank(設定のチャンネルみたいなやつ）の変更
void Sensor::change_bank(uint8_t number){

	uint8_t bank_number = number - 1;
	HAL_I2C_Mem_Write(&hi2c2, ICM20948, BANK, 1,&bank_number, 1, 10);
}

void Sensor::sensor_setting(){

	uint8_t sensor_config = 0x03;
	change_bank(2);
	HAL_I2C_Mem_Write(&hi2c2, ICM20948, GYRO_CONFIG, 1,&sensor_config, 1, 10);
	change_bank(1);

}
/*----------------------------------------------------------------------------------------*/
// 値の取得 キャリブレーション分の値の補正をoffsetで行っている

void Sensor::get_accelvalue(){

	while(accel_value_x[0] == accel_value_x[1]){

		HAL_I2C_Mem_Read(&hi2c2, ICM20948, ACCEL, 1, accel_buffer, 6, 10);

		accel_value_x[0] = ((int16_t)(accel_buffer[0] << 8 | accel_buffer[1]))/ accel_sensitivity * 9.8 - accel_offset[0];
		accel_value_y[0] = ((int16_t)(accel_buffer[2] << 8 | accel_buffer[3]))/ accel_sensitivity * 9.8 - accel_offset[1];
		accel_value_z[0] = ((int16_t)(accel_buffer[4] << 8 | accel_buffer[5]))/ accel_sensitivity * 9.8 - accel_offset[2];
	}
}

void Sensor::get_gyrovalue(){

	while(gyro_value_x[0] == gyro_value_x[1]){

		HAL_I2C_Mem_Read(&hi2c2, ICM20948, GYRO, 1, gyro_buffer, 6, 10);

		gyro_value_x[0] = ((int16_t)(gyro_buffer[0] << 8 | gyro_buffer[1]))/ gyro_sensitivity - gyro_offset[0];
		gyro_value_y[0] = ((int16_t)(gyro_buffer[2] << 8 | gyro_buffer[3]))/ gyro_sensitivity - gyro_offset[1];
		gyro_value_z[0] = ((int16_t)(gyro_buffer[4] << 8 | gyro_buffer[5]))/ gyro_sensitivity - gyro_offset[2];
	}

}

/*----------------------------------------------------------------------------------------*/
// LPF(ローパスフィルター）の関数　平均化をしているだけ

void Sensor::accel_LPF(){

	if(loop_count >= 5){

			accel_fliter_x[0] = (accel_value_x[0]*5 + accel_value_x[1]*4 + accel_value_x[2]*3 + accel_value_x[3]*2 + accel_value_x[4]*1) / 15;
			accel_fliter_y[0] = (accel_value_y[0]*5 + accel_value_y[1]*4 + accel_value_y[2]*3 + accel_value_y[3]*2 + accel_value_y[4]*1) / 15;
			accel_fliter_z[0] = (accel_value_z[0]*5 + accel_value_z[1]*4 + accel_value_z[2]*3 + accel_value_z[3]*2 + accel_value_z[4]*1) / 15;
	}
}

void Sensor::gyro_LPF(){

	if(loop_count >= 5){

			gyro_fliter_x[0] = (gyro_value_x[0]*5 + gyro_value_x[1]*4 + gyro_value_x[2]*3 + gyro_value_x[3]*2 + gyro_value_x[4]*1) / 15;
			gyro_fliter_y[0] = (gyro_value_y[0]*5 + gyro_value_y[1]*4 + gyro_value_y[2]*3 + gyro_value_y[3]*2 + gyro_value_y[4]*1) / 15;
			gyro_fliter_z[0] = (gyro_value_z[0]*5 + gyro_value_z[1]*4 + gyro_value_z[2]*3 + gyro_value_z[3]*2 + gyro_value_z[4]*1) / 15;
	}
}

/*----------------------------------------------------------------------------------------*/
// 積分と値の保存

void Sensor::estimate_angle(){

	if(loop_count >= 2){

		integral_gyro[0] += (gyro_fliter_x[0] + gyro_fliter_x[1])* time / 2;
		integral_gyro[1] += (gyro_fliter_y[0] + gyro_fliter_y[1])* time / 2;
		integral_gyro[2] += (gyro_fliter_z[0] + gyro_fliter_z[1])* time / 2;

//		arctan_accel[0] = atan2(accel_fliter_y[0], accel_fliter_z[0]) * 180 / M_PI;
//		arctan_accel[1] = atan2(accel_fliter_x[0], accel_fliter_z[0]) * 180 / M_PI;
//		arctan_accel[2] = atan2(accel_fliter_y[0], accel_fliter_x[0]) * 180 / M_PI;

		est_agnle[0] = integral_gyro[0]*0.98 + arctan_accel[0] * 0.02;
		est_agnle[1] = integral_gyro[1]*0.98 + arctan_accel[1] * 0.02;
		est_agnle[2] = integral_gyro[2]*0.98 + arctan_accel[2] * 0.02;
	}
}

void Sensor::store_value(){

		//gyro_value
		gyro_value_x[4] = gyro_value_x[3];
		gyro_value_x[3] = gyro_value_x[2];
		gyro_value_x[2] = gyro_value_x[1];
		gyro_value_x[1] = gyro_value_x[0];

		gyro_value_y[4] = gyro_value_y[3];
		gyro_value_y[3] = gyro_value_y[2];
		gyro_value_y[2] = gyro_value_y[1];
		gyro_value_y[1] = gyro_value_y[0];

		gyro_value_z[4] = gyro_value_z[3];
		gyro_value_z[3] = gyro_value_z[2];
		gyro_value_z[2] = gyro_value_z[1];
		gyro_value_z[1] = gyro_value_z[0];

		//accel_value
		accel_value_x[4] = accel_value_x[3];
		accel_value_x[3] = accel_value_x[2];
		accel_value_x[2] = accel_value_x[1];
		accel_value_x[1] = accel_value_x[0];

		accel_value_y[4] = accel_value_y[3];
		accel_value_y[3] = accel_value_y[2];
		accel_value_y[2] = accel_value_y[1];
		accel_value_y[1] = accel_value_y[0];

		accel_value_z[4] = accel_value_z[3];
		accel_value_z[3] = accel_value_z[2];
		accel_value_z[2] = accel_value_z[1];
		accel_value_z[1] = accel_value_z[0];

		//gyro_fliter

		gyro_fliter_x[1] = gyro_fliter_x[0];
		gyro_fliter_y[1] = gyro_fliter_y[0];
		gyro_fliter_z[1] = gyro_fliter_z[0];

		//accel_fliter
		accel_fliter_x[1] = accel_fliter_x[0];
		accel_fliter_y[1] = accel_fliter_y[0];
		accel_fliter_z[1] = accel_fliter_z[0];

		loop_count ++;
}

