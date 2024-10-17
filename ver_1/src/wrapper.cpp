#include "wrapper.hpp"
#include "icm20948.h"
#include "AK09916.h"
#include "AK09916_HAL_I2C.h"
#include "usart.h"
#include <string>

ICM20948 icm20948; //i2c2を使う設定（変更機能→未実装）
AK09916_HAL_I2C ak09916(&hi2c2, AK09916::I2C_ADDRESS::LOW);

void AK09916_setup();
void send_value(float* angle_data);

void init(){

	icm20948.setup();
	AK09916_setup();

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

}

void loop(){

//時間測定用
//	DWT->CYCCNT = 0;
//	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
//	uint64_t start = DWT->CYCCNT;

	//raw data
	static float accel_buffer[3] = {};
	static float gyro_buffer[3]  = {};
	static int16_t mag_buffer[3]   = {};

//時間測定用
//	for(uint16_t i=0; i < 1000; i++){

	//値の取得
	icm20948.get_value(accel_buffer, gyro_buffer);
	ak09916.getMagnet(mag_buffer);


//時間測定用
//	}
//	uint64_t end = DWT->CYCCNT;
//	uint64_t cycles = end - start;

}

void AK09916_setup(){

		uint8_t userbank=0;
		HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x7F, 1, &userbank, 1, 100);

		uint8_t ICM_I2C_MST_EN=0;
	    uint8_t comand=0b00100000;
	    HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x03, 1, &comand, 1, 100);
	    HAL_I2C_Mem_Read(&hi2c2, 0x68<<1, 0x03, 1, &ICM_I2C_MST_EN , 1, 100);

	    if(ICM_I2C_MST_EN==0b00100000){

	    }

	    userbank=0x03 << 4;
	    HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x7F, 1, &userbank, 1, 100);

	    /*detect AK09916*/
	    uint8_t AK09916ID=0;
	    AK09916ID=ak09916.readByte(AK09916::REGISTER::WIA2);
	    while(AK09916ID==0){
	    	HAL_Delay(500);
	    	AK09916ID=ak09916.readByte(AK09916::REGISTER::WIA2);
	    }

	    /*set measurement mode*/
		ak09916.setMesurementMode(AK09916::MODE::mode4);
		uint8_t mode = ak09916.readByte(AK09916::REGISTER::CNTL2);

	    ak09916.writeByte(AK09916::REGISTER::CNTL2, (uint8_t)AK09916::MODE::mode4);
		mode = ak09916.readByte(AK09916::REGISTER::CNTL2);

		userbank=0x03<<4;
		HAL_I2C_Mem_Write(&hi2c2, 0x68<<1, 0x7F, 1, &userbank, 1, 100);
}


void send_value(float* angle_data){

	int16_t send_data[3];

	send_data[0] = (int16_t)(angle_data[0] * 100);
	send_data[1] = (int16_t)(angle_data[1] * 100);
	send_data[2] = (int16_t)(angle_data[2] * 100);

	std::string str = std::to_string(send_data[0]) + " " + std::to_string(send_data[1]) + " " + std::to_string(send_data[2]) + "\n";

	HAL_UART_Transmit(&huart2, (uint8_t *)str.c_str(), str.length(), 100);

}
