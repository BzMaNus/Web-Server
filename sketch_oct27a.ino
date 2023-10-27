#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

ESP8266WebServer server(3000);
DHT dht11(D4, DHT11);
const int led = D6;
bool led_status = false;

void init_wifi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(void) {
  pinMode(led, OUTPUT);
  dht11.begin();
  Serial.begin(115200);
  init_wifi("Chatree01", "0620565502");

  server.on("/", HTTP_GET, []() {
    String html = "<html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css'>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<h1 class='mt-5'>Web Server From ESP8266</h1>";
    html += "<button id='status_button' class='btn btn-success mt-3 mb-3' onclick='OnClick()'>Turn ON</button>";
    html += "<p id='status'>Status: Off</p>";
    html += "<p>Humidity: <span id='humidity'>0.00%</span></p>";
    html += "<p>Temperature: <span id='temperature'>0.00°C</span></p>";
    html += "<script>";
    html += "function OnClick() {";
    html += "  fetch('/OnClick', { method: 'POST' })";
    html += "    .then(response => response.text())";
    html += "    .then(data => {";
    html += "      document.getElementById('status_button').innerHTML = data;";
    html += "      document.getElementById('status').innerHTML = data === 'Turn OFF' ? 'Status: On' : 'Status: Off';";
    html += "      document.getElementById('status_button').classList.toggle('btn-success');";
    html += "      document.getElementById('status_button').classList.toggle('btn-secondary');";
    html += "    });";
    html += "}";
    html += "function updateData() {";
    html += "  fetch('/GetData', { method: 'GET' })";
    html += "    .then(response => response.json())";
    html += "    .then(data => {";
    html += "      document.getElementById('humidity').textContent = data.humid.toFixed(2) + '%';";
    html += "      document.getElementById('temperature').textContent = data.temp.toFixed(2) + '°C';";
    html += "    });";
    html += "}";
    html += "setInterval(updateData, 1000);";  
    html += "</script>";
    html += "</div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/OnClick", HTTP_POST, []() {
    led_status = !led_status;
    digitalWrite(led, led_status ? HIGH : LOW);
    server.send(200, "text/plain", led_status ? "Turn OFF" : "Turn ON");
  });

  server.on("/GetData", HTTP_GET, []() {
    float humid = dht11.readHumidity();
    float temp = dht11.readTemperature();
    String json = "{\"humid\":" + String(humid) + ",\"temp\":" + String(temp) + "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
