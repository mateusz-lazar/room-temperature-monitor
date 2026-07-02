#include <Arduino.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not connect to the BMP180 sensor.");
	while (1) {}
  }
  else{
    Serial.println("connected");
  }

}

void loop() {
    float t;
    Serial.print("Temperature = ");
    Serial.println(bmp.readTemperature());
    delay(1000);
}
