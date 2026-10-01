#include "CoreIoT.h"
#include "neo_blinky.h"
#include "global.h"

// =====================================================
// CoreIoT CONFIGURATION
// =====================================================

const char *coreIOT_Server = "app.coreiot.io";
const char *coreIOT_Token = "dPzMmVtfZIwEdQKuMBGN";
const int mqttPort = 1883;
int lastButtonS1 = -1;

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
                "Subscribed to v1/devices/me/rpc/request/+");
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
    char *topic,
    byte *payload,
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

    const char *method = doc["method"];

    if (method == nullptr)
    {
        Serial.println("RPC method missing.");
        return;
    }

    // -------------------------------------------------
    // Handle RPC
    // -------------------------------------------------

    if (strcmp(method, "setSwitchState") == 0)
    {
        bool state = doc["params"];

        switch_state = state;

        Serial.print("CoreIoT Switch state = ");

        if (switch_state)
        {
            Serial.println("ON");
        }
        else
        {
            Serial.println("OFF");
        }
    }

    String topicStr = String(topic);

    // =================================================
    // RPC request
    // =================================================
    if (topicStr.startsWith("v1/devices/me/rpc/request/"))
    {
        // Lấy RPC ID từ topic
        String requestId =
            topicStr.substring(
                String("v1/devices/me/rpc/request/").length());

        const char *method = doc["method"];

        // -------------------------------------------------
        // setMaxLight(value)
        // -------------------------------------------------
        if (strcmp(method, "setMaxLight") == 0)
        {
            float value = doc["params"].as<float>();

            // Giới hạn an toàn
            if (value < 0.0f)
                value = 0.0f;

            if (value > 500.0f)
                value = 500.0f;

            max_light = value;

            Serial.print("[RPC] max_light = ");
            Serial.println(max_light);

            // Trả lại giá trị hiện tại cho Core IoT
            String responseTopic =
                "v1/devices/me/rpc/response/" + requestId;

            String response =
                String(max_light, 1);

            client.publish(
                responseTopic.c_str(),
                response.c_str());
        }

        // -------------------------------------------------
        // getMaxLight()
        // -------------------------------------------------
        else if (strcmp(method, "getMaxLight") == 0)
        {
            Serial.print("[RPC] getMaxLight -> ");
            Serial.println(max_light);

            String responseTopic =
                "v1/devices/me/rpc/response/" + requestId;

            String response =
                String(max_light, 1);

            client.publish(
                responseTopic.c_str(),
                response.c_str());
        }
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
        mqttPort);

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

    bool lastSwitchState = false;

    while (1)
    {
        Serial.println("[CoreIoT] Task running");

        if (!client.connected())
        {
            Serial.println("MQTT disconnected. Attempting to reconnect...");
            reconnect();
        }

        client.loop();


        // ==========================================
        // CONTROL NEO LED FROM COREIOT SWITCH
        // ==========================================

        if (switch_state != lastSwitchState)
        {
            lastSwitchState = switch_state;

            if (switch_state)
            {
                Serial.println("[CoreIoT] Switch = TRUE");
                Serial.println("[CoreIoT] Turning ON Neo LED");

                initNeoBlinky();
            }
            else
            {
                Serial.println("[CoreIoT] Switch = FALSE");
                Serial.println("[CoreIoT] Turning OFF Neo LED");

                stopNeoBlinky();
            }
        }


        // ==========================================
        // Update S1 attribute state to CoreIoT
        // ==========================================

        if (glob_buttons1 != lastButtonS1)
        {
            lastButtonS1 = glob_buttons1;

            String attributePayload =
                "{\"button_s1\":" +
                String(glob_buttons1 ? "true" : "false") +
                "}";

            if (client.publish(
                    "v1/devices/me/attributes",
                    attributePayload.c_str()))
            {
                Serial.print("S1 updated: ");
                Serial.println(attributePayload);
            }
            else
            {
                Serial.println("Failed to update S1");
            }
        }


        // ==========================================
        // Create telemetry JSON
        // ==========================================

        String payload =
            "{\"temperature\":" + String(global_temperature, 1) +
            ",\"humidity\":" + String(global_humidity, 1) +
            ",\"light\":" + String(global_light) +
            "}";


        if (client.connected())
        {
            bool success = client.publish(
                "v1/devices/me/telemetry",
                payload.c_str()
            );

            if (!success)
            {
                Serial.println("Failed to publish telemetry.");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// =====================================================
// COREIOT INIT
// =====================================================

void coreiot_init()
{
    xTaskCreate(coreiot_task, "CoreIoT Task", 4096, NULL, 2, NULL);
}