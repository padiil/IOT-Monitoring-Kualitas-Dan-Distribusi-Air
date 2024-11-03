 # Project Monitoring Kualitas Air

 Proyek ini adalah sistem monitoring kualitas air menggunakan ESP32 yang terhubung dengan Wi-Fi, frontend berbasis React dengan Vite, dan backend menggunakan Express.js. Proyek ini mengumpulkan data parameter kualitas air seperti pH, DO, BOD, COD, TSS, nitrat, fosfat, dan fecal coliform, dan mengirim data ini ke server serta menyimpannya dalam database.

 ## Struktur Folder
 Struktur direktori proyek ini adalah sebagai berikut:
 ```
 - mikrokontroller/   # Proyek mikrokontroler dengan PlatformIO
 - frontend/          # Frontend berbasis Vite dan React
 - backend/           # Backend menggunakan Express.js
 ```

 ## Cara Instalasi
 Untuk menjalankan proyek ini, ikuti langkah-langkah di bawah ini:

 ### 1. Mikrokontroler (PlatformIO)
 Masuk ke folder `mikrokontroller` untuk menginstal dependensi dan mengonfigurasi PlatformIO:
 ```bash
 cd mikrokontroller
 pio lib install            # Menginstal library yang diperlukan
 pio run -t upload          # Meng-compile dan meng-upload kode ke ESP32
 ```

 ### 2. Backend (Express.js)
 Masuk ke folder `backend` untuk menginstal dependensi backend:
 ```bash
 cd backend
 npm install                # Menginstal library yang diperlukan
 npm start                  # Menjalankan server Express.js
 ```

 Server akan berjalan di port 3000. Jika diperlukan, ubah alamat IP di kode frontend atau mikrokontroler agar sesuai dengan alamat IP komputer.

 ### 3. Frontend (React + Vite)
 Masuk ke folder `frontend` untuk menginstal dependensi frontend:
 ```bash
 cd frontend
 npm install                # Menginstal library yang diperlukan
 npm run dev                # Menjalankan server pengembangan Vite
 ```

 Frontend akan berjalan di port 5173. Anda dapat mengakses aplikasi di `http://localhost:5173`.

 ## Konfigurasi
 Beberapa konfigurasi penting dalam proyek ini:
 - **IP Server**: Ubah variabel `IP_SERVER` di frontend dan alamat `mqtt_server` di kode mikrokontroler agar sesuai dengan alamat IP dari backend.
 - **Interval Pengiriman Data**: Pengiriman data real-time dikonfigurasi dengan interval 1 detik, dan penyimpanan ke database diatur setiap 30 detik. Kamu bisa mengubah interval ini di kode mikrokontroler.

 ## Arsitektur Proyek
 - **ESP32**: Mengumpulkan data dari sensor kualitas air secara acak (dummy data) dan mengirimnya melalui MQTT ke backend, serta menyimpan data JSON menggunakan HTTP.
 - **Backend (Express.js)**: Menangani koneksi MQTT, menerima data JSON, dan menyimpannya ke database.
 - **Frontend (React + Vite)**: Menampilkan data kualitas air secara real-time dari server menggunakan WebSocket.

 ## Teknologi yang Digunakan
 - **PlatformIO**: Untuk pengembangan kode mikrokontroler
 - **Express.js**: Backend server untuk menerima dan menyimpan data sensor
 - **MongoDB**: Database untuk menyimpan data kualitas air 
 - **React**: Frontend untuk monitoring data kualitas air secara real-time
 - **WebSocket**: Untuk mengirim data dari backend ke frontend
 - **MQTT**: Protokol komunikasi antara mikrokontroler dan server backend

 ## Cara Kerja
 1. **ESP32** mengumpulkan data kualitas air secara acak dan menghitung indeks kualitas air (IPj).
 2. Data dikirim ke server **Express.js** melalui MQTT dan disimpan dalam database.
 3. Server mengirim data terbaru ke **frontend React** melalui WebSocket.
 4. **Frontend React** menampilkan data kualitas air dan statusnya sesuai dengan nilai IPj.
