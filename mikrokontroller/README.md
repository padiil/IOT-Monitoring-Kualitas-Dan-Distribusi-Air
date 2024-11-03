## ESP32 Wokwi Simulator on Visual Studio Code X Platformio

[Wokwi](https://wokwi.com/) adalah salah satu platform online simulator berbasis website yang mana kita bisa menjalankan simulasi mikrokontroller(Arduino, ESP32, dll) didalamnya. Wokwi cocok untuk tujuan belajar karena penggunaannya simple dengan interface yang mudah dimengerti.

---

## Persiapan Project

- Install plugin `Wokwi Simulator` di Visual Studio Code
- Buat project baru di Platformio
- Buat file baru dengan nama `wokwi.toml` dan `diagram.json` didalam direktori project.
- Buka file `wokwi.toml` lalu tulis kode berikut:

```java
[wokwi]
version = 1
firmware = 'path-to-your-firmware.hex'
elf = 'path-to-your-firmware.elf'
```

---

## Integrasi VSCode dengan Wokwi

- Pastikan sudah mendaftar akun di Wokwi
- Buka `Command Pallete` di VSCode atau tekan `CTRL-SHIFT-P`.
- Ketik `Wokwi:` lalu pilih yang `Wokwi:Request a New License`.
- Akan muncul pop-up klik saja `open` selanjutnya akan otomatis membuka browser.
- Didalam browser klik `GET YOUR LICENSE` maka akan muncul token, copy token tersebut.
- Akan ada pop-up di browser, klik `Open Visual Studio Code` maka proses instalasi license akan otomatis.
- Jika tidak maka bisa dilakukan secara manual dengan cara:
  - Buka `Command Pallete` atau tekan `CTRL-SHIFT-P`.
  - Ketik `Wokwi:` lalu pilih `Wokwi:Manual Enter License Key`.
  - Paste token yang sudah didapat lalu ENTER.

---

## Create Project

- Wokwi
  - Buat project baru di Wokwi.
  - Buat diagram atau skematiknya dulu.
  - Buka tab `diagram.json` lalu copy semua programnya.
- VSCode
  - Buka file `diagram.json` yang sudah dibuat sebelumnya lalu paste.
  - lakukan Verify program terlebih dahulu jika belum untuk mengetahui direktori firmware project.
  - Buka file `wokwi.toml` yang sudah dibuat sebelumnya.
    - Secara default struktur kodenya seperti ini:
    ```java
    [wokwi]
    version = 1
    firmware = 'path-to-your-firmware.hex'
    elf = 'path-to-your-firmware.elf'
    ```
    - Fokus pada `firmware` dan `elf`. Disini perlu diisi direktori firmware dari project yang sedang dibuat.
    - Untuk mengetahui direktori tersebut, pada folder project masuk ke dalam folder `.pio/bulid/board yang dipakai`. didalamnya akan ada file `firmware.bin` dan `firmware.elf`.
    - Ubah file `wokwi.toml` kurang lebih menjadi seperti berikut:
    ```java
    [wokwi]
    version = 1
    firmware = '.pio\build\esp32doit-devkit-v1\firmware.bin'
    elf = '.pio\build\esp32doit-devkit-v1\firmware.elf'
    ```
    - Pada `esp32doit-devkit-v1` ini adalah board yang dipakai, ini mungkin berbeda tergantung board yang digunakan jadi tidak harus selalu sama.

---

## Run Project

- Setelah dirasa semuanya sudah benar kemudian lakukan Verify lagi untuk me-refrseh firmware yang ada sebelumnya.
- Buka `Command Pallete` atau tekan `CTRL-SHIFT-P`.
- Ketik `Wokwi:` lalu pilih `Wokwi: Start Simulator`.

---

## Note!

- Setiap License umumnya hanya berlaku selama satu bulan.
- Untuk selengkapnya bisa baca langsung dokumentasi dari Wokwi - [Documentation](https://docs.wokwi.com/vscode/getting-started)

---
