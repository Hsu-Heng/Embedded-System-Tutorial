#include <SoftwareSerial.h>
SoftwareSerial esp8266(4,5);
//SoftwareSerial esp8266(10,11);
void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200); // esp韌體的胞率，若為115200，請記得修改
  esp8266.write("AT+UART_DEF=9600,8,1,0,0\r\n");
  delay(333);
  esp8266.begin(9600); 
}

void loop()
{
    if (esp8266.available()) {  //若軟體串列埠 RX 有收到來自 ESP8266 的回應字元
    Serial.write(esp8266.read());  //在串列埠監控視窗顯示 ESP8266 的回應字元
    }
  if (Serial.available()) {  //若串列埠 RX 有收到來自 PC 的 AT 指令字元 (USB TX)
    esp8266.write(Serial.read());  //將 PC 的傳來的字元傳給 ESP8266
    }
}
