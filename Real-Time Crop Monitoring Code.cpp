#define BLYNK_TEMPLATE_ID "TMPL3AXVxaOdt"
#define BLYNK_TEMPLATE_NAME "crop"
#define BLYNK_AUTH_TOKEN "9J8eAoa_WR7X5s6-iTz5xLM1JTjBEUFh"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include "ThingSpeak.h"

// WiFi credentials
char ssid[] = "Hemanth";
char pass[] = "123456789";

// ThingSpeak credentials
WiFiClient client;
unsigned long myChannelNumber = 2943015;
const char *myWriteAPIKey = "CVBV9R83IUX9I14R";

// Sensor pins
#define DHTPIN 4
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN 34
#define LIGHT_SENSOR_PIN 35
#define RELAY_PIN 23  // Active-LOW relay

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soilRaw = analogRead(SOIL_MOISTURE_PIN);
  int lightRaw = analogRead(LIGHT_SENSOR_PIN);

  int soilMoisturePercent = map(soilRaw, 4095, 0, 0, 100);
  int lightPercent = map(lightRaw, 0, 4095, 0, 100);

  Serial.println("=== Sensor Readings ===");
  Serial.print("Temperature (°C): "); Serial.println(temperature);
  Serial.print("Humidity (%): "); Serial.println(humidity);
  Serial.print("Soil Moisture (%): "); Serial.println(soilMoisturePercent);
  Serial.print("Light Intensity (%): "); Serial.println(lightPercent);

  // Blynk virtual pins
  Blynk.virtualWrite(V0, soilMoisturePercent);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, lightPercent);

  // ThingSpeak update
  ThingSpeak.setField(1, soilMoisturePercent);
  ThingSpeak.setField(2, temperature);
  ThingSpeak.setField(3, humidity);
  ThingSpeak.setField(4, lightPercent);

  int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (status == 200) {
    Serial.println("ThingSpeak update successful.");
  } else {
    Serial.print("ThingSpeak update failed. HTTP error code: ");
    Serial.println(status);
  }

  // Relay control based on soil moisture
  if (soilMoisturePercent <= 30) {
    digitalWrite(RELAY_PIN, LOW); // ON (Active LOW)
    Serial.println("Relay ON - Soil is dry");
  } else {
    digitalWrite(RELAY_PIN, HIGH); // OFF
    Serial.println("Relay OFF - Soil is moist");
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // OFF initially (Active LOW relay)

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  ThingSpeak.begin(client);

  timer.setInterval(3000L, sendSensorData);  // 3 seconds
}

void loop() {
  Blynk.run();
  timer.run();
}
which code is this is c file or c++ file for this do we have to install any language 