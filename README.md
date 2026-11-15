# Sistem Monitoring Kualitas Udara / Gas (MQ-135) Berbasis ESP32 & ThingSpeak

Repositori ini berisi kode sumber *firmware* mikrokontroler ESP32, dataset mentah hasil perekaman sensor selama >7 hari, serta dokumentasi reprodusibilitas untuk sistem pemantauan konsentrasi gas menggunakan sensor **MQ-135** yang dikirimkan melalui protokol **HTTP GET** ke platform cloud **ThingSpeak**.

---

## 📁 Struktur Repositori

```text
├── esp32_mq135_thingspeak.ino   # Kode program utama ESP32 (tanpa kredensial)
├── feeds.csv                    # Dataset mentah dari ThingSpeak (28.372 entri, 5–15 Nov 2026)
└── README.md                    # Dokumentasi konfigurasi dan cara menjalankan sistem
