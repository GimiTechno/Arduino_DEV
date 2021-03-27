#ifndef _SHT31_H_
#define _SHT31_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "Arduino.h"
#include <Wire.h>

#ifndef OFF
#define OFF   0x00
#endif
#ifndef ON
#define ON    0x01
#endif

#ifndef LOW
#define LOW   0x00
#endif
#ifndef HIGH
#define HIGH  0x01
#endif

#ifndef DISABLE
#define DISABLE 0x00
#endif
#ifndef ENABLE
#define ENABLE  0x01
#endif

/*******************************************************/
/*********         SHT31 Command Table         *********/
#define SHT31_I2C_CMD_PERIODIC_MODE     0xE000  /* 定周期読み出しモード */
#define SHT31_I2C_CMD_ART_MODE          0x2B32  /* 4Hz周期読み出しモード（仮） */
#define SHT31_I2C_CMD_MODE_STOP         0x3093  /* ブレーク/定周期読み出しモード停止 */

#define SHT31_I2C_CMD_SOFTRESET         0x30A2  /* S/W Reset */
#define SHT31_I2C_CMD_SOFTRESET_2       0x3041

#define SHT31_I2C_CMD_BUS_RESET         0x0006

#define SHT31_I2C_CMD_HEATER_DISABLE    0x3066
#define SHT31_I2C_CMD_HEATER_ENABLE     0x306D

#define SHT31_I2C_CMD_READ_DATA         0x2400
#define SHT31_I2C_CMD_READ_STATUS       0xF32D
/*******************************************************/

struct sht31_Reg_t
{
    union {
        uint16_t WORD;
        struct {
            uint8_t bit15:1;
            uint8_t bit14:1;
            uint8_t bit13:1;
            uint8_t bit12:1;
            uint8_t bit11:1;
            uint8_t bit10:1;
            uint8_t bit9:1;
            uint8_t bit8:1;
            uint8_t bit7:1;
            uint8_t bit6:1;
            uint8_t bit5:1;
            uint8_t bit4:1;
            uint8_t bit3:1;
            uint8_t bit2:1;
            uint8_t bit1:1;
            uint8_t bit0:1;
        } BIT;
    } STATUS_REG;
};

// SHT31　湿温度構造体
typedef struct{
    float temp; /* 温度 */
    float rh;   /* 湿度 */
} SHT31_DATA_T;

void SHT31_Reset(uint8_t i2c_addr);
void SHT31_Read(SHT31_DATA_T *p_sth31_data_t);
void SHT31_StatusCheck(void);
void SHT31_End(void);

#endif /* _SHT31_H_ */