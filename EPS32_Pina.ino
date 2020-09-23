#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "owndata.h"

#define ON 1
#define OFF 0
#define OK 0
#define NG -1

#define PORT_NO12 12
#define PORT_NO13 13

#define REEDSW_ON_STATE 0xFFFFFFFF
#define REEDSW_INI_STATE 0x00000000

#ifndef ROOTER_SSID && ROOTER_PASS
  #define ROOTER_SSID "xxxxxxxxx"
  #define ROOTER_PASS "xxxxxxxxx"
#endif

#ifndef SEND_UDP_IPADDR && SEND_UDP_PORT
  #define SEND_UDP_IPADDR "xxxxxx"
  #define SEND_UDP_PORT 0000
#endif

#ifndef SEND_UDP_DATA
  #define SEND_UDP_DATA "xxxxxx"
#endif

const char* ssid = ROOTER_SSID;     /* 自分のルータのSSID */
const char* password = ROOTER_PASS; /* 自分のルータのPassWord */

static const char ServerIpadr[] = SEND_UDP_IPADDR;
static const char SendUdpData[] = SEND_UDP_DATA;
unsigned int ServerPort = SEND_UDP_PORT;
 
WiFiUDP SendUDPToServer;

int ReedSW_Port;  /* リードSW用ポート */
int Led_Port;    /* LED用ポート */

void setup() {
  /* Serial設定初期化 */
  Serial.begin(115200);
  /* Wifi設定初期化 */
  Wifi_init();
  /* UDP設定初期化 */
  UDP_init();
  /* Port設定初期化 */
  Port_init();

}

void loop() {

#if 0
  boolean Serialdebug_ret = OFF;
  int udpsendresult = NG;
  
  Serialdebug_ret = SerialDebug_Trigger();
  /* TODO Debug用の処理、後でトリガーの処理は置き換える */
  if(Serialdebug_ret == ON) {
    /* UDP送信 */
    Serial.print("SEND Start");
    Serial.print("\n");
    udpsendresult = SendUDPToServerOrder(ServerIpadr, ServerPort, SendUdpData);
    Serial.print("SEND END SendResult:");
    Serial.print(udpsendresult);
    Serial.print("\n");

  }
  delay(2000);
  (void)ReedSW_Read(PORT_NO12);
  delay(2000);
#else
  boolean ReedSW_State = OFF;
  ReedSW_State = ReedSW_StateSetting();
  if(ReedSW_State == ON) {
    print_info_log("スイッチON");   
  }
  
#endif
}

/*!
 * @fn          Port_init
 * @brief       ポート設定イニシャル用処理
 * @param[in]   non
 * @param[out]  non
 * @return      non
 * @author kumo
 * @date 2020/09/21
 * @version
 * @note なし
 * @attention なし
 * @todo 随時追加
 * @par history
 * 
 */
void Port_init(void)
{
  /* 用途ごとのポート設定 */
  ReedSW_Port = PORT_NO12;  /* リードSW用ポート */
  Led_Port = PORT_NO13;    /* LED用ポート */

  /* 各ポートのIN/OUT設定 */
  pinMode(ReedSW_Port, INPUT);  /*リードSW IN*/
  pinMode(Led_Port, OUTPUT);   /*LED用 OUT*/

  return ;
}

/*!
 * @fn          Wifi_init
 * @brief       Wifi接続イニシャル用処理
 * @param[in]   non
 * @param[out]  non
 * @return      non
 * @author kumo
 * @date 2020/08/29
 * @version
 * @note なし
 * @attention なし
 * @todo 現状接続できるまでループするのでどこかでタイムアウト処理入れるかも
 * @par history
 * 
 */
void Wifi_init(void)
{
  /* Connect to WiFi network */
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Wifi初期化 */
  WiFi.begin(ssid, password);
  /* 接続できるまでループ */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP()); /* IPアドレス表示 */
  return ;
}

/*!
 * @fn          UDP_init
 * @brief       UDPイニシャル用処理
 * @param[in]   non
 * @param[out]  non
 * @return      non
 * @author kumo
 * @date 2020/08/29
 * @version
 * @note なし
 * @attention なし
 * @todo UDPの処理でなんか増えたら増やす。
 * @par history
 * 
 */
void UDP_init(void)
{
  /* UDP送信用サーバー初期化設定 */
  SendUDPToServer.begin(ServerPort);
  return ;
}
/*!
 * @fn          SerialDebug_Trigger
 * @brief       シリアルから文字列受信したら戻り値ON返す、デバッグテスト用
 * @param[in]   non
 * @param[out]  non
 * @return      戻り値ONのときテストしたい処理を実行する
 * @author kumo
 * @date 2020/08/29
 * @version
 * @note なし
 * @attention なし
 * @par history
 * 
 */
