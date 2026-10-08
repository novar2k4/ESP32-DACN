#include "BLE.h"
// =====================================================
// Light sensor
// =====================================================
#define LIGHT_DEVICE_NAME "inno-013-db8d"
#define LIGHT_SENSOR_ADDRESS "0c:8b:95:fa:42:fe"

#define TEMP_DEVICE_NAME "inno-004-8645"
#define TEMP_SENSOR_ADDRESS "08:a6:f7:07:79:e6"

#define PH_DEVICE_NAME "BLE-9909"
#define PH_SENSOR_ADDRESS "bc:96:51:5b:d9:ec"

#define PH_SERVICE_UUID "FF01"
#define PH_CHARACTERISTIC_UUID "FF02"

static NimBLEClient *lightClient = nullptr;
static NimBLERemoteCharacteristic *lightNotifyChar = nullptr;

// =====================================================
// Temperature sensor
// =====================================================
// #define TEMP_DEVICE_NAME "inno-004-9377"
// #define TEMP_SENSOR_ADDRESS "08:a6:f7:09:6c:da"
static NimBLEClient *tempClient = nullptr;
static NimBLERemoteCharacteristic *tempNotifyChar = nullptr;

NimBLEClient *phClient = nullptr;
const NimBLEAdvertisedDevice *phDevice = nullptr;

// =====================================================
// UUID - Nordic UART Service
// =====================================================

static const char *UART_SERVICE_UUID =
    "6e400001-b5a3-f393-e0a9-e50e24dcca9e";

static const char *UART_NOTIFY_UUID =
    "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

static const char *UART_WRITE_UUID =
    "6e400002-b5a3-f393-e0a9-e50e24dcca9e";

const NimBLEAdvertisedDevice *findPHSensor()
{
    NimBLEScan *scan = NimBLEDevice::getScan();

    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(80);

    Serial.println("[PH] Scanning for BLE-9909...");

    NimBLEScanResults results = scan->getResults(5000);

    for (int i = 0; i < results.getCount(); i++)
    {
        const NimBLEAdvertisedDevice *device =
            results.getDevice(i);

        String name = device->getName().c_str();
        String address = device->getAddress().toString().c_str();

        Serial.print("[PH] Found: ");
        Serial.print(name);
        Serial.print(" / ");
        Serial.println(address);

        if (
            name == PH_DEVICE_NAME &&
            address.equalsIgnoreCase(PH_SENSOR_ADDRESS))
        {
            Serial.println("[PH] Target sensor found!");
            return device;
        }
    }

    Serial.println("[PH] Sensor not found.");
    return nullptr;
}

void deCode(uint8_t *pValue, uint8_t len)
{
    uint8_t tmp;
    uint8_t hibit;
    uint8_t lobit;
    uint8_t hibit1;
    uint8_t lobit1;

    for (int i = len - 1; i > 0; i--)
    {
        tmp = pValue[i];

        hibit1 = (tmp & 0x55) << 1;
        lobit1 = (tmp & 0xAA) >> 1;

        tmp = pValue[i - 1];

        hibit = (tmp & 0x55) << 1;
        lobit = (tmp & 0xAA) >> 1;

        pValue[i] = ~(hibit1 | lobit);
        pValue[i - 1] = ~(hibit | lobit1);
    }
}

// =====================================================
// PH sensor notification
// =====================================================

// void phNotifyCallback(
//     NimBLERemoteCharacteristic *characteristic,
//     uint8_t *data,
//     size_t length,
//     bool isNotify
// )
// {
//     // BLE-9909 realtime frame của thiết bị bạn đang là 29 byte
//     if (length < 21)
//     {
//         Serial.print("[PH] Invalid packet length: ");
//         Serial.println(length);
//         return;
//     }

//     // Copy dữ liệu vì chúng ta sẽ decode packet
//     uint8_t packet[32];

//     if (length > sizeof(packet))
//     {
//         Serial.println("[PH] Packet too large!");
//         return;
//     }

//     memcpy(packet, data, length);

//     // ================================================
//     // RAW PACKET
//     // ================================================
//     // Serial.print("[PH RAW] ");
//     // Serial.print(length);
//     // Serial.print(" bytes: ");

//     // for (size_t i = 0; i < length; i++)
//     // {
//     //     if (packet[i] < 0x10)
//     //         Serial.print("0");

