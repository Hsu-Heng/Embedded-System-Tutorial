#include <SoftwareSerial.h>
#include <Servo.h>
Servo myServo;
int state = 20;
SoftwareSerial BTserial(8, 9); 
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__); 
  Serial.println(" ");
  BTserial.begin(9600);
  Serial.println(hm10_at("AT+RENEW",50));
  Serial.println(hm10_at("AT+NAMEGroupTa",50));
  Serial.println(hm10_at("AT+IMM1",50));
  Serial.println(hm10_at("AT+ROLE0",50));
  Serial.println(hm10_at("AT+MODE2",50));
  Serial.println(hm10_at("AT+RESET",1000));
  Serial.println(hm10_at("AT+NAME?",50));
  Serial.println(hm10_at("AT+ROLE?",50));
  Serial.println(hm10_at("AT+ADDR?",50)); 
  myServo.attach(12);
  String str = "b,50";
  int commaPosition = str.indexOf(',');
  char motor;
  for(int i =0; i < commaPosition;i++){
    motor+=str.charAt(i);
    }
//  char motor = str.substring(0,commaPosition);
  String angelstr = str.substring(commaPosition+1,str.length());
  int angel = angelstr.toInt();
//  Serial.println(motor);
//  Serial.println(angel);
  servoroate(motor, angel);
}
void servoroate(char varmotor,int varangel){
  switch (varmotor) {
  case 'a':
  Serial.println(varmotor);
  Serial.println(varangel);
  myServo.write(varangel);
    // statements
    break;
  case 'b':
  Serial.println(varmotor);
  Serial.println(varangel);
    // statements
    break;
  default:
    // statements
    break;
}
 }
char c = "";
String str="";
void loop() {
  
  // put your main code here, to run repeatedly:
if(BTserial.available() > 0){ // Checks whether data is comming from the serial port
  String str="";  //儲存接收到的回應字串
  char c;  //儲存接收到的回應字元
  
  while (BTserial.available()) {  //若軟體序列埠接收緩衝器還有資料
    c=BTserial.read();  //必須放入宣告為 char 之變數 (才會轉成字元)
    str.concat(c);  //串接回應字元 
    delay(10);  //務必要延遲, 否則太快
    }
  str.trim();
  int commaPosition = str.indexOf(',');
  char motor;
  for(int i =0; i < commaPosition;i++){
    motor+=str.charAt(i);
    }
//  char motor = str.substring(0,commaPosition);
  String angelstr = str.substring(commaPosition+1,str.length());
  int angel = angelstr.toInt();
  Serial.println(motor);
  Serial.println(angel);
  servoroate(motor, angel);
  
//    state = BTserial.read(); // Reads the data from the serial port
//    Serial.println(state);
//	myServo.write(state);
 }
 // Controlling the servo motor
 
// delay(10);
 
 // Reading the button
// buttonState = digitalRead(button);
// if (buttonState == HIGH) {
//   BTserial.write('1'); // Sends '1' to the master to turn on LED
// }
// else {
//   BTserial.write('0');
// }  
} 
String hm10_at(String at,int delaytime)
{
  BTserial.print(at);  
  BTserial.flush();  
  delay(delaytime);
  String str = hm10_response();
  return str;
  }

void pasringdata(String data)
{
  String astr[5] = { "Group:", "Group:","Humidity = ","Temperature = ","Temperature f = "};
  int count = 0;
  int arrays[4];
  for(int i = 0;i<data.length();i++)
  {
    if(data.substring(i, i+1) == ",")
    {
      arrays[count] = i;
      count+=1;
      }
    }
    String first = astr[0];
      first.concat(data.substring(0, arrays[0]));
      Serial.println(first);
      for(int i =0;i<3;i++)
      {
        String first = astr[i+1];
      first.concat(data.substring(arrays[i]+1, arrays[i+1]));
      Serial.println(first);
        }
      String last = astr[4];
      last.concat(data.substring(arrays[3]+1));
      Serial.println(last);
 
  }
  void hm10senddata(String data)
{
  BTserial.println(data);
  BTserial.flush();
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
 void senddata()
{
//  delay(2000);
//  float h = dht.readHumidity();
//  float t = dht.readTemperature();
//  float f = dht.readTemperature(true);
//  if (isnan(h) || isnan(t) || isnan(f)) {
//    Serial.println("Failed to read from DHT sensor!");
//    return;
//    }
//  String data = "data,";
//  data.concat(h); 
//  data.concat(",");
//  data.concat(t);
//  data.concat(",");
//  data.concat(f);
//  hm10senddata(data);
//  Serial.println(data);
  }
