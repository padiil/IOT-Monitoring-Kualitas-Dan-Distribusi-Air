#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
const int phPin = 34;
const int tempPin = 4;
const int turbidityPin = 35;
const int conductivityPin = 32;
const int tdsPin = 33;
// lcd pin
const int I2C_SDA = 21;
const int I2C_SCL = 22;

// LCD Definitions
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display

// Threshold values
const int tempMin = -55;
const int tempMax = 125;
const int phMax = 14;
const int turbidityMax = 100030;
const int conductivityMax = 100;
const int tdsMax = 1000;

// Variables
float phValue, tempValue, turbidityValue, conductivityValue, tdsValue;

// String Definitions
String tempDef, phDef, turbidityDef, conductivityDef, tdsDef;

void setup()
{
  Serial.begin(115200);

  // Initialize Sensor Pins
  pinMode(tempPin, INPUT);
  pinMode(phPin, INPUT);
  pinMode(turbidityPin, INPUT);
  pinMode(conductivityPin, INPUT);
  pinMode(tdsPin, INPUT);

  // Initialize I2C communication with custom SDA and SCL pins
  Wire.begin(I2C_SDA, I2C_SCL);  // Use GPIO 21 for SDA and GPIO 22 for SCL
  
  // Initialize LCD
  lcd.init();  // initialize the lcd
  lcd.backlight();  // turn on the backlight

  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Vamos!");
  lcd.setCursor(3, 1);
  lcd.print("El Ciponyo");
}

// Function to read sensor values
void readSensors()
{
  tempValue = map(analogRead(tempPin), 0, 4095, tempMin, tempMax);
  phValue = (analogRead(phPin) / 4095.0) * phMax;
  turbidityValue = (analogRead(turbidityPin) / 4095.0) * turbidityMax;
  conductivityValue = (analogRead(conductivityPin) / 4095.0) * conductivityMax;
  tdsValue = (analogRead(tdsPin) / 4095.0) * tdsMax;
}

// Function to define the water conditions
void defineConditions()
{
  // Temperature
  if (tempValue <= 0)
  {
    tempDef = "Beku bjir";
  }
  else if (tempValue > 0 && tempValue <= 20)
  {
    tempDef = "Air dingin";
  }
  else if (tempValue > 20 && tempValue < 60)
  {
    tempDef = "Air hangat";
  }
  else
  {
    tempDef = "Air panas";
  }

  // pH
  if (phValue <= 6)
  {
    phDef = "Asam";
  }
  else if (phValue >= 8)
  {
    phDef = "Basa";
  }
  else
  {
    phDef = "Netral";
  }

  // Turbidity
  if (turbidityValue <= 5)
  {
    turbidityDef = "Jernih";
  }
  else if (turbidityValue > 5 && turbidityValue < 50)
  {
    turbidityDef = "Sedang";
  }
  else
  {
    turbidityDef = "Keruh";
  }

  // Conductivity
  if (conductivityValue <= 50)
  {
    conductivityDef = "Sangat murni";
  }
  else if (conductivityValue > 50 && conductivityValue <= 200)
  {
    conductivityDef = "Murni";
  }
  else if (conductivityValue > 200 && conductivityValue < 500)
  {
    conductivityDef = "Sedang";
  }
  else
  {
    conductivityDef = "Tercemar";
  }

  // TDS
  if (tdsValue <= 50)
  {
    tdsDef = "Sangat baik";
  }
  else if (tdsValue > 50 && tdsValue <= 150)
  {
    tdsDef = "Baik";
  }
  else if (tdsValue > 150 && tdsValue < 300)
  {
    tdsDef = "Cukup";
  }
  else
  {
    tdsDef = "Buruk";
  }
}

// Function to print results to Serial and LCD
void printResults()
{
  // Print results to Serial Monitor
  Serial.println("-----------------");
  Serial.print("Temperature : ");
  Serial.print(tempValue);
  Serial.print(" °C - ");
  Serial.println(tempDef);
  Serial.print("pH          : ");
  Serial.print(phValue);
  Serial.print(" - ");
  Serial.println(phDef);
  Serial.print("Turbidity   : ");
  Serial.print(turbidityValue);
  Serial.print(" - ");
  Serial.println(turbidityDef);
  Serial.print("Conductivity: ");
  Serial.print(conductivityValue);
  Serial.print(" - ");
  Serial.println(conductivityDef);
  Serial.print("Tds         : ");
  Serial.print(tdsValue);
  Serial.print(" - ");
  Serial.println(tdsDef);
  Serial.println("-----------------");
}

void loop()
{
  readSensors();
  defineConditions();
  printResults();
  delay(1000); // Delay for readability
}
