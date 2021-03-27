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

/**************************************************************************************************************/
/*********         SHT31 Command Table         *********/
#define SHT31_I2C_CMD_SOFTRESET                          0x30A2  /* S/W Reset */
#define SHT31_I2C_CMD_SOFTRESET_2                        0x3041
#define SHT31_I2C_CMD_BUS_RESET                          0x0006 /* バスリセット（I2Cバス解放???） */
#define SHT31_I2C_CMD_READ_STATUS                        0xF32D /* ステータスフラグリード */
#define SHT31_I2C_CMD_CLEAR_STATUS                       0x3041 /* ステータスフラグリセット（ Bit 15, 11, 10, 4） */

#define SHT31_I2C_CMD_PERIODIC_MODE                      0xE000  /* 定周期読み出しモード */
#define SHT31_I2C_CMD_ART_MODE                           0x2B32  /* 4Hz周期読み出しモード（仮） */
#define SHT31_I2C_CMD_MODE_STOP                          0x3093  /* 測定停止 */

#define SHT31_I2C_CMD_HEATER_DISABLE                     0x3066  /* 内蔵ヒータ無効 */
#define SHT31_I2C_CMD_HEATER_ENABLE                      0x306D  /* 内蔵ヒータ有効 */

/* ワンショットリード */
#define SHT31_I2C_CMD_ONESHOT_READ_DATA_HIGH             0x2400  /* 繰り返し精度：高 */
#define SHT31_I2C_CMD_ONESHOT_READ_DATA_MIDDLE           0x240B  /* 繰り返し精度：中 */
#define SHT31_I2C_CMD_ONESHOT_READ_DATA_LOW              0x2416  /* 繰り返し精度：低 */

/* 周期読み出し：0.5mps(0.5回/sec) */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_0P5MPS_HIGH     0x2032  /* 繰り返し精度：高 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_0P5MPS_MIDDLE   0x2024  /* 繰り返し精度：中 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_0P5MPS_LOW      0x202F  /* 繰り返し精度：低 */

/* 周期読み出し：1mps(1回/sec) */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_1MPS_HIGH       0x2130  /* 繰り返し精度：高 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_1MPS_MIDDLE     0x2126  /* 繰り返し精度：中 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_1MPS_LOW        0x211D  /* 繰り返し精度：低 */

/* 周期読み出し：2mps(2回/sec) */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_2MPS_HIGH       0x2236  /* 繰り返し精度：高 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_2MPS_MIDDLE     0x2220  /* 繰り返し精度：中 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_2MPS_LOW        0x222B  /* 繰り返し精度：低 */

/* 周期読み出し：4mps(4回/sec) */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_4MPS_HIGH       0x2434  /* 繰り返し精度：高 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_4MPS_MIDDLE     0x2422  /* 繰り返し精度：中 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_4MPS_LOW        0x2429  /* 繰り返し精度：低 */

/* 周期読み出し：10mps(10回/sec) */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_10MPS_HIGH      0x2737  /* 繰り返し精度：高 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_10MPS_MIDDLE    0x2721  /* 繰り返し精度：中 */
#define SHT31_I2C_CMD_PERIODIC_READ_DATA_10MPS_LOW       0x272A  /* 繰り返し精度：低 */
/**************************************************************************************************************/

typedef enum{
    /* ワンショットリード */
    ONESHOT_READ_DATA_HIGH,         /* 繰り返し精度：高 */
    ONESHOT_READ_DATA_MIDDLE,       /* 繰り返し精度：中 */
    ONESHOT_READ_DATA_LOW,          /* 繰り返し精度：低 */

    /* 周期読み出し：0.5mps(0.5回/sec) */
    PERIODIC_READ_0P5MPS_HIGH,      /* 繰り返し精度：高 */
    PERIODIC_READ_0P5MPS_MIDDLE,    /* 繰り返し精度：中 */
    PERIODIC_READ_0P5MPS_LOW,       /* 繰り返し精度：低 */

    /* 周期読み出し：1mps(1回/sec) */
    PERIODIC_READ_1MPS_HIGH,        /* 繰り返し精度：高 */
    PERIODIC_READ_1MPS_MIDDLE,      /* 繰り返し精度：中 */
    PERIODIC_READ_1MPS_LOW,         /* 繰り返し精度：低 */

    /* 周期読み出し：2mps(2回/sec) */
    PERIODIC_READ_2MPS_HIGH,        /* 繰り返し精度：高 */
    PERIODIC_READ_2MPS_MIDDLE,      /* 繰り返し精度：中 */
    PERIODIC_READ_2MPS_LOW,         /* 繰り返し精度：低 */

    /* 周期読み出し：4mps(4回/sec) */
    PERIODIC_READ_4MPS_HIGH,        /* 繰り返し精度：高 */
    PERIODIC_READ_4MPS_MIDDLE,      /* 繰り返し精度：中 */
    PERIODIC_READ_4MPS_LOW,         /* 繰り返し精度：低 */

    /* 周期読み出し：10mps(10回/sec) */
    PERIODIC_READ_10MPS_HIGH,       /* 繰り返し精度：高 */
    PERIODIC_READ_10MPS_MIDDLE,     /* 繰り返し精度：中 */
    PERIODIC_READ_10MPS_LOW         /* 繰り返し精度：低 */
} e_sht31_Measurement_Mode;

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

// SHT31 湿温度構造体
typedef struct{
float temp_c;   /* 摂氏（℃） */
float temp_f;   /* 華氏（℉） */
float rh; /* 湿度（%） */
} SHT31_DATA_T;

// 温度センサ値➡摂氏変換
#define SENSROR_2_TEMP_C(temp) \
    -45.0 + 175.0 * (temp / ((2^16) - 1))

// 温度センサ値➡華氏変換
#define SENSROR_2_TEMP_F(temp)  \
    -49.0 + 315.0 * (temp / ((2^16) - 1))

// 温度センサ値➡湿度変換
#define SENSROR_2_RH(temp)  \
    100.0 * (temp / ((2^16) - 1))

void SHT31_Reset(uint8_t i2c_addr);
void SHT31_Read(SHT31_DATA_T *p_sth31_data_t);
void SHT31_StatusCheck(void);
void SHT31_End(void);

#endif /* _SHT31_H_ */