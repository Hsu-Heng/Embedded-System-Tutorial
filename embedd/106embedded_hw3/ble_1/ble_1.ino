#include <SoftwareSerial.h>
SoftwareSerial myBT(10, 9); 
char chrRecv;              
 
void setup() {
  Serial.begin(9600);
  Serial.print("BT is Ready");
  
  myBT.begin(9600);
  
}
 
void loop() {
  if (Serial.available())
  {
    chrRecv = Serial.read(); // Read from Serial Monitor
    myBT.print(chrRecv);    // Write command or data to Bluetooth
  }
 
  if (myBT.available()) // when receive data
  {
    chrRecv = myBT.read(); // Read from Bluetooth
    Serial.print(chrRecv); // Write command or data to Serial Monitor
  }
}
