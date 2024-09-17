#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL69r-yXF-O"
#define BLYNK_TEMPLATE_NAME "Group P11 1"
#define BLYNK_AUTH_TOKEN "CJTZHTAItg-x9yoAH99yg45YIEHhuauW"

#define WIFI_SSID "Im Not A Witch Im Your Wifi ;)"
#define WIFI_PASSWORD "Walura@458699%"
#define WIFI_TIMEOUT_MS 20000
#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include "WiFi.h"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

#define SM_PIN 4 // D4 PIN
#define LDR_PIN 2 // D2 PIN
#define DHT_PIN 5 // D5 PIN

DHT dhtSensor(DHT_PIN,DHT11);
LiquidCrystal_I2C lcd(0x01, 16, 2); // SDA -TXD SCL- D22

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print(" Failed!");
      // take action
    } else {
      Serial.println("Connected!");
      Serial.print("Current IP: ");
      Serial.println(WiFi.localIP());
    }
  }
}

void setup() {
  Serial.begin(115200);
  // connectToWiFi();
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD, "blynk.cloud", 80);
  dhtSensor.begin();
  
  // Start I2C communication
  lcd.init();
  lcd.noBacklight();

}

void loop() {
  int smValue = analogRead(SM_PIN);
  int lightLevel = analogRead(LDR_PIN);
  float temperature = dhtSensor.readTemperature();
  float humidity = dhtSensor.readHumidity();
  
  
  float smPercentage = ((4095 - smValue) / 4095.0) * 100.0;
  float ldrPercentage = ((1900 - lightLevel) / 1900.0) * 100.0;
  
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, ldrPercentage);
  Blynk.virtualWrite(V3, smPercentage);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);

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

  delay(2000);

}
