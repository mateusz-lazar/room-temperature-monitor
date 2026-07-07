#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <WebServer.h>
#include "../include/config.h"

Adafruit_BMP085 bmp;
WebServer server(80);

void sensor_init(){
  Serial.begin(115200);
  Serial.println("Connecting to the BMP180 sensor");
  while(!bmp.begin()){
    Serial.print(".");
    delay(100);
  }
  Serial.println("Connected to the BMP180 sensor");
}
void wifi_init(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Connecting to WiFi");

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
  }
}
void html_page()
{
  float temperature = bmp.readTemperature();
  String html = "<html>Room temperature sensor<br>Current temperature: ";
  html += String(temperature, 1);
  server.send(200, "text/html", html);
}
void setup() {
  sensor_init();
  wifi_init();
  server.on("/", html_page);
  server.begin();
}
void loop() {
  server.handleClient();
  
}
