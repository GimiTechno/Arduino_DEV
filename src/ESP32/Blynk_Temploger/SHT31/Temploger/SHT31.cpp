#include "SHT31.h"

static void BussReset(void);
static void SoftReset(void);
static void Heater(uint8_t onoff);
static void GetTempHum(void);
static void Set_Measurement_Mode(uint8_t mode);
static uint16_t ReadStatus(void);

static void i2c_write(uint16_t data);
static void i2c_read(uint8_t *p_buf,uint8_t len);

static uint8_t s_i2c_addr;

static float s_temperature_c; /* 摂氏（℃） */
static float s_temperature_f; /* 華氏（℉） */
static float s_humidity;      /* 湿度（%） */

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

    while(Wire.available() != len)
    {
#ifdef DEBUG_MSG
        Serial.println("I2C Data Wait");
#endif
    }

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

#if 1
    i2c_write(SHT31_I2C_CMD_SOFTRESET_2);
    delay(500);
#endif
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

static void Set_Measurement_Mode(uint8_t mode)
{
    switch (mode)
    {
        /* ワンショットリード */
        case ONESHOT_READ_DATA_HIGH:         /* 繰り返し精度：高 */
            i2c_write(SHT31_I2C_CMD_ONESHOT_READ_DATA_HIGH);
            break;
        case ONESHOT_READ_DATA_MIDDLE:       /* 繰り返し精度：中 */
            i2c_write(SHT31_I2C_CMD_ONESHOT_READ_DATA_MIDDLE);
            break;
        case ONESHOT_READ_DATA_LOW:          /* 繰り返し精度：低 */
            i2c_write(SHT31_I2C_CMD_ONESHOT_READ_DATA_LOW);
            break;

        /* 周期読み出し：0.5mps(0.5回/sec) */
        case PERIODIC_READ_0P5MPS_HIGH:      /* 繰り返し精度：高 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_0P5MPS_HIGH);
            break;
        case PERIODIC_READ_0P5MPS_MIDDLE:    /* 繰り返し精度：中 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_0P5MPS_MIDDLE);
            break;
        case PERIODIC_READ_0P5MPS_LOW:       /* 繰り返し精度：低 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_0P5MPS_LOW);
            break;

        /* 周期読み出し：1mps(1回/sec) */
        case PERIODIC_READ_1MPS_HIGH:        /* 繰り返し精度：高 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_1MPS_HIGH);
            break;
        case PERIODIC_READ_1MPS_MIDDLE:      /* 繰り返し精度：中 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_1MPS_MIDDLE);
            break;
        case PERIODIC_READ_1MPS_LOW:         /* 繰り返し精度：低 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_1MPS_LOW);
            break;

        /* 周期読み出し：2mps(2回/sec) */
        case PERIODIC_READ_2MPS_HIGH:        /* 繰り返し精度：高 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_2MPS_HIGH);
            break;
        case PERIODIC_READ_2MPS_MIDDLE:       /* 繰り返し精度：中 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_2MPS_MIDDLE);
            break;
        case PERIODIC_READ_2MPS_LOW:          /* 繰り返し精度：低 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_2MPS_LOW);
            break;

        /* 周期読み出し：4mps(4回/sec) */
        case PERIODIC_READ_4MPS_HIGH:        /* 繰り返し精度：高 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_4MPS_HIGH);
            break;
        case PERIODIC_READ_4MPS_MIDDLE:      /* 繰り返し精度：中 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_4MPS_MIDDLE);
            break;
        case PERIODIC_READ_4MPS_LOW:         /* 繰り返し精度：低 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_4MPS_LOW);
            break;

        /* 周期読み出し：10mps(10回/sec) */
        case PERIODIC_READ_10MPS_HIGH:       /* 繰り返し精度：高 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_10MPS_HIGH);
            break;
        case PERIODIC_READ_10MPS_MIDDLE:     /* 繰り返し精度：中 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_10MPS_MIDDLE);
            break;
        case PERIODIC_READ_10MPS_LOW:         /* 繰り返し精度：低 */
            i2c_write(SHT31_I2C_CMD_PERIODIC_READ_DATA_10MPS_LOW);
            break;

        default:
            break;
    }
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
    uint16_t tmp;

    i2c_read(&data[0],6);

    tmp = (data[0] * 256.0) + data[1];
    s_temperature_c = SENSOR_2_TEMP_C(tmp); /* 摂氏（℃） */
    s_temperature_f = SENSOR_2_TEMP_F(tmp); /* 華氏（℉） */

    tmp = (data[3] * 256.0) + data[4];
    s_humidity = SENSOR_2_RH(tmp);          /* 湿度（%） */
}

void SHT31_Reset(uint8_t i2c_addr)
{
    s_i2c_addr = i2c_addr;
    Wire.begin();

    // S/Wリセット
    SoftReset();
    delay(500);

    // 内蔵ヒータ無効
    Heater(OFF);
    delay(500);

    // ステータスレジスタクリア
    i2c_write(SHT31_I2C_CMD_CLEAR_STATUS);
    delay(500);

    // 測定モード設定
    Set_Measurement_Mode(PERIODIC_READ_10MPS_HIGH);
    delay(500);
}

void SHT31_Read(SHT31_DATA_T *p_sth31_data_t)
{
    GetTempHum();

    p_sth31_data_t->temp_c = s_temperature_c;   /* 摂氏（℃） */
    p_sth31_data_t->temp_f = s_temperature_f;   /* 華氏（℉） */
    p_sth31_data_t->rh   = s_humidity;          /* 湿度（%） */
}

void SHT31_StatusCheck(void)
{
    struct sht31_Reg_t sht31_status_reg;
    union status_reg_bit;

    sht31_status_reg.STATUS_REG.WORD = (uint16_t)ReadStatus();

    // (DEBUG)
#if 0
    Serial.print("Status Reg = 0x");
    Serial.print(sht31_status_reg.STATUS_REG.WORD);
    Serial.print("(0b");
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit15);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit14);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit13);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit12);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit11);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit10);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit9);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit8);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit7);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit6);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit5);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit4);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit3);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit2);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit1);
    Serial.print(sht31_status_reg.STATUS_REG.BIT.bit0);
    Serial.println(")");
#endif

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