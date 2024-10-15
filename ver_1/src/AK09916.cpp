/*
 * AK09916.cpp
 *
 *  Created on: Feb 17, 2024
 *      Author: heila
 */

#include <AK09916.h>


AK09916::AK09916() {
	// TODO Auto-generated constructor stub
}

uint8_t AK09916::getMagnet(int16_t* data_box){

	uint8_t drdy, hofl;	// data ready, overflow

	drdy = readByte(AK09916::REGISTER::ST1) & 0x01;
	if(drdy==0)	return 1;

	data_box[0]=getMagnet_X();//*0.15f;
	data_box[1]=getMagnet_Y();//*0.15f;
	data_box[2]=getMagnet_Z();//*0.15f;

	hofl = readByte(AK09916::REGISTER::ST2) & 0x08;
	if(hofl)	return 2;

	return 0;
}

int16_t AK09916::getMagnet_X() {

	uint8_t tmp[2];
	readBytes(REGISTER::HXL, tmp, 2);
	int16_t rawMagnet=tmp[1]<<8 | tmp[0];
	return rawMagnet;
}

int16_t AK09916::getMagnet_Y() {
	uint8_t tmp[2];
	readBytes(REGISTER::HYL, tmp, 2);
	int16_t rawMagnet=tmp[1]<<8 | tmp[0];
	return rawMagnet;
}

short AK09916::getMagnet_Z() {
	uint8_t tmp[2];
	readBytes(REGISTER::HZL, tmp, 2);
	int16_t rawMagnet=tmp[1]<<8 | tmp[0];
	return rawMagnet;
}

void AK09916::setMesurementMode(MODE mode){
	writeByte(REGISTER::CNTL2, (uint8_t)mode);
}

uint8_t AK09916::DatareadisReady(){
	//uint8_t DRDY=
			return readByte(REGISTER::ST1);
	/*if((DRDY&0x01)==0x01){
		return 1;
	} else{
		return 0;
	}*/
}
