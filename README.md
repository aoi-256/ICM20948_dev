# STM32 ICM20948 I2C (6軸バージョン）


## sample code 

クラス名がsensorなの、他のライブラリとかと競合しそうでよくない、、

LPFはただ平均値取ってるだけなので精度を信用しちゃいけない

加速度と角速度をいい感じに混ぜて値の精度をあげたい

```cpp
#include "wrapper.hpp"
#include "i2c.h"
#include "sensor.h"

Sensor sensor;

void init(){

    sensor.setup();

}

void loop(){


  sensor.get_accelvalue();
  sensor.get_gyrovalue();

  sensor.accel_LPF();
  sensor.gyro_LPF();

  sensor.estimate_angle();
  sensor.store_value();

}

```

3dの描画について

processing4を使っています

データはシリアルで
```
"x軸の角度" "y軸の角度" "z軸の角度" \n
```
スペースで分割して、改行コードで末端を管理しています
