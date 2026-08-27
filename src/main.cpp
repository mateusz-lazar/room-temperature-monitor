#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <WebServer.h>
#include "time.h"
#include "../include/config.h"
#include "../include/webpage.h"

Adafruit_BMP085 bmp;
WebServer server(80);

struct min_max{
  float min_temp = INIT_MIN;
  float max_temp = INIT_MAX;
  float temp = 0;
};
min_max data_struct;
struct tm timeinfo;
bool midnight_reset = false;
typedef bool (*CheckFn)();

void go_to_sleep(){
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
void init_function(CheckFn check_function, const char* message1, const char* message2, const char* message3){
  int counter = 0;
  Serial.println(message1);
  while(!check_function()){
    if(counter >= CONNECTION_TIMEOUT){
      Serial.println();
      Serial.println(message2);
      go_to_sleep();
    }
    Serial.print(".");
    delay(1000);
    counter++;
  }
  Serial.println(message3);
}
bool sensor_check(){
  return(bmp.begin());
}
bool wifi_check(){
  return(WiFi.status() == WL_CONNECTED);
}
bool ntp_check(){
  return(getLocalTime(&timeinfo));
}
void check_midnight_reset(){
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
  server.send(200, "text/html", html);
}
void data_send(){
  float t = bmp.readTemperature();
  char json[128];
  
  if(isnan(t)){
    Serial.println("Sensor read failed");
    snprintf(json, sizeof(json), "{\"temp\": \"failed to read data\",\"max\":%.1f,\"min\":%.1f}", data_struct.max_temp, data_struct.min_temp);
    server.send(500, "text/plain", json);
    return;
  }
  data_struct.temp = t;

  //change min and max temperature if exceeded
  if(data_struct.temp > data_struct.max_temp){
    data_struct.max_temp = data_struct.temp;
  }
  if(data_struct.temp < data_struct.min_temp){
    data_struct.min_temp = data_struct.temp;
  }

  snprintf(json, sizeof(json), "{\"temp\":%.1f,\"max\":%.1f,\"min\":%.1f}", data_struct.temp, data_struct.max_temp, data_struct.min_temp);
  server.send(200, "text/plain", json);
}
void setup(){
  Serial.begin(115200);
  init_function(sensor_check,"Connecting to the BMP180 sensor", "Failed to connect to the BMP180 sensor, retrying in 15 minutes", "Connected to the BMP180 sensor");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  init_function(wifi_check,"Connecting to the WiFi network", "Failed to connect to the WiFi network, retrying in 15 minutes", "Connected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  configTime(GMT_OFFSET, DST_OFFSET, NTP_ADRESS);
  init_function(ntp_check,"Connecting to the NTP server", "Failed to connect to the NTP server, retrying in 15 minutes", "Connected to the NTP server");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  server.on("/", html_page);
  server.on("/data", data_send);
  server.begin();
 
}
void loop(){
  server.handleClient();
  check_midnight_reset();
}