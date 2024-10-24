/*
 * ICM20948.cpp
 *
 *  Created on: Oct 23, 2024
 *      Author: Sezakiaoi
 */

#include <ICM20948.h>

// sensor.cpp
#include "ICM20948.h"
#include "stm32f7xx_hal.h"
#include "i2c.h"
#include <cstdint>
#include <cmath>
#include <string>
#include "usart.h"

/*----------------------------------------------------------------------------------------*/

void ICM20948::icm20948_read(uint8_t register_address, uint8_t* buffer , uint8_t len){

	HAL_I2C_Mem_Read(&hi2c2, ICM20948, register_address, 1, buffer, len, 100);
}

void ICM20948::icm20948_write(uint8_t register_address, uint8_t* data , uint8_t len){

	HAL_I2C_Mem_Write(&hi2c2, ICM20948, register_address, 1, data, len, 100);
}

void ICM20948::AK09916_read(uint8_t register_address, uint8_t* buffer , uint8_t len){

	uint8_t command[3] = {};

	command[0] = (0b1<<7) + 0x0C;
	command[1] = (uint8_t)register_address;
	command[2] = 0b10000000 + len;


	selectUserBank(3);
	icm20948_write(I2C_SLV0_ADDR, command, 3);

	selectUserBank(0);
	icm20948_read(0x3B, buffer, len);
}

void ICM20948::AK09916_write(uint8_t register_address, uint8_t data){
	//write only 1byte

	uint8_t command[4] = {};
	command[0] = 0x0C;
	command[1] = register_address;
	command[2] = 0x80;
	command[3] = data;

	selectUserBank(3);

	icm20948_write(I2C_SLV0_ADDR, &command[0], 1);
	icm20948_write(I2C_SLV0_REG , &command[1], 1);
	icm20948_write(I2C_SLV0_DO  , &command[3], 1);
	icm20948_write(I2C_SLV0_CTRL, &command[2], 1);

	selectUserBank(0);
}

/*----------------------------------------------------------------------------------------*/
// 初期設定
// 下3つの関数を順番に実行して初期設定をする
void ICM20948::setup(){

	selectUserBank(0);

	ICM20948_identify();
	ICM20948_activation();
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);

	AK09916_identify();
	AK09116_activation();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

	ICM20948_sensor_setting();
	ICM20948_calibration();
	AK09116_calibration();

	selectUserBank(0);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
}

//ICM20948との通信ができるかをチェックする
void ICM20948::ICM20948_identify(){

	uint8_t icm20948_id = 0;

	while(icm20948_id != 0xEA){

		icm20948_read(WHO_AM_I, &icm20948_id, 1);
	}
}

//sleepモードを解除する
void  ICM20948::ICM20948_activation(){

	uint8_t command = 0x01;
	icm20948_write(RWR_MGMT_1, &command, 1);

	command = 0b00100000;
	icm20948_write(USER_CTRL, &command, 1);
}

//AK09916との通信ができるかをチェックする
void ICM20948::AK09916_identify(){

	uint8_t ak09916_id = 0;

	while(ak09916_id != 9){

		AK09916_read(WIA, &ak09916_id, 1);
	}
}

//地磁気センサーを有効化する
void ICM20948::AK09116_activation(){

	uint8_t command = 0x08;
	AK09916_write(CNTL2, command);
}

//加速度と角速度センサーの設定
void ICM20948::ICM20948_sensor_setting(){

	selectUserBank(2);

	uint8_t command = 0b00000001;
	icm20948_write(GYRO_CONFIG, &command, 1);

	command = 0b00110001;
	icm20948_write(ACEEL_CONFIG, &command, 1);

	selectUserBank(0);
}


/*----------------------------------------------------------------------------------------*/
//bank変更用の関数
void ICM20948::selectUserBank(uint8_t num){

	uint8_t bank = num << 4;
	icm20948_write(0x7F, &bank, 1);
}
/*----------------------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------------------*/

void ICM20948::get_sensor_value(float* accel_buffer, float* gyro_buffer, float* mag_buffer){

	ICM20948_getdata(accel_buffer, gyro_buffer);

	if(getdata_count % 11 == 0){

		AK09916_getdata(mag_buffer);
		getdata_count = 1;
	}
	getdata_count ++;
}

