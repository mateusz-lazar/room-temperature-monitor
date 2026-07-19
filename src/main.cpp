#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <WebServer.h>
#include "../include/config.h"

Adafruit_BMP085 bmp;
WebServer server(80);

struct min_max{
  float min_temp = 100;
  float max_temp = -100;
};
min_max temp_data;

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
void html_page(){
  String html = 
  "<html><body>Room temperature sensor<br>"
    "Current temperature: "
    "<span id='temp'>"
      "--"
    "</span> &deg;C"
    "<script>"
      "function temp_fetch(){fetch('/temperature').then(r=>r.text()).then(t=>{"
        "document.getElementById('temp').innerText=t;"
      "})};"
    "</script>"
    "<br>Day maximum: "
    "<span id='maximum'>"
      "--"
    "</span> &deg;C"
    "<script>"
      "function maximum_fetch(){fetch('/maximum').then(r=>r.text()).then(t=>{"
        "document.getElementById('maximum').innerText=t;"
      "})};"
    "</script>"
    "<br>Day minimum: "
    "<span id='minimum'>"
      "--"
    "</span> &deg;C"
    "<script>"
      "function minimum_fetch(){fetch('/minimum').then(r=>r.text()).then(t=>{"
        "document.getElementById('minimum').innerText=t;"
      "})};"

      "temp_fetch();"
      "setInterval(temp_fetch, 2000);"
      "maximum_fetch();"
      "setInterval(maximum_fetch, 2000);"
      "minimum_fetch();"
      "setInterval(minimum_fetch, 2000);"
      "</script>"

  "</body></html>";
  server.send(200, "text/html", html);
}
void temperature_send(){
  float temperature = bmp.readTemperature();
  server.send(200, "text/plain", String(temperature, 1));
}
void maximum_send(){
  float temperature = bmp.readTemperature();

  //change min and max temperature if exceeded
  if (temperature > temp_data.max_temp){
    temp_data.max_temp = temperature;
  }
   if (temperature < temp_data.min_temp){
    temp_data.min_temp = temperature;
  }
   server.send(200, "text/plain", String(temp_data.max_temp, 1));
}
void minimum_send(){
  float temperature = bmp.readTemperature();

  //change min and max temperature if exceeded
  if (temperature > temp_data.max_temp){
    temp_data.max_temp = temperature;
  }
   if (temperature < temp_data.min_temp){
    temp_data.min_temp = temperature;
  }
   server.send(200, "text/plain", String(temp_data.min_temp, 1));
}
void setup(){
  sensor_init();
  wifi_init();
  server.on("/", html_page);
  server.on("/temperature", temperature_send);
  server.on("/maximum", maximum_send);
  server.on("/minimum", minimum_send);
  server.begin();
}
void loop(){
  server.handleClient();
}
