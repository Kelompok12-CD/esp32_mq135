#include <WiFi.h>
#include <HTTPClient.h>

// Konfigurasi WiFi (Ganti dengan kredensial lokal sebelum upload ke board)
const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// Konfigurasi ThingSpeak
String apiKey = "WRITE_API_KEY_THINGSPEAK_ANDA";
const char* serverName = "http://api.thingspeak.com/update";

// Pin ESP32 yang terhubung ke A0 MQ-135
const int mqPin = 34; 

void setup() {
  Serial.begin(115200);
  
  // Memulai koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Terhubung ke WiFi! IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Mengecek apakah WiFi masih terhubung
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    
    // Membaca nilai analog dari sensor MQ-135 (resolusi 12-bit: 0-4095)
    int mqValue = analogRead(mqPin); 
    
    // Membuat URL untuk mengirim data ke ThingSpeak Field 1
    String serverPath = String(serverName) + "?api_key=" + apiKey + "&field1=" + String(mqValue);
    
    // Memulai koneksi HTTP
    http.begin(serverPath.c_str());
    
    // Mengirim permintaan GET
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("Sukses! Nilai Gas: ");
      Serial.print(mqValue);
      Serial.print(" | Kode HTTP: ");
      Serial.println(httpResponseCode); // Kode 200 menandakan data berhasil diterima server
    } else {
      Serial.print("Gagal mengirim data. Kode Error: ");
      Serial.println(httpResponseCode);
    }
    
    // Menutup koneksi
    http.end(); 
  } else {
    Serial.println("WiFi Terputus, mencoba menghubungkan kembali...");
    WiFi.reconnect();
  }
  
  // Jeda 30 detik (30000 milidetik)
  delay(30000); 
}
