#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ================= WIFI =================
#define WIFI_SSID "ExtendVoltrabot"
#define WIFI_PASSWORD "r@w@1985"

// ================= MQTT =================
#define MQTT_HOST "broker.emqx.io"
#define MQTT_PORT 1883

#define TEMP_TOPIC "your_own_temp_topic"
#define HUM_TOPIC  "your_own_humidity_topic"
#define SUB_TOPIC  "esp32/led/control"   // 👈 tambah ni

// ================= HARDWARE =================
#define DHTPIN 25
#define DHTTYPE DHT11
#define LED_PIN 2   // 👈 LED onboard

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

      mqttClient.subscribe(SUB_TOPIC);   // 👈 subscribe
      Serial.println("📩 Subscribed LED control");

    } else {
      Serial.println("❌ MQTT retry...");
      delay(2000);
    }
  }
}

// ================= CALLBACK =================
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.toLowerCase();
  message.trim();

  Serial.println("\n📩 MQTT MESSAGE: " + message);

  if (message == "onled1") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("💡 LED ON");
  }
  else if (message == "offled1") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("💡 LED OFF");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();

  connectWiFi();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);  // 👈 penting
}

// ================= LOOP =================
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

  String tempStr = String(temp, 1);
  String humStr  = String(hum, 1);

  mqttClient.publish(TEMP_TOPIC, tempStr.c_str());
  mqttClient.publish(HUM_TOPIC, humStr.c_str());

  Serial.println("📤 Publish data:");
  Serial.println("🌡️ Temp: " + tempStr);
  Serial.println("💧 Hum : " + humStr);

  delay(2000);
}