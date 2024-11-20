#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// Initialize LCD with I2C address and 16x2 display size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Declare functions + variables
void reconnect();
void generateSensorData();
String createJSON();
void saveToDatabase();
void printData();
float calculateIPj();
String setWaterQuality(float IPj);
void controlWaterGates(float IPj);
float getSensorPH();
void callback(char *topic, byte *payload, unsigned int length);

const char *ssid = "MIMIN KOST";
const char *password = "17081945";
const char *mqtt_server = "192.168.18.34";
const char *saveToDbUrl = "http://192.168.18.34:3000/sensor-data";
const char *mqttSendTopic = "sensor/data";
const int realTimeDataInterval = 1000;  // 1 second
const int saveDataToDbInterval = 30000; // 30 seconds

const int mqttPort = 1883;
const char *mqttAcceptTopics[] = { // Daftar topik untuk berbagai sensor
    "sensor/update/pH",
    "sensor/update/DO",
    "sensor/update/BOD",
    "sensor/update/COD",
    "sensor/update/TSS",
    "sensor/update/nitrat",
    "sensor/update/fosfat",
    "sensor/update/fecal_coliform"};
const int numTopics = sizeof(mqttAcceptTopics) / sizeof(mqttAcceptTopics[0]);

// Tambahkan variabel untuk melacak mode useServer untuk setiap sensor
bool useServerPH = true, useServerDO = true, useServerBOD = true, useServerCOD = true;
bool useServerTSS = true, useServerNitrat = true, useServerFosfat = true, useServerFecalColiform = true;

// Tambahkan variabel untuk menyimpan nilai dari server
float serverPH = 7.0, serverDO = 9.0, serverBOD = 5.0, serverCOD = 15.0;
int serverTSS = 100, serverNitrat = 5, serverFosfat = 2, serverFecalColiform = 300;

WiFiClient espClient;
PubSubClient client(espClient);

// Parameters for sensor data
float pH = 7.0; // Default pH value as float
int turbidity, DO, BOD, COD, TSS, nitrat, fosfat, fecal_coliform;

// Servo objects and pin definitions for water gates
Servo communityGateServo;
Servo treatmentGateServo;
const int communityGatePin = 33; // Pin for community gate servo
const int treatmentGatePin = 32; // Pin for treatment gate servo

// Pin for pH sensor
#define PH_SENSOR_PIN 34 // Sesuaikan dengan pin yang Anda pilih

// Nilai PH pada buffer pH 7.0
const float TeganganPH7 = 2.65; // Tegangan yang Anda ukur pada pH 7.0 (gunakan pH air mineral sebagai acuan)
const float PHStep = 0.17;      // PH step yang sudah dihitung sebelumnya

