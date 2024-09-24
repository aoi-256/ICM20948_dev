# 現在開発中

姿勢推定の練習の１つとして書いています

プログラム未経験からのスタートですので、変数名がひどいかもです

（過去にget_kasokudoとget_kakusokudoがいた）

一通り完成したら関数の使い方などのメモを書きます

# 万一これを使おうという方へ

知り合いに見せるように公開設定にしていますが、万一外部の方で使おうと考えてしまった場合、

[データシート](https://invensense.tdk.com/wp-content/uploads/2024/03/DS-000189-ICM-20948-v1.6.pdf)

[マイクロソフトの検索AI](https://www.bing.com/chat?form=NTPCHB)

を使うとこれの100倍いいコードを自作することができますので、ぜひ挑戦してください！

もっとよいものを簡単に作れるので、ぜひ自作してみてください！

# sample code 

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
