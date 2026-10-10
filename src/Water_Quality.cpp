#include "Water_Quality.h"

static const char *PH_SERVICE_UUID = "FF01";
static const char *PH_CHARACTERISTIC_UUID = "FF02";


static NimBLEClient *waterQualityClient = nullptr;
static NimBLERemoteCharacteristic *waterQualityNotifyChar = nullptr;

bool isWaterQualityConnected()
{
    return (
        waterQualityClient != nullptr &&
        waterQualityClient->isConnected());
}

bool isWaterQualityDevice(const NimBLEAdvertisedDevice *device)
{
    if (device == nullptr || !device->haveName())
        return false;

    String name = device->getName().c_str();
    String address = device->getAddress().toString().c_str();

    return name == PH_DEVICE_NAME &&
           address.equalsIgnoreCase(PH_SENSOR_ADDRESS);
}

// DECODE ///

// static void deCode(uint8_t *pValue, uint8_t len)
// {
//     uint8_t tmp;
//     uint8_t hibit;
//     uint8_t lobit;
//     uint8_t hibit1;
//     uint8_t lobit1;

//     for (int i = len - 1; i > 0; i--)
//     {
//         tmp = pValue[i];

//         hibit1 = (tmp & 0x55) << 1;
//         lobit1 = (tmp & 0xAA) >> 1;

//         tmp = pValue[i - 1];

//         hibit = (tmp & 0x55) << 1;
//         lobit = (tmp & 0xAA) >> 1;

//         pValue[i] = ~(hibit1 | lobit);
//         pValue[i - 1] = ~(hibit | lobit1);
//     }
// }

/// TESTING

static void deCode(
    uint8_t *pValue,
    uint8_t len)
{
    uint8_t tmp;
    uint8_t hibit;
    uint8_t lobit;
    uint8_t hibit1;
    uint8_t lobit1;

    for (int i = len - 1; i > 0; i--)
    {
        tmp = pValue[i];

        hibit1 =
            (tmp & 0x55) << 1;

        lobit1 =
            (tmp & 0xAA) >> 1;

        tmp = pValue[i - 1];

        hibit =
            (tmp & 0x55) << 1;

        lobit =
            (tmp & 0xAA) >> 1;

        pValue[i] =
            ~(hibit1 | lobit);

        pValue[i - 1] =
            ~(hibit | lobit1);
    }
}

static bool decodeWaterQualityPacket(
    const uint8_t *data,
    size_t length)
{
    if (length < 22 || length > 32)
    {
        Serial.print("[WATER] Invalid packet length: ");
        Serial.println(length);
        return false;
    }

    uint8_t packet[32];
    memcpy(packet, data, length);

    deCode(packet, static_cast<uint8_t>(length));

    // pH: byte 3-4, /100
    uint16_t rawPH =
        (static_cast<uint16_t>(packet[3]) << 8) |
        packet[4];

    global_ph = rawPH / 100.0f;

    // EC: byte 5-6, uS/cm
    uint16_t rawEC =
        (static_cast<uint16_t>(packet[5]) << 8) |
        packet[6];

    global_ec = static_cast<float>(rawEC);

    return true;
}

static void printWaterQualityValues()
{
    Serial.print("[WATER] pH = ");
    Serial.print(global_ph, 2);

    Serial.print(" | EC = ");
    Serial.print(global_ec, 0);
    Serial.println(" uS/cm");
}
//// CALLBACK ////

// static void waterQualityNotifyCallback(
//     NimBLERemoteCharacteristic *characteristic,
//     uint8_t *data,
//     size_t length,
//     bool isNotify)
// {
//     if (decodeWaterQualityPacket(data, length))
//         printWaterQualityValues();
// }

//// TESTING ////

