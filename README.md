# STM32 ICM20948 I2C (6軸バージョン）

ICM20948の6軸のデータが取れるコードです

## 使い方

I2CのStandard・Fast Modeに対応しています

### 設定変更について

Config関数の引数は、Enumで定義してあるので、```icm.Scale::```まで入力すると候補が出てきます

センサーの設定で選べるものはすべて定義したので、選んで使ってください

### エラー処理について

**Connection**・**Activation**・**Config**関数は、エラー時の戻り値が違います

(Get_Data関数はエラー時に1つ前の値を返すようにしています）

実際にシステムに組み込む際には、戻り値が0であることを確認することをおすすめします
 
```cpp
if( icm.Connection() != 0){

	//通信エラーが発生したときの処理
}
```

## Sample Code

UART2を使って、取得したデータを一定間隔ごとに送信しています

TeraTermなどを使ってデータを確認することができます

```cpp
#include "wrapper.hpp"
#include "usart.h"
#include "tim.h"
#include "i2c.h"
#include "ICM20948.h"

ICM20948 icm(&hi2c2);

void Send_Data(int16_t Data[3]);
int16_t Accel_Data[3] = {};
int16_t Gyro_Data[3] = {};

void init(){

	icm.Connection();
	icm.Activation();
	icm.Accel_Config(icm.Accel_Scale::scale_2g, icm.FCHOICE::Enable, icm.DLPFCFG::Setting_6, icm.Accel_Ave::x1);
	icm.Gyro_Config(icm.Gyro_Scale::dps_0250, icm.FCHOICE::Enable, icm.DLPFCFG::Setting_6, icm.Gyro_Ave::x1);
}

void loop(){

	icm.Get_Data(Accel_Data, Gyro_Data);

	Send_Data(Accel_Data);
	Send_Data(Gyro_Data);
	HAL_Delay(10);
}

void Send_Data(int16_t Data[3]){

	str = std::to_string(Data[0]) + " " + std::to_string(Data[1]) + " " + std::to_string(Data[2]) + "\n";

	HAL_UART_Transmit(&huart2, (uint8_t *)str.c_str(),str.length(),100);
}
```

## アップデートについて

- Get_Data関数のエラー判定と処理を追加する予定です
