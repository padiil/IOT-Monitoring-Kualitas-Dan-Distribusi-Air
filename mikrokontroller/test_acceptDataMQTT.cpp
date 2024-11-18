#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi
const char* ssid = "Emmm2";
const char* password = "emmmtriplem";

// Konfigurasi MQTT
const char* mqttServer = "192.168.100.187"; // Ganti dengan IP server MQTT Anda
const int mqttPort = 1883;
const char* mqttTopics[] = { // Daftar topik untuk berbagai sensor
  "sensor/update/pH",
  "sensor/update/DO",
  "sensor/update/BOD",
  "sensor/update/COD",
  "sensor/update/TSS",
  "sensor/update/nitrat",
  "sensor/update/fosfat",
  "sensor/update/fecal_coliform"
};
const int numTopics = sizeof(mqttTopics) / sizeof(mqttTopics[0]);

WiFiClient espClient;
PubSubClient client(espClient);

void setupWiFi() {
  Serial.print("Menghubungkan ke WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima di topik: ");
  Serial.println(topic);

  // Parsing JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print("Gagal mem-parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Mendapatkan nilai dari JSON
  bool useServer = doc["useServer"];
  float sensorValue = doc["sensorValue"];
  
  // Menampilkan data ke serial monitor
  Serial.println("Data diterima:");
  Serial.print("useServer: ");
  Serial.println(useServer ? "true" : "false");
  Serial.print("sensorValue: ");
  Serial.println(sensorValue);

  // Tindakan berdasarkan data
  String sensorName = String(topic).substring(String(topic).lastIndexOf('/') + 1); // Nama sensor dari topik
  if (!useServer) {
    Serial.print("Mengatur sensor ");
    Serial.print(sensorName);
    Serial.print(" secara manual dengan nilai: ");
    Serial.println(sensorValue);
    // Lakukan tindakan sesuai sensorValue
    // Misalnya: atur aktuator, relay, atau LED
  } else {
    Serial.print("Mode server aktif untuk sensor ");
    Serial.println(sensorName);
  }
}

void setup() {
  Serial.begin(115200);
  setupWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Menghubungkan ke MQTT
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Terhubung!");
      // Subscribing ke semua topik
      for (int i = 0; i < numTopics; i++) {
        client.subscribe(mqttTopics[i]);
        Serial.print("Subscribed ke topik: ");
        Serial.println(mqttTopics[i]);
      }
    } else {
      Serial.print("Gagal terhubung. Kesalahan: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  client.loop();
}
