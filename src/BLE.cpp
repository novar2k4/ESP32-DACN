// #include "BLE.h"

// static bool bleStatusTaskStarted = false;

// void BLE1()
// {
//     delay(5000);

//     Serial.println();
//     Serial.println("===== YOLO UNO - THREE BLE SENSORS =====");

//     // =================================================
//     // Initialize BLE
//     // =================================================
//     NimBLEDevice::init("YoloUNO");

//     // =================================================
//     // Scan once for all sensors
//     // =================================================
//     NimBLEScan *scan = NimBLEDevice::getScan();

//     scan->setActiveScan(true);
//     scan->setInterval(100);
//     scan->setWindow(80);

//     Serial.println();
//     Serial.println("========== BLE SCAN ==========");

//     NimBLEScanResults results =
//         scan->getResults(10000, false);

//     Serial.print("Found devices: ");
//     Serial.println(results.getCount());

//     const NimBLEAdvertisedDevice *lightDevice = nullptr;
//     const NimBLEAdvertisedDevice *tempDevice = nullptr;
//     const NimBLEAdvertisedDevice *waterDevice = nullptr;

//     // =================================================
//     // Find all three devices
//     // =================================================
//     for (int i = 0; i < results.getCount(); i++)
//     {
//         const NimBLEAdvertisedDevice *device =
//             results.getDevice(i);

//         Serial.print("Device: ");
//         Serial.println(device->toString().c_str());

//         if (isLightDevice(device))
//         {
//             lightDevice = device;

//             Serial.println("*** LIGHT SENSOR FOUND ***");
//             Serial.print("Address: ");
//             Serial.println(device->getAddress().toString().c_str());
//         }

//         if (isTempDevice(device))
//         {
//             tempDevice = device;

//             Serial.println("*** TEMP/HUMID SENSOR FOUND ***");
//             Serial.print("Address: ");
//             Serial.println(device->getAddress().toString().c_str());
//         }

//         if (isWaterQualityDevice(device))
//         {
//             waterDevice = device;

//             Serial.println("*** WATER QUALITY SENSOR FOUND ***");
//             Serial.print("Address: ");
//             Serial.println(device->getAddress().toString().c_str());
//         }
//     }

//     // =================================================
//     // Connect Light
//     // =================================================
//     bool lightOK = false;

//     if (lightDevice != nullptr)
//         lightOK = connectLightSensor(lightDevice);
//     else
//         Serial.println("[LIGHT] Sensor not found!");

//     // =================================================
//     // Connect Temp/Humid
//     // =================================================
//     bool tempOK = false;

//     if (tempDevice != nullptr)
//         tempOK = connectTempSensor(tempDevice);
//     else
//         Serial.println("[TEMP] Sensor not found!");

//     // =================================================
//     // Connect Water Quality (pH + EC)
//     // =================================================
//     bool waterOK = false;

//     if (waterDevice != nullptr)
//         waterOK = connectWaterQualitySensor(waterDevice);
//     else
//         Serial.println("[WATER] Sensor not found!");

//     // Clear scan results after all connect calls are finished.
//     scan->clearResults();

//     // =================================================
//     // Final status
//     // =================================================
//     Serial.println();
//     Serial.println("===== BLE SENSOR SYSTEM STATUS =====");

//     Serial.print("Light sensor: ");
//     Serial.println(lightOK ? "OK" : "FAILED");

//     Serial.print("Temp/Humid sensor: ");
//     Serial.println(tempOK ? "OK" : "FAILED");

//     Serial.print("Water quality (pH + EC): ");
//     Serial.println(waterOK ? "OK" : "FAILED");

//     Serial.println("====================================");
// }

// void printBLEStatus()
// {
//     Serial.println();
//     Serial.println("========== BLE CONNECTION STATUS ==========");

//     Serial.print("Light Sensor:        ");
//     Serial.println(
//         isLightConnected()
//             ? "CONNECTED"
//             : "DISCONNECTED"
//     );

//     Serial.print("Temperature Sensor:  ");
//     Serial.println(
//         isTemperatureConnected()
//             ? "CONNECTED"
//             : "DISCONNECTED"
//     );