void setup()
{
  Serial.begin(115200);       // Start serial communication
  WiFi.begin(ssid, password); // Connect to Wi-Fi

  // Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Set up MQTT server
  client.setServer(mqtt_server, mqttPort);

  // Start I2C with default pins (SDA = GPIO21, SCL = GPIO22)
  Wire.begin();

  // Initialize LCD with 16x2 display size
  lcd.begin(16, 2);
  lcd.backlight(); // Turn on LCD backlight

  // Attach servos to pins
  communityGateServo.attach(communityGatePin);
  treatmentGateServo.attach(treatmentGatePin);

  // Set initial positions (closed)
  communityGateServo.write(0); // Closed position
  treatmentGateServo.write(0); // Closed position

  client.setCallback(callback);

  // Menghubungkan ke MQTT
  while (!client.connected())
  {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("Terhubung!");
      // Subscribing ke semua topik
      for (int i = 0; i < numTopics; i++)
      {
        client.subscribe(mqttAcceptTopics[i]);
        Serial.print("Subscribed ke topik: ");
        Serial.println(mqttAcceptTopics[i]);
      }
    }
    else
    {
      Serial.print("Gagal terhubung. Kesalahan: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop()
{
  // Check connection to MQTT server
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // Send sensor data to server every realTimeDataInterval
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > realTimeDataInterval)
  {
    lastMsg = millis();
    generateSensorData();
    String data = createJSON();
    client.publish(mqttSendTopic, data.c_str());
    Serial.println("Real-time data sent:");
    printData();
  }

  // Save sensor data to database every saveDataToDbInterval
  static unsigned long lastSave = 0;
  if (millis() - lastSave > saveDataToDbInterval)
  {
    lastSave = millis();
    saveToDatabase();
  }

  // Update LCD with IPj value every second
  static unsigned long lastLcdUpdate = 0; // Timer for LCD update
  if (millis() - lastLcdUpdate > 1000)    // Update every 1 second
  {
    lastLcdUpdate = millis();
    float IPj = calculateIPj(); // Hitung IPj

    // Tulis baris pertama, tambahkan padding spasi untuk membersihkan sisa karakter
    lcd.setCursor(0, 0);
    lcd.print("IPj: ");
    lcd.print(IPj, 2);
    lcd.print("    "); // Tambahkan spasi untuk menimpa karakter sisa (4 spasi cukup untuk angka lama)

    // Tulis baris kedua, tambahkan padding spasi untuk membersihkan sisa karakter
    lcd.setCursor(0, 1);
    lcd.print(setWaterQuality(IPj));
    lcd.print("      "); // Tambahkan spasi untuk menimpa karakter sisa (6 spasi cukup untuk string lama)

    // Control water gates based on IPj value
    controlWaterGates(IPj);
  }

  client.loop();
}

// Function to generate random sensor data (dummy data)
void generateSensorData()
{
  // Gunakan data dari sensor atau server sesuai status useServer
  if (useServerPH)
    pH = getSensorPH(); // Ambil data dari sensor pH
  else
    pH = serverPH; // Gunakan data dari server pH

  if (useServerDO)
    DO = random(5, 10); // Simulasi data random DO
  else
    DO = serverDO; // Gunakan data dari server DO

  if (useServerBOD)
    BOD = random(2, 6); // Simulasi data random BOD
  else
    BOD = serverBOD; // Gunakan data dari server BOD

  if (useServerCOD)
    COD = random(10, 20); // Simulasi data random COD
  else
    COD = serverCOD; // Gunakan data dari server COD

  if (useServerTSS)
    TSS = random(50, 200); // Simulasi data random TSS
  else
    TSS = serverTSS; // Gunakan data dari server TSS

  if (useServerNitrat)
    nitrat = random(1, 10); // Simulasi data random nitrat
  else
    nitrat = serverNitrat; // Gunakan data dari server nitrat

  if (useServerFosfat)
    fosfat = random(1, 5); // Simulasi data random fosfat
  else
    fosfat = serverFosfat; // Gunakan data dari server fosfat

  if (useServerFecalColiform)
    fecal_coliform = random(50, 500); // Simulasi data random fecal_coliform
  else
    fecal_coliform = serverFecalColiform; // Gunakan data dari server fecal_coliform
}

// Function to create JSON from sensor data
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
  doc["waterQuality"] = setWaterQuality(IPj);

  String json;
  serializeJson(doc, json); // Serialize JSON to string
  return json;
}

// Function to calculate IPj based on parameters
float calculateIPj()
{
  float L_pH = 7.0, L_DO = 6.0, L_BOD = 3.0, L_COD = 10.0, L_TSS = 100.0, L_nitrat = 10.0, L_fosfat = 3.0, L_fecal_coliform = 200.0;

  float ratio_pH = pH / L_pH;
  float ratio_DO = DO / L_DO;
  float ratio_BOD = BOD / L_BOD;
  float ratio_COD = COD / L_COD;
  float ratio_TSS = TSS / L_TSS;
  float ratio_nitrat = nitrat / L_nitrat;
  float ratio_fosfat = fosfat / L_fosfat;
  float ratio_fecal_coliform = fecal_coliform / L_fecal_coliform;

  float maxRatio = max({ratio_pH, ratio_DO, ratio_BOD, ratio_COD, ratio_TSS, ratio_nitrat, ratio_fosfat, ratio_fecal_coliform});
  float avgRatio = (ratio_pH + ratio_DO + ratio_BOD + ratio_COD + ratio_TSS + ratio_nitrat + ratio_fosfat + ratio_fecal_coliform) / 8.0;

  float IPj = pow(maxRatio, 2) + pow(avgRatio, 2);
  return IPj;
}

// Function to set water quality based on IPj
String setWaterQuality(float IPj)
{
  if (IPj >= 0 && IPj <= 1.0)
  {
    return "baik";
  }
  else if (IPj > 1.0 && IPj <= 5.0)
  {
    return "cemar ringan";
  }
  else if (IPj > 5.0 && IPj <= 10.0)
  {
    return "cemar sedang";
  }
  else if (IPj > 10.0)
  {
    return "cemar berat";
  }
}

// Function to control water gates based on IPj
void controlWaterGates(float IPj)
{
  if (IPj <= 5.0)
  {
    communityGateServo.write(90);
    treatmentGateServo.write(90);
    Serial.println("Water quality is good. Opening community gate.");
  }
  else
  {
    communityGateServo.write(0);
    treatmentGateServo.write(0);
    Serial.println("Poor water quality. Redirecting to treatment gate.");
  }
}

// Function to save sensor data to database
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

// Function to reconnect to MQTT server
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

// Function to print sensor data to Serial Monitor
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
  Serial.println("IPj: " + String(calculateIPj(), 2));
  Serial.println();
}

