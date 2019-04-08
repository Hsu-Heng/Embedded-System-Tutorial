/**
 * This is an example demonstrating the use of the LiFuelGauge library
 * The example prints battery status information on the serial monitor
 * When an Alert Threshold interrupt is generated, the lowPower ISR is
 * called and afterwards (hypothetically) any running operations are 
 * getting finalized before the system enters sleep mode
 * 
 * Note:
 *   After exiting sleep mode or resetting, give the MAX17043/4 
 *   half a second to perform the first measurements
 */

#include <Wire.h>
#include <LiFuelGauge.h>
#include <SoftwareSerial.h>
SoftwareSerial BTserial(9, 8); 
volatile boolean alert = false;
void lowPower() { alert = true; }
// LiFuelGauge constructor parameters
// 1. IC type, MAX17043 or MAX17044
// 2. Number of interrupt to which the alert pin is associated (Optional) 
// 3. ISR to call when an alert interrupt is generated (Optional)
//
// Creates a LiFuelGauge instance for the MAX17043 IC
// and attaches lowPower to INT0 (PIN2 on most boards, PIN3 on Leonardo)
LiFuelGauge gauge(MAX17043, 0, lowPower);
// A flag to indicate a generated alert interrupt



void setup()
{
    Serial.begin(9600); // Initializes serial port
    // Waits for serial port to connect. Needed for Leonardo only
    BTserial.begin(9600);
    digitalWrite(13, HIGH);
    hm10_at("AT+RENEW",50);
    hm10_at("AT+ROLE0",50);
    hm10_at("AT+IMM1?",50);
    hm10_at("AT+MODE2",50);
    hm10_at("AT+NAMEGroupTa",50);
    hm10_at("AT+NAMEGroupTa",50);
//    hm10_at("AT+RESET",1000);
    myserialsentdata("Version: ");
    myserialsentdata(String(gauge.getVersion()));
    while ( !Serial ) ;
    gauge.reset();  // Resets MAX17043
    delay(200);  // Waits for the initial measurements to be made
    // Sets the Alert Threshold to 10% of full capacity
    gauge.setAlertThreshold(10);
    myserialsentdata(String("Alert Threshold is set to ") + 
                   String(gauge.getAlertThreshold()) + '%');
    //
    digitalWrite(13, LOW);
}

void loop()
{

senddata();
}
 void senddata()
{
  gauge.reset();  // Resets MAX17043
  delay(200);  // Waits for the initial measurements to be made
  gauge.setAlertThreshold(10);
  delay(2000);
  String data = "";
  data.concat(gauge.getSOC()); 
  data.concat(",");
  data.concat(gauge.getVoltage());
  myserialsentdata(data);
   if ( alert )
    {
      
        gauge.clearAlertInterrupt();  // Resets the ALRT pin
        alert = false;
        myserialsentdata("System operations are halted...");
        gauge.sleep();  // Forces the MAX17043 into sleep mode
        while ( true ) ;
    }
  }
void myserialsentdata(String data)
{
  BTserial.println(data);
  BTserial.flush();
  }
void hm10_at(String at,int delaytime)
{
  BTserial.print(at);  
  BTserial.flush();  
  delay(delaytime);
  }


