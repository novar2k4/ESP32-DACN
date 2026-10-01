#include "BLE.h"
// =====================================================
// Light sensor
// =====================================================
#define LIGHT_DEVICE_NAME "inno-013-db8d"
#define LIGHT_SENSOR_ADDRESS "0c:8b:95:fa:42:fe"

static NimBLEClient *lightClient = nullptr;
static NimBLERemoteCharacteristic *lightNotifyChar = nullptr;

// =====================================================
// Temperature sensor
// =====================================================
// #define TEMP_DEVICE_NAME "inno-004-9377"
// #define TEMP_SENSOR_ADDRESS "08:a6:f7:09:6c:da"
#define TEMP_DEVICE_NAME "inno-004-8645"
#define TEMP_SENSOR_ADDRESS  "08:a6:f7:07:79:e6"

static NimBLEClient *tempClient = nullptr;
static NimBLERemoteCharacteristic *tempNotifyChar = nullptr;

// =====================================================
// UUID - Nordic UART Service
// =====================================================

static const char *UART_SERVICE_UUID =
    "6e400001-b5a3-f393-e0a9-e50e24dcca9e";

static const char *UART_NOTIFY_UUID =
    "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

static const char *UART_WRITE_UUID =
    "6e400002-b5a3-f393-e0a9-e50e24dcca9e";

// =====================================================
// Light sensor notification
// =====================================================

void lightNotifyCallback(
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

    float lux;

    memcpy(
        &lux,
        &data[5],
        sizeof(float));

    global_light = lux;

    Serial.print("[LIGHT] ");
    Serial.print(global_light, 2);
    Serial.println(" lux");
    
    vTaskDelay(50);
}

// =====================================================
// Temperature sensor notification
// =====================================================

void tempNotifyCallback(
    NimBLERemoteCharacteristic *characteristic,
    uint8_t *data,
    size_t length,
    bool isNotify)
{
    // Expected packet:
    // AA 04 00 xx 04 [4-byte float] xx BB

    if (length != 11)
        return;

    // Check header / packet type / footer
    if (data[0] != 0xAA ||
        data[1] != 0x04 ||
        data[10] != 0xBB)
    {
        return;
    }

    float temperature = 0.0f;

    memcpy(
        &temperature,
        &data[5],
        sizeof(float));

    global_temperature = temperature;

    Serial.print("[TEMPERATURE] ");
    Serial.print(global_temperature, 2);
    Serial.println(" °C");
    vTaskDelay(50);
}

// =====================================================
// Connect light sensor
// =====================================================

bool connectLightSensor(
    const NimBLEAdvertisedDevice *device)
{
    if (device == nullptr)
    {
        Serial.println("Light sensor device is null.");
        return false;
    }

    Serial.println();
    Serial.println("========== CONNECTING LIGHT SENSOR ==========");

    Serial.print("Name: ");
    Serial.println(device->getName().c_str());

    Serial.print("Address: ");
    Serial.println(
        device->getAddress().toString().c_str());

    Serial.print("RSSI: ");
    Serial.println(device->getRSSI());

    // Create client
    lightClient = NimBLEDevice::createClient();

    if (lightClient == nullptr)
    {
        Serial.println("Failed to create light BLE client.");
        return false;
    }

    // Connect
    if (!lightClient->connect(device))
    {
        Serial.println("Light sensor connection FAILED.");
        return false;
    }

    Serial.println("Light sensor CONNECTED.");

    // =================================================
    // Find Nordic UART Service
    // =================================================

    NimBLERemoteService *service =
        lightClient->getService(
            UART_SERVICE_UUID);

    if (service == nullptr)
    {
        Serial.println("Light UART service not found.");
        lightClient->disconnect();
        return false;
    }

    // =================================================
    // Find Notify characteristic
    // =================================================

    lightNotifyChar =
        service->getCharacteristic(
            UART_NOTIFY_UUID);

    if (lightNotifyChar == nullptr)
    {
        Serial.println(
            "Light notify characteristic not found.");

        lightClient->disconnect();
        return false;
    }

    // =================================================
    // Subscribe
    // =================================================

    if (!lightNotifyChar->canNotify())
    {
        Serial.println(
            "Light characteristic cannot notify.");

        lightClient->disconnect();
        return false;
    }

    if (!lightNotifyChar->subscribe(
            true,
            lightNotifyCallback))
    {
        Serial.println(
            "Light notification subscribe FAILED.");

        lightClient->disconnect();
        return false;
    }

    Serial.println(
        "Light notifications enabled.");

    Serial.println(
        "========== LIGHT SENSOR READY ==========");

    return true;
}

// =====================================================
// Connect temperature sensor
// =====================================================

