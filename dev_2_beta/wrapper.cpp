#include "wrapper.hpp"
#include "ICM20948.h"
#include "usart.h"
#include <string>

ICM20948 icm20948; //i2c2を使う設定（変更機能→未実装）
//AK09916_HAL_I2C ak09916(&hi2c2, AK09916::I2C_ADDRESS::LOW);
//
//void AK09916_setup();
void send_value(float* angle_data);

uint32_t count = 0;

void init(){

	icm20948.setup();

}

void loop(){

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

	//raw data
	static float accel_buffer[3] = {};
	static float gyro_buffer[3]  = {};
	static float mag_buffer[3]   = {};
	//angle data
	//static float angle_buffer[3] = {};

	//値の取得
	icm20948.get_sensor_value(accel_buffer, gyro_buffer, mag_buffer);

	if(count % 50 == 0 && mag_buffer[0] != 0){

		send_value(mag_buffer);
	}
	count ++;
}

void send_value(float* angle_data){

	int16_t send_data[3];

	send_data[0] = (int16_t)(angle_data[0]);
	send_data[1] = (int16_t)(angle_data[1]);
	send_data[2] = (int16_t)(angle_data[2]);

	std::string str = std::to_string(send_data[0]) + " " + std::to_string(send_data[1]) + " " + std::to_string(send_data[2]) + "\n";

	HAL_UART_Transmit(&huart2, (uint8_t *)str.c_str(), str.length(), 100);

}