void ICM20948::ICM20948_calibration(){

	uint8_t calibration_buffer[12] = {};
	uint8_t calibration_pre[3] = {};

	float accel_sum[3] = {};
	float gyro_sum[3] = {};

	uint16_t calibration_count = 10000;

	for(uint16_t i=0; i < calibration_count; i++){

		while(calibration_pre[0] == calibration_buffer[1] && calibration_pre[1] == calibration_buffer[3] && calibration_pre[2] == calibration_buffer[5]){

			icm20948_read(ACCEL_XOUT_H, calibration_buffer, 12);
		}


		accel_sum[0] += ((int16_t)(calibration_buffer[0] << 8  | calibration_buffer[1] ))  / accel_scale * 9.8;
		accel_sum[1] += ((int16_t)(calibration_buffer[2] << 8  | calibration_buffer[3] ))  / accel_scale * 9.8;
		accel_sum[2] += ((int16_t)(calibration_buffer[4] << 8  | calibration_buffer[5] ))  / accel_scale * 9.8 - 9.8;

		gyro_sum[0] += ((int16_t)(calibration_buffer[6] << 8   | calibration_buffer[7] ))  / gyro_scale;
		gyro_sum[1] += ((int16_t)(calibration_buffer[8] << 8   | calibration_buffer[9] ))  / gyro_scale;
		gyro_sum[2] += ((int16_t)(calibration_buffer[10] << 8  | calibration_buffer[11] )) / gyro_scale;

		calibration_pre[0] = calibration_buffer[1];
		calibration_pre[1] = calibration_buffer[3];
		calibration_pre[2] = calibration_buffer[5];

	}

	accel_offset[0] = accel_sum[0] / calibration_count;
	accel_offset[1] = accel_sum[1] / calibration_count;
	accel_offset[2] = accel_sum[2] / calibration_count;

	gyro_offset[0] = gyro_sum[0] / calibration_count;
	gyro_offset[1] = gyro_sum[1] / calibration_count;
	gyro_offset[2] = gyro_sum[2] / calibration_count;

}

void ICM20948::AK09116_calibration(){

	//地磁気のキャリブレーション
}

