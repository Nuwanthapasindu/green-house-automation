#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6srUtl_JO"
#define BLYNK_TEMPLATE_NAME "Group P1 12"
#define BLYNK_AUTH_TOKEN "kVhLX2rOgfTpoPP-jlXsJ5FWZT323EOS"

#define WIFI_SSID "Bisheka's hotspot"
#define WIFI_PASSWORD "Bishweer"
#define WIFI_TIMEOUT_MS 20000

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include "WiFi.h"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// SENSORS PIN DEFINE
#define DHT_PIN 27 // G27 PIN
#define LDR_PIN 33 // G33 PIN
#define SM_PIN 32 // G32 PIN

// ACTUATORS PIN DEFINE
#define HEATER_PIN 26 // G5 PIN
#define COOLER_PIN 25 // G4 PIN
#define DAY_LIGHT_PIN 17 // G17 PIN
#define WATER_PUMP_PIN 16 // G16 PIN

// VARIABLE DEFFINE
int smValue = 0;
int lightLevel = 0;
float temperature = 0;
float humidity = 0;
float smPercentage = 0;
float ldrPercentage = 0;

bool dayLightOn = false;
bool waterPumpOn = false;
bool heaterOn = false;
bool coolerOn = false;

int TEMPERATURE_THRESHOLD = 30;
const int LDR_LIMIT = 36;
const int SM_LIMIT = 50;

DHT dhtSensor(DHT_PIN, DHT11);
BlynkTimer timer;

int DATA_INTERVAL = 3000L;
int WiFi_INTERVAL = 60000L;

// WiFi connection function
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected!");
    Serial.print("Current IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi.");
  }
}

// Check WiFi status
void WiFiStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Status: Not Connected!");
    connectToWiFi();
  } else {
    Serial.println("WiFi Status: Connected.");
  }
}

// Read sensors and update Blynk
void sensorRead() {
  smValue = analogRead(SM_PIN);
  lightLevel = analogRead(LDR_PIN);
  temperature = dhtSensor.readTemperature();
  humidity = dhtSensor.readHumidity();

  // Convert analog values to percentage
  smPercentage = ((4095 - smValue) / 4095.0) * 100.0;
  ldrPercentage = ((4095 - lightLevel) / 4095.0) * 100.0;

  // Light control using LDR value
  dayLightOn = (ldrPercentage < LDR_LIMIT);
  Blynk.virtualWrite(V7, dayLightOn ? 255 : 0);

  // Water pump control using soil moisture
  waterPumpOn = (smPercentage < SM_LIMIT);
  Blynk.virtualWrite(V8, waterPumpOn ? 255 : 0);

  // Heater and cooler control based on temperature
  if (temperature < TEMPERATURE_THRESHOLD) {
    // Temperature is below the threshold, turn on the heater and turn off the cooler
    heaterOn = true;
    coolerOn = false;
    Serial.println("Heater ON, Cooler OFF");
    Blynk.virtualWrite(V6, 255); // Indicate heater is ON in the app
    Blynk.virtualWrite(V5, 0);   // Indicate cooler is OFF in the app
  } else {
    // Temperature is above the threshold, turn on the cooler and turn off the heater
    heaterOn = false;
    coolerOn = true;
    Serial.println("Heater OFF, Cooler ON");
    Blynk.virtualWrite(V6, 0);   // Indicate heater is OFF in the app
    Blynk.virtualWrite(V5, 255); // Indicate cooler is ON in the app
  }


  // Write temperature and humidity to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, ldrPercentage);
  Blynk.virtualWrite(V3, smPercentage);
 Serial.println(TEMPERATURE_THRESHOLD);
  Serial.print("Soil moisture value: ");
  Serial.print(smPercentage);
  Serial.println("%");
  Serial.print("LDR percentage: ");
  Serial.print(ldrPercentage);
  Serial.println("%");
  Serial.print("Temperature value: ");
  Serial.print(temperature);
  Serial.print("°C");
  Serial.print(" \t Humidity value: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.println("Sensor readings updated");
}

// Control actuators
void handleActuators() {
  // Control daylight
  digitalWrite(DAY_LIGHT_PIN, dayLightOn ? LOW : HIGH);
  
  // Control water pump
  digitalWrite(WATER_PUMP_PIN, waterPumpOn ? LOW : HIGH);

  // Control heater and cooler
  digitalWrite(HEATER_PIN, heaterOn ? LOW : HIGH);
  digitalWrite(COOLER_PIN, coolerOn ? LOW : HIGH);
}

// Sync temperature threshold from Blynk
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V4);
}

BLYNK_WRITE(V4) {
  TEMPERATURE_THRESHOLD = param.asInt();
}

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

  dhtSensor.begin();
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);
  pinMode(DAY_LIGHT_PIN, OUTPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);

  // Sync initial temperature threshold
  Blynk.virtualWrite(V4, TEMPERATURE_THRESHOLD);

  // Set up a timer to read sensors every 3 seconds
  timer.setInterval(3000, sensorRead);
}

void loop() {
  Blynk.run();
  timer.run();
  sensorRead();
  handleActuators();  // Control actuators based on sensor data
}