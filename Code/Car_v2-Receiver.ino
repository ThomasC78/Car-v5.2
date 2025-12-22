#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// Incoming message format
struct ControlMsg {
  uint16_t esc1Pulse;
  uint16_t esc2Pulse;
  uint16_t servoPulse;
};

Servo esc1;
Servo esc2;
Servo servo;

const int escPin1 = 5;    // ESC #1 signal
const int escPin2 = 18;   // ESC #2 signal
const int servoPin = 19;  // Servo signal

// Only accept commands from this MAC
uint8_t senderMAC[6] = {
  0x3C, 0x8A, 0x1F, 0xAE, 0x4E, 0xA8
};

void onDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (memcmp(info->src_addr, senderMAC, 6) != 0) return;

  ControlMsg incoming;
  memcpy(&incoming, data, sizeof(incoming));

  esc1.writeMicroseconds(incoming.esc1Pulse);
  esc2.writeMicroseconds(incoming.esc2Pulse);
  servo.writeMicroseconds(incoming.servoPulse);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  esc1.setPeriodHertz(50);
  esc2.setPeriodHertz(50);
  esc1.attach(escPin1, 1000, 2000);
  esc2.attach(escPin2, 1000, 2000);

  // ESC Calibration
  esc1.writeMicroseconds(2000);
  esc2.writeMicroseconds(2000);
  delay(2000);
  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  delay(2000);
  Serial.println("ESCs calibrated");

  servo.attach(servoPin, 1000, 2000);
  servo.writeMicroseconds(1500);
  delay(500);
  Serial.println("Servo ready");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("Receiver ready");
}

void loop() {
  delay(100);
}