#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Pixel_2603";
const char* password = "12345678";

const int LED_PIN = 32;

WebServer server(80);
bool ledOn = false;

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 LED Control</title>
</head>
<body>
  <h1>ESP32 LED Control</h1>

  <button onclick="controlLED('/on')">TURN ON</button>
  <button onclick="controlLED('/off')">TURN OFF</button>

  <p id="status">LED status: OFF</p>
  <p id="result">Click a button to measure response time.</p>

  <script>
    async function controlLED(path) {
      const start = performance.now();

      try {
        const response = await fetch(path);
        const result = await response.text();
        const end = performance.now();

        document.getElementById("status").textContent =
          "LED status: " + result;

        document.getElementById("result").textContent =
          "Request/response time: " +
          (end - start).toFixed(2) + " ms";
      } catch (error) {
        document.getElementById("result").textContent =
          "Request failed. Check your Wi-Fi connection.";
      }
    }
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(LED_PIN, HIGH);
  ledOn = true;
  server.send(200, "text/plain", "ON");
}

void handleOff() {
  digitalWrite(LED_PIN, LOW);
  ledOn = false;
  server.send(200, "text/plain", "OFF");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("Open this address in your browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
}