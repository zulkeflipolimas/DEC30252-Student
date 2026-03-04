#include <WiFi.h>              // 📡 Library WiFi ESP32
#include <PubSubClient.h>      // 🌐 Library MQTT

// ================= WIFI CONFIG =================
#define WIFI_SSID "_______________"      // 📶 Nama WiFi
#define WIFI_PASSWORD "_______________"         // 🔐 Password WiFi

// ================= MQTT CONFIG =================
#define MQTT_HOST "_______________"        // 🌍 MQTT Broker
#define MQTT_PORT 1883                   // Port MQTT

#define SUB_TOPIC "_______________" // 📩 Topic SUBSCRIBE

// ================= HARDWARE =================
#define LED1_PIN 18                      // 💡 LED 1 (Pin 18)

WiFiClient espClient;                    
PubSubClient mqttClient(espClient);      

// ================= WIFI FUNCTION =================
void connectToWifi() {
  Serial.println("\n📶 [WIFI] Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("⏳");
  }

  Serial.println("\n✅ [WIFI] Connected!");
  Serial.print("🌍 IP: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT CONNECT =================
void connectToMqtt() {
  while (!mqttClient.connected()) {

    String clientId = "ESP32-LED1-" + String(random(1000, 9999));
    Serial.println("🌐 [MQTT] Connecting...");

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("✅ [MQTT] Connected");

      mqttClient.subscribe(SUB_TOPIC);
      Serial.print("📩 Subscribed: ");
      Serial.println(SUB_TOPIC);

    } else {
      Serial.print("❌ Failed rc=");
      Serial.print(mqttClient.state());
      Serial.println(" → retry 2s");
      delay(2000);
    }
  }
}

// ================= MQTT CALLBACK =================
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.toLowerCase();
  message.trim();

  Serial.println("\n📩 MQTT MESSAGE");
  Serial.print("📝 Message: ");
  Serial.println(message);

  if (message == "on") {
    digitalWrite(LED1_PIN, HIGH);
    Serial.println("💡 LED 1 ON");
  }
  else if (message == "off") {
    digitalWrite(LED1_PIN, LOW);
    Serial.println("💡 LED 1 OFF");
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Serial.println("⚙️ ESP32 MQTT | 1 LED MODE");

  pinMode(LED1_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);

  connectToWifi();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  randomSeed(micros());
}

// ================= LOOP =================
void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }

  if (!mqttClient.connected()) {
    connectToMqtt();
  }

  mqttClient.loop();   // 🔁 WAJIB untuk terima mesej
}
