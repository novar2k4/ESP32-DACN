#include "CoreIoT.h"

#include "global.h"


// =====================================================
// CoreIoT CONFIGURATION
// =====================================================

const char* coreIOT_Server = "app.coreiot.io";
const char* coreIOT_Token = "cbgahhpy0q409xy74gya";
const int mqttPort = 1883;


// =====================================================
// MQTT CLIENT
// =====================================================

WiFiClient espClient;
PubSubClient client(espClient);


// =====================================================
// MQTT RECONNECT
// =====================================================

void reconnect()
{
    while (!client.connected())
    {
        // Make sure Wi-Fi is connected
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi is not connected.");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        Serial.print("Attempting MQTT connection... ");

        // CoreIoT:
        // username = Device Access Token
        // password = NULL
        if (client.connect("ESP32Client", coreIOT_Token, NULL))
        {
            Serial.println("connected to CoreIoT!");

            // Subscribe to RPC requests
            client.subscribe("v1/devices/me/rpc/request/+");

            Serial.println(
                "Subscribed to v1/devices/me/rpc/request/+"
            );
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" - retrying in 5 seconds");

            vTaskDelay(5000);
        }
    }
}


// =====================================================
// MQTT CALLBACK
// =====================================================

void callback(
    char* topic,
    byte* payload,
    unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.println("]");

    // Create temporary message buffer
    char message[length + 1];

    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.print("Payload: ");
    Serial.println(message);


    // -------------------------------------------------
    // Parse JSON
    // -------------------------------------------------

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, message);

    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }


    // -------------------------------------------------
    // Read RPC method
    // -------------------------------------------------

    const char* method = doc["method"];

    if (method == nullptr)
    {
        Serial.println("RPC method missing.");
        return;
    }


    // -------------------------------------------------
    // Handle LED RPC
    // -------------------------------------------------

    if (strcmp(method, "setStateLED") == 0)
    {
        const char* params = doc["params"];

        if (params == nullptr)
        {
            Serial.println("RPC params missing.");
            return;
        }

        if (strcmp(params, "ON") == 0)
        {
            Serial.println("Device turned ON.");

            // TODO:
            // Turn LED ON here
        }
        else
        {
            Serial.println("Device turned OFF.");

            // TODO:
            // Turn LED OFF here
        }
    }
    else
    {
        Serial.print("Unknown method: ");
        Serial.println(method);
    }
}


// =====================================================
// COREIOT SETUP
// =====================================================

void setup_coreiot()
{
    // Wi-Fi is already initialized elsewhere
    // by your WebSocket / Wi-Fi module.
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Warning: WiFi is not connected.");
    }
    else
    {
        Serial.print("WiFi connected. ESP32 IP: ");
        Serial.println(WiFi.localIP());
    }


    // Configure MQTT server
    client.setServer(
        coreIOT_Server,
        mqttPort
    );

    // Configure MQTT callback
    client.setCallback(callback);

    Serial.println("CoreIoT MQTT initialized.");
}


// =====================================================
// COREIOT TASK
// =====================================================

void coreiot_task(void *pvParameters)
{
    setup_coreiot();

    while (1)
    {
        // -------------------------------------------------
        // Reconnect if MQTT connection is lost
        // -------------------------------------------------
        Serial.println("[CoreIoT] Task running");
        if (!client.connected())
        {
            Serial.println("MQTT disconnected. Attempting to reconnect...");
            reconnect();
        }
        // -------------------------------------------------
        // Keep MQTT connection alive
        // -------------------------------------------------
        client.loop();
        // -------------------------------------------------
        // Create telemetry JSON
        // -------------------------------------------------
        String payload =
            "{\"temperature\":" +
            String(global_temperature, 1) +
            ",\"humidity\":" +
            String(global_humidity, 1) +
            "}";


        // -------------------------------------------------
        // Publish telemetry
        // -------------------------------------------------

        if (client.connected())
        {
            bool success = client.publish("v1/devices/me/telemetry",payload.c_str()
            );

            if (success)
            {
                Serial.print("Published to CoreIoT: ");
                Serial.println(payload);
            }
            else
            {
                Serial.println(
                    "Failed to publish telemetry."
                );
            }
        }
        // -------------------------------------------------
        // Publish every 2 seconds
        // -------------------------------------------------
        vTaskDelay(2000);
    }
}


// =====================================================
// COREIOT INIT
// =====================================================

void coreiot_init()
{
    xTaskCreate(coreiot_task,"CoreIoT Task",4096,NULL,2,NULL);
}