//     Serial.print("Water Quality:       ");
//     Serial.println(
//         isWaterQualityConnected()
//             ? "CONNECTED"
//             : "DISCONNECTED"
//     );

//     Serial.println("===========================================");
// }

// void BLEStatusTask(void *pvParameters)
// {
//     while (1)
//     {
//         printBLEStatus();

//         vTaskDelay(7000);
//     }
// }

// void initBLEStatusTask(){
//     xTaskCreate(BLEStatusTask,"BLE Status Task",4096,NULL,1,NULL);
// }   


////TESTING 


#include "BLE.h"

#include "Light.h"
#include "TempHumid.h"
#include "Water_Quality.h"

#include <NimBLEDevice.h>


// =====================================================
// BLE STATUS TASK
// =====================================================

static bool bleStatusTaskStarted = false;


// =====================================================
// BLE DEVICE NAME
// =====================================================

#define LIGHT_DEVICE_NAME "inno-013-db8d"
#define TEMP_DEVICE_NAME  "inno-004-8645"
#define WATER_DEVICE_NAME "BLE-9909"


// =====================================================
// BLE DEVICE ADDRESS
// =====================================================

#define LIGHT_SENSOR_ADDRESS "0c:8b:95:fa:42:fe"
#define TEMP_SENSOR_ADDRESS  "08:a6:f7:07:79:e6"
#define WATER_SENSOR_ADDRESS "bc:96:51:5b:d9:ec"


// =====================================================
// CONFIG
// =====================================================

#define INITIAL_SCAN_TIME_MS 10000
#define RECONNECT_SCAN_TIME_MS 5000
#define STATUS_INTERVAL_MS 3000


// =====================================================
// BLE1
// Scan + connect all sensors at startup
// =====================================================

void BLE1()
{
    delay(3000);

    Serial.println();
    Serial.println(
        "===== YOLO UNO - THREE BLE SENSORS ====="
    );

    // =================================================
    // Initialize BLE
    // =================================================

    NimBLEDevice::init("YoloUNO");

    // =================================================
    // Scan once for all sensors
    // =================================================

    NimBLEScan *scan =
        NimBLEDevice::getScan();

    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(80);

    Serial.println();
    Serial.println(
        "========== BLE SCAN =========="
    );

    NimBLEScanResults results =
        scan->getResults(
            INITIAL_SCAN_TIME_MS,
            false
        );

    Serial.print("Found devices: ");
    Serial.println(
        results.getCount()
    );


    // =================================================
    // Device pointers
    // =================================================

    const NimBLEAdvertisedDevice *lightDevice =
        nullptr;

    const NimBLEAdvertisedDevice *tempDevice =
        nullptr;

    const NimBLEAdvertisedDevice *waterDevice =
        nullptr;


    // =================================================
    // Find all three devices
    // =================================================

    for (int i = 0;
         i < results.getCount();
         i++)
    {
        const NimBLEAdvertisedDevice *device =
            results.getDevice(i);

        if (device == nullptr)
            continue;

        Serial.print("Device: ");
        Serial.println(
            device->toString().c_str()
        );


        // -------------------------------------------------
        // Light
        // -------------------------------------------------

        if (isLightDevice(device))
        {
            lightDevice = device;

            Serial.println(
                "*** LIGHT SENSOR FOUND ***"
            );

            Serial.print("Address: ");

            Serial.println(
                device->getAddress()
                    .toString()
                    .c_str()
            );
        }


        // -------------------------------------------------
        // Temperature / Humidity
        // -------------------------------------------------

        if (isTempDevice(device))
        {
            tempDevice = device;

            Serial.println(
                "*** TEMP/HUMID SENSOR FOUND ***"
            );

            Serial.print("Address: ");

            Serial.println(
                device->getAddress()
                    .toString()
                    .c_str()
            );
        }


        // -------------------------------------------------
        // Water Quality
        // -------------------------------------------------

        if (isWaterQualityDevice(device))
        {
            waterDevice = device;

            Serial.println(
                "*** WATER QUALITY SENSOR FOUND ***"
            );

            Serial.print("Address: ");

            Serial.println(
                device->getAddress()
                    .toString()
                    .c_str()
            );
        }
    }


    // =================================================
    // Connect Light
    // =================================================

    bool lightOK = false;

    if (lightDevice != nullptr)
    {
        lightOK =
            connectLightSensor(
                lightDevice
            );
    }
    else
    {
        Serial.println(
            "[LIGHT] Sensor not found!"
        );
    }


    // =================================================
    // Connect Temp/Humid
    // =================================================

    bool tempOK = false;

    if (tempDevice != nullptr)
    {
        tempOK =
            connectTempSensor(
                tempDevice
            );
    }
    else
    {
        Serial.println(
            "[TEMP] Sensor not found!"
        );
    }


    // =================================================
    // Connect Water Quality
    // pH + EC
    // =================================================

    bool waterOK = false;

    if (waterDevice != nullptr)
    {
        waterOK =
            connectWaterQualitySensor(
                waterDevice
            );
    }
    else
    {
        Serial.println(
            "[WATER] Sensor not found!"
        );
    }


    // =================================================
    // Clear scan results
    // =================================================

    scan->clearResults();


    // =================================================
    // Initial status
    // =================================================

    Serial.println();
    Serial.println(
        "===== BLE SENSOR SYSTEM STATUS ====="
    );

    Serial.print(
        "Light sensor: "
    );

    Serial.println(
        lightOK ? "OK" : "FAILED"
    );


    Serial.print(
        "Temp/Humid sensor: "
    );

    Serial.println(
        tempOK ? "OK" : "FAILED"
    );


    Serial.print(
        "Water quality (pH + EC): "
    );

    Serial.println(
        waterOK ? "OK" : "FAILED"
    );

    Serial.println(
        "===================================="
    );
}


