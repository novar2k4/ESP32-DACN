#include "TempHumid.h"

DHT20 DHT;

static NimBLEClient *tempClient = nullptr;
static NimBLERemoteCharacteristic *tempNotifyChar = nullptr;

static const char *UART_SERVICE_UUID =
    "6e400001-b5a3-f393-e0a9-e50e24dcca9e";

static const char *UART_NOTIFY_UUID =
    "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

//// HUMID SENSOR DHT20 /////

void dht_task(void *pvParameters)
{
    Wire.begin(11, 12);
    DHT.begin();

    while (1)
    {
        DHT.read();
        // global_temperature = DHT.getTemperature();
        global_humidity = DHT.getHumidity();
        // Serial.print("Temperature: ");
        // Serial.print(global_temperature, 1);
        // Serial.print("°C     ");
        Serial.print("[HUMI] ");
        Serial.print(global_humidity, 1);
        Serial.println("%");
        vTaskDelay(3000);
    }
}

void initHumid()
{
    xTaskCreate(dht_task, "DHT Task", 2048, NULL, 2, NULL);
}

//// TEMP SENSOR /////

bool isTemperatureConnected()
{
    return (
        tempClient != nullptr &&
        tempClient->isConnected());
}

bool isTempDevice(const NimBLEAdvertisedDevice *device)
{
    if (device == nullptr || !device->haveName())
        return false;

    String name = device->getName().c_str();
    String address = device->getAddress().toString().c_str();

    return name == TEMP_DEVICE_NAME &&
           address.equalsIgnoreCase(TEMP_SENSOR_ADDRESS);
}

static void tempNotifyCallback(
    NimBLERemoteCharacteristic *characteristic,
    uint8_t *data,
    size_t length,
    bool isNotify)
{
    // Expected packet:
    // AA 04 00 xx 04 [4-byte float] xx BB
    if (length != 11)
        return;

    if (data[0] != 0xAA ||
        data[1] != 0x04 ||
        data[10] != 0xBB)
    {
        return;
    }

    float temperature = 0.0f;

    memcpy(&temperature, &data[5], sizeof(float));

    global_temperature = temperature;

    // Serial.print("[TEMP] ");
    // Serial.print(global_temperature, 2);
    // Serial.println(" C");
}

// bool connectTempSensor(const NimBLEAdvertisedDevice *device)
// {
//     if (device == nullptr)
//     {
//         Serial.println("[TEMP] Device is null.");
//         return false;
//     }

//     Serial.println();
//     Serial.println("========== CONNECTING TEMP/HUMID SENSOR ==========");
//     Serial.print("[TEMP] Name: ");
//     Serial.println(device->getName().c_str());
//     Serial.print("[TEMP] Address: ");
//     Serial.println(device->getAddress().toString().c_str());

//     tempClient = NimBLEDevice::createClient();

//     if (tempClient == nullptr)
//     {
//         Serial.println("[TEMP] Failed to create client.");
//         return false;
//     }

//     if (!tempClient->connect(device))
//     {
//         Serial.println("[TEMP] Connection FAILED.");
//         NimBLEDevice::deleteClient(tempClient);
//         tempClient = nullptr;
//         return false;
//     }

//     Serial.println("[TEMP] Connected.");

//     const auto &services = tempClient->getServices(true);

//     for (auto *service : services)
//     {
//         const auto &characteristics =
//             service->getCharacteristics(true);

//         for (auto *characteristic : characteristics)
//         {
//             if (characteristic->canNotify())
//             {
//                 Serial.print("[TEMP] NOTIFY: ");
//                 Serial.println(
//                     characteristic->getUUID().toString().c_str());

//                 if (characteristic->subscribe(
//                         true,
//                         tempNotifyCallback))
//                 {
//                     Serial.println("[TEMP] Notification subscribed.");

//                     if (tempNotifyChar == nullptr)
//                         tempNotifyChar = characteristic;

//                     // We have found a notify characteristic that matches
//                     // the current temperature sensor design.
//                     return true;
//                 }
//             }
//         }
//     }

//     Serial.println("[TEMP] No usable NOTIFY characteristic found.");
//     tempClient->disconnect();
//     return false;
// }

