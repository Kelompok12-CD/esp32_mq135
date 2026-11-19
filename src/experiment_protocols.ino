#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <time.h>
#include "secrets.h"

// ========================================================
// PENGATURAN SKENARIO (Ubah angka 1 - 6 sebelum upload)
// 1: HTTP 20B     | 2: HTTP 500B
// 3: MQTT QoS0 20B | 4: MQTT QoS0 500B
// 5: MQTT QoS1 20B | 6: MQTT QoS1 500B
// ========================================================
const int SKENARIO     = 1; 
const int JUMLAH_PESAN = 100;
const int INTERVAL_MS  = 2000;
const int TIMEOUT_MS   = 5000;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
HTTPClient http;

String payloadStr = "";
int payloadLen = 20;
int seq = 1;
unsigned long t_kirim = 0;
bool waitingEcho = false;

String getTimestampWIB() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "2026-11-15 00:00:00";
  }
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buf);
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Menghubungkan MQTT");
    String clientId = "ESP32_Jarots_" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("\nMQTT Terhubung");
      Serial.print("Broker         : "); Serial.println(MQTT_BROKER);
      Serial.print("Topic          : "); Serial.println(MQTT_TOPIC_ECHO);
      int qos = (SKENARIO == 5 || SKENARIO == 6) ? 1 : 0;
      mqttClient.subscribe(MQTT_TOPIC_ECHO, qos);
    } else {
      Serial.print(".");
      delay(500);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (!waitingEcho) return;
  unsigned long t_terima = millis();
  unsigned long rtt = t_terima - t_kirim;
  int rssi = WiFi.RSSI();
  
  Serial.printf("%-8d %-6d %-20s %-8d %-9lu %-10lu %-5lu %-8d %d\n", 
                SKENARIO, seq, getTimestampWIB().c_str(), length, t_kirim, t_terima, rtt, 1, rssi);
  waitingEcho = false;
  seq++;
}

void setup() {
  Serial.begin(115200);
  
  payloadLen = (SKENARIO % 2 == 1) ? 20 : 500;
  for (int i = 0; i < payloadLen; i++) {
    payloadStr += (char)('A' + (i % 26));
  }

  Serial.print("Menghubungkan WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung");
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  Serial.println("Sinkronisasi waktu");
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(200);
  }
  Serial.println("Waktu berhasil disinkronkan\n");

  if (SKENARIO == 1 || SKENARIO == 2) {
    http.setReuse(true);
    http.setTimeout(TIMEOUT_MS);
    Serial.println("========================================");
    Serial.println("       PENGUJIAN HTTP KEEP-ALIVE");
    Serial.println("========================================");
  } else {
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(1024);
    Serial.println("========================================");
    Serial.println("       PENGUJIAN PROTOKOL MQTT");
    Serial.println("========================================");
  }

  Serial.printf("Skenario       : %d\n", SKENARIO);
  Serial.printf("Jumlah pesan   : %d\n", JUMLAH_PESAN);
  Serial.printf("Payload        : %d byte\n", payloadLen);
  Serial.printf("Interval       : %d ms\n", INTERVAL_MS);
  Serial.printf("Timeout        : %d ms\n", TIMEOUT_MS);
  Serial.printf("Waktu mulai    : %s\n\n", getTimestampWIB().c_str());

  Serial.println("==============================================================================================================");
  Serial.println("skenario seq    waktu_mulai          payload  t_kirim  t_terima  rtt   status  rssi");
  Serial.println("==============================================================================================================");

  if (SKENARIO >= 3) {
    reconnectMQTT();
  }
}

void loop() {
  if (seq > JUMLAH_PESAN) return;

  // --- SKENARIO HTTP POST (S1 & S2) ---
  if (SKENARIO == 1 || SKENARIO == 2) {
    String waktuStr = getTimestampWIB();
    int rssi = WiFi.RSSI();
    t_kirim = millis();
    
    http.begin(HTTP_SERVER_URL);
    http.addHeader("Content-Type", "text/plain");
    int httpCode = http.POST(payloadStr);
    unsigned long t_terima = millis();
    unsigned long rtt = t_terima - t_kirim;

    if (httpCode > 0 && rtt <= TIMEOUT_MS) {
      Serial.printf("%-8d %-6d %-20s %-8d %-9lu %-10lu %-5lu %-8d %d\n", 
                    SKENARIO, seq, waktuStr.c_str(), payloadLen, t_kirim, t_terima, rtt, 1, rssi);
    } else {
      Serial.printf("%-8d %-6d %-20s %-8d %-9lu %-10s %-5s %-8d %d\n", 
                    SKENARIO, seq, waktuStr.c_str(), payloadLen, t_kirim, "-", "-", 0, rssi);
    }
    
    seq++;
    long sisaDelay = INTERVAL_MS - (millis() - t_kirim);
    if (sisaDelay > 0) delay(sisaDelay);
  }

  // --- SKENARIO MQTT (S3 - S6) ---
  else {
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();

    if (!waitingEcho) {
      t_kirim = millis();
      waitingEcho = true;
      mqttClient.publish(MQTT_TOPIC_ECHO, payloadStr.c_str());
    }

    if (waitingEcho && (millis() - t_kirim > TIMEOUT_MS)) {
      int rssi = WiFi.RSSI();
      Serial.printf("%-8d %-6d %-20s %-8d %-9lu %-10s %-5s %-8d %d\n", 
                    SKENARIO, seq, getTimestampWIB().c_str(), payloadLen, t_kirim, "-", "-", 0, rssi);
      waitingEcho = false;
      seq++;
      mqttClient.disconnect();
    }

    if (!waitingEcho && seq <= JUMLAH_PESAN) {
      long sisaDelay = INTERVAL_MS - (millis() - t_kirim);
      if (sisaDelay > 0) delay(sisaDelay);
    }
  }
}
