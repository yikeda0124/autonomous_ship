#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLVZcF8_Ys"
#define BLYNK_DEVICE_NAME "esp32"
#include <BlynkSimpleEsp32.h>
#include <SoftwareSerial.h>
#include <ESP32Servo.h>
#include "config.h"
#include "HUSKYLENS.h"

HUSKYLENS huskylens;

int value_servo1 = 1550;
int value_servo2 = 1550;
int value_servo3 = 1550;

bool is_power_on = false;
bool is_autonomous_mode = false;

HUSKYLENSResult result;
Servo servo1;
Servo servo2;
Servo servo3;
const int maxUs = 1900;
const int minUs = 1100;
const int servo1Pin = 25;
const int servo2Pin = 26;
const int servo3Pin = 27;
const int huskyPin1 = 16;
const int huskyPin2 = 17;
const int value_straight1 = 1550; // decide me!
const int value_straight2 = 1500;
const int value_straight3 = 1500;
const int value_right1 = 1500; // decide me!
const int value_right2 = 1450;
const int value_right3 = 1450;
const int value_left1 = 1500; // decide me!
const int value_left2 = 1550;
const int value_left3 = 1550;
const int value_xmargin = 80; // decide me!

//ID==2のQRコードのとき（十分近づいたとき）
const int value_near_straight1 = 1550; // decide me!
const int value_near_straight2 = 1500;
const int value_near_straight3 = 1500;
const int value_near_right1 = 1500; // decide me!
const int value_near_right2 = 1450;
const int value_near_right3 = 1450;
const int value_near_left1 = 1500; // decide me!
const int value_near_left2 = 1550;
const int value_near_left3 = 1550;
const int value_near_xmargin = 50; // decide me!



SoftwareSerial huskySerial(huskyPin1, huskyPin2);

void setup()
{
  Serial.begin(115200);
  Blynk.begin(BLINK_API_KEY, WIFI_SSID, WIFI_PASSWORD);
  huskySerial.begin(9600);
  while (!huskylens.begin(huskySerial)){
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  servo1.attach(servo1Pin, minUs, maxUs);
  servo2.attach(servo2Pin, minUs, maxUs);
  servo3.attach(servo3Pin, minUs, maxUs);
}

void loop()
{
  Blynk.run();
  if (is_power_on){
    if (!is_autonomous_mode){
      servo1.writeMicroseconds(value_servo1);
      servo2.writeMicroseconds(value_servo2);
      servo3.writeMicroseconds(value_servo3);
    }else{
      autonomous_main();
    }
  }else{
    stop_servos();
  }
}

void stop_servos(){
  servo1.writeMicroseconds(1500);
  servo2.writeMicroseconds(1500);
  servo3.writeMicroseconds(1500);
}


void turn_right(int tim, char mode){
  if (mode=='n'){
    servo1.writeMicroseconds(value_near_right1);
    servo2.writeMicroseconds(value_near_right2);
    servo3.writeMicroseconds(value_near_right3);
  }else{
    servo1.writeMicroseconds(value_right1);
    servo2.writeMicroseconds(value_right2);
    servo3.writeMicroseconds(value_right3);
  }
  delay(tim);
  stop_servos();
}


void turn_left(int tim, char mode){
  if (mode=='n'){
    servo1.writeMicroseconds(value_near_left1);
    servo2.writeMicroseconds(value_near_left2);
    servo3.writeMicroseconds(value_near_left3);
  }else{
    servo1.writeMicroseconds(value_left1);
    servo2.writeMicroseconds(value_left2);
    servo3.writeMicroseconds(value_left3);
  }
  delay(tim);
  stop_servos();
}


void go_straight(int tim, char mode){
  if (mode=='n'){
    servo1.writeMicroseconds(value_near_straight1);
    servo2.writeMicroseconds(value_near_straight2);
    servo3.writeMicroseconds(value_near_straight3);  
  }else{
    servo1.writeMicroseconds(value_straight1);
    servo2.writeMicroseconds(value_straight2);
    servo3.writeMicroseconds(value_straight3);  
  }
  
  delay(tim);
  stop_servos();
}

bool look_for_qr(){
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
  else{
    result = huskylens.read();
    //printResult(result);  
    return true;
  }
  
  return false;
}

bool is_center(int res){
  if (res <= value_xmargin || res >= 320 - value_xmargin){
    return false;
  }
  return true;
}

void autonomous_main(){
  stop_servos();
  while(is_autonomous_mode && is_power_on){
    bool find_qr = look_for_qr();
    while(!find_qr && is_autonomous_mode && is_power_on){
      turn_right(100, 'a');
      delay(2000);
      find_qr = look_for_qr();
      Blynk.run();    
    }
    if (result.ID == 1){
      if (!is_center(result.xCenter)){
        if (result.xCenter <= value_xmargin){
          turn_right(10, 'a');
        }else{
          turn_left(10, 'a');
        }
        find_qr = false;
        for (int i = 0; i < 10; i++){
          find_qr = look_for_qr();
          if (find_qr) break;
        }
      }
      if (is_center(result.xCenter)){
        go_straight(300, 'a');
        delay(2000);
      }
      Blynk.run();
    }else if (result.ID == 2){
      if (!is_center(result.xCenter)){
        if (result.xCenter <= value_near_xmargin){
          turn_right(10, 'n');
        }else{
          turn_left(10, 'n');
        }
        find_qr = false;
        for (int i = 0; i < 10; i++){
          find_qr = look_for_qr();
          if (find_qr) break;
        }
      }
      if (is_center(result.xCenter)){
        go_straight(300, 'n');
        delay(2000);
      }
      Blynk.run();
    }
    //ドッキングの条件
    //バックのプログラム
  } 
 }//近づいた時にどうするか+回転止める
  
  




    
//320*240左上原点,横x軸縦y
void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){
        Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
    }
    else if (result.command == COMMAND_RETURN_ARROW){
        Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
    }
    else{
        Serial.println("Object unknown!");
    }
}

BLYNK_WRITE(V1){
  is_power_on = param[0].asInt();
}

BLYNK_WRITE(V2){
  is_autonomous_mode = param[0].asInt();
}

BLYNK_WRITE(V5){
  double value = param[0].asDouble();
  value_servo1 = (value/255.0-0.5)*800+1500;
}

BLYNK_WRITE(V6){
  double value = param[0].asDouble();
  value_servo2 = (value/255.0-0.5)*800+1500;
}

BLYNK_WRITE(V7){
  double value = param[0].asDouble();
  value_servo3 = (value/255.0-0.5)*800+1500;
}