// Function to get pH from sensor (now returning float)
float getSensorPH()
{
  int sensorValue = analogRead(PH_SENSOR_PIN);
  float voltage = sensorValue * (3.3 / 4095.0);        // Convert ADC to voltage
  float pH = 7.0 + ((TeganganPH7 - voltage) / PHStep); // Calculate pH value

  // Ensure pH is within the range 0 to 14
  if (pH < 0)
    pH = 0;
  if (pH > 14)
    pH = 14;

  pH = round(pH * 100) / 100.0; // Round to 2 decimal places
  return pH;                    // Return as float
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // Parsing JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error)
  {
    Serial.print("Gagal mem-parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  bool useServer = doc["useServer"];
  float sensorValue = doc["sensorValue"];

  String sensorName = String(topic).substring(String(topic).lastIndexOf('/') + 1);

  if (sensorName == "pH")
  {
    useServerPH = useServer;
    if (!useServerPH) // Jika useServer false, simpan nilai server
      serverPH = sensorValue;
  }
  else if (sensorName == "DO")
  {
    useServerDO = useServer;
    if (!useServerDO)
      serverDO = sensorValue;
  }
  else if (sensorName == "BOD")
  {
    useServerBOD = useServer;
    if (!useServerBOD)
      serverBOD = sensorValue;
  }
  else if (sensorName == "COD")
  {
    useServerCOD = useServer;
    if (!useServerCOD)
      serverCOD = sensorValue;
  }
  else if (sensorName == "TSS")
  {
    useServerTSS = useServer;
    if (!useServerTSS)
      serverTSS = (int)sensorValue;
  }
  else if (sensorName == "nitrat")
  {
    useServerNitrat = useServer;
    if (!useServerNitrat)
      serverNitrat = (int)sensorValue;
  }
  else if (sensorName == "fosfat")
  {
    useServerFosfat = useServer;
    if (!useServerFosfat)
      serverFosfat = (int)sensorValue;
  }
  else if (sensorName == "fecal_coliform")
  {
    useServerFecalColiform = useServer;
    if (!useServerFecalColiform)
      serverFecalColiform = (int)sensorValue;
  }
}