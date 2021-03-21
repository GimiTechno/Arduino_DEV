#include <stdint.h>

#include <mcp_can.h>
#include <SPI.h>


#define FALSE                   0x00
#define TRUE                    0x01

#define SPI_CS_CAN              10      /* CAN Module CS*/
#define STANDARD_CAN_FRAME      0x00    /* クラシックCANフレーム */
#define CAN_DATA_LENGTH         0x08
#define CAN0_INT                2   /* Int Pin = Arduino Pin2 */

#define CAN_RX_BUF_SIZE         0x20

#define CAN_ID_DEBUG            0x100
#define CAN_ID_ECU_BROADCAST    0x7DF   /* 全ECU反応可能 */

static void App_CAN_Init(void);
static void App_CAN_Main(void);

MCP_CAN CAN0(SPI_CS_CAN);
uint8_t g_can_rx_f = FALSE;
unsigned char g_led_f;  //CANで送信するLEDのON・OFF情報(を格納するための変数)

typedef struct{
    uint8_t data_len;
    uint8_t *p_data_buf;
    uint32_t can_id;
}CAN_DATA_T;

CAN_DATA_T g_can_data_t;
uint8_t g_can_rx_buf[CAN_RX_BUF_SIZE] = {0};
const uint8_t g_can_data_buf[CAN_DATA_LENGTH] = {0x0A,0x0B,0x0C,0xD,0x0E,0x0F,0x00,0xFF};

/**
 * @brief CAN 受信割込みハンドラ
 * 
 */
static void ISR_CAN_RX(void)
{
    g_can_data_t.p_data_buf = g_can_rx_buf;
    CAN0.readMsgBuf(&g_can_data_t.can_id,
                    STANDARD_CAN_FRAME,
                    &g_can_data_t.data_len,
                    g_can_data_t.p_data_buf);

    g_can_rx_f = TRUE;
}

/**
 * @brief CAN初期化
 * 
 */
static void App_CAN_Init(void)
{
    memset(g_can_rx_buf,0x00,sizeof(g_can_rx_buf));
    g_can_rx_f = FALSE;
    g_can_data_t.p_data_buf = g_can_rx_buf;

    if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    {
        Serial.println("MCP2515 Initialized Successfully!");
    }else
    {
        Serial.println("Error Initializing MCP2515...");
    }

    CAN0.setMode(MCP_NORMAL);

    g_can_data_t.can_id = CAN_ID_DEBUG;
    g_can_data_t.data_len = CAN_DATA_LENGTH;
    g_can_data_t.p_data_buf = g_can_data_buf;
}

/**
 * @brief CANアプリメイン
 * 
 */
static void App_CAN_Main(void)
{
    uint8_t res;
    uint8_t cnt;

    // データの受信があれば
    if(g_can_rx_f != FALSE)
    {
        g_can_rx_f = FALSE;

        //格納したデータを表示
        Serial.print("ID : 0x");
        Serial.println(g_can_data_t.can_id,HEX);
        Serial.print("Data_length : 0x");
        Serial.println(g_can_data_t.data_len,HEX);
        Serial.print("Receive_data : ");

        for(cnt = 0;cnt < g_can_data_t.data_len;cnt++)
        {
            Serial.print("0x");
            Serial.print(g_can_data_t.p_data_buf[cnt],HEX);
            Serial.print(" ");
        }
        Serial.println("");
    }else{ /* データ受信していないからリクエスト */
        g_can_data_t.can_id = CAN_ID_DEBUG;
        g_can_data_t.p_data_buf = g_can_data_buf;
        res = CAN0.sendMsgBuf(g_can_data_t.can_id,
                            STANDARD_CAN_FRAME,
                            g_can_data_t.data_len,
                            g_can_data_t.p_data_buf);
#ifdef CAN_DEBUG
        if(res == CAN_OK){
            Serial.println("Message Sent Successfully!");
        }
        else {
            Serial.println("Error Sending Message...");
        }
#endif
    }
}

void setup()
{
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(CAN0_INT),
                    ISR_CAN_RX,
                    CHANGE);
    App_CAN_Init();
}

void loop()
{
    App_CAN_Main();
    delay(1000);
}