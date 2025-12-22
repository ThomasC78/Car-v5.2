#include <WiFi.h>
#include <esp_wifi.h>

void setup() {
  Serial.begin(115200);
  delay(100);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_AP);
  delay(100);
  Serial.print("AP  MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  uint8_t ch; wifi_second_chan_t sc;
  esp_wifi_get_channel(&ch, &sc);
  Serial.print("Channel: "); Serial.println(ch);
}

void loop() {}
