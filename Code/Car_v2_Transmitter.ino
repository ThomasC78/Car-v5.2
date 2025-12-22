#include <WiFi.h>
#include <esp_now.h>
#include <esp_err.h>

uint8_t PEER_MAC[6] = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}; // Change this MAC address to match yours
const int pinX=36;
const int pinY=39;

void onSend(const wifi_tx_info_t*, esp_now_send_status_t s){
  Serial.print("Delivery: "); Serial.println(s==ESP_NOW_SEND_SUCCESS?"OK":"FAIL");
}

void setup(){
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true,true);
  WiFi.persistent(false);
  WiFi.setSleep(false);
  delay(150);
  if(esp_now_init()!=ESP_OK){ Serial.println("esp_now_init failed"); while(true){} }
  esp_now_peer_info_t p={};
  memcpy(p.peer_addr,PEER_MAC,6);
  p.channel=0; p.ifidx=WIFI_IF_STA; p.encrypt=false;
  if(esp_now_add_peer(&p)!=ESP_OK){ Serial.println("add_peer failed"); while(true){} }
  esp_now_register_send_cb(onSend);
  pinMode(pinX,INPUT);
  pinMode(pinY,INPUT);
}

void loop(){
  uint16_t x=analogRead(pinX);
  uint16_t y=analogRead(pinY);
  uint8_t payload[4]={ (uint8_t)(x&0xFF),(uint8_t)(x>>8),(uint8_t)(y&0xFF),(uint8_t)(y>>8) };
  Serial.print("RAW X: "); Serial.print(x); Serial.print("  RAW Y: "); Serial.println(y);
  esp_err_t r=esp_now_send(PEER_MAC,payload,4);
  if(r!=ESP_OK){ Serial.print("esp_now_send error: "); Serial.println(esp_err_to_name(r)); }
  delay(50);
}
