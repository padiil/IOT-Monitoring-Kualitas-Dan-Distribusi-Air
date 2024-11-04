#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// deklarasi fungsi + variabel
void reconnect();
void generateSensorData();
String createJSON();
void saveToDatabase();
void printData();
float calculateIPj();

const char *ssid = "Emmm2";
const char *password = "emmmtriplem";
const char *mqtt_server = "192.168.100.187";
const char *saveToDbUrl = "http://192.168.100.187:3000/sensor-data";
const char *topic = "sensor/data";
const int realTimeDataInterval = 1000;  // 1 detik
const int saveDataToDbInterval = 30000; // 30 detik

WiFiClient espClient;
PubSubClient client(espClient);

// Parameter untuk data sensor
int pH, turbidity, DO, BOD, COD, TSS, nitrat, fosfat, fecal_coliform;

void setup()
{
  Serial.begin(115200);       // Memulai komunikasi serial
  WiFi.begin(ssid, password); // Menghubungkan ke Wi-Fi

  // Loop untuk menunggu koneksi Wi-Fi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Mengatur server MQTT
  client.setServer(mqtt_server, 1883);
}

void loop()
{
  // cek koneksi ke server MQTT
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // Kirim data sensor ke server setiap realTimeDataInterval
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > realTimeDataInterval)
  {
    lastMsg = millis();
    generateSensorData();
    String data = createJSON();
    client.publish(topic, data.c_str());
    Serial.println("Real-time data sent:");
    printData();
  }

  // Simpan data sensor ke database setiap saveDataToDbInterval
  static unsigned long lastSave = 0;
  if (millis() - lastSave > saveDataToDbInterval)
  {
    lastSave = millis(); // Update lastSave sebelum menyimpan
    saveToDatabase();
  }
}

// Fungsi untuk menghasilkan data sensor secara acak (dummy)
void generateSensorData()
{
  pH = random(5, 9);
  DO = random(5, 10);
  BOD = random(2, 6);
  COD = random(10, 20);
  TSS = random(50, 200);
  nitrat = random(1, 10);
  fosfat = random(1, 5);
  fecal_coliform = random(50, 500);
}

// Fungsi untuk membuat JSON dari data sensor
String createJSON()
{
  float IPj = calculateIPj();
  JsonDocument doc;

  doc["pH"] = pH;
  doc["DO"] = DO;
  doc["BOD"] = BOD;
  doc["COD"] = COD;
  doc["TSS"] = TSS;
  doc["nitrat"] = nitrat;
  doc["fosfat"] = fosfat;
  doc["fecal_coliform"] = fecal_coliform;
  doc["IPj"] = IPj;

  if (IPj >= 0 && IPj <= 1.0)
  {
    doc["waterQuality"] = "baik";
  }
  else if (IPj > 1.0 && IPj <= 5.0)
  {
    doc["waterQuality"] = "cemar ringan";
  }
  else if (IPj > 5.0 && IPj <= 10.0)
  {
    doc["waterQuality"] = "cemar sedang";
  }
  else if (IPj > 10.0)
  {
    doc["waterQuality"] = "cemar berat";
  }

  String json;
  serializeJson(doc, json); // Serialisasi JSON ke string
  return json;
}

// Fungsi untuk menghitung IPj berdasarkan parameter yang ada
float calculateIPj()
{
  // Contoh nilai baku Lij (untuk perhitungan contoh)
  float L_pH = 7.0, L_DO = 6.0, L_BOD = 3.0, L_COD = 10.0, L_TSS = 100.0, L_nitrat = 10.0, L_fosfat = 3.0, L_fecal_coliform = 200.0;

  // Perhitungan nilai Ci/Lij untuk masing-masing parameter
  float ratio_pH = pH / L_pH;
  float ratio_DO = DO / L_DO;
  float ratio_BOD = BOD / L_BOD;
  float ratio_COD = COD / L_COD;
  float ratio_TSS = TSS / L_TSS;
  float ratio_nitrat = nitrat / L_nitrat;
  float ratio_fosfat = fosfat / L_fosfat;
  float ratio_fecal_coliform = fecal_coliform / L_fecal_coliform;

  // Menghitung nilai maksimum dan rata-rata
  float maxRatio = max({ratio_pH, ratio_DO, ratio_BOD, ratio_COD, ratio_TSS, ratio_nitrat, ratio_fosfat, ratio_fecal_coliform});
  float avgRatio = (ratio_pH + ratio_DO + ratio_BOD + ratio_COD + ratio_TSS + ratio_nitrat + ratio_fosfat + ratio_fecal_coliform) / 8.0;

  // Menghitung IPj
  float IPj = pow(maxRatio, 2) + pow(avgRatio, 2);
  return IPj;
}

// Fungsi untuk menyimpan data sensor ke database
void saveToDatabase()
{
  String jsonData = createJSON();
  Serial.println("Saving data to database: " + jsonData);
  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(saveToDbUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println("Response from server: " + response);
    }
    else
    {
      Serial.println("Error sending data: " + String(httpResponseCode));
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi not connected, can't send data to server.");
  }
}

// Fungsi untuk melakukan koneksi ulang ke server MQTT
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void printData()
{
  Serial.println("pH: " + String(pH));
  Serial.println("DO: " + String(DO));
  Serial.println("BOD: " + String(BOD));
  Serial.println("COD: " + String(COD));
  Serial.println("TSS: " + String(TSS));
  Serial.println("Nitrat: " + String(nitrat));
  Serial.println("Fosfat: " + String(fosfat));
  Serial.println("Fecal Coliform: " + String(fecal_coliform));
  Serial.println("IPj: " + String(calculateIPj()));
  Serial.println();
}
