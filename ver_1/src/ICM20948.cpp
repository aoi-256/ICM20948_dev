/*
 * ICM20948.cpp
 *
 *  Created on: Oct 12, 2024
 *      Author: Sezakiaoi
 */

#include "ICM20948.h"
#include <math.h>
#include "gpio.h"

/*----------------------------------------------------------------------------------------*/
// 初期設定
// 下3つの関数を順番に実行して初期設定をする
void ICM20948::setup(){

	change_bank(0);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

	if(identify()){

		activation();
		sensor_setting();

		calibration();

		change_bank(0);
	}

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
}

//ICM20948との通信ができるかをチェックする
bool  ICM20948::identify(){

	uint8_t device_number = 0;

	while(device_number != 0xEA){

		HAL_I2C_Mem_Read(&hi2c2, ICM20948, WHO_AM_I, 1,&device_number, 1, 100);
	}

	return true;
}

//sleepモードを解除する
void ICM20948::activation(){

	uint8_t PWR_ON = 0x01;
	HAL_I2C_Mem_Write(&hi2c2, ICM20948, RWR_MGMT_1, 1,&PWR_ON, 1, 10);
}

//初期値を設定する
void ICM20948::calibration(){

	float accel_buffer[3] = {};
	float gyro_buffer[3]  = {};

	float accel_sum[3] = {};
	float gyro_sum[3]  = {};

	uint16_t loop = 10000;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

	for(uint16_t i=0; i < loop; i++){

		get_value(accel_buffer, gyro_buffer);

		accel_sum[0] += accel_buffer[0];
		accel_sum[1] += accel_buffer[1];
		accel_sum[2] += accel_buffer[2] - 9.8; //重量加速度の分は消してほしくないので引いておく

		gyro_sum[0] += gyro_buffer[0];
		gyro_sum[1] += gyro_buffer[1];
		gyro_sum[2] += gyro_buffer[2];

		if(i % 1000 == 0 || i % 2000 != 0){

			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);
		}
		else if(i % 2000 == 0){

			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_RESET);
		}

	}

	accel_offset[0] = accel_sum[0] / loop;
	accel_offset[1] = accel_sum[1] / loop;
	accel_offset[2] = accel_sum[2] / loop;

	gyro_offset[0] = gyro_sum[0] / loop;
	gyro_offset[1] = gyro_sum[1] / loop;
	gyro_offset[2] = gyro_sum[2] / loop;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
}

//センサーの感度（解像度）の設定
void ICM20948::sensor_setting(){

	change_bank(2);
	uint8_t command = 0b00110111;
	uint8_t command_fb = 0;

	while(command != command_fb){

		change_bank(2);
		HAL_I2C_Mem_Write(&hi2c2, ICM20948, GYRO_CONFIG, 1,&command, 1, 10);
		HAL_I2C_Mem_Read(&hi2c2, ICM20948, GYRO_CONFIG, 1,&command_fb, 1, 10);
	}

	command = 0b00110011;
	command_fb = 0;

	while(command != command_fb){

		HAL_I2C_Mem_Write(&hi2c2, ICM20948, ACEEL_CONFIG, 1,&command, 1, 10);
		HAL_I2C_Mem_Read(&hi2c2, ICM20948, ACEEL_CONFIG, 1,&command_fb, 1, 10);
	}
	change_bank(0);

}

/*----------------------------------------------------------------------------------------*/

//bank(設定のチャンネルみたいなやつ）の変更
void ICM20948::change_bank(uint8_t bank){

	uint8_t userbank = bank <<4;
	HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x7F, 1, &userbank, 1, 100);
}

/*----------------------------------------------------------------------------------------*/

// 値の取得 キャリブレーション分の値の補正をoffsetで行っている →オイラー角での出力
//　0~360°を扱いたいので、%360をしている

void ICM20948::get_value(float* accel_buffer, float* gyro_buffer){

		//直前の値と比較し、データの更新を待機する
	    //3軸の下位ビットがすべて一致するかを確認する

		while(data_buffer[1] == pre_data[0] && data_buffer[3] == pre_data[1] && data_buffer[5] == pre_data[2]){

			HAL_I2C_Mem_Read(&hi2c2, ICM20948, ACCEL, 1, data_buffer, 12, 100);
		}

		accel_buffer[0] = (((int16_t)(data_buffer[0] << 8  | data_buffer[1] )) / accel_sensitivity * 9.8) - accel_offset[0];
		accel_buffer[1] = (((int16_t)(data_buffer[2] << 8  | data_buffer[3] )) / accel_sensitivity * 9.8) - accel_offset[1];
		accel_buffer[2] = (((int16_t)(data_buffer[4] << 8  | data_buffer[5] )) / accel_sensitivity * 9.8) - accel_offset[2];

		gyro_buffer[0]  = ((int16_t)(data_buffer[6] << 8  | data_buffer[7] )) / gyro_sensitivity - gyro_offset[0];
		gyro_buffer[1]  = ((int16_t)(data_buffer[8] << 8  | data_buffer[9] )) / gyro_sensitivity - gyro_offset[1];
		gyro_buffer[2]  = ((int16_t)(data_buffer[10] << 8 | data_buffer[11])) / gyro_sensitivity - gyro_offset[2];

		//値の保存

		pre_data[0] = data_buffer[1];
		pre_data[1] = data_buffer[3];
		pre_data[2] = data_buffer[5];
}

/*----------------------------------------------------------------------------------------*/
