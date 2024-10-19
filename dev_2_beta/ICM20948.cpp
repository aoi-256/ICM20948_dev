#include "icm20948.h"

/* ICM20948とAK09916への読み書きを行うプログラム*/
//i2cのピンを変える場合は、ここの4か所を変えてね

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

/* 初期設定用の関数　基本的にsetup()ですべて実行される*/
void ICM20948::setup(){

	//ICM20948側の設定
	ICM20948_identify();
	ICM20948_activation();
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);

	//AK09916側の設定
	AK09916_identify();
	AK09116_activation();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

	//bankを戻す
	selectUserBank(0);
}

bool ICM20948::ICM20948_identify(){

	uint8_t icm20948_id = 0;

	while(icm20948_id != 0xEA){

		icm20948_read(WHO_AM_I, &icm20948_id, 1);
	}
	return true;
}

void ICM20948::ICM20948_activation(){

	uint8_t command = 0x08;
	icm20948_write(RWR_MGMT_1, &command, 1);

	command = 0b00100000;
	icm20948_write(USER_CTRL, &command, 1);
}

bool ICM20948::AK09916_identify(){

	uint8_t ak09916_id = 0;

	while(ak09916_id != 9){

		AK09916_read(WIA, &ak09916_id, 1);
	}
	return true;
}

void ICM20948::AK09116_activation(){

	uint8_t command = 0x08;
	uint8_t command_fb = 0x00;

	while(command != command_fb){

		AK09916_write(CNTL2, command);
		AK09916_read(CNTL2, &command_fb, 1);
	}
}

void ICM20948::ICM20948_calibration(){

	//取得した値の合計を保存する
	float accel_calibration[3] = {};
	float gyro_calibration[3]  = {};

	for(uint16_t i=0; i < 10000; i++){

		icm20948_read(ACCEL_XOUT_H, data_buffer, 12);

		accel_calibration[0] = (int16_t)(data_buffer[0]  << 8 | data_buffer[1]);
		accel_calibration[1] = (int16_t)(data_buffer[2]  << 8 | data_buffer[3]);
		accel_calibration[2] = (int16_t)(data_buffer[4]  << 8 | data_buffer[5]);

		gyro_calibration[0]  = (int16_t)(data_buffer[6]  << 8 | data_buffer[7]);
		gyro_calibration[1]  = (int16_t)(data_buffer[8]  << 8 | data_buffer[9]);
		gyro_calibration[2]  = (int16_t)(data_buffer[10] << 8 | data_buffer[11]);

	}

	accel_offset[0] = accel_calibration[0] / 10000.0;
	accel_offset[1] = accel_calibration[1] / 10000.0;
	accel_offset[2] = accel_calibration[2] / 10000.0;

	gyro_offset[0] = gyro_calibration[0] / 10000.0;
	gyro_offset[1] = gyro_calibration[1] / 10000.0;
	gyro_offset[2] = gyro_calibration[2] / 10000.0;

}

/*----------------------------------------------------------------------------------------*/
//bank変更用の関数
void ICM20948::selectUserBank(uint8_t num){

	uint8_t bank = num << 4;
	icm20948_write(0x7F, &bank, 1);
}

/*----------------------------------------------------------------------------------------*/

/*センサーの値を取得する*/
void ICM20948::get_sensor_value(float* accel_buffer, float* gyro_buffer, float* mag_buffer){

	//data_bufferにすべてのデータがまとめて格納されている
	static uint32_t count = 0;

	ICM20948_getdata(accel_buffer, gyro_buffer);

	if(count % 10 == 0){

		AK09916_getdata(mag_buffer);
	}
	count ++;

}

void ICM20948::ICM20948_getdata(float* accel_buffer, float* gyro_buffer){

	icm20948_read(ACCEL_XOUT_H, data_buffer, 12);

	accel_buffer[0] = ((int16_t)(data_buffer[0]  << 8 | data_buffer[1] )) - accel_offset[0];
	accel_buffer[1] = ((int16_t)(data_buffer[2]  << 8 | data_buffer[3] )) - accel_offset[1];
	accel_buffer[2] = ((int16_t)(data_buffer[4]  << 8 | data_buffer[5] )) - accel_offset[2];

	gyro_buffer[0]  = ((int16_t)(data_buffer[6]  << 8 | data_buffer[7] )) - gyro_offset[0];
	gyro_buffer[1]  = ((int16_t)(data_buffer[8]  << 8 | data_buffer[9] )) - gyro_offset[1];
	gyro_buffer[2]  = ((int16_t)(data_buffer[10] << 8 | data_buffer[11])) - gyro_offset[2];

}

void ICM20948::AK09916_getdata(float* mag_buffer){

	static uint8_t dummy_buffer[6];
	mag_buffer[0]   = 0;
	mag_buffer[1]   = 0;
	mag_buffer[2]   = 0;//受信の成功判定のために初期化

	selectUserBank(3);
	AK09916_read(0x10, dummy_buffer, 1);
	AK09916_read(0x11, &data_buffer[14], 7);
	AK09916_read(0x18, dummy_buffer, 1);

	mag_buffer[0]   = (int16_t)(data_buffer[15] << 8 | data_buffer[14]);
	mag_buffer[1]   = (int16_t)(data_buffer[17] << 8 | data_buffer[16]);
	mag_buffer[2]   = (int16_t)(data_buffer[19] << 8 | data_buffer[18]);

	if(mag_buffer[0] > 5000 || mag_buffer[1] > 5000 || mag_buffer[2] > 5000){

		mag_buffer[0]   = 0;
		mag_buffer[1]   = 0;
		mag_buffer[2]   = 0;

	}
	selectUserBank(0);
}



