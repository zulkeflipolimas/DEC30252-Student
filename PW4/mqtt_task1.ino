#include <WiFi.h>          // Library WiFi untuk ESP32
#include <PubSubClient.h>  // Library MQTT

// ================= WIFI CONFIG =================
// Nama dan password WiFi
#define WIFI_SSID "____________"  
#define WIFI_PASSWORD "____________"

// ================= MQTT CONFIG =================
// Alamat dan port MQTT broker
#define MQTT_HOST "____________"
#define MQTT_PORT 1883

// Topic MQTT
#define SUB_TOPIC "____________"  // Topic untuk TERIMA arahan
#define PUB_TOPIC "____________"   // Topic untuk HANTAR status LED

// ================= HARDWARE =================
#define LEDPIN 18  // Pin LED onboard ESP32

WiFiClient espClient;                // Objek WiFi
PubSubClient mqttClient(espClient);  // Objek MQTT guna WiFi client

// ================= WIFI FUNCTION =================
// Fungsi untuk sambung ESP32 ke WiFi
void connectToWifi() {
  Serial.println("\n📶 [WIFI] Connecting...");

  WiFi.mode(WIFI_STA);                   // ESP32 sebagai WiFi client
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Mula sambung WiFi

  // Tunggu sehingga WiFi berjaya disambungkan
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("⏳");
  }

  // Papar status bila WiFi berjaya connect
  Serial.println("\n✅ [WIFI] Connected");
  Serial.println(WiFi.localIP());  // Papar IP address ESP32
}

// ================= MQTT CONNECT FUNCTION =================
// Fungsi untuk sambung ESP32 ke MQTT broker
void connectToMqtt() {
  while (!mqttClient.connected()) {  // Ulang selagi belum connected

    // Client ID unik untuk elak clash
    String clientId = "ESP32-PUBSUB-" + String(random(1000, 9999));
    Serial.print("🌐 [MQTT] Connecting as ");
    Serial.println(clientId);

    // Cuba sambung ke MQTT broker
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("✅ [MQTT] Connected");

      // Subscribe topic kawalan LED
      mqttClient.subscribe(SUB_TOPIC);
      Serial.println("📩 [SUB] esp32/led/control");

    } else {
      // Jika gagal sambung
      Serial.print("❌ [MQTT] Failed, rc=");
      Serial.println(mqttClient.state());
      delay(2000);  // Tunggu sebelum cuba semula
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

  message.toLowerCase();  // Tukar huruf ke lowercase
  message.trim();         // Buang space / newline

  // Papar mesej yang diterima di Serial Monitor
  Serial.println("\n📩 [MQTT RECEIVED]");
  Serial.print("📌 Topic : ");
  Serial.println(topic);
  Serial.print("📝 Data  : ");
  Serial.println(message);

  // Pastikan mesej datang dari topic yang betul
  if (String(topic) == SUB_TOPIC) {

    // Jika mesej "on"
    if (message == "on") {
      digitalWrite(LEDPIN, HIGH);  // Hidupkan LED
      Serial.println("💡 [LED] ON");

      // Publish status LED ke broker
      mqttClient.publish(PUB_TOPIC, "on");
      Serial.println("📤 [PUB] LED is ON");
    }

    // Jika mesej "off"
    else if (message == "off") {
      digitalWrite(LEDPIN, LOW);  // Matikan LED
      Serial.println("💡 [LED] OFF");

      // Publish status LED ke broker
      mqttClient.publish(PUB_TOPIC, "off");
      Serial.println("📤 [PUB] LED is OFF");
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);  // Mulakan Serial Monitor
  Serial.println("⚙️ [SYSTEM] MQTT SUBSCRIBE + PUBLISH");

  pinMode(LEDPIN, OUTPUT);    // Set pin LED sebagai OUTPUT
  digitalWrite(LEDPIN, LOW);  // LED mula dalam keadaan OFF

  connectToWifi();  // Sambung ke WiFi

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);  // Set broker dan port
  mqttClient.setCallback(mqttCallback);        // Set fungsi callback

  randomSeed(micros());  // Seed random untuk client ID
}

// ================= LOOP =================
void loop() {

  // Jika WiFi terputus, sambung semula
  if (WiFi.status() != WL_CONNECTED)
    connectToWifi();

  // Jika MQTT terputus, sambung semula
  if (!mqttClient.connected())
    connectToMqtt();

  mqttClient.loop();  // WAJIB untuk terima & hantar mesej MQTT
}
