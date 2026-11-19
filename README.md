# Sistem Monitoring Kualitas Udara / Gas (MQ-135) Berbasis ESP32 & ThingSpeak

Repositori ini berisi kode sumber *firmware* mikrokontroler ESP32 (Bagian A & Bagian B), dataset mentah hasil perekaman sensor selama >7 hari, serta dokumentasi reprodusibilitas untuk sistem pemantauan konsentrasi gas menggunakan sensor **MQ-135** yang dikirimkan melalui protokol **HTTP GET** ke platform cloud **ThingSpeak**, serta pengujian komparasi performa protokol **HTTP Keep-Alive vs MQTT (QoS 0 & QoS 1)**.

---

## 📁 Struktur Repositori

- `esp32_mq135_thingspeak.ino` — Kode program utama ESP32 Bagian A (tanpa kredensial)
- `src/` — Direktori kode sumber pengujian protokol Bagian B:
  - `src/experiment_protocols.ino` — Kode pengujian latensi (RTT) & PDR 6 skenario (HTTP vs MQTT)
  - `src/secrets.h` — Template konfigurasi kredensial jaringan & broker MQTT
- `feeds.csv` — Dataset mentah dari ThingSpeak (28.372 entri, 5–15 Nov 2026)
- `analisis_mq135.py` — Skrip Python untuk analisis statistik dan pembuatan grafik
- `analisis_mq135.png` — Visualisasi grafik hasil pengukuran sensor MQ-135
- `README.md` — Dokumentasi konfigurasi dan cara menjalankan sistem

---

## 🛠️ Spesifikasi Perangkat Keras & Rangkaian Pin

- **Mikrokontroler:** ESP32 DevKit V1 (ESP32 Dev Module)
- **Sensor:** Modul Sensor Gas MQ-135 (Output Analog)

| Pin Sensor MQ-135 | Pin ESP32 | Keterangan |
| :--- | :--- | :--- |
| `VCC` | `VIN` (5V) | Catu daya pemanas (*heater*) sensor MQ-135 |
| `GND` | `GND` | Ground bersama |
| `A0` | `GPIO 34` (ADC1) | Pembacaan sinyal analog (resolusi 12-bit: 0–4095) |

---

## 📦 Dependensi Perangkat Lunak

- **IDE:** Arduino IDE (v1.8.19 atau v2.x) / Wokwi ESP32 Simulator
- **Board Package:** `esp32` by Espressif Systems (v2.0.x atau v3.0.x)
- **Library ESP32:**
  * `WiFi.h` — Manajemen koneksi jaringan nirkabel (IEEE 802.11 / Layer 2).
  * `HTTPClient.h` — Pengiriman permintaan HTTP GET & POST (Layer 7).
  * `PubSubClient.h` — Klien komunikasi protokol MQTT (khusus pengujian Bagian B).

---

## 🔒 Konfigurasi Aman

Demi keamanan dan integritas repositori, seluruh informasi sensitif telah dihapus dari kode sumber (`esp32_mq135_thingspeak.ino` dan `src/secrets.h`) dan diganti dengan *placeholder*. Sebelum mengunggah kode ke ESP32, isi variabel berikut secara lokal:

    const char* ssid = "NAMA_WIFI_ANDA";
    const char* password = "PASSWORD_WIFI_ANDA";
    String apiKey = "WRITE_API_KEY_THINGSPEAK_ANDA";

---

## 🚀 Langkah Menjalankan & Verifikasi (Bagian A — Monitoring)

### 1. Menjalankan Firmware pada ESP32
1. Rangkai sensor MQ-135 ke board ESP32 sesuai tabel pin di atas.
2. Buka file `esp32_mq135_thingspeak.ino` di Arduino IDE.
3. Masukkan kredensial WiFi (`ssid`, `password`) dan `Write API Key` channel ThingSpeak Anda.
4. Pilih board **ESP32 Dev Module** dan port COM yang sesuai, lalu klik **Upload**.
5. Buka **Serial Monitor** pada baud rate **115200**.
6. **Hasil Verifikasi:** ESP32 akan terhubung ke WiFi, menampilkan alamat IP lokal, membaca nilai analog ADC setiap **30 detik** (`delay(30000)`), dan mencetak `Kode HTTP: 200` yang menandakan data berhasil masuk ke `Field 1` ThingSpeak.

### 2. Reprodusibilitas Data & Grafik (`feeds.csv`)
1. File `feeds.csv` diunduh langsung dari menu **Data Import / Export** pada channel ThingSpeak.
2. Dataset mencakup **28.372 baris data** (`entry_id` 1 s.d. 28.372) dari tanggal **5 November 2026 pukul 20:14 WIB** hingga **15 November 2026 pukul 22:57 WIB** (durasi >10 hari) dengan interval rata-rata ~30,8 detik dan tanpa nilai kosong (*0 missing values*).
3. Kolom `field1` merepresentasikan nilai mentah ADC 12-bit (0–4095) yang dapat langsung diolah menggunakan Microsoft Excel, Google Sheets, atau Python (`pandas` & `matplotlib`) untuk menghasilkan kembali statistik harian maupun grafik deret waktu (*time-series*).