boolean SerialDebug_Trigger(void)
{
  boolean ret;  /* 戻り値 */
  int ReadData; /* 読み込みデータ */

  /* 入力があるかチェック */
  ReadData = Serial.read();
  if(ReadData > 0) {
    /*入力あり*/
    ret = ON; 
    Serial.print(ReadData);
    Serial.print("\n");
  }
  else {
    /* 入力なし */
    ret = OFF;
  }
  delay(1000);/* wait用ディレイ */
  return ret;
}
/*!
 * @fn          SendUDPToServerOrder
 * @brief       UDPの送信要求処理
 * @param[in]   IPaddr、port、SendData
 * @param[out]  なし
 * @return      送信結果 1:successful、0:error
 * @author kumo
 * @date 2020/08/29
 * @version
 * @note なし
 * @attention なし
 * @par history
 * 
 */
int SendUDPToServerOrder(const char* IPaddr, uint16_t port, const char* SendData)
{
  int SendResult_ret = NG;

  SendUDPToServer.beginPacket(IPaddr, port);
  SendUDPToServer.write(SendData);
  SendResult_ret = SendUDPToServer.endPacket();

  return SendResult_ret;
}

/*!
 * @fn          LED_CONTRL
 * @brief       LED設定
 * @param[in]   Ctrl_LED、Setting
 * @param[out]  non
 * @return      non
 * @author kumo
 * @date 2020/09/21
 * @version
 * @note なし
 * @attention なし
 * @todo 随時追加
 * @par history
 * 
 */
void LED_CONTRL(int Ctrl_LED,int Setting)
{
  digitalWrite(Ctrl_LED, Setting);
  return ;
}

/*!
 * @fn          ReedSW_Read
 * @brief       ReedSW読み込み
 * @param[in]   ReadPort
 * @param[out]  non
 * @return      読み込み結果 1:ON/0:OFF
 * @author kumo
 * @date 2020/09/21
 * @version
 * @note なし
 * @attention なし
 * @todo 随時追加
 * @par history
 * 
 */
int ReedSW_Read(int ReadPort)
{
  int readValret = 0;
  readValret = digitalRead(ReadPort);
  return readValret;
}

/*!
 * @fn          ReedSW_StateSetting
 * @brief       ReedSWのステータスを設定して戻り値で設定値を返す
 * @param[in]   non
 * @param[out]  non
 * @return      状態 1:ONし続けた/0:OFF
 * @author kumo
 * @date 2020/09/23
 * @version
 * @note なし
 * @attention 一応チャタリング対策のため連続で読み出しを行ってからReedSWの設定をする
 * @todo 随時追加
 * @par history
 * 
 */
boolean ReedSW_StateSetting(void)
{
  int Reed_ret = OFF;
  unsigned int Reed_ON_Continuity = REEDSW_INI_STATE;
  boolean Reed_State_Ret = OFF;

  /* リードスイッチが押されたかチェック */
  Reed_ret = ReedSW_Read(ReedSW_Port);
  if(Reed_ret == ON) {
    /* リードスイッチが押された */
    Reed_ON_Continuity = Reed_ret;
    /*****************************************/
    /* リードスイッチがONし続けたかチェック     */
    /* whileから抜けて戻り値OFFの場合は        */
    /* 途中でOFFしたかチャタリングが発生している*/
    /*****************************************/
    do {
      Reed_ret = ReedSW_Read(ReedSW_Port);
      Reed_ON_Continuity = Reed_ON_Continuity << 1;
      Reed_ON_Continuity += Reed_ret;
      Serial.println(Reed_ON_Continuity, BIN);
    } while((Reed_ret == ON) && (Reed_ON_Continuity != REEDSW_ON_STATE));

    /* リードスイッチの状態設定 */
    switch (Reed_ON_Continuity)
    {
    case REEDSW_ON_STATE:
      /* リードスイッチONし続けた */
      Reed_State_Ret = ON;
      break;
    default:
      /* リードスイッチが途中でOFFしたかチャタリングが発生した */
      /* 無処理 */
      break;
    }
  }
  else {
    /* リードスイッチは押されていない */
    /* 無処理 */
  }
  return Reed_State_Ret;
}

void print_info_log(String str)
{
  Serial.println("[INFO] " + str);
}

void print_debug_log(String str)
{
  Serial.println("[DEBUG] " + str);
}

void print_error_log(String str)
{
  Serial.println("[ERROR] " + str); 
}