//// TESTING ////

bool connectTempSensor(
    const NimBLEAdvertisedDevice *device)
{
    if (tempClient != nullptr)
    {
        if (tempClient->isConnected())
            return true;

        NimBLEDevice::deleteClient(
            tempClient);

        tempClient = nullptr;
        tempNotifyChar = nullptr;
    }
    // =================================================
    // Check device
    // =================================================

    if (device == nullptr)
    {
        Serial.println(
            "[TEMP] Device is null.");

        return false;
    }

    // =================================================
    // Delete old client if disconnected
    // =================================================

    if (tempClient != nullptr)
    {
        if (tempClient->isConnected())
        {
            Serial.println(
                "[TEMP] Already connected.");

            return true;
        }

        Serial.println(
            "[TEMP] Deleting old client...");

        NimBLEDevice::deleteClient(
            tempClient);

        tempClient = nullptr;
        tempNotifyChar = nullptr;
    }

    // =================================================
    // Debug
    // =================================================

    Serial.println();
    Serial.println(
        "========== CONNECTING TEMPERATURE SENSOR ==========");

    Serial.print("[TEMP] Name: ");
    Serial.println(
        device->getName().c_str());

    Serial.print("[TEMP] Address: ");
    Serial.println(
        device->getAddress()
            .toString()
            .c_str());

    Serial.print("[TEMP] RSSI: ");
    Serial.println(
        device->getRSSI());

    // =================================================
    // Create client
    // =================================================

    tempClient =
        NimBLEDevice::createClient();

    if (tempClient == nullptr)
    {
        Serial.println(
            "[TEMP] Failed to create BLE client.");

        return false;
    }

    // =================================================
    // Connect
    // =================================================

    Serial.println(
        "[TEMP] Connecting...");

    if (!tempClient->connect(device))
    {
        Serial.println(
            "[TEMP] Connection FAILED.");

        NimBLEDevice::deleteClient(
            tempClient);

        tempClient = nullptr;

        return false;
    }

    Serial.println(
        "[TEMP] Connected!");

    Serial.print("[TEMP] Peer: ");
    Serial.println(
        tempClient->getPeerAddress()
            .toString()
            .c_str());

    // =================================================
    // Find UART service
    // =================================================

    NimBLERemoteService *service =
        tempClient->getService(
            UART_SERVICE_UUID);

    if (service == nullptr)
    {
        Serial.println(
            "[TEMP] UART service NOT found.");

        tempClient->disconnect();

        NimBLEDevice::deleteClient(
            tempClient);

        tempClient = nullptr;

        return false;
    }

    Serial.println(
        "[TEMP] UART service found.");

    // =================================================
    // Find notify characteristic
    // =================================================

    tempNotifyChar =
        service->getCharacteristic(
            UART_NOTIFY_UUID);

    if (tempNotifyChar == nullptr)
    {
        Serial.println(
            "[TEMP] Notify characteristic NOT found.");

        tempClient->disconnect();

        NimBLEDevice::deleteClient(
            tempClient);

        tempClient = nullptr;

        return false;
    }

    Serial.println(
        "[TEMP] Notify characteristic found.");

    // =================================================
    // Check notify
    // =================================================

    if (!tempNotifyChar->canNotify())
    {
        Serial.println(
            "[TEMP] Characteristic does NOT support notify.");

        tempClient->disconnect();

        NimBLEDevice::deleteClient(
            tempClient);

        tempClient = nullptr;
        tempNotifyChar = nullptr;

        return false;
    }

    // =================================================
    // Subscribe
    // =================================================

    Serial.println(
        "[TEMP] Subscribing...");

    if (!tempNotifyChar->subscribe(
            true,
            tempNotifyCallback))
    {
        Serial.println(
            "[TEMP] Subscribe FAILED.");

        tempClient->disconnect();

        NimBLEDevice::deleteClient(
            tempClient);

        tempClient = nullptr;
        tempNotifyChar = nullptr;

        return false;
    }

    Serial.println(
        "[TEMP] Notification subscribed.");

    Serial.println(
        "========== TEMPERATURE SENSOR READY ==========");

    return true;
}
