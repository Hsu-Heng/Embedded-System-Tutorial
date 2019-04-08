#include <SoftwareSerial.h>
SoftwareSerial esp8266(4,5);
int ledsw420 = 2;
int pirled = 3;
int buzzer = 6;
int sw420 = 12;
int pir = 13;
String ssid = "KDD912";
  String pwd = "kdd912@nchu";
  String address = "192.168.0.101";
  int port = 8877;
  int link = 0;
  unsigned long t=millis();
void setup() {
  //初始化
  pinMode(ledsw420, OUTPUT);
pinMode(pirled, OUTPUT);
pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(115200); // esp韌體的胞率，若為115200，請記得修改
  esp8266.write("AT+UART_DEF=9600,8,1,0,0\r\n");
  delay(333);
  esp8266.begin(9600); 
  Serial.println("SDK version:    "+sendAT("AT+GMR",1000));
  Serial.println("Mode "+sendAT("AT+CWMODE=1",1000));
  Serial.println("AP Dissconnect"+sendAT("AT+CWQAP",1000));
  Serial.println("Join Ap"+sendAT("AT+CWJAP=\"" + ssid + "\",\"" + pwd + "\"",7000));
  Serial.println("IP:   "+sendAT("AT+CIFSR",1000));
  //Client 設定"AT+CIPSTART=" + String(link)+",\"TCP\",\"" + address + "\"," + String(port)
  
  Serial.println("RST   "+sendAT("AT+RST",5000));
  Serial.println("Cipmux:   "+sendAT("AT+CIPMUX=0",1000));
  Serial.println("IP:   "+sendAT("AT+CIFSR",1000));
  Serial.println("Send Server:   "+sentserver("Test"));
}
/* pir sw420 中的判斷值請自行調整*/
void loop() {
  
    Serial.println(sentserver("have man and stole"));
    delay(3000);
  
}
boolean getsw420(int timeinterval)
{
  int mearunt = 0;
  unsigned long t=millis();
  while(millis()<(t+timeinterval))
    {
      if (digitalRead(sw420)==HIGH) 
      {
        mearunt++;       
      }
    }
    if(mearunt>1000)
    {
      digitalWrite(ledsw420,HIGH);
      digitalWrite(buzzer,HIGH);
      return true;
      }
      else
      {
        digitalWrite(ledsw420,LOW);
        digitalWrite(buzzer,LOW);
        return false;
        }
}
boolean getpirled(int timeinterval)
{
  int mearunt = 0;
  unsigned long t=millis();
  while(millis()<(t+timeinterval))
    {
      if (digitalRead(pir)==HIGH) 
      {
        mearunt++;       
      }
    }
    if(mearunt>200)
    {
      digitalWrite(pirled,HIGH);
      return true;
      }
      else
      {
        digitalWrite(pirled,LOW);
        return false;
        }
} 
String getesp8266_response()
{
  String str="";  //儲存接收到的回應字串
  char c;  //儲存接收到的回應字元
  
  while (esp8266.available()) {  //若軟體序列埠接收緩衝器還有資料
    c=esp8266.read();  //必須放入宣告為 char 之變數 (才會轉成字元)
    str.concat(c);  //串接回應字元 
    delay(10);  //務必要延遲, 否則太快
    }
  str.trim();  //去除頭尾空白字元
  return str;
  
  }
  String sendAT(String atcommand, int itime)
{
  esp8266.println(atcommand);  //取得韌體版本
  esp8266.flush();  //等待序列埠傳送完畢
  delay(itime);
  String str = getesp8266_response();
  return str;
  }
void close_ip(byte port) {
  esp8266.println("AT+CIPCLOSE="+String(port));  //關閉 IP 連線
  esp8266.flush();  //等待序列埠傳送完畢
  delay(100);
  
  }
 String sentserver(String s)
 {
  esp8266.println("AT+CIPSTART=\"TCP\",\"" + address + "\"," + String(port)); 
  esp8266.flush();  //等待序列埠傳送完畢
  delay(100);
  String str=getesp8266_response();
  Serial.println(str);
  String s1=s + "\r\n";  //務必加上跳行
  esp8266.println("AT+CIPSEND="+ String(s1.length())); 
  esp8266.flush();  //等待序列埠傳送完畢
  delay(100);
  String str1=getesp8266_response();
  if (str1.indexOf(">") != -1) {  //收到 > 開始傳送資料
    esp8266.println(s1); //傳送資料
    esp8266.flush();  //等待序列埠傳送完畢
    delay(100);
    String res = getesp8266_response();
    if (res.indexOf("SEND") != -1) {
      close_ip(port); 
      return "ok";}  //傳送成功會自動拆線
    else {  //傳送不成功須自行拆線
      close_ip(port);  //關閉 IP 連線 
      return "fail";
      }   
    }
  else {  //傳送不成功須自行拆線
    close_ip(port);  //關閉 IP 連線
    return "fail";
    }
  }