// =====================================================
// PRINT BLE STATUS
// =====================================================

void printBLEStatus()
{
    Serial.println();
    Serial.println(
        "========== BLE CONNECTION STATUS =========="
    );


    // =================================================
    // Light
    // =================================================

    Serial.print(
        "Light Sensor:        "
    );

    Serial.println(
        isLightConnected()
            ? "CONNECTED"
            : "DISCONNECTED"
    );


    // =================================================
    // Temperature
    // =================================================

    Serial.print(
        "Temperature Sensor:  "
    );

    Serial.println(
        isTemperatureConnected()
            ? "CONNECTED"
            : "DISCONNECTED"
    );


    // =================================================
    // Water Quality
    // =================================================

    Serial.print(
        "Water Quality:       "
    );

    Serial.println(
        isWaterQualityConnected()
            ? "CONNECTED"
            : "DISCONNECTED"
    );


    Serial.println(
        "==========================================="
    );
}


// =====================================================
// RECONNECT MISSING DEVICES
// =====================================================

void reconnectBLEDevices()
{
    // =================================================
    // Check which sensor is disconnected
    // =================================================

    bool lightMissing =
        !isLightConnected();

    bool tempMissing =
        !isTemperatureConnected();

    bool waterMissing =
        !isWaterQualityConnected();


    // =================================================
    // Nothing to reconnect
    // =================================================

    if (!lightMissing &&
        !tempMissing &&
        !waterMissing)
    {
        return;
    }


    Serial.println();
    Serial.println(
        "========== BLE RECONNECT =========="
    );


    if (lightMissing)
    {
        Serial.println(
            "[BLE] Light sensor disconnected."
        );
    }


    if (tempMissing)
    {
        Serial.println(
            "[BLE] Temperature sensor disconnected."
        );
    }


    if (waterMissing)
    {
        Serial.println(
            "[BLE] Water Quality sensor disconnected."
        );
        water_state = true;
    }


    // =================================================
    // Start scan
    // =================================================

    NimBLEScan *scan =
        NimBLEDevice::getScan();

    if (scan == nullptr)
    {
        Serial.println(
            "[BLE] Scan object is NULL."
        );

        return;
    }


    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(80);


    Serial.println(
        "[BLE] Scanning for missing devices..."
    );


    NimBLEScanResults results =
        scan->getResults(
            RECONNECT_SCAN_TIME_MS,
            false
        );


    Serial.print(
        "[BLE] Found devices: "
    );

    Serial.println(
        results.getCount()
    );


    // =================================================
    // Search devices
    // =================================================

    for (int i = 0;
         i < results.getCount();
         i++)
    {
        const NimBLEAdvertisedDevice *device =
            results.getDevice(i);


        if (device == nullptr)
            continue;


        // =================================================
        // LIGHT
        // =================================================

        if (lightMissing &&
            isLightDevice(device))
        {
            Serial.println(
                "[BLE] Light sensor found again."
            );

            Serial.print(
                "[BLE] Address: "
            );

            Serial.println(
                device->getAddress()
                    .toString()
                    .c_str()
            );


            Serial.println(
                "[BLE] Reconnecting Light..."
            );


            if (connectLightSensor(device))
            {
                Serial.println(
                    "[BLE] Light reconnected!"
                );
            }
            else
            {
                Serial.println(
                    "[BLE] Light reconnect FAILED."
                );
            }
        }


        // =================================================
        // TEMPERATURE / HUMIDITY
        // =================================================

        if (tempMissing &&
            isTempDevice(device))
        {
            Serial.println(
                "[BLE] Temperature sensor found again."
            );

            Serial.print(
                "[BLE] Address: "
            );

            Serial.println(
                device->getAddress()
                    .toString()
                    .c_str()
            );


            Serial.println(
                "[BLE] Reconnecting Temperature..."
            );


            if (connectTempSensor(device))
            {
                Serial.println(
                    "[BLE] Temperature reconnected!"
                );
            }
            else
            {
                Serial.println(
                    "[BLE] Temperature reconnect FAILED."
                );
            }
        }


        // =================================================
        // WATER QUALITY
        // pH + EC
        // =================================================

        if (waterMissing &&
            isWaterQualityDevice(device))
        {
            water_state = false;
            Serial.println(
                "[BLE] Water Quality sensor found again."
            );

            Serial.print(
                "[BLE] Address: "
            );

            Serial.println(
                device->getAddress()
                    .toString()
                    .c_str()
            );


            Serial.println(
                "[BLE] Reconnecting Water Quality..."
            );


            if (connectWaterQualitySensor(device))
            {
                Serial.println(
                    "[BLE] Water Quality reconnected!"
                );
            }
            else
            {
                Serial.println(
                    "[BLE] Water Quality reconnect FAILED."
                );
            }
        }
    }


    // =================================================
    // Clear scan results
    // =================================================

    scan->clearResults();


    Serial.println(
        "========== BLE RECONNECT DONE =========="
    );
}


