/*
 * ICM20948.cpp
 *
 *  Created on: Feb 26, 2025
 *      Author: Sezakiaoi
 */

#include <ICM20948.h>

ICM20948::ICM20948(I2C_HandleTypeDef *I2C_Pin):ICM20948_HAL(I2C_Pin){


}

/* @brief センサーとの接続を確認
 *
 * ICM20948のWHO_AM_Iレジスタとの通信を用いて、接続を確認します
 * 最大100回の再試行をします
 *
 * @return uint8_t 0: 接続成功、1: 接続失敗
 */
uint8_t ICM20948::Connection(){

	uint8_t Product_Id = 0x00;
	uint8_t Error = 0;

	Select_Bank(0);

	while(Product_Id != 0xEA){

		ICM20948_HAL::ICM20948_Read(REGISTER::WHO_AM_I, &Product_Id, 1);
		Error ++;

		if(Error > 100){

			return 1;//接続失敗
		}
	}

	Select_Bank(0);
	return 0;//接続成功
}

/* @brief センサーのSleepModeを解除
 *
 * ICM20948のRWR_MGMT_1のスリーブモードを解除します
 * 最大100回の再試行をします
 *
 * @return uint8_t 0: 解除成功、1: 解除失敗
 */
uint8_t ICM20948::Activation(){

	uint8_t Command = 0x01;
	uint8_t Now_Mode = 0x00;
	uint8_t Error = 0;

	Select_Bank(0);

	while(Command != Now_Mode){

		ICM20948_HAL::ICM20948_Write(REGISTER::PWR_MGMT_1, &Command, 1);

		ICM20948_HAL::ICM20948_Read(REGISTER::PWR_MGMT_1, &Now_Mode, 1);

		Error ++;

		if(Error > 100){

			return 1;//解除失敗
		}
	}
	Select_Bank(0);
	return 0;//解除成功
}

/* @brief 加速度データの設定
 *
 * ICM20948のAccel_Configの値を変更する
 * 最大100回の再試行をします
 *
 * 詳しい説明はデータシート参照
 * @param Accel_	Scale 加速度センサーのスケール設定
 * @param FCHOICE   フィルターの有効化
 * @param DLPFCFG   デジタルローパスフィルタの設定
 * @param Accel_Ave 平均化フィルターの設定
 *
 * @return uint8_t 0: 設定完了、1: Accel_Config1 失敗、2: Accel_Config2 失敗
 */
uint8_t ICM20948::Accel_Config(ICM20948::Accel_Scale Scale, ICM20948::FCHOICE FCHOICE, ICM20948::DLPFCFG DLPFCFG, ICM20948::Accel_Ave Ave){

	uint8_t FCHOICE_Value = 0;

	Select_Bank(2);

	if(uint8_t(Ave) == 0x0F){

		FCHOICE_Value = 0;
	}
	else{

		FCHOICE_Value = uint8_t(Ave);
	}

	uint8_t Command = (uint8_t(DLPFCFG) << 3) + (uint8_t(Scale) << 1) + FCHOICE_Value;
	uint8_t Now_Mode = 0x00;
	uint8_t Error = 0x00;

	while(Command != Now_Mode){

		ICM20948_HAL::ICM20948_Write(REGISTER::ACCEL_CONFIG_1, &Command, 1);

		ICM20948_HAL::ICM20948_Read(REGISTER::ACCEL_CONFIG_1, &Now_Mode, 1);

		Error ++;

		if(Error > 100){

			return 1;//Accel_Config1 解除失敗
		}
	}

	Command = uint8_t(Ave);
	Now_Mode = 0x00;
	Error = 0x00;

	while(Command != Now_Mode){

		ICM20948_HAL::ICM20948_Write(REGISTER::ACCEL_CONFIG_2, &Command, 1);

		ICM20948_HAL::ICM20948_Read(REGISTER::ACCEL_CONFIG_2, &Now_Mode, 1);

		Error ++;

		if(Error > 100){

			return 2;//Accel_Config2 解除失敗
		}
	}
	Select_Bank(0);
	return 0;
}