bool connectTemperatureSensor(
    const NimBLEAdvertisedDevice *device)
{
    if (device == nullptr)
    {
        Serial.println(
            "Temperature sensor device is null.");

        return false;
    }

    Serial.println();
    Serial.println(
        "========== CONNECTING TEMPERATURE SENSOR ==========");

    Serial.print("Name: ");
    Serial.println(device->getName().c_str());

    Serial.print("Address: ");
    Serial.println(device->getAddress().toString().c_str());

    Serial.print("RSSI: ");
    Serial.println(device->getRSSI());

    // =================================================
    // Create client
    // =================================================

    tempClient =
        NimBLEDevice::createClient();
    if (tempClient == nullptr)
    {
        Serial.println(
            "Failed to create temperature BLE client.");
        return false;
    }

    // =================================================
    // Connect
    // =================================================

    if (!tempClient->connect(device))
    {
        Serial.println("Temperature sensor connection FAILED.");
        return false;
    }

    Serial.println("Temperature sensor CONNECTED.");

    Serial.print("Peer: ");
    Serial.println(
        tempClient->getPeerAddress().toString().c_str());

    Serial.print("RSSI: ");
    Serial.println(tempClient->getRssi());

    // =================================================
    // Discover ALL services
    // =================================================

    const auto &services =
        tempClient->getServices(true);

    Serial.println();
    Serial.println("========== TEMPERATURE SERVICES ==========");

    for (auto *service : services)
    {
        Serial.print("SERVICE: ");
        Serial.println(
            service->getUUID().toString().c_str());
        // =============================================
        // Discover characteristics
        // =============================================

        const auto &characteristics = service->getCharacteristics(true);

        for (auto *characteristic : characteristics)
        {
            Serial.print("  CHARACTERISTIC: ");

            Serial.println(
                characteristic->getUUID().toString().c_str());

            Serial.print("    READ: ");
            Serial.println(characteristic->canRead() ? "YES" : "NO");

            Serial.print("    WRITE: ");
            Serial.println(characteristic->canWrite() ? "YES" : "NO");

            Serial.print("    NOTIFY: ");
            Serial.println(
                characteristic->canNotify() ? "YES" : "NO");

            Serial.print("    INDICATE: ");
            Serial.println(
                characteristic->canIndicate() ? "YES" : "NO");

            // =========================================
            // READ
            // =========================================

            if (characteristic->canRead())
            {
                try
                {
                    std::string value =
                        characteristic->readValue();

                    Serial.print("    VALUE HEX: ");

                    for (size_t i = 0; i < value.length(); i++)
                    {
                        Serial.printf("%02X ", (uint8_t)value[i]);
                    }

                    Serial.println();
                }
                catch (...)
                {
                    Serial.println(" READ FAILED");
                }
            }

            // =========================================
            // NOTIFY
            // =========================================

            if (characteristic->canNotify()){
                Serial.println(
                    "    Subscribing to notification...");

                if (characteristic->subscribe(true, tempNotifyCallback)){
                    Serial.println("    Notification subscribed.");

                    // Save the characteristic.
                    // Later, once we identify which one
                    // is the temperature data characteristic,
                    // we'll keep only that one.
                    if (tempNotifyChar == nullptr){
                        tempNotifyChar = characteristic;
                    }
                }
                else{
                    Serial.println("    Notification subscription FAILED.");
                }
            }
        }
    }

    Serial.println("========== TEMPERATURE DISCOVERY DONE ==========");

    return true;
}

void BLE1()
{
    delay(5000);

    Serial.println();
    Serial.println("===== YOLO UNO - TWO BLE SENSORS =====");

    // =================================================
    // Initialize BLE
    // =================================================

    NimBLEDevice::init("YoloUNO");

    // =================================================
    // Scan
    // =================================================

    NimBLEScan *scan = NimBLEDevice::getScan();

    scan->setActiveScan(true);

    Serial.println();
    Serial.println("========== BLE SCAN ==========");

    NimBLEScanResults results = scan->getResults(10000, false);

    Serial.print("Found devices: ");

    Serial.println(results.getCount());

    // =================================================
    // Search for both sensors
    // =================================================

    const NimBLEAdvertisedDevice *
        lightDevice = nullptr;

    const NimBLEAdvertisedDevice *
        temperatureDevice = nullptr;

    for (int i = 0; i < results.getCount(); i++){
        const NimBLEAdvertisedDevice *device =
            results.getDevice(i);

        // Print device
        Serial.print("Device: ");
        Serial.println(device->toString().c_str());

        // ---------------------------------------------
        // Light sensor
        // ---------------------------------------------

        if (device->haveName() && device->getName() == LIGHT_DEVICE_NAME){
            lightDevice = device;

            Serial.println();
            Serial.println("*** LIGHT SENSOR FOUND ***");

            Serial.print("Address: ");
            Serial.println(device->getAddress().toString().c_str());
        }

        // ---------------------------------------------
        // Temperature sensor
        // ---------------------------------------------

        if (device->haveName() && device->getName() == TEMP_DEVICE_NAME){
            temperatureDevice = device;

            Serial.println();
            Serial.println("*** TEMPERATURE SENSOR FOUND ***");

            Serial.print("Address: ");
            Serial.println(device->getAddress().toString().c_str());
        }
    }

    // =================================================
    // Connect light sensor
    // =================================================

    bool lightOK = false;

    if (lightDevice != nullptr){
        lightOK = connectLightSensor(lightDevice);
    }
    else{
        Serial.println("Light sensor not found!");
    }

    // =================================================
    // Connect temperature sensor
    // =================================================

    bool temperatureOK = false;

    if (temperatureDevice != nullptr)
    {
        temperatureOK = connectTemperatureSensor(temperatureDevice);
    }
    else
    {
        Serial.println("Temperature sensor not found!");
    }

    // =================================================
    // Final status
    // =================================================

    Serial.println();
    Serial.println("===== BLE SENSOR SYSTEM STATUS =====");

    Serial.print("Light sensor: ");
    Serial.println(lightOK ? "OK" : "FAILED");

    Serial.print("Temperature sensor: ");
    Serial.println(temperatureOK ? "OK" : "FAILED");

    Serial.println("====================================");
}