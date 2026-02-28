#include <WiFi.h>
#include <PubSubClient.h>

// ================= WIFI =================
#define WIFI_SSID "______________"
#define WIFI_PASSWORD "______________"

// ================= MQTT =================
#define MQTT_HOST "______________"
#define MQTT_PORT 1883
#define PUB_TOPIC "______________"

// ================= HARDWARE =================
#define POT_PIN 34   // 🎚️ ADC pin

WiFiClient espClient;
PubSubClient mqttClient(espClient);

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
    if (mqttClient.connect("ESP32-POT")) {
      Serial.println("✅ MQTT Connected");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  if (!mqttClient.connected()) connectMQTT();
  mqttClient.loop();

  int potValue = analogRead(POT_PIN);
  char msg[10];
  sprintf(msg, "%d", potValue);

  mqttClient.publish(PUB_TOPIC, msg);
  Serial.print("🎚️ POT Value: ");
  Serial.println(msg);

  delay(1000); // publish setiap 1 saat
}
