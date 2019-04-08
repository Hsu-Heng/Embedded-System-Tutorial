#include <SoftwareSerial.h>
SoftwareSerial myBT(10, 9); 
char chrRecv;
String connectmac = "985DAD243165";
boolean blestatus = false;
static unsigned long lastDebounceTime1;
static unsigned long lastDebounceTime2;
int sw1 = 2,sw2 = 3;
#define DEBOUNCE_DELAY 200
void setup() {
  Serial.begin(9600);
  Serial.print("BT is Ready"); 
  myBT.begin(9600); 
  Serial.println("Set BT Mode:"+setmode(1));
  Serial.println("My MAC:"+getmac());
  Serial.println("scanDevice:"+scanDevice());
  Serial.println("connectble_mac:"+connectble_mac(connectmac));
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
}
 
void loop() {
  int switchStatus1 = digitalRead(sw1);
  int switchStatus2 = digitalRead(sw2);
  if(switchStatus1 == HIGH){
    updateLed1();
  }
   if(switchStatus2 == HIGH){
    updateLed2();
  }
}
void updateLed1(){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime1) > DEBOUNCE_DELAY){
    lastDebounceTime1 = currentTime;  
    senddata("change-led1");
  }
}
void updateLed2(){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime2) > DEBOUNCE_DELAY){
    lastDebounceTime2 = currentTime;  
    senddata("change-led2");
  }
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


