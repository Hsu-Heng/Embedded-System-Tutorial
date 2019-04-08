#include <SoftwareSerial.h>
SoftwareSerial esp8266(4,5);
String ssid = "KDD912";
  String pwd = "kdd912@nchu";
  String address = "192.168.50.110";
  int port = 8000;
  int link = 0;
void setup() {
  //初始化
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
  //Server 設定
  Serial.println("RST   "+sendAT("AT+RST",5000));
  Serial.println("Cipmux:   "+sendAT("AT+CIPMUX=1",1000));
  Serial.println("Setport:   "+sendAT("AT+CIPSERVER=1,"+String(8000),1000));
Serial.println("IP:   "+sendAT("AT+CIFSR",1000));
  
}

void loop() {
  // put your main code here, to run repeatedly:
if (esp8266.available()) {  //若軟體串列埠 RX 有收到來自 ESP8266 的回應字元
    Serial.write(esp8266.read());  //在串列埠監控視窗顯示 ESP8266 的回應字元
    }
  if (Serial.available()) {  //若串列埠 RX 有收到來自 PC 的 AT 指令字元 (USB TX)
    esp8266.write(Serial.read());  //將 PC 的傳來的字元傳給 ESP8266
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
String close_ip(int port) {
  esp8266.println("AT+CIPCLOSE="+String(port));  //關閉 IP 連線
  esp8266.flush();  //等待序列埠傳送完畢
  delay(100);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("OK") != -1) {return "OK";}
  else {return "NG";}
  }
 String sentserver(String s)
 {
   String s1=s + "\r\n";  //務必加上跳行
  esp8266.println("AT+CIPSEND="+ String(s1.length())); 
  esp8266.flush();  //等待序列埠傳送完畢
  delay(100);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  delay(100);
  if (str.indexOf(">") != -1) {  //收到 > 開始傳送資料
    esp8266.println(s1); //傳送資料
    esp8266.flush();  //等待序列埠傳送完畢
    String res = getesp8266_response();
    if (res.indexOf("+IPD") != -1) {
      return "seend ok";}  //傳送成功會自動拆線
    else {  //傳送不成功須自行拆線
      close_ip(8000);  //關閉 IP 連線 
      return "fail";
      }   
    }
  else {  //傳送不成功須自行拆線
    close_ip(8000);  //關閉 IP 連線
    return "fail";
    }
  }