static void waterQualityNotifyCallback(
    NimBLERemoteCharacteristic *characteristic,
    uint8_t *data,
    size_t length,
    bool isNotify)
{
    if (length < 22 || length > 32)
    {
        Serial.print(
            "[WATER] Invalid packet length: ");

        Serial.println(length);

        return;
    }

    uint8_t packet[32];

    memcpy(
        packet,
        data,
        length);

    // Decode BLE-9909
    deCode(
        packet,
        static_cast<uint8_t>(length));

    // =================================================
    // pH
    // =================================================

    uint16_t rawPH =
        ((uint16_t)packet[3] << 8) |
        packet[4];

    global_ph =
        rawPH / 100.0f;

    // =================================================
    // EC
    // =================================================

    uint16_t rawEC =
        ((uint16_t)packet[5] << 8) |
        packet[6];

    global_ec =
        (float)rawEC;

    // =================================================
    // Debug
    // =================================================

    Serial.print("[WATER] pH = ");
    Serial.print(global_ph, 2);

    Serial.print(" | EC = ");
    Serial.print(global_ec, 0);

    Serial.println(" uS/cm");
}

// bool connectWaterQualitySensor(
//     const NimBLEAdvertisedDevice *device)
// {
//     if (device == nullptr)
//     {
//         Serial.println("[WATER] Device is null.");
//         return false;
//     }

//     Serial.println();
//     Serial.println("========== CONNECTING WATER QUALITY SENSOR ==========");
//     Serial.print("[WATER] Name: ");
//     Serial.println(device->getName().c_str());
//     Serial.print("[WATER] Address: ");
//     Serial.println(device->getAddress().toString().c_str());

//     waterQualityClient = NimBLEDevice::createClient();

//     if (waterQualityClient == nullptr)
//     {
//         Serial.println("[WATER] Failed to create client.");
//         return false;
//     }

//     if (!waterQualityClient->connect(device))
//     {
//         Serial.println("[WATER] Connection FAILED.");
//         NimBLEDevice::deleteClient(waterQualityClient);
//         waterQualityClient = nullptr;
//         return false;
//     }

//     Serial.println("[WATER] Connected.");

//     NimBLERemoteService *service =
//         waterQualityClient->getService(PH_SERVICE_UUID);

//     if (service == nullptr)
//     {
//         Serial.println("[WATER] FF01 service not found.");
//         waterQualityClient->disconnect();
//         return false;
//     }

//     waterQualityNotifyChar =
//         service->getCharacteristic(PH_CHARACTERISTIC_UUID);

//     if (waterQualityNotifyChar == nullptr)
//     {
//         Serial.println("[WATER] FF02 characteristic not found.");
//         waterQualityClient->disconnect();
//         return false;
//     }

//     Serial.println("[WATER] FF02 found.");

//     if (waterQualityNotifyChar->canNotify())
//     {
//         bool ok = waterQualityNotifyChar->subscribe(
//             true,
//             waterQualityNotifyCallback);

//         Serial.print("[WATER] Subscribe: ");
//         Serial.println(ok ? "OK" : "FAILED");

//         if (!ok)
//         {
//             waterQualityClient->disconnect();
//             return false;
//         }
//     }
//     else
//     {
//         Serial.println("[WATER] FF02 does not support NOTIFY.");
//         return false;
//     }

//     // Read current FF02 value once.
//     if (waterQualityNotifyChar->canRead())
//     {
//         NimBLEAttValue value =
//             waterQualityNotifyChar->readValue();

//         if (decodeWaterQualityPacket(
//                 value.data(),
//                 value.size()))
//         {
//             printWaterQualityValues();
//         }
//     }

//     Serial.println("========== WATER QUALITY SENSOR READY ==========");
//     return true;
// }

/// TEST

