#include "wrapper.hpp"
#include "i2c.h"
#include "sensor.h"

Sensor sensor;

void init(){

    sensor.setup();

}

void loop(){

//	DWT->CYCCNT = 0;
//	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
//	uint64_t start = DWT->CYCCNT;
//
//	for(uint16_t i=0; i < 10000; i++){

    sensor.get_accelvalue();
    sensor.get_gyrovalue();

    sensor.accel_LPF();
    sensor.gyro_LPF();

    sensor.estimate_angle();
	sensor.store_value();

//	}
//	uint64_t end = DWT->CYCCNT;
//	uint64_t cycles = end - start;

}