/* @brief ジャイロデータの設定
 *
 * ICM20948のGyro_Configの値を変更する
 * 最大100回の再試行をします
 *
 * 詳しい説明はデータシート参照
 * @param Gyro_Scale ジャイロセンサーのスケール設定
 * @param FCHOICE    フィルターの有効化
 * @param DLPFCFG    デジタルローパスフィルタの設定
 * @param Gyro_Ave   平均化フィルターの設定
 *
 * @return uint8_t 0: 設定完了、1: GYRO_Config1 失敗、2: GYRO_Config2 失敗
 */
uint8_t ICM20948::Gyro_Config(ICM20948::Gyro_Scale Scale, ICM20948::FCHOICE FCHOICE, ICM20948::DLPFCFG DLPFCFG, ICM20948::Gyro_Ave Ave){

	uint8_t Command = (uint8_t(DLPFCFG) << 3) + (uint8_t(Scale) << 1) + uint8_t(FCHOICE);
	uint8_t Now_Mode = 0x00;
	uint8_t Error = 0x00;

	Select_Bank(2);

	while(Command != Now_Mode){

		ICM20948_HAL::ICM20948_Write(REGISTER::GYRO_CONFIG_1, &Command, 1);

		ICM20948_HAL::ICM20948_Read(REGISTER::GYRO_CONFIG_1, &Now_Mode, 1);

		Error ++;

		if(Error > 100){

			return 1;//Accel_Config1 解除失敗
		}
	}

	//平均化フィルターが適応できるとき
	if(uint8_t(FCHOICE) != 0 && uint8_t(Ave) != 0){

		Command = uint8_t(Ave);
		Now_Mode = 0x00;
		Error = 0x00;

		while(Command != Now_Mode){

			ICM20948_HAL::ICM20948_Write(REGISTER::GYRO_CONFIG_2, &Command, 1);

			ICM20948_HAL::ICM20948_Read(REGISTER::GYRO_CONFIG_2, &Now_Mode, 1);

			Error ++;

			if(Error > 100){

				return 2;//Accel_Config2 解除失敗
			}
		}
	}
	Select_Bank(0);
	return 0;
}

/* @brief データの読み取り
 *
 * ICM20948のGyro_Configの値を変更する
 * 最大100回の再試行をします
 *
 * 詳しい説明はデータシート参照
 * @param Gyro_Scale ジャイロセンサーのスケール設定
 * @param FCHOICE    フィルターの有効化
 * @param DLPFCFG    デジタルローパスフィルタの設定
 * @param Gyro_Ave   平均化フィルターの設定
 *
 * @return uint8_t 0: 設定完了、1: GYRO_Config1 失敗、2: GYRO_Config2 失敗
 */
uint8_t ICM20948::Get_Data(int16_t Accel_Data[3], int16_t Gyro_Data[3]){

	Select_Bank(0);

	ICM20948_HAL::ICM20948_Read(REGISTER::ACCEL_XOUT_H, Raw_Data, 12);

	Accel_Data[0] = (int16_t)(Raw_Data[0] << 8 | Raw_Data[1]);
	Accel_Data[1] = (int16_t)(Raw_Data[2] << 8 | Raw_Data[3]);
	Accel_Data[2] = (int16_t)(Raw_Data[4] << 8 | Raw_Data[5]);

	Gyro_Data[0] = (int16_t)(Raw_Data[6] << 8  | Raw_Data[7]);
	Gyro_Data[1] = (int16_t)(Raw_Data[8] << 8  | Raw_Data[9]);
	Gyro_Data[2] = (int16_t)(Raw_Data[10] << 8 | Raw_Data[11]);

	return 0;
}
/* @brief BANKの変更
 *
 * ICM20948のGyro_Configの値を変更する
 * 最大100回の再試行をします
 *
 * @param  Bank_Channel Bankのチャンネル
 *
 * @return uint8_t 0: 設定完了、1: GYRO_Config1 失敗、2: GYRO_Config2 失敗
 */
uint8_t ICM20948::Select_Bank(uint8_t Bank_Channel){

	ICM20948_HAL::ICM20948_Write(REGISTER::REG_BANK_SEL, &Bank_Channel, 1);

	return 0;
}
