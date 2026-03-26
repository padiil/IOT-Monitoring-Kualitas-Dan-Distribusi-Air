# IoT Monitoring Kualitas dan Distribusi Air

Sistem ini memonitor kualitas air secara real-time menggunakan ESP32, MQTT, backend Node.js, MongoDB, dan dashboard React.

## Ringkasan Arsitektur (Versi Terbaru)

1. ESP32 membaca/simulasi data sensor: `pH`, `DO`, `BOD`, `COD`, `TSS`, `nitrat`, `fosfat`, `fecal_coliform`.
2. ESP32 menghitung `IPj`, lalu publish payload ke topic MQTT `sensor/data` setiap 1 detik.
3. Backend subscribe `sensor/data`, simpan ke MongoDB collection time-series `realtime_data`, lalu broadcast ke semua client WebSocket.
4. Frontend menerima data lewat WebSocket, menghitung IPj aktif (mode server/manual), menampilkan chart dan status kualitas air.
5. Dari frontend, perubahan mode/nilai manual dikirim via WebSocket ke backend, lalu backend publish ke topic kontrol `sensor/update/<sensor>` untuk ESP32.

## Struktur Folder

```
backend/         # Express + MQTT + WebSocket + MongoDB
frontend/        # React + Vite + Tailwind + Recharts
mikrokontroller/ # PlatformIO (ESP32)
mosquitto.conf   # Konfigurasi broker MQTT lokal
```

## Prasyarat

- Node.js 18+
- npm
- MongoDB 6+ (disarankan)
- Mosquitto MQTT broker (atau broker MQTT lain)
- PlatformIO (untuk upload firmware ESP32)

## Menjalankan Sistem (Local Development)

### 1) Jalankan broker MQTT

Contoh dengan konfigurasi yang ada di `mosquitto.conf`:

- Listener: `1883`
- `allow_anonymous true`

### 2) Jalankan backend

```bash
cd backend
npm install
node app.js
```

Backend default berjalan di `http://localhost:3000`.

### 3) Jalankan frontend

```bash
cd frontend
npm install
npm run dev
```

Frontend default berjalan di `http://localhost:5173`.

### 4) Upload firmware ESP32 (opsional untuk device nyata)

```bash
cd mikrokontroller
pio run -t upload
pio device monitor
```

## Konfigurasi Environment

### Backend (`backend/.env`)

Variabel yang dipakai:

- `PORT` (default: `3000`)
- `MQTT_BROKER` (default: `mqtt://localhost`)
- `MQTT_TOPIC` (default: `sensor/data`)
- `MONGODB_URI` (default: `mongodb://127.0.0.1:27017/IOT_Monitoring_Kualitas_Dan_Distribusi_Air`)
- `REALTIME_COLLECTION` (default: `realtime_data`)
- `REALTIME_TTL_SECONDS` (default: `86400` / 24 jam)

### Frontend (`frontend/.env`)

- `VITE_WS_URL` (default: `ws://localhost:3000`)

### Mikrokontroler (`mikrokontroller/include/secrets.h`)

Salin dari `mikrokontroller/include/secrets.h.example`, lalu isi:

- `WIFI_SSID`
- `WIFI_PASSWORD`
- `MQTT_SERVER`
- `TOPIC` (umumnya `sensor/data`)
- `SAVE_TO_DB_URL` (saat ini tidak dipakai aktif di loop utama)

## Format Payload MQTT

### Data sensor (ESP32 -> backend)

Topic: `sensor/data`

```json
{
	"pH": 7.12,
	"DO": 8,
	"BOD": 4,
	"COD": 13,
	"TSS": 120,
	"nitrat": 5,
	"fosfat": 2,
	"fecal_coliform": 180,
	"IPj": 3.21,
	"waterQuality": "cemar ringan"
}
```

### Kontrol sensor (frontend -> backend -> ESP32)

Topic: `sensor/update/<sensor>`

Payload:

- `useServer: true` -> gunakan bacaan sensor lokal ESP32
- `useServer: false` + `sensorValue` -> gunakan nilai manual dari dashboard

Contoh:

```json
{ "useServer": false, "sensorValue": 9.5 }
```

## Database

- `realtime_data`: collection time-series + TTL (default 24 jam)
- `longterm_data`: ringkasan periodik (avg/max/min)

Catatan: proses agregasi berjalan setiap menit dan mengambil 60 data terakhir dari `realtime_data`.

## Endpoint dan Komunikasi

- HTTP health check: `GET /` -> `WebSocket and MQTT server is running!`
- WebSocket: 1 channel yang sama dengan server backend (`ws://host:3000`)

## Catatan Penting

- Dokumentasi ini sudah disesuaikan dengan kode saat ini (Maret 2026).
- `npm start` belum didefinisikan di backend; gunakan `node app.js`.
- Pengiriman HTTP dari ESP32 ke endpoint `/sensor-data` masih ada di fungsi, tetapi default-nya tidak dipanggil di loop utama.