bool connectWaterQualitySensor(
    const NimBLEAdvertisedDevice *device)
{
    if (waterQualityClient != nullptr)
    {
        if (waterQualityClient->isConnected())
            return true;

        NimBLEDevice::deleteClient(
            waterQualityClient);

        waterQualityClient = nullptr;
        waterQualityNotifyChar = nullptr;
    }
    // =================================================
    // Check device
    // =================================================

    if (device == nullptr)
    {
        Serial.println(
            "[WATER] Device is null.");

        return false;
    }

    // =================================================
    // Delete old client if disconnected
    // =================================================

    if (waterQualityClient != nullptr)
    {
        if (waterQualityClient->isConnected())
        {
            Serial.println(
                "[WATER] Already connected.");

            return true;
        }

        Serial.println(
            "[WATER] Deleting old client...");

        NimBLEDevice::deleteClient(
            waterQualityClient);

        waterQualityClient = nullptr;
        waterQualityNotifyChar = nullptr;
    }

    // =================================================
    // Debug
    // =================================================

    Serial.println();
    Serial.println(
        "========== CONNECTING WATER QUALITY SENSOR ==========");

    Serial.print("[WATER] Name: ");
    Serial.println(
        device->getName().c_str());

    Serial.print("[WATER] Address: ");
    Serial.println(
        device->getAddress()
            .toString()
            .c_str());

    Serial.print("[WATER] RSSI: ");
    Serial.println(
        device->getRSSI());

    // =================================================
    // Create client
    // =================================================

    waterQualityClient =
        NimBLEDevice::createClient();

    if (waterQualityClient == nullptr)
    {
        Serial.println(
            "[WATER] Failed to create BLE client.");

        return false;
    }

    // =================================================
    // Connect
    // =================================================

    Serial.println(
        "[WATER] Connecting...");

    if (!waterQualityClient->connect(device))
    {
        Serial.println(
            "[WATER] Connection FAILED.");

        NimBLEDevice::deleteClient(
            waterQualityClient);

        waterQualityClient = nullptr;

        return false;
    }

    Serial.println(
        "[WATER] Connected!");

    // =================================================
    // FF01 service
    // =================================================

    NimBLERemoteService *waterService =
        waterQualityClient->getService(
            PH_SERVICE_UUID);

    if (waterService == nullptr)
    {
        Serial.println(
            "[WATER] FF01 service NOT found.");

        waterQualityClient->disconnect();

        NimBLEDevice::deleteClient(
            waterQualityClient);

        waterQualityClient = nullptr;

        return false;
    }

    Serial.println(
        "[WATER] FF01 service found.");

    // =================================================
    // FF02 characteristic
    // =================================================

    waterQualityNotifyChar =
        waterService->getCharacteristic(
            PH_CHARACTERISTIC_UUID);

    if (waterQualityNotifyChar == nullptr)
    {
        Serial.println(
            "[WATER] FF02 characteristic NOT found.");

        waterQualityClient->disconnect();

        NimBLEDevice::deleteClient(
            waterQualityClient);

        waterQualityClient = nullptr;

        return false;
    }

    Serial.println(
        "[waterQuality] FF02 characteristic found.");

    // =================================================
    // Check notify
    // =================================================

    if (!waterQualityNotifyChar->canNotify())
    {
        Serial.println(
            "[WATER] FF02 does NOT support NOTIFY.");

        waterQualityClient->disconnect();

        NimBLEDevice::deleteClient(
            waterQualityClient);

        waterQualityClient = nullptr;
        waterQualityNotifyChar = nullptr;

        return false;
    }

    // =================================================
    // Subscribe
    // =================================================

    Serial.println(
        "[WATER] Subscribing to FF02...");

    if (!waterQualityNotifyChar->subscribe(
            true,
            waterQualityNotifyCallback))
    {
        Serial.println(
            "[WATER] Subscribe FAILED.");

        waterQualityClient->disconnect();

        NimBLEDevice::deleteClient(
            waterQualityClient);

        waterQualityClient = nullptr;
        waterQualityNotifyChar = nullptr;

        return false;
    }

    Serial.println(
        "[WATER] Notification subscribed.");

    // =================================================
    // READ initial value
    // =================================================

    if (waterQualityNotifyChar->canRead())
    {
        Serial.println(
            "[WATER] Reading initial FF02 value...");

        NimBLEAttValue value =
            waterQualityNotifyChar->readValue();

        Serial.print(
            "[WATER] Initial packet: ");

        Serial.print(
            value.size());

        Serial.print(
            " bytes: ");

        const uint8_t *data =
            value.data();

        for (size_t i = 0;
             i < value.size();
             i++)
        {
            if (data[i] < 0x10)
                Serial.print("0");

            Serial.print(
                data[i],
                HEX);

            Serial.print(" ");
        }

        Serial.println();
    }

    Serial.println(
        "========== WATER QUALITY SENSOR READY ==========");

    return true;
}