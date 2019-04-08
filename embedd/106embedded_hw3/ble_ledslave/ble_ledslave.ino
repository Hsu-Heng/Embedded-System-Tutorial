#include <SoftwareSerial.h>
SoftwareSerial myBT(10, 9); 
static int led1Status;
static int led2Status;
boolean blestatus = false;
static unsigned long lastDebounceTime1;
static unsigned long lastDebounceTime2;
#define DEBOUNCE_DELAY 200
int led1=13,led2=12;
int sw1 = 2,sw2 = 3;
String bt_recevier = "";
void setup(){
  Serial.begin(9600);
  Serial.print("BT is Ready"); 
  myBT.begin(9600); 
  Serial.println("Set BT Mode:"+setmode(0));
  Serial.println("My MAC:"+getmac());
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);  
  led1Status = LOW;
  led2Status = LOW;
  digitalWrite(led1, led1Status);
  digitalWrite(led2, led2Status);
}

void updateLed1(){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime1) > DEBOUNCE_DELAY){
    lastDebounceTime1 = currentTime;  
    led1Status = led1Status == HIGH ? LOW : HIGH;
    digitalWrite(led1, led1Status);
    Serial.println(led1Status);
  }
}
void updateLed2(){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime2) > DEBOUNCE_DELAY){
    lastDebounceTime2 = currentTime;  
    led2Status = led2Status == HIGH ? LOW : HIGH;
    digitalWrite(led2, led2Status);
    Serial.println(led2Status);
  }
}
void bleled1(){
    led1Status = led1Status == HIGH ? LOW : HIGH;
    digitalWrite(led1, led1Status);
    Serial.println(led1Status);
  }

void bleled2(){
    led2Status = led2Status == HIGH ? LOW : HIGH;
    digitalWrite(led2, led2Status);
    Serial.println(led2Status);
  }
void processmaster()
{
  char c;
  while(myBT.available()) 
      {
        
        c=myBT.read();
        bt_recevier.concat(c);
      }
      bt_recevier.trim();
  }
void passingdata()
{
  if(bt_recevier.equals("change-led1"))
  {
    led1Status = led1Status == HIGH ? LOW : HIGH;
    digitalWrite(led1, led1Status);
    bt_recevier = "";
    }
  if(bt_recevier.equals("change-led2"))  {
     led2Status = led2Status == HIGH ? LOW : HIGH;
    digitalWrite(led2, led2Status);
    bt_recevier = "";
    }
  }
void loop(){
  int switchStatus1 = digitalRead(sw1);
  int switchStatus2 = digitalRead(sw2);
  if(switchStatus1 == HIGH){
    updateLed1();
  }
   if(switchStatus2 == HIGH){
    updateLed2();
  }
  processmaster();
  passingdata();
}

String getbleResponse(int timeout)
{
  String str="";  //儲存接收到的回應字串
  char c;  //儲存接收到的回應字元
  unsigned long t=millis();
    while(millis()<(t+timeout))
    {
      if (myBT.available()) 
      {
        c=myBT.read();
        str.concat(c);
      }
    }     
    str.trim();
       
  return str;
  }
String setmode(int mode)    //mode 0: slave, mode 1: master
{
    myBT.println("AT+ROLE"+String(mode));
    myBT.flush();
    String str = getbleResponse(1000);
  if(str.indexOf("+ROLE")!=-1)
  {
    return str;
    }else{
      return "connected fail";}
  }
String getmac()    //mode 0: slave, mode 1: master
{
    myBT.println("AT+LADDR");
    myBT.flush();
    String str = getbleResponse(1000);
  if(str.indexOf("+LADDR")!=-1)
  {
    return str;
    }else{
      return "connected fail";}
  }
String connectble_mac(String mac)
{
  myBT.println("AT+CONA"+mac);
  myBT.flush();
  String str = getbleResponse(1000);
  if(str.indexOf("OK")!=-1)
  {
    blestatus = true;
    return "connected";
    }else{
      blestatus = false;
      return "connected fail";}
}
String scanDevice()
{
  myBT.println("AT+INQ");
  myBT.flush();
  delay(5000);
  String str = getbleResponse(3000);
  if(str.indexOf("OK")!=-1)
  {
    return str;
    }else{
      return "at fail";}
}
String closeconnect()
{
  myBT.println("AT+DISC");
  myBT.flush();
  delay(100);
  String str = getbleResponse(100);
  if(str.indexOf("OK")!=-1)
  {
    blestatus = false;
    return "connected close";
    }else{
      blestatus = true;
      return "connected now";
      }
} 
void senddata(String data)
{
      myBT.println(data);
      myBT.flush();
}
