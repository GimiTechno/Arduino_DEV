#include <stdint.h>

#include <Arduino.h>
#include <Wire.h>

#include "SHT31.h"

// SHT31 I2C Address
// Plz,if ADDR Pin(Pin2) is High,define ADDR_PIN_HIGH
// #define ADDR_PIN_HIGH
#ifdef ADDR_PIN_HIGH
#define SHT31_ADDR  0x45
#else
#define SHT31_ADDR  0x44
#endif

static void op_msg(void);

// SHT31 Data Struct
SHT31_DATA_T g_sth31_data_t;


static void op_msg(void)
{
    Serial.println("--------------------------");
    Serial.println("SHT31 Sample By GimiTechno");
    Serial.println("--------------------------");
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    op_msg();

    SHT31_Reset(SHT31_ADDR);
    delay(300);
}

void loop()
{
    op_msg();

    SHT31_Read(&g_sth31_data_t);

    Serial.println("--------------------------");
    Serial.print("Temp = ");
    Serial.print(g_sth31_data_t.temp_c);
    Serial.println(" 'C ");

    Serial.print("Humidity = ");
    Serial.print(g_sth31_data_t.rh);
    Serial.println(" % ");
    Serial.println("--------------------------");

    delay(1000);
}
