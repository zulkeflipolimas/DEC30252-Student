#include <WiFi.h>          // 📡 Library WiFi untuk ESP32
#include <PubSubClient.h>  // 🌐 Library MQTT

// ================= WIFI CONFIG =================
#define WIFI_SSID "ExtendVoltrabot"      // Nama WiFi
#define WIFI_PASSWORD "r@w@1985"         // Password WiFi

// ================= MQTT CONFIG =================
#define MQTT_HOST "broker.emqx.io"        // MQTT Broker
#define MQTT_PORT 1883                   // Port MQTT (tanpa TLS)

// Topic MQTT
#define SUB_TOPIC "esp32/led/control"    // Topic untuk TERIMA me
#define PUB_TOPIC "esp32/led/status"    // 📤 Topic untuk HANTAR status LED

// ================= HARDWARE =================
#define LED1_PIN 2   // 💡 LED 1 (onboard ESP32)
#define LED2_PIN 4   // 💡 LED 2 (pin 4)

WiFiClient espClient;                // Objek WiFi
PubSubClient mqttClient(espClient);  // Objek MQTT guna WiFi client

// ================= WIFI FUNCTION =================
// Fungsi untuk sambung ESP32 ke WiFi
void connectToWifi() {
  Serial.println("\n📶 [WIFI] Connecting...");

  WiFi.mode(WIFI_STA);                   
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  

  // Tunggu sehingga WiFi berjaya disambungkan
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("⏳");
  }

  // Papar status bila WiFi berjaya connect
  Serial.println("\n✅ [WIFI] Connected");
  Serial.println(WiFi.localIP());  
}

// ================= MQTT CONNECT FUNCTION =================
// Fungsi untuk sambung ESP32 ke MQTT broker
void connectToMqtt() {
  while (!mqttClient.connected()) {

    // Client ID unik untuk elak clash
    String clientId = "ESP32-PUBSUB-" + String(random(1000, 9999));
    Serial.print("🌐 [MQTT] Connecting as ");
    Serial.println(clientId);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("✅ [MQTT] Connected");

      // Subscribe topic kawalan LED
      mqttClient.subscribe(SUB_TOPIC);
      Serial.println("📩 [SUB] Subscribed control topic");

    } else {
      Serial.print("❌ [MQTT] Failed, rc=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

// ================= MQTT CALLBACK =================
// Fungsi ini dipanggil AUTOMATIK bila mesej MQTT diterima
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  // Tukar payload (byte) kepada String
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.toLowerCase();  
  message.trim();         

  // Papar mesej yang diterima
  Serial.println("\n📩 [MQTT RECEIVED]");
  Serial.print("📌 Topic : ");
  Serial.println(topic);
  Serial.print("📝 Data  : ");
  Serial.println(message);

  // Pastikan mesej datang dari topic yang betul
  if (String(topic) == SUB_TOPIC) {

    // ===== LED 1 CONTROL =====
    if (message == "onled1") {
      digitalWrite(LED1_PIN, HIGH);
      Serial.println("💡 [LED 1] ON");

      mqttClient.publish(PUB_TOPIC, "onled1");
      Serial.println("📤 [PUB] LED 1 ON");
    }
    else if (message == "offled1") {
      digitalWrite(LED1_PIN, LOW);
      Serial.println("💡 [LED 1] OFF");

      mqttClient.publish(PUB_TOPIC, "offled1");
      Serial.println("📤 [PUB] LED 1 OFF");
    }

    // ===== LED 2 CONTROL =====
    else if (message == "onled2") {
      digitalWrite(LED2_PIN, HIGH);
      Serial.println("💡 [LED 2] ON");

      mqttClient.publish(PUB_TOPIC, "onled2");
      Serial.println("📤 [PUB] LED 2 ON");
    }
    else if (message == "offled2") {
      digitalWrite(LED2_PIN, LOW);
      Serial.println("💡 [LED 2] OFF");

      mqttClient.publish(PUB_TOPIC, "offled2");
      Serial.println("📤 [PUB] LED 2 OFF");
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Serial.println("⚙️ [SYSTEM] MQTT SUBSCRIBE + PUBLISH (2 LED)");

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  connectToWifi();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  randomSeed(micros());
}

// ================= LOOP =================
void loop() {

  if (WiFi.status() != WL_CONNECTED)
    connectToWifi();

  if (!mqttClient.connected())
    connectToMqtt();

  mqttClient.loop();  // 🔁 WAJIB untuk MQTT berjalan
}
