import RPi.GPIO as GPIO
import time
def blinkled( ledarray):
    GPIO.setmode(GPIO.BOARD)
    for r in ledarray:
        GPIO.setup( r, GPIO.OUT)
        GPIO.output(r, GPIO.LOW)

    while(True):
        for r in ledarray:
            for i in range(2):
                if( i==0):
                    GPIO.output(r, GPIO.HIGH)
                    time.sleep(0.5)
                else:
                    GPIO.output(r, GPIO.LOW)
                    time.sleep(0.5)

blinkled([7,11])
