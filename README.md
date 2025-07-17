# Smart AC Control with ESP32, Blynk, and DHT22

Proyek ini adalah sistem otomatisasi sederhana untuk mengontrol AC (Air Conditioner) menggunakan ESP32. AC akan menyala dan mati secara otomatis berdasarkan suhu yang terbaca dari sensor DHT22. Sistem ini juga dapat dikontrol secara manual melalui aplikasi Blynk.

## 🔧 Fitur Utama

- ✅ Kontrol AC otomatis berdasarkan suhu:
  - AC menyala jika suhu >= batas ON
  - AC mati jika suhu <= batas OFF
- ✅ Kontrol manual melalui aplikasi Blynk
- ✅ Status AC ditampilkan secara real-time di LCD dan aplikasi
- ✅ Indikator LED ketika AC menyala
- ✅ Dukungan mode **Manual** dan **Otomatis**
- ✅ Nilai suhu ambang dapat diatur dari aplikasi (V4 dan V5)

## 🧰 Perangkat Keras

- ESP32
- Sensor DHT22
- Modul Relay (1 channel)
- LED (indikator AC)
- LCD I2C 16x2
- Kabel jumper

## 📱 Konfigurasi di Blynk

Gunakan aplikasi **Blynk (versi baru)** dan buat widget sebagai berikut:

| Widget Type   | Virtual Pin | Label                            | Catatan                         |
|---------------|-------------|----------------------------------|----------------------------------|
| Button        | V0          | `Manual AC Control`              | Mode Manual only, ON/OFF (0/1)  |
| Display       | V1          | `Temperature`                    | Menampilkan suhu saat ini       |
| Display       | V3          | `AC Status`                      | Menampilkan "Menyala" / "Mati"  |
| Segmented Switch | V6       | `Mode`                           | Manual = 0, Auto = 1            |
| Slider        | V4          | `Auto ON Temp (>=)`              | Nilai suhu nyala otomatis       |
| Slider        | V5          | `Auto OFF Temp (<=)`             | Nilai suhu mati otomatis        |

> 🧪 Gunakan nilai suhu antara 20–40 derajat Celcius untuk slider.

## 🔌 Pin Konfigurasi

| Fungsi               | Pin ESP32 |
|----------------------|-----------|
| Sensor DHT22         | D14       |
| Relay AC             | D13       |
| LED AC Indicator     | D19       |
| LCD I2C              | SDA/SCL   |

## 🧠 Cara Kerja

1. Saat pertama menyala, sistem tersambung ke WiFi dan Blynk.
2. Mode bisa diatur dari aplikasi (Manual / Auto).
3. Jika dalam mode otomatis:
   - AC menyala jika suhu >= V4
   - AC mati jika suhu <= V5
4. Jika dalam mode manual:
   - AC mengikuti kontrol tombol (V0)
5. Status AC dikirim ke Blynk (V3) dan ditampilkan di LCD.

## 📄 File Utama

- `main.ino` — kode utama ESP32
- `README.md` — dokumentasi ini

## 📦 Library yang Digunakan

Pastikan library berikut sudah diinstal di Arduino IDE:

- `Blynk` (versi terbaru)
- `WiFi` / `WiFiClient`
- `DHT sensor library` oleh Adafruit
- `LiquidCrystal_I2C` untuk LCD
