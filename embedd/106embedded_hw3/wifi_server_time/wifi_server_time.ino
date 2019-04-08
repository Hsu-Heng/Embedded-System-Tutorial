#include <SoftwareSerial.h>
 int arraysdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  String allmonth[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  String myear = "",mmon="",mdays="",mtime = "",mhour = "",mmmin  = "",mmmmins  = "";
  int year = 0;
  int mon = 0;
  int days = 0;
   int hour = 0;
  int mmin = 0;
  int mmins = 0;
  int gmt=8;
  unsigned long timeinterval = 1000;
  unsigned long nowtime = 0;
  String ssid = "Yours ssid";
  String pwd = "pwd";
SoftwareSerial esp8266(4,5);
unsigned long time = 0;
String  timme= "";
void setup() {
Serial.begin(9600);
Serial.println("Arduino...OK");
esp8266.begin(115200);
esp8266.write("AT+UART_DEF=9600,8,1,0,0\r\n");
delay(333);
esp8266.begin(9600);
Serial.println("SDK version:    "+getesp8266sdkversion());
Serial.println("AP Dissconnect"+quit_ap());
Serial.println("Join Ap"+joint_ap(ssid, pwd));
Serial.println("IP:   "+getesp8266getip());
Serial.println("mode:   "+set_mode(1));
Serial.println("set_mux:   "+set_mux(1));
Serial.println("set_server:   "+set_server(8000));
Serial.println("connect:   "+start_tcp(0,"www.google.com",80));
Serial.println("DATE:"+msend_data(0,"GET /"));
Serial.println(isodate(timme));
}

void loop() {
  renew();
if (esp8266.available()) {
Serial.write(esp8266.read());
Serial.println(timme);
}
if (Serial.available()) {
esp8266.write(Serial.read());
}
}
/*ESP8266 函數
*取得ESP8266的回應 getesp8266_response
*開啟重新連接   rstesp8266
*取得SDK VERSION getesp8266sdkversion
*選擇 STATION AP AP+STATION set_mode
*多重連線設定 set_mux  
*SERVER 開啟設定 set_server
*TCP連線  start_tcp
*取得連線 msend_data
*斷開連線  
*
*/
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
String gethttp_response()
{
  String str="";  //儲存接收到的回應字串
  char c;  //儲存接收到的回應字元
  while (esp8266.available()) {  //若軟體序列埠接收緩衝器還有資料
    c=esp8266.read();  //必須放入宣告為 char 之變數 (才會轉成字元)
    Serial.print(c);
    str.concat(c);  //串接回應字元 
    delay(10);  //務必要延遲, 否則太快
    }
  str.trim();  //去除頭尾空白字元
  return str;
  
  }
String rstesp8266()
{
  esp8266.println("AT+RST");  //取得韌體版本
  esp8266.flush();  //等待序列埠傳送完畢
  delay(5000);
  String str = getesp8266_response();
   if (str.indexOf("OK") == -1) {return str.substring(0,str.indexOf("\r\n"));}
  else {return "ng";}
  }
String getesp8266sdkversion()
{
  esp8266.println("AT+GMR");  //取得韌體版本
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str = getesp8266_response();
   if (str.indexOf("OK") == -1) {
    String sdk = str.substring(str.indexOf("SDK version"));
    return sdk.substring(sdk.indexOf(":")+1);}
  else {return "ng";}
  }
String getesp8266getip()
{
  esp8266.println("AT+CIFSR");  
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str = getesp8266_response();
   if (str.indexOf("OK") == -1) {
    String staip = str.substring(str.indexOf("STAIP")+1);
    return staip.substring(staip.indexOf(",")+1,staip.indexOf("+")-1);}
  else {return "ng";}
  }
String joint_ap(String ssid, String pwd) {
  esp8266.println("AT+CWJAP=\"" + ssid + "\",\"" + pwd + "\"");  //連線
  esp8266.flush();  //等待序列埠傳送完畢
  delay(7000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("OK") != -1) {return "OK";}
  else {return "NG";}
  }
String quit_ap() {
  esp8266.println("AT+CWQAP");  //離線
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("OK") != -1) {return "OK";}
  else {return "NG";}
  }
String set_mode(byte mode) {
  esp8266.println("AT+CWMODE=" + String(mode));  //設定工作模式
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("OK") != -1 || str.indexOf("no change") != -1) {return "OK";}
  else {return "NG";}
  }
String set_mux(byte mux) {  //0=single, 1=multiple
  esp8266.println("AT+CIPMUX=" + String(mux));  //設定連線模式
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("OK") != -1) {return "OK";}
  else {return "NG";}
  }
String set_server(byte port) {  //0=single, 1=multipl
  esp8266.println("AT+CIPSERVER=1," + String(port));  //設定連線模式
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("OK") != -1) {return "OK";}
  else {return "NG";}
  }
String start_tcp(byte link ,String address, int port) {
  esp8266.println("AT+CIPSTART=" + String(link)+",\"TCP\",\"" + address + "\"," + String(port)); 
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("CONNECT") != -1) {return "ok";}
  else {return str;}
  }
