#include "wrapper.hpp"
#include "icm20948.h"
#include "AK09916.h"
#include "AK09916_HAL_I2C.h"
#include "est_angle.h"
#include "usart.h"
#include <string>

ICM20948 icm20948; //i2c2を使う設定（変更機能→未実装）
AK09916_HAL_I2C ak09916(&hi2c2, AK09916::I2C_ADDRESS::LOW);
EST_ANGLE est_angle;

void AK09916_setup();
void send_value(float* angle_data);

void init(){

	icm20948.setup();
	AK09916_setup();

}

void loop(){


	//raw data
	float accel_buffer[3] = {};
	float gyro_buffer[3]  = {};
	float mag_buffer[3]   = {};

	//angle data
	static float angle_buffer[3] = {};

	//値の取得
	icm20948.get_value(accel_buffer, gyro_buffer);
	ak09916.getMagnet(mag_buffer);

	//姿勢推定
	est_angle.input_value(accel_buffer, gyro_buffer, mag_buffer);
	est_angle.output_value(angle_buffer);

	//データの送信
	send_value(angle_buffer);

	//debug
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

}

void AK09916_setup(){

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);

	uint8_t userbank= 0;
	HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x7F, 1, &userbank, 1, 100);

	//I2CMaster機能の有効化
	uint8_t command=0b00100000;
	uint8_t command_fb = 0;

	while(command != command_fb){

		HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x03, 1, &command, 1, 100);
	    HAL_I2C_Mem_Read(&hi2c2, 0x68<<1, 0x03, 1, &command_fb , 1, 100);
	}

	userbank= 0x03 << 4;
	HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x7F, 1, &userbank, 1, 100);

	//AK09916へのWHOAMI
	 uint8_t AK09916ID=0;
	 while(AK09916ID != 0x09){

	  AK09916ID=ak09916.readByte(AK09916::REGISTER::WIA2);
	 }

	    /*set measurement mode*/
	 uint8_t mode = 0;
	 while(mode != 8){

	 	ak09916.writeByte(AK09916::REGISTER::CNTL2, (uint8_t)AK09916::MODE::mode4);
	 	mode = ak09916.readByte(AK09916::REGISTER::CNTL2);
	 }
		userbank=0;
		HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x7F, 1, &userbank, 1, 100);

		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_RESET);
}


void send_value(float* angle_data){

	int16_t send_data[3];

	send_data[0] = (int16_t)(angle_data[0] * 100);
	send_data[1] = (int16_t)(angle_data[1] * 100);
	send_data[2] = (int16_t)(angle_data[2] * 100);

	std::string str = std::to_string(send_data[0]) + " " + std::to_string(send_data[1]) + " " + std::to_string(send_data[2]) + "\n";

	HAL_UART_Transmit(&huart2, (uint8_t *)str.c_str(), str.length(), 100);

}
