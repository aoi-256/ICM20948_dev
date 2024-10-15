#include "est_angle.h"
#include <cmath>

void EST_ANGLE::input_value(float* accel_buffer, float* gyro_buffer, float* mag_buffer){

	for(uint8_t i=0; i<3; i++){

		current_accel[i] = accel_buffer[i];
		current_gyro[i]  = gyro_buffer[i];
		current_mag[i]   = mag_buffer[i];
	}
}

/*---------------------------------------------------------------------------*/

//計算と出力
//output_valueに下2つの関数が含まれている

void EST_ANGLE::output_value(float* output_buffer){

	integral_value();
	calc_angle();

	output_buffer[0] = est_angle[0];
	output_buffer[1] = est_angle[1];
	output_buffer[2] = est_angle[2];
}


void EST_ANGLE::integral_value(){

	//台形近似しつつ区分求積
	integral_gyro[0] += (current_gyro[0] + pre_gyro[0]) * time / 2;
	integral_gyro[1] += (current_gyro[1] + pre_gyro[1]) * time / 2;
	integral_gyro[2] += (current_gyro[2] + pre_gyro[2]) * time / 2;

	//値の保存

	pre_gyro[0] = current_gyro[0];
	pre_gyro[1] = current_gyro[1];
	pre_gyro[2] = current_gyro[2];
}

void EST_ANGLE::calc_angle(){

	//加速度データからオイラー角（ピッチとロール）の計算
	static float arctan_accel[3] = {};
	arctan_accel[0] = atan(current_accel[1] / current_accel[2]) * 180 / M_PI;// y/z
	arctan_accel[1] = atan(current_accel[0] / sqrt(current_accel[2]*current_accel[2] + current_accel[1]*current_accel[1])) * 180 / M_PI;// x/z


	//相補フィルターを使ってオイラー角（ピッチとロール）を算出する
	est_angle[0] = integral_gyro[0] * 0 + arctan_accel[0] * 1;
	est_angle[1] = integral_gyro[1] * 0 + arctan_accel[1] * 1;

	//地磁気センサーの値と角速度の積分からヨー方向のオイラー角を求めてね（地獄）

	float a = current_mag[0]*cos(arctan_accel[1]) + current_mag[1]*sin(arctan_accel[0])*sin(arctan_accel[1]) + current_mag[2]*sin(arctan_accel[1]*cos(arctan_accel[0]));
	float b = current_mag[1]*cos(arctan_accel[0]) + current_mag[2]*sin(arctan_accel[0]);

	if(atan2f(a,b) != 0){

		est_angle[2] = integral_gyro[2] *0 + (atan2f(a,b) * 180 / M_PI) * 1;
	}

}