// =====================================================
// BLE STATUS + RECONNECT TASK
// =====================================================

void BLEStatusTask(void *pvParameters)
{
    Serial.println(
        "[BLE] Status/Reconnect task started."
    );


    while (1)
    {
        // =================================================
        // Show current status
        // =================================================

        printBLEStatus();


        // =================================================
        // Check and reconnect missing devices
        // =================================================

        reconnectBLEDevices();


        // =================================================
        // Wait
        // =================================================

        vTaskDelay(
            pdMS_TO_TICKS(
                STATUS_INTERVAL_MS
            )
        );
    }
}


// =====================================================
// START BLE STATUS TASK
// =====================================================

void initBLEStatusTask()
{
    // Prevent duplicate task creation
    if (bleStatusTaskStarted)
    {
        Serial.println(
            "[BLE] Status task already started."
        );

        return;
    }


    Serial.println(
        "[BLE] Starting BLE status task..."
    );


    BaseType_t result =
        xTaskCreate(
            BLEStatusTask,
            "BLE Status Task",
            4096,
            NULL,
            1,
            NULL
        );


    if (result == pdPASS)
    {
        bleStatusTaskStarted = true;

        Serial.println(
            "[BLE] Status task started successfully."
        );
    }
    else
    {
        Serial.println(
            "[BLE] Failed to create status task!"
        );
    }
}