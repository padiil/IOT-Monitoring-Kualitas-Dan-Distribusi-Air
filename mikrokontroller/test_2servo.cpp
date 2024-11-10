#include <ESP32Servo.h>  // Library untuk mengontrol servo

// Definisikan objek servo untuk dua pin
Servo communityGateServo;
Servo treatmentGateServo;

const int communityGatePin = 33;  // Pin untuk servo gerbang komunitas
const int treatmentGatePin = 32;  // Pin untuk servo gerbang pengolahan

void setup() {
  Serial.begin(115200);  // Memulai komunikasi serial

  // Menyambungkan servo ke pin yang sesuai
  communityGateServo.attach(communityGatePin);
  treatmentGateServo.attach(treatmentGatePin);

  // Atur posisi awal kedua servo ke 0 derajat (terkunci)
  communityGateServo.write(0);
  treatmentGateServo.write(0);

  delay(2000);  // Tunggu sebentar agar servo stabil
}

void loop() {
  // Gerakkan kedua servo ke 90 derajat (posisi terbuka)
  communityGateServo.write(90);
  treatmentGateServo.write(90);
  Serial.println("Servo membuka pada 90 derajat");
  delay(2000);  // Tunggu 2 detik

  // Gerakkan kedua servo kembali ke 0 derajat (posisi tertutup)
  communityGateServo.write(0);
  treatmentGateServo.write(0);
  Serial.println("Servo menutup pada 0 derajat");
  delay(2000);  // Tunggu 2 detik
}
