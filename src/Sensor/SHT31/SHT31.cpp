#include "SHT31.h"

static void BussReset(void);
static void SoftReset(void);
static void Heater(uint8_t onoff);
static void GetTempHum(void);
static uint16_t ReadStatus(void);

static void i2c_write(uint16_t data);
static void i2c_read(uint8_t *p_buf,uint8_t len);

static uint8_t s_i2c_addr;
static float s_humidity, s_temperature;

/*************************************************************/
/***********************************/
/*         Status Reg Flag         */
/***********************************/
// ステータスレジスタ Bit15 ... Alert Pending Status
static uint8_t pendig_alert_f = DISABLE;
// ステータスレジスタ Bit13 ... Heater Status
static uint8_t heater_f = DISABLE;
// ステータスレジスタ Bit11 ... RH Tracking Alert
static uint8_t rh_track_alert_f = DISABLE;
// ステータスレジスタ Bit10 ... T Tracking Alert
static uint8_t t_track_alert_f = DISABLE;
// ステータスレジスタ Bit4 ... System Reset Detected
static uint8_t reset_f = DISABLE;
// ステータスレジスタ Bit1 ... Command Status
static uint8_t cmd_fail_f = DISABLE;
// ステータスレジスタ Bit0 ... Write Data Checksum Status
static uint8_t write_fail_f = DISABLE;
/*************************************************************/

static void i2c_write(uint16_t data)
{
    Wire.beginTransmission(s_i2c_addr);
    Wire.write(data >> 8);
    Wire.write(data & 0xFF);
    Wire.endTransmission();
}

static void i2c_read(uint8_t *p_buf,uint8_t len)
{
    uint8_t cnt;

    Wire.requestFrom(s_i2c_addr,(uint8_t)len);

    while(Wire.available() != len);

    for(cnt = 0; cnt < len; cnt++)
    {
        *p_buf = (uint8_t) Wire.read();
        p_buf++;
    }
}

static void BussReset(void)
{
    i2c_write(SHT31_I2C_CMD_BUS_RESET);
}

static void SoftReset(void)
{
    i2c_write(SHT31_I2C_CMD_SOFTRESET);
    delay(500);
    i2c_write(SHT31_I2C_CMD_SOFTRESET_2);
    delay(500);
}

static void Heater(uint8_t onoff)
{
    if(onoff == ON){
        i2c_write(SHT31_I2C_CMD_HEATER_ENABLE);
    }else{
        i2c_write(SHT31_I2C_CMD_HEATER_DISABLE);
        }

    delay(500);
}

static uint16_t ReadStatus(void)
{
    uint8_t data[3];
    uint16_t tmp;

    i2c_write(SHT31_I2C_CMD_READ_STATUS);
    i2c_read(&data[0],3);
    tmp = (data[0] << 8) | data[1];

    return tmp;
}

static void GetTempHum(void)
{
    uint8_t data[7];

    i2c_write(SHT31_I2C_CMD_READ_DATA);
    delay(300);

    i2c_read(&data[0],6);

    s_temperature = -45.0 + (175.0 * ((data[0] * 256.0) + data[1]) / 65535.0);
    s_humidity = (100.0 * ((data[3] * 256.0) + data[4])) / 65535.0;
}

void SHT31_Reset(uint8_t i2c_addr)
{
    s_i2c_addr = i2c_addr;
    Wire.begin();

    SoftReset();
    Heater(OFF);
}

void SHT31_Read(SHT31_DATA_T *p_sth31_data_t)
{
    GetTempHum();

    p_sth31_data_t->temp = s_temperature;
    p_sth31_data_t->rh   = s_humidity;
}

void SHT31_StatusCheck(void)
{
    struct sht31_Reg_t sht31_status_reg;
    union status_reg_bit;

    sht31_status_reg.STATUS_REG.WORD = (uint16_t)ReadStatus();

    /*************************************************************/
    /***********************************/
    /* Status Reg Check(Bit0 to Bit15) */
    /***********************************/
    // Bit15 ... Alert Pending Status
    if(sht31_status_reg.STATUS_REG.BIT.bit15 != DISABLE)
    {
        // 保留中アラート発生
        pendig_alert_f = ENABLE;
    }

    // Bit14 ... Reserved

    // Bit13 ... Heater Status
    if(sht31_status_reg.STATUS_REG.BIT.bit13 != DISABLE){
        // Heater ON
        heater_f = ENABLE;
    }

    // Bit12 ... Reserved

    // Bit11 ... RH Tracking Alert
    if(sht31_status_reg.STATUS_REG.BIT.bit11 != DISABLE){
        // アラート発生中
        rh_track_alert_f = ENABLE;
    }

    // Bit10 ... T Tracking Alert
    if(sht31_status_reg.STATUS_REG.BIT.bit10 != DISABLE){
        // アラート発生中
        t_track_alert_f = ENABLE;
    }

    // Bit9 ... Reserved
    // Bit8 ... Reserved
    // Bit7 ... Reserved
    // Bit6 ... Reserved
    // Bit5 ... Reserved

    // Bit4 ... System Reset Detected
    if(sht31_status_reg.STATUS_REG.BIT.bit4 != DISABLE){
        // Reset detected
        reset_f = ENABLE;
    }

    // Bit3 ... Reserved
    // Bit2 ... Reserved

    // Bit1 ... Command Status
    if(sht31_status_reg.STATUS_REG.BIT.bit1 != DISABLE){
        // (ERROR)コマンド実行失敗
        cmd_fail_f = ENABLE;
    }

    // Bit0 ... Write Data Checksum Status
    if(sht31_status_reg.STATUS_REG.BIT.bit0 != DISABLE){
        // (ERROR)最後の書き込み転送のチェックサムが失敗
        write_fail_f = ENABLE;
    }
    /*************************************************************/
}

void SHT31_End(void)
{
    BussReset();
}