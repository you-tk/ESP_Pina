#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "owndata.h"

#define ON 1
#define OFF 0
#define OK 1
#define NG 0

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

//int button=14;          // 変数buttonに14を代入する
//int led=13;             // 変数ledに13を代入する

void setup() {
  /* Serial設定初期化 */
  Serial.begin(115200);
  /* Wifi設定初期化 */
  Wifi_init();
  /* UDP設定初期化 */
  UDP_init();

 // pinMode(led, OUTPUT);         // ledピン(13ピン)を出力にする
}

void loop() {

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
  //digitalWrite(led, HIGH);
  //delay(2000);
  //digitalWrite(led, LOW);
  //delay(2000);
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
void Wifi_init(void )
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
void UDP_init(void )
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
