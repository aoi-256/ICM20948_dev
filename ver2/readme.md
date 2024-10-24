# ちょっと改良したバージョン

## 変更の概要

加速度センサーの精度向上（LFPの導入）
実行速度の向上

## サンプルコード

```cpp
#include "wrapper.hpp"
#include "ICM20948.h"
#include "usart.h"
#include <string>

ICM20948 icm20948; //i2c2を使う設定（変更機能→未実装）
void send_value(float* angle_data);

void init(){

	icm20948.setup();

}

void loop(){

	static float accel_buffer[3] = {};
	static float gyro_buffer[3] = {};
	static float mag_buffer[3] = {};
	static float angle_buffer[3] = {};


  icm20948.get_sensor_value(accel_buffer, gyro_buffer, mag_buffer);

  //計算結果をsend_value()で送信
  send_value(angle_buffer);
}

void send_value(float* angle_data){

	int16_t send_data[3];

	send_data[0] = (int16_t)(angle_data[0]*100);
	send_data[1] = (int16_t)(angle_data[1]*100);
	send_data[2] = (int16_t)(angle_data[2]*100);

	std::string str = std::to_string(send_data[0]) + " " + std::to_string(send_data[1]) + " " + std::to_string(send_data[2]) + "\n";

	HAL_UART_Transmit(&huart2, (uint8_t *)str.c_str(), str.length(), 100);

}
```
