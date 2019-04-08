#include <wiringPi.h>
#include <stdio.h>
int main (void)
{
  int pin1 = 7, pin2 = 0;
  wiringPiSetup () ;
  printf ("Raspberry Pi blink\n") ;
  pinMode (pin1, OUTPUT) ;
  pinMode (pin2, OUTPUT) ;
  for (;;)
  {
    digitalWrite (pin1, HIGH) ; delay (500) ;
    digitalWrite (pin1,  LOW) ; delay (500) ;
	digitalWrite (pin2, HIGH) ; delay (500) ;
    digitalWrite (pin2,  LOW) ; delay (500) ;
  }
  return 0 ;
}