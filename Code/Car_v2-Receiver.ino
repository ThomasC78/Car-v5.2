#include <WiFi.h>
#include <esp_now.h>
#include <esp_err.h>
#include <ESP32Servo.h>

Servo esc1,esc2,servo;
const int escPin1=5, escPin2=18, servoPin=19;
volatile unsigned long lastRxMs=0;

int mapClamp(int raw,int inMin,int inMax,int outMin,int outMax){
  if(raw<=inMin) return outMin;
  if(raw>=inMax) return outMax;
  return outMin + (raw-inMin)*(outMax-outMin)/(inMax-inMin);
}

int rawToEscUs(uint16_t raw){
  return mapClamp(raw,1820,2800,1000,1200);
}

int rawToServoAngle(uint16_t raw){
  if(raw<=840) return 90;
  if(raw>=2800) return 0;
  if(raw<=1830){
    return mapClamp(raw,840,1830,90,45);
  }else{
    return mapClamp(raw,1830,2800,45,0);
  }
}

void applyOutputs(uint16_t rx,uint16_t ry){
  int escUs=rawToEscUs(rx);
  int ang=rawToServoAngle(ry);
  esc1.writeMicroseconds(escUs);
  esc2.writeMicroseconds(escUs);
  servo.write(ang);
  Serial.print("X: "); Serial.print(rx); Serial.print(" -> ESC: "); Serial.print(escUs);
  Serial.print("   Y: "); Serial.print(ry); Serial.print(" -> Servo: "); Serial.print(ang); Serial.println(" deg");
}

void onRecv(const esp_now_recv_info_t* info,const uint8_t* d,int len){
  if(len>=4){
    uint16_t rx=(uint16_t)d[0] | ((uint16_t)d[1]<<8);
    uint16_t ry=(uint16_t)d[2] | ((uint16_t)d[3]<<8);
    lastRxMs=millis();
    applyOutputs(rx,ry);
  }
}

void setup(){
  Serial.begin(115200);
  delay(1000);

  esc1.setPeriodHertz(50);
  esc2.setPeriodHertz(50);
  esc1.attach(escPin1,1000,2000);
  esc2.attach(escPin2,1000,2000);
  esc1.writeMicroseconds(2000);
  esc2.writeMicroseconds(2000);
  delay(2000);
  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  delay(2000);
  Serial.println("ESCs calibrated");

  servo.attach(servoPin,1000,2000);
  servo.write(90);
  Serial.println("Servo ready");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true,true);
  WiFi.persistent(false);
  WiFi.setSleep(false);
  delay(150);
  if(esp_now_init()!=ESP_OK){ Serial.println("esp_now_init failed"); while(true){} }
  esp_now_register_recv_cb(onRecv);
  lastRxMs=millis();
  Serial.println("Receiver ready");
}

void loop(){
  if(millis()-lastRxMs>500){
    esc1.writeMicroseconds(1000);
    esc2.writeMicroseconds(1000);
    servo.write(90);
  }
  delay(10);
}
