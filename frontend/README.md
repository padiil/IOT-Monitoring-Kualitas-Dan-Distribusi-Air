# Frontend - IoT Monitoring Kualitas Air

Dashboard frontend berbasis React + Vite untuk:

- menampilkan data sensor real-time via WebSocket,
- menghitung dan menampilkan nilai `IPj`,
- menampilkan status kualitas air,
- mengontrol tiap sensor dalam mode `Server` atau `Manual`.

## Stack

- React 18
- Vite 5
- Tailwind CSS
- Recharts
- Radix UI primitives (switch, slider)

## Menjalankan Frontend

```bash
npm install
npm run dev
```

Default URL: `http://localhost:5173`

## Environment Variable

Buat file `.env` di folder `frontend` jika perlu:

```env
VITE_WS_URL=ws://localhost:3000
```

Jika tidak diisi, frontend otomatis memakai `ws://localhost:3000`.

## Fitur Utama

1. **WebSocket status indicator** (connected/disconnected).
2. **Live chart IPj** (menyimpan hingga 10 titik terbaru).
3. **Kartu sensor per parameter**:
	- Mode Server: menampilkan nilai dari backend/ESP32.
	- Mode Manual: slider aktif dan mengirim nilai manual.
4. **Klasifikasi kualitas air** berdasarkan nilai IPj:
	- `<= 1.0`: baik
	- `<= 5.0`: cemar ringan
	- `<= 10.0`: cemar sedang
	- `> 10.0`: cemar berat

## Protokol Data ke Backend

Frontend mengirim pesan WebSocket dengan format:

```json
{
  "type": "switchUpdate",
  "sensor": "pH",
  "useServer": false,
  "sensorValue": 7.5
}
```

Backend akan meneruskan payload ini ke MQTT topic `sensor/update/<sensor>`.

## Scripts

- `npm run dev` - jalankan mode development
- `npm run build` - build production
- `npm run preview` - preview hasil build
- `npm run lint` - jalankan ESLint
