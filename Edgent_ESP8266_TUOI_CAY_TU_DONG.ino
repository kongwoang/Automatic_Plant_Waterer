#define BLYNK_TEMPLATE_ID "TMPLr2fKCO0y"
#define BLYNK_DEVICE_NAME "stembiology"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"
#include "DHTesp.h"
DHTesp dht;
int dht_pin=5;
int relay_bomtuoi=4;
int doam_bomtuoi=0;
int doam_tatbomtuoi=0;
int button_bomtuoi=14;
boolean button_bomtuoiState=HIGH;
boolean chedo_hoatdong=0;
unsigned long times=millis();
WidgetLED ledconnect(V0);

void setup()
{
  Serial.begin(115200);
  delay(100);
  pinMode(button_bomtuoi,INPUT_PULLUP);
  pinMode(relay_bomtuoi,OUTPUT);
  digitalWrite(relay_bomtuoi,LOW);
  BlynkEdgent.begin();
  dht.setup(dht_pin, DHTesp::DHT11);
}

void loop() {
  BlynkEdgent.run();
  if(millis()-times>1000){
    //Chớp tắt led connect trên app blynk
    if(ledconnect.getValue()){
      ledconnect.off();
    }else{
      ledconnect.on();
    }
    //Đọc và ghi giá trị nhiệt độ và độ ẩm không khí lên blynk
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity, 1);
    Serial.print("\t\t");
    Serial.print(temperature, 1);
    Blynk.virtualWrite(V1,temperature);
    Blynk.virtualWrite(V2,humidity);
    //Ghi giá trị độ ẩm đất lên blynk
    int doam_dat = analogRead(A0);
    doam_dat = map(doam_dat,0,1023,100,0);
    Serial.println("Độ ẩm đất: " + String(doam_dat));
    Blynk.virtualWrite(V3,doam_dat);
    //Chế độ tự động bơm tưới
    if(chedo_hoatdong==0){
      if(doam_dat<doam_bomtuoi){
        digitalWrite(relay_bomtuoi,HIGH);
        Blynk.virtualWrite(V5,digitalRead(relay_bomtuoi));
        Serial.println("Bật bơm tưới!");
      }else{
        if(doam_dat>doam_tatbomtuoi){
          digitalWrite(relay_bomtuoi,LOW);
          Blynk.virtualWrite(V5,digitalRead(relay_bomtuoi));
          Serial.println("Tắt bơm tưới!");
        }
      }
    }
    times=millis();
  }
  //Điều khiển trực tiếp bằng nút nhấn vật lý
  if(digitalRead(button_bomtuoi)==LOW){
    if(button_bomtuoiState==HIGH){
      button_bomtuoiState=LOW;
      chedo_hoatdong=1;
      Blynk.virtualWrite(V4,HIGH);
      digitalWrite(relay_bomtuoi,!digitalRead(relay_bomtuoi));
      Blynk.virtualWrite(V5,digitalRead(relay_bomtuoi));
      Serial.println("Relay bơm tưới: " + String(digitalRead(relay_bomtuoi)));
      delay(200);
    }
  }else{
    button_bomtuoiState=HIGH;
  }
}
BLYNK_CONNECTED(){
  Blynk.syncVirtual(V4,V5,V6,V7); //Đồng bộ dữ liệu từ server xuống esp khi kết nối
}
BLYNK_WRITE(V4){ 
  chedo_hoatdong = param.asInt();
  Serial.println("Chế độ hoạt động: " + String(chedo_hoatdong));
}
BLYNK_WRITE(V5){
  if(chedo_hoatdong==1){
    int p = param.asInt();
    digitalWrite(relay_bomtuoi,p);
    Serial.println("Relay bơm tưới: " + String(digitalRead(relay_bomtuoi)));
  }
}
BLYNK_WRITE(V6){
  doam_bomtuoi = param.asInt();
  Serial.println("Độ ẩm bơm tưới: " + String(doam_bomtuoi));
}
BLYNK_WRITE(V7){
  doam_tatbomtuoi = param.asInt();
  Serial.println("Độ ẩm tắt bơm tưới: " + String(doam_tatbomtuoi));
}
