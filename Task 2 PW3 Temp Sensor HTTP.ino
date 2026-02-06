#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "XXXXXXXX";
const char* password = "XXXXXXX";

// DHT setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WebServer server(80);

float temperature = 0.0;
float humidity = 0.0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("🔧 DHT11 sensor initialized");

  Serial.print("📡 Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected");
  Serial.print("📶 IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);

  server.begin();
  Serial.println("🚀 HTTP server started");
}

void loop() {
  server.handleClient();
}

void readSensor() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("❌ Failed to read from DHT11");
    return;
  }

  Serial.print("🌡️ Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("💧 Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}

void handleRoot() {
  Serial.println("🌐 HTTP GET / - Webpage loaded");

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>DHT11 Monitor</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: linear-gradient(135deg, #74ebd5, #ACB6E5);
      text-align: center;
      padding: 40px;
    }
    .card {
      background: #ffffff;
      width: 300px;
      margin: auto;
      padding: 25px;
      border-radius: 15px;
      box-shadow: 0 8px 16px rgba(0,0,0,0.2);
    }
    h1 {
      color: #333;
    }
    .temp {
      font-size: 26px;
      color: #e53935;
      margin: 10px 0;
    }
    .hum {
      font-size: 26px;
      color: #1e88e5;
      margin: 10px 0;
    }
  </style>
</head>
<body>

  <div class="card">
    <h1>DHT11 Sensor Data</h1>
    <div class="temp">Temperature: <span id="temp">--</span> C</div>
    <div class="hum">Humidity: <span id="hum">--</span> %</div>
  </div>

<script>
  function fetchData() {
    fetch('/data')
      .then(response => response.json())
      .then(data => {
        document.getElementById('temp').innerText = data.temperature;
        document.getElementById('hum').innerText = data.humidity;
      });
  }

  fetchData();
  setInterval(fetchData, 3000);
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleData() {
  readSensor();

  String json = "{";
  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1);
  json += "}";

  server.send(200, "application/json", json);
}
