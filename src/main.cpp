#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <WebServer.h>
#include "time.h"
#include "../include/config.h"
#include <ArduinoJson.h>

Adafruit_BMP085 bmp;
WebServer server(80);

struct min_max{
  float min_temp = INIT_MIN;
  float max_temp = INIT_MAX;
  float temp = 0;
};
min_max data_struct;
bool midnight_reset = false;

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
void get_local_time(){
  struct tm timeinfo;
  configTime(GMT_OFFSET, DST_OFFSET, NTP_ADRESS);
  
  if(!getLocalTime(&timeinfo)) {
      Serial.println("Failed to retrieve time data");
      return;
   }
   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
void check_midnight_reset(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return;
  }
  if(timeinfo.tm_hour == 0 && timeinfo.tm_min == 0 && !midnight_reset){
    data_struct.min_temp = INIT_MIN;
    data_struct.max_temp = INIT_MAX;
    midnight_reset = true;
  }
  if(timeinfo.tm_hour == 1 && timeinfo.tm_min == 0 && midnight_reset){
    midnight_reset = false;
  }
}
void html_page(){
  String html = 
   "<html><body>Room temperature sensor<br>"
    "Current temperature: <span id='temp'>--</span> &deg;C<br>"
    "Day maximum: <span id='max'>--</span> &deg;C<br>"
    "Day minimum: <span id='min'>--</span> &deg;C"
    
    "<script>"
      "function data_fetch(){"
        "fetch('/data').then(r=>r.json()).then(d=>{"
          "document.getElementById('temp').innerText=d.temp;"
          "document.getElementById('max').innerText=d.max;"
          "document.getElementById('min').innerText=d.min;"
        "});"
      "}"
      "data_fetch();"
      "setInterval(data_fetch, 2000);"
    "</script>"
  "</body></html>";

  server.send(200, "text/html", html);
}
void data_send(){
  data_struct.temp = bmp.readTemperature();

    //change min and max temperature if exceeded
  if (data_struct.temp > data_struct.max_temp){
    data_struct.max_temp = data_struct.temp;
  }
   if (data_struct.temp < data_struct.min_temp){
    data_struct.min_temp = data_struct.temp;
  }

  String json = "{";
  json += "\"temp\":" + String(data_struct.temp, 1) + ",";
  json += "\"max\":" + String(data_struct.max_temp, 1) + ",";
  json += "\"min\":" + String(data_struct.min_temp, 1) + "}";

  server.send(200, "text/plain", json);
}
void setup(){
  sensor_init();
  wifi_init();
  get_local_time();
  server.on("/", html_page);
  server.on("/data", data_send);
  server.begin();
}
void loop(){
  server.handleClient();
  check_midnight_reset();
}