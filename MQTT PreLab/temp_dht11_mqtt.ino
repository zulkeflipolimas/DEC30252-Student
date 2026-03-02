#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ================= WIFI =================
#define WIFI_SSID "_______________"
#define WIFI_PASSWORD "______________"

// ================= MQTT =================
#define MQTT_HOST "________________"
#define MQTT_PORT 1883
#define PUB_TOPIC "_______________"

// ================= HARDWARE =================
#define DHTPIN 25
#define DHTTYPE DHT11

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHT dht(DHTPIN, DHTTYPE);

// ================= WIFI =================
void connectWiFi() {
  Serial.print("📶 WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print("⏳");
  }
  Serial.println("\n✅ Connected");
}

// ================= MQTT =================
void connectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32-DHT")) {
      Serial.println("✅ MQTT Connected");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  connectWiFi();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  if (!mqttClient.connected()) connectMQTT();
  mqttClient.loop();

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ DHT read failed");
    return;
  }

  char payload[50];
  sprintf(payload, "{\"temp\":%.1f,\"hum\":%.1f}", temp, hum);

  mqttClient.publish(PUB_TOPIC, payload);
  Serial.print("🌡️📤 ");
  Serial.println(payload);

  delay(2000); // DHT11 slow sikit 😉
}
