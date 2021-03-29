#include <stdint.h>

#include <Arduino.h>
#include <Wire.h>

#include "SHT31.h"

#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <Wire.h>

// SHT31 I2C Address
// Plz,if ADDR Pin(Pin2) is High,define ADDR_PIN_HIGH
// #define ADDR_PIN_HIGH
#ifdef ADDR_PIN_HIGH
#define SHT31_ADDR  0x45
#else
#define SHT31_ADDR  0x44
#endif

#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#define WORK_BUF_SIZE   64

static void op_msg(void);
static void formattedFloat(float x, uint8_t precision, char *g_work_buf);

// SHT31 Data Struct
SHT31_DATA_T g_sth31_data_t;
float g_pressure;
// Blynk Token
char g_auth_token[] = "PAXbzG5tUatGRAo_RaMpP13iCHRq-xFo";
char g_work_buf[WORK_BUF_SIZE];

static void op_msg(void)
{
    Serial.println("--------------------------");
    Serial.println("SHT31 Sample By GimiTechno");
    Serial.println("--------------------------");
}

static void formattedFloat(float x, uint8_t precision, char *g_work_buf)
{
    dtostrf(x, 7, precision, g_work_buf);
}

BLYNK_READ(V0)
{
    formattedFloat(g_sth31_data_t.temp_c, 1, g_work_buf);
    Blynk.virtualWrite(V0, g_work_buf);
}

BLYNK_READ(V1)
{
    formattedFloat(g_sth31_data_t.rh, 1, g_work_buf);
    Blynk.virtualWrite(V1, g_work_buf);
}

BLYNK_READ(V2)
{
    formattedFloat(g_pressure, 0, g_work_buf);
    Blynk.virtualWrite(V2, g_work_buf);
}

void setup()
{
    Serial.begin(115200);

    op_msg();

    SHT31_Reset(SHT31_ADDR);
    delay(300);

    Blynk.setDeviceName("Blynk_TempBox");
    Blynk.begin(g_auth_token);
}

void loop()
{
    SHT31_Read(&g_sth31_data_t);
    Blynk.run();

#if 0
    op_msg();
    Serial.println("--------------------------");
    Serial.print("Temp = ");
    Serial.print(g_sth31_data_t.temp_c);
    Serial.println(" 'C ");

    Serial.print("Humidity = ");
    Serial.print(g_sth31_data_t.rh);
    Serial.println(" % ");
    Serial.println("--------------------------");
#endif
    delay(100);

}