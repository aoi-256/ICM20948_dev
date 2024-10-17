# 引継ぎ用のコード

とりあえず値の取得ができるようになった

```cpp
ICM20948 icm20948; 
AK09916_HAL_I2C ak09916(&hi2c2, AK09916::I2C_ADDRESS::LOW);

void init(){

  icm20948.setup();
  ak09916.setup();
}
void loop(){

  icm20948.get_value(accel_buffer, gyro_buffer);
	ak09916.getMagnet(mag_buffer);
}


```
