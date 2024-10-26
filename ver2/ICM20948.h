/*
 * ICM20948.h
 *
 *  Created on: Oct 23, 2024
 *      Author: Sezakiaoi
 */

#ifndef INC_ICM20948_H_
#define INC_ICM20948_H_

#include <cstdint>

class ICM20948 {

	public:

		//初期設定用の関数 ピンの設定をここで行うことにした
		void setup();

		//setup()内で呼び出される関数
		void ICM20948_identify();//ICM20948との通信を試す
		void ICM20948_activation();//sleepモードを解除する
		void I2C_MASTER_setting();//AK09916との通信用
		void AK09916_identify();//AK09916との通信を試す
		void ICM20948_sensor_setting();
		void ICM20948_calibration();//加速度と角速度のキャリブレーション
		void AK09116_calibration();//地磁気センサーのキャリブレーション
		void AK09116_activation();//地磁気センサーの有効化


		//センサーの値を読み取る関数
		void AK09916_getdata(float* mag_buffer);
		void ICM20948_getdata(float* accel_buffer, float* gyro_buffer);
		void get_sensor_value(float* accel_buffer, float* gyro_buffer, float* mag_buffer);

		void selectUserBank(uint8_t bank);

		//値の出力をする関数
		void send_value();

		//読み書きの関数(元あったものの、毎回同じ部分を入れなくていいようにした)

		//icm20948
		void icm20948_write(uint8_t register_address, uint8_t* data, uint8_t len);
		void icm20948_read (uint8_t register_address, uint8_t* buffer , uint8_t len);

		//AK09916
		void AK09916_write(uint8_t register_address, uint8_t data);
		void AK09916_read (uint8_t register_address, uint8_t* buffer ,uint8_t len);

	private:

		/*ICM20948 address*/
		const uint8_t ICM20948 = 0x68 <<  1;

		//bank1
		const uint8_t WHO_AM_I   = 0x00;
		const uint8_t USER_CTRL  = 0x03;
		const uint8_t RWR_MGMT_1 = 0x06;

		const uint8_t ACCEL_XOUT_H = 0x2D; //センサーの値のスタート地点
		const uint8_t GYRO = 0x33;

		//bank_2
		const uint8_t GYRO_CONFIG = 0x01;
		const uint8_t ACEEL_CONFIG = 0x14;

		//bank3
		const uint8_t I2C_SLV0_ADDR =  0x03;
		const uint8_t I2C_SLV0_REG  =  0x04;
		const uint8_t I2C_SLV0_CTRL =  0x05;
		const uint8_t I2C_SLV0_DO   =  0x06;

		//AK09916

		const uint8_t WIA   = 0x01;
		const uint8_t HXL   = 0x11;
		const uint8_t ST2   = 0x18;
		const uint8_t CNTL2 = 0x31;
		const uint8_t CNTL3 = 0x32;

		//キャリブレーション用の値
		float accel_offset[3] = {};
		float gyro_offset[3]  = {};

		/*data buffer*/
		uint8_t data_buffer[20] = {};

		//
		float   accel_scale  = 16384.0;
		float   gyro_scale   = 32768.0 / 500;

		//データ更新を判定するための変数

		uint8_t pre_data[3] = {};

		//LFP用の値
		float accel_x[10] = {};
		float accel_y[10] = {};
		float accel_z[10] = {};

		float gyro_x[10] = {};
		float gyro_y[10] = {};
		float gyro_z[10] = {};

		float mag_x[5] = {};
		float mag_y[5] = {};
		float mag_z[5] = {};

		//loop_count
		uint64_t icm20948_count = 0;
		uint8_t getdata_count = 0;//地磁気の取得タイミング調整用






};

#endif /* INC_ICM20948_H_ */
