#include "WebSocket.h"

// Replace with your network credentials
const char* ssid = "HCMUT-MEETING";
const char* password = "hcmut@meeting";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JsonDocument readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

String getSensorReadings(){
  readings["temperature"] = String(global_temperature);
  readings["humidity"] =  String(global_humidity);
  // readings["pressure"] = String(global_pressure);
  String jsonString;
  serializeJson(readings, jsonString);
  Serial.println(WiFi.localIP());
  return jsonString;
}

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  wifi = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print('.');
  //   delay(1000);
  // }
}

void checkWiFistatus() { // Check
  if (WiFi.status() != WL_CONNECTED) {
    wifi = 0;
  } else {
    wifi = 1;
  }
  // Serial.print("WiFi status: ");
  // Serial.println(wifi);
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //data[len] = 0;
    //String message = (char*)data;
    // Check if the message is "getReadings"
    //if (strcmp((char*)data, "getReadings") == 0) {
      //if it is, send current sensor readings
      String sensorReadings = getSensorReadings();
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    //}
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket()
{
    // Attach WebSocket event handler
    ws.onEvent(onEvent);
    // Attach WebSocket to web server
    server.addHandler(&ws);
    // Main webpage
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS,"/index.html","text/html");
    });

    server.serveStatic("/",LittleFS,"/");
    // Start server
    server.begin();
    Serial.println("Web server started");
}

void webSocketLoop()
{
    if ((millis() - lastTime) > timerDelay)
    {
        String sensorReadings = getSensorReadings();
        Serial.println(sensorReadings);
        notifyClients(sensorReadings);
        lastTime = millis();
    }
    ws.cleanupClients();
}