---

## 🔬 Pengujian Performa Protokol Jaringan (Bagian B — HTTP vs MQTT)

Selain pemantauan kontinu, dilakukan pengujian komparatif antara **HTTP POST (Keep-Alive)** dan **MQTT (`broker.hivemq.com`, topik `trisjoli/uts/iot/echo`)** menggunakan kode `src/experiment_protocols.ino` untuk mengukur *Round-Trip Time* (RTT), *Packet Delivery Ratio* (PDR), dan *overhead* header.

### Matriks 6 Skenario Pengujian
- **Skenario 1 (S1):** HTTP Keep-Alive — Payload 20 Byte (100 pesan, interval 2000 ms, timeout 5000 ms)
- **Skenario 2 (S2):** HTTP Keep-Alive — Payload 500 Byte (100 pesan, interval 2000 ms, timeout 5000 ms)
- **Skenario 3 (S3):** MQTT QoS 0 — Payload 20 Byte
- **Skenario 4 (S4):** MQTT QoS 0 — Payload 500 Byte
- **Skenario 5 (S5):** MQTT QoS 1 — Payload 20 Byte
- **Skenario 6 (S6):** MQTT QoS 1 — Payload 500 Byte

### Cara Menjalankan Pengujian Bagian B
1. Buka `src/secrets.h` dan sesuaikan kredensial Wi-Fi serta endpoint pengujian.
2. Buka `src/experiment_protocols.ino`, ubah nilai konstanta `const int SKENARIO = 1;` (pilih angka `1` sampai `6` sesuai skenario yang diuji).
3. Unggah program ke ESP32 / jalankan pada simulator dan buka **Serial Monitor** (baud rate **115200**).
4. Sistem akan otomatis melakukan sinkronisasi waktu NTP dan mencetak log pengiriman per paket (`skenario`, `seq`, `waktu_mulai`, `payload`, `t_kirim`, `t_terima`, `rtt`, `status`, `rssi`) hingga mencapai 100 pesan beserta ringkasan PDR di akhir pengujian.

---

## 📊 Grafik & Hasil Analisis Data (`analisis_mq135.py`)

![Grafik Analisis MQ-135](analisis_mq135.png)

### Tabel Statistik Harian Sensor MQ-135 (WIB)

Hasil pengolahan 28.372 entri dari `feeds.csv` menggunakan skrip `analisis_mq135.py`:

| Tanggal (WIB) | Jumlah Data (`count`) | Rata-rata (`mean`) | Median | Minimum (`min`) | Maksimum (`max`) | Standar Deviasi (`std`) |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| 2026-11-05 | 439 | 406,41 | 391,0 | 351 | 3.647 | 165,97 |
| 2026-11-06 | 2.809 | 413,42 | 397,0 | 305 | 3.909 | 138,54 |
| 2026-11-07 | 2.806 | 367,67 | 351,0 | 279 | 2.858 | 81,73 |
| 2026-11-08 | 2.809 | 334,28 | 325,0 | 272 | 1.296 | 54,17 |
| 2026-11-09 | 2.809 | 306,56 | 293,0 | 208 | 4.095 | 93,93 |
| 2026-11-10 | 2.786 | 258,65 | 250,0 | 208 | 1.246 | 41,15 |
| 2026-11-11 | 2.809 | 226,94 | 223,0 | 182 | 481 | 24,53 |
| 2026-11-12 | 2.808 | 240,05 | 233,0 | 166 | 543 | 39,56 |
| 2026-11-13 | 2.807 | 227,10 | 226,0 | 170 | 1.499 | 36,61 |
| 2026-11-14 | 2.806 | 226,90 | 215,0 | 176 | 598 | 35,37 |
| 2026-11-15 | 2.684 | 249,37 | 230,0 | 166 | 4.095 | 110,60 |

### Penjelasan Hasil Pengamatan
- **Kondisi Udara Normal (*Baseline*):** Pembacaan dasar sensor berada di rentang stabil **166 hingga 413,42 ADC** dengan median harian **215,0–397,0 ADC**, menunjukkan kondisi kualitas udara ruangan normal.
- **Lonjakan Sesaat (*Spike*):** Terdeteksi lonjakan hingga batas maksimum ADC **4.095** pada tanggal 9 dan 15 November 2026 (serta 3.647–3.909 pada 5–6 November 2026) akibat paparan gas/asap sesaat sebelum kembali turun ke *baseline*.
- **Stabilitas Pembacaan:** Setelah fase pemanasan awal (*preheating*) pada 5–6 November 2026, nilai standar deviasi harian turun signifikan dari **165,97** menjadi kisaran **24,53–39,56** pada 11–14 November 2026, menandakan sensor bekerja sangat stabil.
