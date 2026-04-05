#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ================= WIFI =================
#define WIFI_SSID "_______________"
#define WIFI_PASSWORD "_____________"

// ================= MQTT =================
#define MQTT_HOST "_____________" //server broker address ex: broker.emqx.io
#define MQTT_PORT 1883

#define TEMP_TOPIC "______________"   // 🌡️ suhu
#define HUM_TOPIC  "______________"    // 💧 humidity

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
    delay(500);
    Serial.print("⏳");
  }

  Serial.println("\n✅ WiFi Connected");
}

// ================= MQTT =================
void connectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32-DHT")) {
      Serial.println("✅ MQTT Connected");
    } else {
      Serial.println("❌ MQTT retry...");
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

  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqttClient.connected()) connectMQTT();

  mqttClient.loop();

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ DHT read failed");
    return;
  }

  // 🔄 Convert ke string
  String tempStr = String(temp, 1);
  String humStr  = String(hum, 1);

  // 📤 Publish ke 2 topic
  mqttClient.publish(TEMP_TOPIC, tempStr.c_str());
  mqttClient.publish(HUM_TOPIC, humStr.c_str());

  Serial.println("📤 Publish data:");
  Serial.println("🌡️ Temp: " + tempStr);
  Serial.println("💧 Hum : " + humStr);

  delay(2000); // DHT11 slow
}
