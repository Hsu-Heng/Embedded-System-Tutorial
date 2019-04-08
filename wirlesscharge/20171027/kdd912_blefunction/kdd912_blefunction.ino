/*
 * Author: Hus Heng Chou
 * Company: NCHU EE KDD
 * Professor: Hsio Ping Tasi
 * Date: 10/26/2017
 * Version: 1.00
 * Descripe: Setting HM10 and Chat with android
*/
#include <SoftwareSerial.h>
//#include "DHT.h"
//#define DHTPIN 10  // DHT-11 PIN
//#define DHTTYPE DHT11   
//DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor
SoftwareSerial BTserial(8, 9); 
#define ledPin 7
int state = 0;
int potValue = 0;
int potValue1 = 0;
int potValueMapped1 = 0;
char c=' ';   
boolean NL = true; 

          
void setup() {
  Serial.begin(9600);
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__); 
  Serial.println(" ");
  BTserial.begin(9600);
  Serial.println(hm10_at("AT+RENEW",50));
  Serial.println(hm10_at("AT+RESET",1000));
  Serial.println(hm10_at("AT+NAMEGroupTa",50));
  Serial.println(hm10_at("AT+ROLE1",50));
  Serial.println(hm10_at("AT+IMM1?",50));
  Serial.println(hm10_at("AT+MODE2",50));
  
  Serial.println(hm10_at("AT+NAME?",50));
  Serial.println(hm10_at("AT+ROLE?",50));
  Serial.println(hm10_at("AT+ADDR?",50)); 
  Serial.println(hm10_at("AT+DISC?",50)); 
  Serial.println(hm10_at("AT+COND43639DAFB1F",50));
  Serial.println("Input motor.");
//  dht.begin();
}
 String inString = ""; 
 String motor = ""; 
 int angel = 0; 
void loop() {
    // Read from the Serial Monitor and send to the Bluetooth module
    if(BTserial.available() > 0){ // Checks whether data is comming from the serial port
    state = BTserial.read(); // Reads the data from the serial port
 }
 
 if(Serial.available() > 0){
  String str="";  //儲存接收到的回應字串
  char c;  //儲存接收到的回應字元
  while (Serial.available()) {  //若軟體序列埠接收緩衝器還有資料
    c=Serial.read();  //必須放入宣告為 char 之變數 (才會轉成字元)
    str.concat(c);  //串接回應字元 
    delay(10);  //務必要延遲, 否則太快
    }
  str.trim();
  int strlen = str.length();
  str.remove(strlen-5,strlen-1);
  Serial.println(str);
  if(motor==""){
    motor=str;
    Serial.print("motor = ");
    Serial.println(motor);
    Serial.println("Input angle:");
  }else{
    angel = str.toInt();
    Serial.print("motor = ");
    Serial.print(motor);
    Serial.print(" angle = ");
    Serial.println(angel);
    BTserial.print(motor+","+String(angel));  
  BTserial.flush();  
  delay(100);
    motor = "";
    Serial.println("Input motor:");
    }
  
//  char a = Serial.read();
//  int srvostate = (int)a - 48;
//  if(srvostate>0){
////  Serial.println(c);
////	 srvostate = int(c);
//  Serial.println(srvostate);
//	   switch (srvostate) {
//    case 1:
//      BTserial.write(3);
//      break;
//    case 2:
//      //do something when var equals 2
//	   BTserial.write(45);
//      break;
//	case 3:
//      //do something when var equals 2
//	   BTserial.write(90);
//      break;
//	case 4:
//      //do something when var equals 2
//	   BTserial.write(135);
//      break;
//	case 5:
//      //do something when var equals 2
//	   BTserial.write(180);
//      break;
//    default:
//    Serial.println("error");
//      break;
//  }
//  }
	 
  
  
  
 }
 
 // Controlling the LED
 if (state == '1') {
  digitalWrite(ledPin, HIGH); // LED ON
  state = 0;
 }
 else if (state == '0') {
  digitalWrite(ledPin, LOW); // LED ON
  state = 0;
 }
 // Reading the potentiometer
 //potValue = analogRead(A0);
 //int potValueMapped = map(potValue, 0, 1023, 0, 255);
 //if(abs(potValueMapped-potValueMapped1) > 0){
  //BTserial.write(potValueMapped);
  //potValueMapped1 = potValueMapped;
  //}
// BTserial.write(potValueMapped);
// hm10senddata(String(potValueMapped)); // Sends potValue to servo motor
 delay(10);

}

String hm10_response()
{
  String str="";  //儲存接收到的回應字串
  char c;  //儲存接收到的回應字元
  
  while (BTserial.available()) {  //若軟體序列埠接收緩衝器還有資料
    c=BTserial.read();  //必須放入宣告為 char 之變數 (才會轉成字元)
    str.concat(c);  //串接回應字元 
    delay(10);  //務必要延遲, 否則太快
    }
  str.trim();  //去除頭尾空白字元
  return str; 
  }
String hm10_at(String at,int delaytime)
{
  BTserial.print(at);  
  BTserial.flush();  
  delay(delaytime);
  String str = hm10_response();
  return str;
  }
void hm10senddata(String data)
{
  BTserial.println(data);
  BTserial.flush();
  }