//     //     Serial.print(packet[i], HEX);
//     //     Serial.print(" ");
//     // }

//     // Serial.println();

//     // ================================================
//     // DECODE
//     // ================================================
//     deCode(packet, length);

//     // ================================================
//     // pH
//     // byte 3 = high
//     // byte 4 = low
//     // ================================================
//     uint16_t rawPH =
//         ((uint16_t)packet[3] << 8) |
//         packet[4];

//     global_ph = rawPH / 100.0f;

//     // ================================================
//     // Temperature của BLE-9909
//     // byte 13 = high
//     // byte 14 = low
//     // ================================================
//     uint16_t rawTemp =
//         ((uint16_t)packet[13] << 8) |
//         packet[14];

//     float waterTemp =
//         rawTemp / 10.0f;

//     // ================================================
//     // DEBUG
//     // ================================================
//     Serial.print("[PH] pH = ");
//     Serial.print(global_ph, 2);

//     Serial.print(" | Temp = ");
//     Serial.print(waterTemp, 1);

//     Serial.println(" C");
// }

static void phNotifyCallback(
    NimBLERemoteCharacteristic *characteristic,
    uint8_t *data,
    size_t length,
    bool isNotify)
{
    if (length < 22 || length > 32)
    {
        Serial.print("[PH] Invalid packet length: ");
        Serial.println(length);
        return;
    }

    uint8_t packet[32];

    memcpy(packet, data, length);

    // ================================================
    // DECODE
    // ================================================
    deCode(
        packet,
        static_cast<uint8_t>(length)
    );

    // ================================================
    // pH: byte 3-4
    // ================================================
    uint16_t rawPH =
        ((uint16_t)packet[3] << 8) |
        packet[4];

    global_ph =
        rawPH / 100.0f;

    // ================================================
    // EC: byte 5-6
    // Unit: µS/cm
    // ================================================
    uint16_t rawEC =
        ((uint16_t)packet[5] << 8) |
        packet[6];

    global_ec =
        (float)rawEC;

    // ================================================
    // Temperature: byte 13-14
    // ================================================
    uint16_t rawTemp =
        ((uint16_t)packet[13] << 8) |
        packet[14];

    float waterTemp =
        rawTemp / 10.0f;

    // ================================================
    // Debug
    // ================================================
    Serial.print("[WATER] pH = ");
    Serial.print(global_ph, 2);

    Serial.print(" | EC = ");
    Serial.print(global_ec, 0);
    Serial.print(" uS/cm");

    Serial.print(" | Temp = ");
    Serial.print(waterTemp, 1);
    Serial.println(" C");
}

