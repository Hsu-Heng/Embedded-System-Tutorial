/*
 * Author: Hsu Heng Chou
 * Company: NCHU EE KDD LAB
 * Professor: Hsio Ping Tasi
 * Version: 1.00
 * Date: 2017/10/26
 * Description: Get the temperture and humidity
 * Use Library: Adafruit DHT. Version:1.2.3
 * Don't use 1.3.0. They have some bug don't fix
 */
#include "DHT.h"
#define DHTPIN yourpin  // DHT-11 PIN
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 test!");
  dht.begin();
  }

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("*C\t");
  Serial.print(f);
  Serial.print("*F\n");
  }


