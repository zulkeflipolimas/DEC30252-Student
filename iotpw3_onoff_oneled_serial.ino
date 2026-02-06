#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "ExtendVoltrabot";
const char* password = "r@w@1985";

WebServer server(80);

const int ledPin = 26;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  updateLEDs();

  Serial.print("🔌 Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
  Serial.print("📶 IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleGetStatus);
  server.on("/led", HTTP_POST, handlePostLED);

  server.begin();
  Serial.println("🚀 HTTP server started");
}

void loop() {
  server.handleClient();
}

// Jangan ubah function ni
void updateLEDs() {
  digitalWrite(ledPin, ledState ? HIGH : LOW);
}

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 LED Control</title>
  <style>
    body { font-family: Arial; text-align: center; padding: 20px; }
    button {
      width: 140px; height: 40px; margin: 6px;
      font-size: 16px; border-radius: 8px;
      border: none; cursor: pointer;
    }
    .on { background: #4CAF50; color: white; }
    .off { background: #f44336; color: white; }
    .refresh { background: #2196F3; color: white; }
  </style>
</head>
<body>
  <h1>ESP32 LED Control</h1>

  <button class="on" onclick="setLED('on')">ON</button>
  <button class="off" onclick="setLED('off')">OFF</button>

  <h3 id="status">Status: OFF</h3>

  <button class="refresh" onclick="loadStatus()">Update Status</button>

  <script>
    function setLED(state) {
      fetch('/led', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: 'state=' + state
      }).then(() => {});
    }

    function loadStatus() {
      fetch('/status')
        .then(res => res.json())
        .then(data => {
          document.getElementById('status').innerText =
            'Status: ' + (data.led ? 'ON' : 'OFF');
        });
    }

    window.onload = loadStatus;
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  Serial.println("📡 HTTP GET / - Webpage requested");
  server.send_P(200, "text/html", htmlPage);
}

void handleGetStatus() {
  Serial.print("📡 HTTP GET /status - LED status requested, ");
  Serial.println(ledState ? "ON" : "OFF");

  String json = "{";
  json += "\"led\": " + String(ledState ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void handlePostLED() {
  Serial.println("✉️ HTTP POST /led - LED control requested");

  if (!server.hasArg("state")) {
    Serial.println("⚠️ Missing 'state' parameter");
    server.send(400, "text/plain", "Missing state parameter");
    return;
  }

  String state = server.arg("state");
  ledState = (state == "on");

  updateLEDs();

  Serial.printf("💡 LED set to %s\n", state.c_str());
  server.send(200, "text/plain", "LED set to " + state);
}
