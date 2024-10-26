#include "wrapper.hpp"
#include "ICM20948.h"
#include "usart.h"
#include <string>
#include "est_angle.h"

ICM20948 icm20948; //i2c2を使う設定（変更機能→未実装）
EST_ANGLE est_angle;
void send_value(float* angle_data);
uint32_t count = 0;

void init(){

	icm20948.setup();

}

void loop(){

	static float accel_buffer[3] = {};
	static float gyro_buffer[3] = {};
	static float mag_buffer[3] = {};
	static float angle_buffer[3] = {};

//	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
//	DWT->CYCCNT = 0;
//	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
//
//	uint64_t start = DWT->CYCCNT;
//
//	for(uint16_t i = 0; i < 10000; i++){

		icm20948.get_sensor_value(accel_buffer, gyro_buffer, mag_buffer);

		if(count%10 == 0){

			send_value(mag_buffer);

		}

		count++;

//	}
//
//	uint64_t end = DWT->CYCCNT;
//	uint64_t cycles = end - start;


}

void send_value(float* angle_data){

	int16_t send_data[3];

	send_data[0] = (int16_t)(angle_data[0]*100);
	send_data[1] = (int16_t)(angle_data[1]*100);
	send_data[2] = (int16_t)(angle_data[2]*100);

	std::string str = std::to_string(send_data[0]) + " " + std::to_string(send_data[1]) + " " + std::to_string(send_data[2]) + "\n";

	HAL_UART_Transmit(&huart2, (uint8_t *)str.c_str(), str.length(), 100);

}
