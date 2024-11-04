#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD dengan alamat I2C dan ukuran layar 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Ganti 0x27 dengan alamat I2C LCD Anda jika berbeda

void setup() {
  // Mulai I2C dengan pin default (SDA = GPIO21, SCL = GPIO22)
  Wire.begin();

  // Inisialisasi LCD dengan ukuran layar 16x2
  lcd.begin(16, 2);
  lcd.backlight();  // Aktifkan lampu latar LCD
  
  // Tampilkan pesan di LCD
  lcd.setCursor(0, 0);  // Pindah ke baris pertama
  lcd.print("Hello, ESP32!");
  lcd.setCursor(0, 1);  // Pindah ke baris kedua
  lcd.print("LCD I2C Display");
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
}