bool connectPHSensor(const NimBLEAdvertisedDevice *device)
{
    if (device == nullptr)
        return false;

    phClient = NimBLEDevice::createClient();

    if (phClient == nullptr)
    {
        Serial.println("[PH] Failed to create client.");
        return false;
    }

    Serial.println("[PH] Connecting...");

    if (!phClient->connect(device))
    {
        Serial.println("[PH] Connection failed.");
        NimBLEDevice::deleteClient(phClient);
        phClient = nullptr;
        return false;
    }

    Serial.println("[PH] Connected!");

    auto services = phClient->getServices(true);

    Serial.println();
    Serial.println("========== PH SENSOR GATT ==========");

    if (services.empty())
    {
        Serial.println("[PH] No services found!");
        Serial.println("====================================");
        return false;
    }

    for (auto *service : services)
    {
        Serial.print("SERVICE: ");
        Serial.println(
            service->getUUID().toString().c_str());

        auto characteristics = service->getCharacteristics(true);

        if (characteristics.empty())
        {
            Serial.println("  No characteristics");
            continue;
        }

        for (auto *characteristic : characteristics)
        {
            Serial.print("  CHARACTERISTIC: ");
            Serial.println(
                characteristic->getUUID().toString().c_str());

            Serial.print("    Properties: ");

            if (characteristic->canRead())
                Serial.print("READ ");

            if (characteristic->canWrite())
                Serial.print("WRITE ");

            if (characteristic->canWriteNoResponse())
                Serial.print("WRITE_NR ");

            if (characteristic->canNotify())
                Serial.print("NOTIFY ");

            if (characteristic->canIndicate())
                Serial.print("INDICATE ");

            Serial.println();
        }
    }

    Serial.println("====================================");


    // =================================================
    // LẤY SERVICE 0xFF01
    // =================================================

    NimBLERemoteService *phService =
        phClient->getService("FF01");

    if (phService == nullptr)
    {
        Serial.println("[PH] FF01 service not found!");
        return false;
    }

    Serial.println("[PH] FF01 service found!");


    // =================================================
    // LẤY CHARACTERISTIC 0xFF02
    // =================================================

    NimBLERemoteCharacteristic *phCharacteristic =
        phService->getCharacteristic("FF02");

    if (phCharacteristic == nullptr)
    {
        Serial.println("[PH] FF02 characteristic not found!");
        return false;
    }

    Serial.println("[PH] FF02 characteristic found!");


    // =================================================
    // SUBSCRIBE NOTIFY
    // =================================================

    if (phCharacteristic->canNotify())
    {
        bool ok = phCharacteristic->subscribe(
            true,
            phNotifyCallback
        );

        Serial.print("[PH] Subscribe: ");
        Serial.println(ok ? "OK" : "FAILED");
    }
    else
    {
        Serial.println("[PH] FF02 does not support NOTIFY!");
    }


    // =================================================
    // READ GIÁ TRỊ HIỆN TẠI CỦA FF02
    // =================================================

    if (phCharacteristic->canRead())
    {
        NimBLEAttValue value =
            phCharacteristic->readValue();

        Serial.print("[PH READ] ");
        Serial.print(value.size());
        Serial.print(" bytes: ");

        const uint8_t *data = value.data();

        for (size_t i = 0; i < value.size(); i++)
        {
            if (data[i] < 0x10)
                Serial.print("0");

            Serial.print(data[i], HEX);
            Serial.print(" ");
        }

        Serial.println();
    }
    else
    {
        Serial.println("[PH] FF02 does not support READ!");
    }

    return true;
}

void connectWaterPHSensor()
{
    const NimBLEAdvertisedDevice *device =
        findPHSensor();

    if (device == nullptr)
        return;

    if (!connectPHSensor(device))
        return;
}

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

    // Serial.print("[LIGHT] ");
    // Serial.print(global_light, 2);
    // Serial.println(" lux");

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
    if (data[0] != 0xAA || data[1] != 0x04 || data[10] != 0xBB) {
        return;
    }

    float temperature = 0.0f;

    memcpy( &temperature, &data[5], sizeof(float));

    global_temperature = temperature;

    Serial.print("[TEMPERATURE] ");
    Serial.print(global_temperature, 2);
    Serial.println(" °C");
    vTaskDelay(50);
}

// =====================================================
// Connect light sensor
// =====================================================

bool connectLightSensor(const NimBLEAdvertisedDevice *device)
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

            if (characteristic->canNotify())
            {
                Serial.println(
                    "    Subscribing to notification...");

                if (characteristic->subscribe(true, tempNotifyCallback))
                {
                    Serial.println("    Notification subscribed.");

                    // Save the characteristic.
                    // Later, once we identify which one
                    // is the temperature data characteristic,
                    // we'll keep only that one.
                    if (tempNotifyChar == nullptr)
                    {
                        tempNotifyChar = characteristic;
                    }
                }
                else
                {
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

    for (int i = 0; i < results.getCount(); i++)
    {
        const NimBLEAdvertisedDevice *device =
            results.getDevice(i);

        // Print device
        Serial.print("Device: ");
        Serial.println(device->toString().c_str());

        // ---------------------------------------------
        // Light sensor
        // ---------------------------------------------

        if (device->haveName() && device->getName() == LIGHT_DEVICE_NAME)
        {
            lightDevice = device;

            Serial.println();
            Serial.println("*** LIGHT SENSOR FOUND ***");

            Serial.print("Address: ");
            Serial.println(device->getAddress().toString().c_str());
        }

        // ---------------------------------------------
        // Temperature sensor
        // ---------------------------------------------

        if (device->haveName() && device->getName() == TEMP_DEVICE_NAME)
        {
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

    if (lightDevice != nullptr)
    {
        lightOK = connectLightSensor(lightDevice);
    }
    else
    {
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

    connectWaterPHSensor();

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