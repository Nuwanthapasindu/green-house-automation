#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6srUtl_JO"
#define BLYNK_TEMPLATE_NAME "Group P1 12"
#define BLYNK_AUTH_TOKEN "kVhLX2rOgfTpoPP-jlXsJ5FWZT323EOS"

#define WIFI_SSID "NuwanthaWifi"
#define WIFI_PASSWORD "Nuwantha@2004"
#define WIFI_TIMEOUT_MS 20000

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
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
bool heaterOn = true;
bool coolerOn = true;

const int TEMPERATURE_THRESHOLD= 35;
const int LDR_LIMIT = 36;
const int SM_LIMIT = 50;

DHT dhtSensor(DHT_PIN,DHT11);
BlynkTimer timer;

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


  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, ldrPercentage);
  Blynk.virtualWrite(V3, smPercentage);

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


void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD, "blynk.cloud", 80);
  dhtSensor.begin();
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);
  pinMode(DAY_LIGHT_PIN, OUTPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  timer.setInterval(3000, sensorRead);


}

void loop() {
  Blynk.run();
  timer.run();
   // DAY LIGHT TURN ON AND TURN OFF
  if(dayLightOn){
    digitalWrite(DAY_LIGHT_PIN,LOW);
  }else{
    digitalWrite(DAY_LIGHT_PIN,HIGH);

  }

   // WATER PUMP TURN ON AND TURN OFF
  if(waterPumpOn){
    digitalWrite(DAY_LIGHT_PIN,LOW);
  }else{
    digitalWrite(DAY_LIGHT_PIN,HIGH);

  }
}