void ICM20948::ICM20948_getdata(float *accel_buffer, float *gyro_buffer){

	while(pre_data[0] == data_buffer[1] && pre_data[1] == data_buffer[3] && pre_data[2] == data_buffer[5]){

		icm20948_read(ACCEL_XOUT_H, data_buffer, 12);
		icm20948_count ++;
	}
		accel_x[0] = ((int16_t)(data_buffer[0] << 8 | data_buffer[1] )) / accel_scale * 9.8 - accel_offset[0];
		accel_y[0] = ((int16_t)(data_buffer[2] << 8 | data_buffer[3] )) / accel_scale * 9.8 - accel_offset[1];
		accel_z[0] = ((int16_t)(data_buffer[4] << 8 | data_buffer[5] )) / accel_scale * 9.8 - accel_offset[2];

		gyro_x[0] = ((int16_t)(data_buffer[6] << 8  | data_buffer[7] )) / gyro_scale - gyro_offset[0];
		gyro_y[0] = ((int16_t)(data_buffer[8] << 8  | data_buffer[9] )) / gyro_scale - gyro_offset[1];
		gyro_z[0] = ((int16_t)(data_buffer[10] << 8 | data_buffer[11])) / gyro_scale - gyro_offset[2];

		if(accel_x[10] == 0){

			accel_buffer[0] = 0;
			accel_buffer[1] = 0;
			accel_buffer[2] = 0;

			gyro_buffer[0] = 0;
			gyro_buffer[1] = 0;
			gyro_buffer[2] = 0;
		}
		else{

			accel_buffer[0] = (float(accel_x[0]*10 + accel_x[1]*9 + accel_x[2]*8 + accel_x[3]*7 + accel_x[4]*6 + accel_x[5]*5 + accel_x[6]*4 + accel_x[7]*3 + accel_x[8]*2 + accel_x[9]*1 )) / 55.0;
			accel_buffer[1] = (float(accel_y[0]*10 + accel_y[1]*9 + accel_y[2]*8 + accel_y[3]*7 + accel_y[4]*6 + accel_y[5]*5 + accel_y[6]*4 + accel_y[7]*3 + accel_y[8]*2 + accel_y[9]*1 )) / 55.0;
			accel_buffer[2] = (float(accel_z[0]*10 + accel_z[1]*9 + accel_z[2]*8 + accel_z[3]*7 + accel_z[4]*6 + accel_z[5]*5 + accel_z[6]*4 + accel_z[7]*3 + accel_z[8]*2 + accel_z[9]*1 )) / 55.0;

			gyro_buffer[0] = (float(gyro_x[0]*10 + gyro_x[1]*9 + gyro_x[2]*8 + gyro_x[3]*7 + gyro_x[4]*6 + gyro_x[5]*5 + gyro_x[6]*4 + gyro_x[7]*3 + gyro_x[8]*2 + gyro_x[9]*1 )) / 55.0;
			gyro_buffer[1] = (float(gyro_y[0]*10 + gyro_y[1]*9 + gyro_y[2]*8 + gyro_y[3]*7 + gyro_y[4]*6 + gyro_y[5]*5 + gyro_y[6]*4 + gyro_y[7]*3 + gyro_y[8]*2 + gyro_y[9]*1 )) / 55.0;
			gyro_buffer[2] = (float(gyro_z[0]*10 + gyro_z[1]*9 + gyro_z[2]*8 + gyro_z[3]*7 + gyro_z[4]*6 + gyro_z[5]*5 + gyro_z[6]*4 + gyro_z[7]*3 + gyro_z[8]*2 + gyro_z[9]*1 )) / 55.0;
		}

		//次の実行のために値を保存する
		accel_x[9] = accel_x[8];
		accel_x[8] = accel_x[7];
		accel_x[7] = accel_x[6];
		accel_x[6] = accel_x[5];
		accel_x[5] = accel_x[4];
		accel_x[4] = accel_x[3];
		accel_x[3] = accel_x[2];
		accel_x[2] = accel_x[1];
		accel_x[1] = accel_x[0];

		accel_y[9] = accel_y[8];
		accel_y[8] = accel_y[7];
		accel_y[7] = accel_y[6];
		accel_y[6] = accel_y[5];
		accel_y[5] = accel_y[4];
		accel_y[4] = accel_y[3];
		accel_y[3] = accel_y[2];
		accel_y[2] = accel_y[1];
		accel_y[1] = accel_y[0];

		accel_z[9] = accel_z[8];
		accel_z[8] = accel_z[7];
		accel_z[7] = accel_z[6];
		accel_z[6] = accel_z[5];
		accel_z[5] = accel_z[4];
		accel_z[4] = accel_z[3];
		accel_z[3] = accel_z[2];
		accel_z[2] = accel_z[1];
		accel_z[1] = accel_z[0];

		gyro_x[9] = gyro_x[8];
		gyro_x[8] = gyro_x[7];
		gyro_x[7] = gyro_x[6];
		gyro_x[6] = gyro_x[5];
		gyro_x[5] = gyro_x[4];
		gyro_x[4] = gyro_x[3];
		gyro_x[3] = gyro_x[2];
		gyro_x[2] = gyro_x[1];
		gyro_x[1] = gyro_x[0];

		gyro_y[9] = gyro_y[8];
		gyro_y[8] = gyro_y[7];
		gyro_y[7] = gyro_y[6];
		gyro_y[6] = gyro_y[5];
		gyro_y[5] = gyro_y[4];
		gyro_y[4] = gyro_y[3];
		gyro_y[3] = gyro_y[2];
		gyro_y[2] = gyro_y[1];
		gyro_y[1] = gyro_y[0];

		gyro_z[9] = gyro_z[8];
		gyro_z[8] = gyro_z[7];
		gyro_z[7] = gyro_z[6];
		gyro_z[6] = gyro_z[5];
		gyro_z[5] = gyro_z[4];
		gyro_z[4] = gyro_z[3];
		gyro_z[3] = gyro_z[2];
		gyro_z[2] = gyro_z[1];
		gyro_z[1] = gyro_z[0];

		pre_data[0] = data_buffer[1];
		pre_data[1] = data_buffer[3];
		pre_data[2] = data_buffer[5];

}


void ICM20948::AK09916_getdata(float* mag_buffer){



}

/*----------------------------------------------------------------------------------------*/
