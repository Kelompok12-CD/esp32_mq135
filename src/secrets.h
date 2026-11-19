#ifndef SECRETS_H
#define SECRETS_H

// Kredensial Wi-Fi (Isi sesuai hotspot/WiFi lapangan)
const char* WIFI_SSID = "Kost Jarot";
const char* WIFI_PASS = "takoniis";

// Konfigurasi ThingSpeak (Bagian A)
const char* TS_WRITE_API_KEY = "UWQZEST0IIH5WUTR";
const char* TS_CHANNEL_ID    = "3567594";

// Konfigurasi Endpoint Pengujian (Bagian B - Sesuai Log S1-S6)
const char* HTTP_SERVER_URL  = "http://httpbin.org/post"; 
const char* MQTT_BROKER      = "broker.hivemq.com";
const int   MQTT_PORT        = 1883;
const char* MQTT_TOPIC_ECHO  = "trisjoli/uts/iot/echo";

#endif
