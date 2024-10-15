/*
 *
 * angle_est.h
 *
 *  Created on: 10, 11, 2024
 *      Author: aoi
 */
#ifndef EST_ANGLE_H
#define EST_ANGLE_H

#include <cstdint>

class EST_ANGLE{

	public:

	//センサーから得たデータ（静的キャリブレーション済み）を取得
	//オイラー角に変換しておいてね
	void input_value(float* accel_buffer, float* gyro_buffer, float* mag_buffer);

//	void check_value();//値が更新されているか確認（現在は一定時間ごとにくるから気にしない方針）

	//角速度の積分
	void integral_value();
	void calc_angle();
	void output_value(float* output_buffer);
	void store_value();


	private:

		//最新のセンサーデータ
		float current_accel[3];
		float current_gyro[3];
		float current_mag[3];

		//1つ前のセンサーデータ（積分用）
		float pre_accel[3];
		float pre_gyro[3];
		float pre_mag[3];

		//角速度の積分値
		float integral_gyro[3] = {};

		//計算結果（オイラー角）
		float est_angle[3];

		const float time = 1.0 / 630; //1ループあたりの実行時間

};

#endif //EST_ANGLE_H
