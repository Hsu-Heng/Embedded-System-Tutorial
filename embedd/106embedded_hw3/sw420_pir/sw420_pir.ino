int ledsw420 = 2;
int pirled = 3;
int buzzer = 4;
int sw420 = 12;
int pir = 13;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
pinMode(ledsw420, OUTPUT);
pinMode(pirled, OUTPUT);
pinMode(buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  getsw420(50);
  getpirled(50); 
 
}
void getsw420(int timeinterval)
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
      }
      else
      {
        digitalWrite(ledsw420,LOW);
        digitalWrite(buzzer,LOW);
        }
}
void getpirled(int timeinterval)
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
      }
      else
      {
        digitalWrite(pirled,LOW);
        }
} 
