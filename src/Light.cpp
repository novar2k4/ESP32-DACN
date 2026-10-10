#include "Light.h"

static NimBLEClient *lightClient = nullptr;
static NimBLERemoteCharacteristic *lightNotifyChar = nullptr;

// Nordic UART Service
static const char *UART_SERVICE_UUID =
    "6e400001-b5a3-f393-e0a9-e50e24dcca9e";

static const char *UART_NOTIFY_UUID =
    "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

bool isLightDevice(const NimBLEAdvertisedDevice *device)
{
    if (device == nullptr || !device->haveName())
        return false;

    String name = device->getName().c_str();
    String address = device->getAddress().toString().c_str();

    return name == LIGHT_DEVICE_NAME &&
           address.equalsIgnoreCase(LIGHT_SENSOR_ADDRESS);
}

static void lightNotifyCallback(
    NimBLERemoteCharacteristic *characteristic,
    uint8_t *data,
    size_t length,
    bool isNotify)
{
    if (length != 11)
        return;

    if (data[0] != 0xAA ||
        data[1] != 0x0D ||
        data[10] != 0xBB)
    {
        return;
    }

    float lux = 0.0f;

    memcpy(&lux, &data[5], sizeof(float));

    global_light = lux;

    // Serial.print("[LIGHT] ");
    // Serial.print(global_light, 2);
    // Serial.println(" lux");
}

//  bool connectLightSensor(const NimBLEAdvertisedDevice *device)
// {
//     if (device == nullptr)
//     {
//         Serial.println("[LIGHT] Device is null.");
//         return false;
//     }

//     Serial.println();
//     Serial.println("========== CONNECTING LIGHT SENSOR ==========");
//     Serial.print("[LIGHT] Name: ");
//     Serial.println(device->getName().c_str());
//     Serial.print("[LIGHT] Address: ");
//     Serial.println(device->getAddress().toString().c_str());

//     lightClient = NimBLEDevice::createClient();

//     if (lightClient == nullptr)
//     {
//         Serial.println("[LIGHT] Failed to create client.");
//         return false;
//     }

//     if (!lightClient->connect(device))
//     {
//         Serial.println("[LIGHT] Connection FAILED.");
//         NimBLEDevice::deleteClient(lightClient);
//         lightClient = nullptr;
//         return false;
//     }

//     Serial.println("[LIGHT] Connected.");

//     NimBLERemoteService *service =
//         lightClient->getService(UART_SERVICE_UUID);

//     if (service == nullptr)
//     {
//         Serial.println("[LIGHT] UART service not found.");
//         lightClient->disconnect();
//         return false;
//     }

//     lightNotifyChar =
//         service->getCharacteristic(UART_NOTIFY_UUID);

//     if (lightNotifyChar == nullptr)
//     {
//         Serial.println("[LIGHT] Notify characteristic not found.");
//         lightClient->disconnect();
//         return false;
//     }

//     if (!lightNotifyChar->canNotify())
//     {
//         Serial.println("[LIGHT] Characteristic cannot notify.");
//         lightClient->disconnect();
//         return false;
//     }

//     if (!lightNotifyChar->subscribe(true, lightNotifyCallback))
//     {
//         Serial.println("[LIGHT] Subscribe FAILED.");
//         lightClient->disconnect();
//         return false;
//     }

//     Serial.println("[LIGHT] Notification subscribed.");
//     Serial.println("========== LIGHT SENSOR READY ==========");

//     return true;
// }

//// Testing ////

bool connectLightSensor(
    const NimBLEAdvertisedDevice *device)
{
    if (lightClient != nullptr)
    {
        if (lightClient->isConnected())
            return true;

        NimBLEDevice::deleteClient(
            lightClient);

        lightClient = nullptr;
        lightNotifyChar = nullptr;
    }
    // =================================================
    // Check device
    // =================================================

    if (device == nullptr)
    {
        Serial.println("[LIGHT] Device is null.");
        return false;
    }

    // =================================================
    // Nếu client cũ tồn tại
    // =================================================

    if (lightClient != nullptr)
    {
        // Đã connected -> không cần connect lại
        if (lightClient->isConnected())
        {
            Serial.println(
                "[LIGHT] Already connected.");

            return true;
        }

        // Client cũ đã mất kết nối
        Serial.println(
            "[LIGHT] Deleting old client...");

        NimBLEDevice::deleteClient(
            lightClient);

        lightClient = nullptr;
        lightNotifyChar = nullptr;
    }

    // =================================================
    // Debug device
    // =================================================

    Serial.println();
    Serial.println(
        "========== CONNECTING LIGHT SENSOR ==========");

    Serial.print("[LIGHT] Name: ");
    Serial.println(
        device->getName().c_str());

    Serial.print("[LIGHT] Address: ");
    Serial.println(
        device->getAddress()
            .toString()
            .c_str());

    Serial.print("[LIGHT] RSSI: ");
    Serial.println(
        device->getRSSI());

    // =================================================
    // Create BLE Client
    // =================================================

    lightClient =
        NimBLEDevice::createClient();

    if (lightClient == nullptr)
    {
        Serial.println(
            "[LIGHT] Failed to create BLE client.");

        return false;
    }

    // =================================================
    // Connect
    // =================================================

    Serial.println(
        "[LIGHT] Connecting...");

    if (!lightClient->connect(device))
    {
        Serial.println(
            "[LIGHT] Connection FAILED.");

        NimBLEDevice::deleteClient(
            lightClient);

        lightClient = nullptr;

        return false;
    }

    Serial.println(
        "[LIGHT] Connected!");

    // =================================================
    // Find UART Service
    // =================================================

    NimBLERemoteService *service =
        lightClient->getService(
            UART_SERVICE_UUID);

    if (service == nullptr)
    {
        Serial.println(
            "[LIGHT] UART service NOT found.");

        lightClient->disconnect();

        NimBLEDevice::deleteClient(
            lightClient);

        lightClient = nullptr;

        return false;
    }

    Serial.println(
        "[LIGHT] UART service found.");

    // =================================================
    // Find Notify Characteristic
    // =================================================

    lightNotifyChar =
        service->getCharacteristic(
            UART_NOTIFY_UUID);

    if (lightNotifyChar == nullptr)
    {
        Serial.println(
            "[LIGHT] Notify characteristic NOT found.");

        lightClient->disconnect();

        NimBLEDevice::deleteClient(
            lightClient);

        lightClient = nullptr;

        return false;
    }

    Serial.println(
        "[LIGHT] Notify characteristic found.");

    // =================================================
    // Check NOTIFY
    // =================================================

    if (!lightNotifyChar->canNotify())
    {
        Serial.println(
            "[LIGHT] Characteristic does NOT support notify.");

        lightClient->disconnect();

        NimBLEDevice::deleteClient(
            lightClient);

        lightClient = nullptr;

        lightNotifyChar = nullptr;

        return false;
    }

    // =================================================
    // Subscribe
    // =================================================

    Serial.println(
        "[LIGHT] Subscribing...");

    if (!lightNotifyChar->subscribe(
            true,
            lightNotifyCallback))
    {
        Serial.println(
            "[LIGHT] Subscribe FAILED.");

        lightClient->disconnect();

        NimBLEDevice::deleteClient(
            lightClient);

        lightClient = nullptr;
        lightNotifyChar = nullptr;

        return false;
    }

    Serial.println(
        "[LIGHT] Notification subscribed.");

    Serial.println(
        "========== LIGHT SENSOR READY ==========");

    return true;
}

bool isLightConnected()
{
    return (
        lightClient != nullptr &&
        lightClient->isConnected());
}