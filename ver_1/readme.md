# 引継ぎ用のコード

とりあえず値の取得ができるようになった

```cpp
void init(){

  icm20948.setup();
  ak09916.setup();
}
void loop(){

  icm20948.get_value(accel_buffer, gyro_buffer);
	ak09916.getMagnet(mag_buffer);
}


```
