#pragma once

constexpr const char* WIFI_SSID = "your_WiFi_SSID";
constexpr const char* WIFI_PASS = "your_WiFi_password";
constexpr const char* NTP_ADRESS = "pool.ntp.org";

constexpr int TIME_TO_SLEEP = 900; //time to sleep in seconds, default 900 (15 minutes)
constexpr unsigned long long uS_TO_S_FACTOR = 1000000ULL;

constexpr float INIT_MIN = 100;
constexpr float INIT_MAX = -100;

constexpr long GMT_OFFSET = 3600;
constexpr int DST_OFFSET = 3600;

constexpr int CONNECTION_TIMEOUT = 10; //in seconds
