#include <Arduino.h>

#define PH_SENSOR_PIN 34  // Sesuaikan dengan pin yang Anda pilih

// Nilai PH pada buffer pH 7.0
const float TeganganPH7 = 2.65;  // Tegangan yang Anda ukur pada pH 7.0 (gunakan pH air mineral sebagai acuan)
const float PHStep = 0.17;      // PH step yang sudah dihitung sebelumnya

void setup() {
  Serial.begin(115200);
  pinMode(PH_SENSOR_PIN, INPUT);
}

void loop() {
  // Membaca nilai ADC
  int sensorValue = analogRead(PH_SENSOR_PIN);
  
  // Menghitung tegangan berdasarkan pembacaan ADC
  float voltage = sensorValue * (3.3 / 4095.0);  // Membaca tegangan dalam rentang 0-3.3V

  // Menghitung pH berdasarkan rumus kalibrasi
  float pH = 7.0 + ((TeganganPH7 - voltage) / PHStep);

  // Menampilkan hasil ke serial monitor
  Serial.print("Nilai ADC PH: ");
  Serial.println(sensorValue);
  Serial.print("Tegangan PH: ");
  Serial.println(voltage, 3);
  Serial.print("Nilai PH cairan: ");
  Serial.println(pH, 3);
  Serial.println();

  delay(1000);  // Tunggu 1 detik sebelum pembacaan berikutnya
}

