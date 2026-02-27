#include <WiFi.h>          // 📡 WiFi ESP32
#include <PubSubClient.h>  // 🌐 MQTT

// ================= WIFI =================
#define WIFI_SSID "________"
#define WIFI_PASSWORD "________"

// ================= MQTT =================
#define MQTT_HOST "_____________" //server broker adresss (no need 1883)
#define MQTT_PORT 1883
#define PUB_TOPIC "_________" // same with your node red setting MQTT IN Node

// ================= HARDWARE =================
#define BUTTON_PIN 27      // 🔘 Push Button

WiFiClient espClient;
PubSubClient mqttClient(espClient);

int lastState = LOW;

// ================= WIFI CONNECT =================
void connectWiFi() {
  Serial.print("📶 Connecting WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print("⏳");
  }
  Serial.println("\n✅ WiFi Connected");
}

// ================= MQTT CONNECT =================
void connectMQTT() {
  while (!mqttClient.connected()) {
    String cid = "ESP32-BUTTON-" + String(random(1000));
    Serial.println("🌐 Connecting MQTT...");
    if (mqttClient.connect(cid.c_str())) {
      Serial.println("✅ MQTT Connected");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

  connectWiFi();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  if (!mqttClient.connected()) connectMQTT();
  mqttClient.loop();

  int currentState = digitalRead(BUTTON_PIN);

  if (currentState != lastState) {
    if (currentState == HIGH) {
      mqttClient.publish(PUB_TOPIC, "pressed");
      Serial.println("🔘 Button PRESSED → MQTT");
    } else {
      mqttClient.publish(PUB_TOPIC, "released");
      Serial.println("🔘 Button RELEASED → MQTT");
    }
    lastState = currentState;
    delay(200); // debounce sikit 😉
  }
}
