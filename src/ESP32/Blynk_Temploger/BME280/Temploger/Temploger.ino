#include <stdint.h>

#include <Arduino.h>
#include <Wire.h>

#include "bme280_i2C.h"

// #define BLYNK_DISABLE   /* Blybk,BLE無効 */

#ifndef BLYNK_DISABLE

#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>

static void app_blynk_init(char *p_auth_token, char *p_device_name);
#define WORK_BUF_SIZE   64

// Blynk Token
char g_auth_token[] = "PAXbzG5tUatGRAo_RaMpP13iCHRq-xFo";
// BLE Deveice Name
char g_device_name[] = "TempBox_BME280";
// BLE Work RAM
char g_work_buffer[WORK_BUF_SIZE];

static void formattedFloat(float x, uint8_t precision, char *p_work_buf);
#endif

// For BME280
bme280_data_t g_bme280_data_t;

static void op_msg(void);

static void op_msg(void)
{
    Serial.println("--------------------------");
    Serial.println("BME280 TempBox By GimiTechno");
    Serial.println("--------------------------");
}

#ifndef BLYNK_DISABLE
static void formattedFloat(float x, uint8_t precision, char *p_work_buf)
{
    dtostrf(x, 7, precision, p_work_buf);
}

static void app_blynk_init(char *p_auth_token, char *p_device_name)
{
    Blynk.setDeviceName(p_device_name);
    Blynk.begin(p_auth_token);
}

BLYNK_READ(V0)
{
    formattedFloat(g_bme280_data_t.temperature, 1, g_work_buffer);
    Blynk.virtualWrite(V0, g_work_buffer);
}

BLYNK_READ(V1)
{
    formattedFloat(g_bme280_data_t.humidity, 1, g_work_buffer);
    Blynk.virtualWrite(V1, g_work_buffer);
}

BLYNK_READ(V2)
{
    formattedFloat(g_bme280_data_t.pressure, 0, g_work_buffer);
    Blynk.virtualWrite(V2, g_work_buffer);
}
#endif

void setup()
{
    Serial.begin(115200);

    op_msg();

#ifndef BLYNK_DISABLE
    app_blynk_init(g_auth_token,g_device_name);
    delay(500);
#endif

    bme280_i2C_init();
    delay(500);
}

void loop()
{
#ifndef BLYNK_DISABLE
    Blynk.run();
#endif
    op_msg();
    bme280_i2c_main(&g_bme280_data_t);
}