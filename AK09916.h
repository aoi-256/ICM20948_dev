/*
 * AK09916.h
 *
 *  Created on: Feb 17, 2024
 *      Author: heila
 */

#ifndef INC_AK09916_H_
#define INC_AK09916_H_

#include <stdint.h>
#include <array>

class AK09916 {
public:
    enum class REGISTER{
    	WIA1=0x00,
        WIA2,
        ST1=0x10,
        HXL,
        HXH,
        HYL,
        HYH,
        HZL,
        HZH,
		ST2=0x18,
        CNTL2=0x31,
        CNTL3
    };

    enum class MODE{
        Power_down,
        single_measurement,
        mode1,      //10hz
        mode2=0x04,      //20hz
        mode3=0x06,      //50hz
        mode4=0x08,      //100hz
        self_test=0x10
    };

    enum class I2C_ADDRESS{
    	LOW=0x68,
		HIGH
    };

    AK09916();

    uint8_t getMagnet(float* data_box);
    int16_t getMagnet_X();
    int16_t getMagnet_Y();
    int16_t getMagnet_Z();

    uint8_t DatareadisReady();
    void setMesurementMode(MODE mode);

    virtual uint8_t readByte(REGISTER reg){return 0;}
    virtual void writeByte(REGISTER reg, uint8_t value){}
    virtual void readBytes(REGISTER reg, uint8_t *pData, uint8_t size=1){}

protected:
    uint8_t setBit(uint8_t arg, uint8_t position, bool value=true);
    uint8_t resetBit(uint8_t arg, uint8_t poition);


};

#endif /* INC_AK09916_H_ */
