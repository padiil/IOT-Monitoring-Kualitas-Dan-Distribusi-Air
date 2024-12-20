#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h> // Include the ESP32 Servo library

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
void controlWaterGates(float IPj); // New function for controlling water gates

const char *ssid = "Emmm2";
const char *password = "emmmtriplem";
const char *mqtt_server = "192.168.100.187";
const char *saveToDbUrl = "http://192.168.100.187:3000/sensor-data";
const char *topic = "sensor/data";
const int realTimeDataInterval = 1000;  // 1 second
const int saveDataToDbInterval = 30000; // 30 seconds

WiFiClient espClient;
PubSubClient client(espClient);

// Parameters for sensor data
int pH, turbidity, DO, BOD, COD, TSS, nitrat, fosfat, fecal_coliform;

// Servo objects and pin definitions for water gates
Servo communityGateServo;
Servo treatmentGateServo;
const int communityGatePin = 33;  // Pin for community gate servo
const int treatmentGatePin = 32;  // Pin for treatment gate servo

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
  client.setServer(mqtt_server, 1883);

  // Start I2C with default pins (SDA = GPIO21, SCL = GPIO22)
  Wire.begin();

  // Initialize LCD with 16x2 display size
  lcd.begin(16, 2);
  lcd.backlight(); // Turn on LCD backlight

  // Attach servos to pins
  communityGateServo.attach(communityGatePin);
  treatmentGateServo.attach(treatmentGatePin);

  // Set initial positions (closed)
  communityGateServo.write(0);  // Closed position
  treatmentGateServo.write(0);  // Closed position
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
    client.publish(topic, data.c_str());
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
    float IPj = calculateIPj(); // Calculate IPj
    lcd.setCursor(0, 0);
    lcd.print("IPj: ");
    lcd.print(IPj, 2); // Print IPj with 2 decimal places
    lcd.setCursor(0, 1);
    lcd.print(setWaterQuality(IPj));

    // Control water gates based on IPj value
    controlWaterGates(IPj);
  }
}

// Function to generate random sensor data (dummy data)
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
  if (IPj <= 4.0)
  {
    communityGateServo.write(90); // Open community gate
    treatmentGateServo.write(0);  // Close treatment gate
    Serial.println("Water quality is good. Opening community gate.");
  }
  else
  {
    communityGateServo.write(0);  // Close community gate
    treatmentGateServo.write(90); // Open treatment gate
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
