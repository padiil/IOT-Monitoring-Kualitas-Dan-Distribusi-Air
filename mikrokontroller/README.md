# Mikrokontroler (ESP32 + PlatformIO)

Firmware utama berada di `src/main.cpp`.

Fungsi utama firmware saat ini:

1. Baca/simulasi parameter kualitas air.
2. Hitung `IPj` dan klasifikasi kualitas air.
3. Publish data ke MQTT topic `sensor/data` setiap 1 detik.
4. Subscribe topic kontrol `sensor/update/<sensor>` untuk mode manual/server.
5. Tampilkan status di LCD 16x2.
6. Kontrol 2 servo pintu air berdasarkan IPj.

## Board & Library

- Board: `esp32doit-devkit-v1`
- Framework: Arduino
- Dependensi utama:
  - `PubSubClient`
  - `ArduinoJson`
  - `LiquidCrystal_I2C`
  - `ESP32Servo`

## Setup Credential dan Endpoint

1. Copy file:

```bash
cp include/secrets.h.example include/secrets.h
```

2. Isi nilai sesuai jaringan lokal:

- `WIFI_SSID`
- `WIFI_PASSWORD`
- `MQTT_SERVER`
- `TOPIC` (default: `sensor/data`)
- `SAVE_TO_DB_URL`

> Catatan: `SAVE_TO_DB_URL` ada untuk fungsi HTTP `saveToDatabase()`, tapi default-nya belum dipanggil di loop utama.

## Compile, Upload, Monitor

```bash
pio run
pio run -t upload
pio device monitor
```

## Topic MQTT

### Publish data sensor

- Topic: `sensor/data`
- Payload berisi: `pH`, `DO`, `BOD`, `COD`, `TSS`, `nitrat`, `fosfat`, `fecal_coliform`, `IPj`, `waterQuality`

### Subscribe kontrol dari dashboard

- `sensor/update/pH`
- `sensor/update/DO`
- `sensor/update/BOD`
- `sensor/update/COD`
- `sensor/update/TSS`
- `sensor/update/nitrat`
- `sensor/update/fosfat`
- `sensor/update/fecal_coliform`

Payload kontrol:

```json
{ "useServer": false, "sensorValue": 7.5 }
```

Arti:

- `useServer: true` -> ESP32 pakai pembacaan sensor lokal
- `useServer: false` -> ESP32 pakai nilai manual `sensorValue`

## Ringkas Pin yang Dipakai

- Servo pintu distribusi: GPIO `18`
- Servo pintu treatment: GPIO `19`
- pH sensor: GPIO `34`
- DO sensor: GPIO `35`
- BOD sensor: GPIO `32`
- TSS sensor: GPIO `33`
- Sensor tambahan (mapping COD): GPIO `39`

## Wokwi (Opsional)

File `diagram.json` dan `wokwi.toml` tetap bisa dipakai untuk simulasi.
Pastikan path firmware di `wokwi.toml` mengarah ke hasil build terbaru.