String send_data(byte link,String s) {
  String s1=s + "\r\n";  //務必加上跳行
  esp8266.println("AT+CIPSEND="+String(link)+","+ String(s1.length())); 
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf(">") != -1) {  //收到 > 開始傳送資料
    esp8266.println(s1); //傳送資料
    esp8266.flush();  //等待序列埠傳送完畢
    delay(7000);   
    str=getesp8266_response();  //取得 ESP8266 回應字串
    if (str.indexOf("+IPD") != -1) {return str;}  //傳送成功會自動拆線
    else {  //傳送不成功須自行拆線
      close_ip(0);  //關閉 IP 連線 
      return "NG";
      }   
    }
  else {  //傳送不成功須自行拆線
    close_ip(0);  //關閉 IP 連線
    return "NG";
    }
  }
String getresponse(int timeout)
{
    String str="";  //儲存接收到的回應字串
  char c;  //儲存接收到的回應字元
   unsigned long t=millis();
    while(millis()<(t+timeout))
    {
      if (esp8266.available()) {
         c=esp8266.read();
          str.concat(c);
//         Serial.print(c);
          }
      }
      
       str.trim();
       
  return str;
   
  }

String msend_data(byte link,String s) {
  
  String s1=s + "\r\n";  //務必加上跳行
  esp8266.println("AT+CIPSEND="+String(link)+","+ String(s1.length())); 
  esp8266.flush();  //等待序列埠傳送完畢
  delay(2000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  delay(1000);
  if (str.indexOf(">") != -1) {  //收到 > 開始傳送資料
    esp8266.println(s1); //傳送資料
    esp8266.flush();  //等待序列埠傳送完畢
    String res = getresponse(5000);
    if (res.indexOf("+IPD") != -1) {
      timme = res.substring(res.indexOf("Date:")+6,res.indexOf("GMT"));
      return res.substring(res.indexOf("Date:")+11,res.indexOf("GMT"));}  //傳送成功會自動拆線
    else {  //傳送不成功須自行拆線
      close_ip(0);  //關閉 IP 連線 
      return "NG";
      }   
    }
  else {  //傳送不成功須自行拆線
    close_ip(0);  //關閉 IP 連線
    return "NG";
    }
  }
String close_ip(byte port) {
  esp8266.println("AT+CIPCLOSE="+String(port));  //關閉 IP 連線
  esp8266.flush();  //等待序列埠傳送完畢
  delay(1000);
  String str=getesp8266_response();  //取得 ESP8266 回應字串
  if (str.indexOf("OK") != -1) {return "OK";}
  else {return "NG";}
  }
/*ISO時間函數
*自動刷新  processrenew
*解析時間  ISODATE
*閏年      decideleapyear
*
*/ 
void renew()
{
  
   if(millis()>(nowtime+timeinterval))
    {
      mmins = mmins+1;
      nowtime = millis();
      timme = processrenew();
      }
  }
String processrenew()
{
  if(mmins>=60)
  {
    mmins=0;
    mmin+=1;
    }
    
   if(mmin>=60)
   {
    mmin=0;
    hour+=1;
    }
     else if(hour>=24)
   {
    hour=0;
    days+=1;
    }
    int monss = mon-1;
     if(days>=arraysdays[monss]&&decideleapyear(year))
   {
    days=1;
    year+=1;
    }
     if(mon==2&&decideleapyear(year))
    {
      if(days==28)
      {
        days+=1;
        }
        else if(days==29)
        {
          days=1;
          year+=1;
          }
     }
     if(days >= arraysdays[monss]&&decideleapyear(year)==false)
     {
      days=1;
    year+=1;
      }
       else if(mon>=12)
     {
      mon=1;
    year+=1;
      }
     
  mmmmins = String(mmins);
  mmmin =  String(mmin);
  mhour = String(hour);
  mdays = String(days);
  mmon = String(mmins);
  myear = String(year);
  
  checkiso();
  String iso = myear+"-" + mmon+"-" + mdays +"T"+ mhour+":"+ mmmin+":"+ mmmmins+"+08:00";
  return iso;
  }
void checkiso()
{
  
   if(mon<10)
   {
    mmon = "0"+String(mon);
    }
      else if(days<10)
   {
    mdays = "0"+String(days);
    }
      if(hour<10)
   {
    mhour = "0"+String(hour);
    }
      if(mmin <10)
   {
    mmmin = "0"+String(mmin);
    }
      if(mmins < 10)
    {
      mmmmins = "0"+String(mmins);
    }
  }
String isodate(String date) 
{  
   myear = myear+date.substring(12,16);   
   mmon = mmon+date.substring(7,11);   
   mmon.trim();
   mdays = date.substring(5,7);
   mon = processmonth(mmon);
   days = mdays.toInt();
   mtime = mtime + date.substring(17);
   mhour = mtime.substring(0,2);
   mmmin = mtime.substring(3,5);
   mmmmins = mtime.substring(6,8);
   year = myear.toInt();
   days = mdays.toInt();
   hour = mhour.toInt();
   if(hour<=16)
   {
    hour+=gmt;
    }
   else
   {
    hour=hour+gmt-24;
    days+=1;
    }
   mmin = mmmin.toInt();
   mmins = mmmmins.toInt();
   checkiso();
  String iso = myear+"-" + mmon+"-" + mdays +"T"+ String(hour)+":"+ mmmin+":"+ mmmmins+"+08:00";
  return iso;
  }
 

boolean decideleapyear(int year)
{
  if((year%4==0)&&((year%100)!=0)||(year%400==0))
  {
    return true;
    }
    else
    {
      return false;
      }
  }


int processmonth(String month)
{
  String allmonth[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  int mmon = 0;
  for(int i = 0; i < 12;i++)
  {
    if(allmonth[i].equals(month))
    {
      mmon=i+1;
      break;
      
      }
    }
    return mmon; 
  }


