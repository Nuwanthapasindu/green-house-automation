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
#define HEATER_PIN 25 // G4 PIN
#define COOLER_PIN 26 // G5 PIN
#define DAY_LIGHT_PIN 17 // G17 PIN
#define WATER_PUMP_PIN 16 // G16 PIN


// VARIABLE DEFFINE
int smValue = 0;
int lightLevel = 0;
float temperature = 0;
float humidity = 0;
float smPercentage=0;
float ldrPercentage =0;

bool dayLightOn = false;
bool waterPumpOn = false;
bool heaterOn = false;
bool coolerOn = false;

int TEMPERATURE_THRESHOLD= 35;
const int LDR_LIMIT = 36;
const int SM_LIMIT = 50;

DHT dhtSensor(DHT_PIN,DHT11);
BlynkTimer timer;


int DATA_INTERVAL = 3000L;
int WiFi_INTERVAL = 60000L;

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connection Failed!");

    } else {
      Serial.println("WiFi Connected!");
      Serial.print("Current IP: ");
      Serial.println(WiFi.localIP());

    }
  }
}

void WiFiStatus() {
  if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi Status: Not Connected! \n");

    } else {
      Serial.println("WiFi Status: Connected! \n");
    }
}

void sensorRead() {
    smValue = analogRead(SM_PIN);
  smValue = analogRead(SM_PIN);
  lightLevel = analogRead(LDR_PIN);
  temperature = dhtSensor.readTemperature();
  humidity = dhtSensor.readHumidity();
  smPercentage = ((4095 - smValue) / 4095.0) * 100.0;
  ldrPercentage = ((4095 - lightLevel) / 4095.0) * 100.0;

//  HANDLE LIGHT WITH LDR SENSOR VALUE
  if(ldrPercentage < LDR_LIMIT){
    dayLightOn=true;
    Blynk.virtualWrite(V7, 255);

  }else{
    dayLightOn=false;
    Blynk.virtualWrite(V7, 0);

  }

//  HANDLE WATER PUMP WITH SOIL MOISTURE SENSOR VALUE
  if(smPercentage < SM_LIMIT){
    waterPumpOn=true;
    Blynk.virtualWrite(V8, 255);

  }else{
    waterPumpOn=false;
    Blynk.virtualWrite(V8, 0);

  }
//  HANDLE COOLER FAN AND HEATER FAN WITH DHT 11 SENSOR VALUE
  // eaterOn = false;
  // coolerOn = false;
  if(temperature < TEMPERATURE_THRESHOLD){
    heaterOn = true;
    coolerOn = false;
    Blynk.virtualWrite(V6, 255); 
    Blynk.virtualWrite(V5, 0); 

  }else{
    heaterOn = false;
    coolerOn = true;
    Blynk.virtualWrite(V6, 0); 
    Blynk.virtualWrite(V5, 255); 

  }


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
}


void handleActuators(){
   // DAY LIGHT TURN ON AND TURN OFF
  if(dayLightOn){
    digitalWrite(DAY_LIGHT_PIN,LOW);
  }else{
    digitalWrite(DAY_LIGHT_PIN,HIGH);

  }

   // WATER PUMP TURN ON AND TURN OFF
  if(waterPumpOn){
    digitalWrite(WATER_PUMP_PIN,LOW);
  }else{
    digitalWrite(WATER_PUMP_PIN,HIGH);

  }

   // HEATER TURN ON AND TURN OFF
  if(heaterOn){
    digitalWrite(HEATER_PIN,LOW);
  }else{
    digitalWrite(HEATER_PIN,HIGH);

  }

   // COOLER TURN ON AND TURN OFF
  if(coolerOn){
    digitalWrite(COOLER_PIN,LOW);
  }else{
    digitalWrite(COOLER_PIN,HIGH);

  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V4);
}

BLYNK_WRITE(V4) {
  TEMPERATURE_THRESHOLD = param.asInt();
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD, "blynk.cloud", 80);
  dhtSensor.begin();
  Blynk.virtualWrite(V4,TEMPERATURE_THRESHOLD);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);
  pinMode(DAY_LIGHT_PIN, OUTPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  timer.setInterval(3000, sensorRead);
}

void loop() {
  Blynk.run();
  timer.run();
  handleActuators();
}
