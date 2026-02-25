#include <WiFi.h>              // Library WiFi ESP32
#include <PubSubClient.h>      // Library MQTT

// ================= WIFI CONFIG =================
#define WIFI_SSID "______________"      // Nama WiFi
#define WIFI_PASSWORD "______________"         // Password WiFi

// ================= MQTT CONFIG =================
#define MQTT_HOST "______________"        // MQTT Broker
#define MQTT_PORT 1883                   // Port MQTT (tanpa TLS)

#define SUB_TOPIC "______________"    // Topic untuk TERIMA mesej

// ================= HARDWARE =================
#define LEDPIN 18                         // LED 1 on pin 

WiFiClient espClient;                    // Objek WiFi
PubSubClient mqttClient(espClient);      // Objek MQTT

// ================= WIFI FUNCTION =================
void connectToWifi() {
  Serial.println("\n📶 [WIFI] Connecting to WiFi...");
  WiFi.mode(WIFI_STA);                   // ESP32 sebagai client WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Sambung ke WiFi

  // Tunggu sehingga WiFi berjaya connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("⏳");
  }

  Serial.println("\n✅ [WIFI] Connected!");
  Serial.print("🌍 [WIFI] IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT CONNECT FUNCTION =================
void connectToMqtt() {

  // Cuba sambung selagi MQTT belum connected
  while (!mqttClient.connected()) {

    String clientId = "ESP32-SUB-" + String(random(1000, 9999));
    Serial.print("🌐 [MQTT] Connecting as ");
    Serial.println(clientId);

    if (mqttClient.connect(clientId.c_str())) {

      Serial.println("✅ [MQTT] Connected");

      // SUBSCRIBE topic (langkah PALING PENTING)
      mqttClient.subscribe(SUB_TOPIC);
      Serial.print("📩 [SUB] Listening to topic: ");
      Serial.println(SUB_TOPIC);

    } else {
      Serial.print("❌ [MQTT] Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" → retry in 2 seconds");
      delay(2000);
    }
  }
}

// ================= MQTT CALLBACK =================
// Function ini dipanggil AUTOMATIK bila mesej diterima
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  // Tukar payload (byte) kepada String
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.toLowerCase();   // Tukar ke huruf kecil
  message.trim();          // Buang space / newline

  // Papar mesej yang diterima
  Serial.println("\n📩 [MQTT MESSAGE RECEIVED]");
  Serial.print("📌 Topic  : ");
  Serial.println(topic);
  Serial.print("📝 Message: ");
  Serial.println(message);

  // Contoh logic mudah (visual sahaja)
  if (message == "on") {
    digitalWrite(LEDPIN, HIGH);
    Serial.println("💡 [LED] ON (command received)");
  }
  else if (message == "off") {
    digitalWrite(LEDPIN, LOW);
    Serial.println("💡 [LED] OFF (command received)");
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Serial.println("⚙️ [SYSTEM] ESP32 MQTT SUBSCRIBE MODE");

  pinMode(LEDPIN, OUTPUT);       // Set LED sebagai output
  digitalWrite(LEDPIN, LOW);     // LED mula OFF

  connectToWifi();               // Sambung WiFi

  mqttClient.setServer(MQTT_HOST, MQTT_PORT); // Set broker
  mqttClient.setCallback(mqttCallback);       // Set callback

  randomSeed(micros());          // Untuk client ID rawak
}

// ================= LOOP =================
void loop() {

  // Jika WiFi terputus → sambung semula
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ [WIFI] Disconnected. Reconnecting...");
    connectToWifi();
  }

  // Jika MQTT terputus → sambung semula
  if (!mqttClient.connected()) {
    Serial.println("❌ [MQTT] Disconnected. Reconnecting...");
    connectToMqtt();
  }

  mqttClient.loop();   // WAJIB untuk terima mesej MQTT
}
