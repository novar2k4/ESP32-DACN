#include "BLE.h"

static bool bleStatusTaskStarted = false;

void BLE1()
{
    delay(5000);

    Serial.println();
    Serial.println("===== YOLO UNO - THREE BLE SENSORS =====");

    // =================================================
    // Initialize BLE
    // =================================================
    NimBLEDevice::init("YoloUNO");

    // =================================================
    // Scan once for all sensors
    // =================================================
    NimBLEScan *scan = NimBLEDevice::getScan();

    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(80);

    Serial.println();
    Serial.println("========== BLE SCAN ==========");

    NimBLEScanResults results =
        scan->getResults(10000, false);

    Serial.print("Found devices: ");
    Serial.println(results.getCount());

    const NimBLEAdvertisedDevice *lightDevice = nullptr;
    const NimBLEAdvertisedDevice *tempDevice = nullptr;
    const NimBLEAdvertisedDevice *waterDevice = nullptr;

    // =================================================
    // Find all three devices
    // =================================================
    for (int i = 0; i < results.getCount(); i++)
    {
        const NimBLEAdvertisedDevice *device =
            results.getDevice(i);

        Serial.print("Device: ");
        Serial.println(device->toString().c_str());

        if (isLightDevice(device))
        {
            lightDevice = device;

            Serial.println("*** LIGHT SENSOR FOUND ***");
            Serial.print("Address: ");
            Serial.println(device->getAddress().toString().c_str());
        }

        if (isTempDevice(device))
        {
            tempDevice = device;

            Serial.println("*** TEMP/HUMID SENSOR FOUND ***");
            Serial.print("Address: ");
            Serial.println(device->getAddress().toString().c_str());
        }

        if (isWaterQualityDevice(device))
        {
            waterDevice = device;

            Serial.println("*** WATER QUALITY SENSOR FOUND ***");
            Serial.print("Address: ");
            Serial.println(device->getAddress().toString().c_str());
        }
    }

    // =================================================
    // Connect Light
    // =================================================
    bool lightOK = false;

    if (lightDevice != nullptr)
        lightOK = connectLightSensor(lightDevice);
    else
        Serial.println("[LIGHT] Sensor not found!");

    // =================================================
    // Connect Temp/Humid
    // =================================================
    bool tempOK = false;

    if (tempDevice != nullptr)
        tempOK = connectTempSensor(tempDevice);
    else
        Serial.println("[TEMP] Sensor not found!");

    // =================================================
    // Connect Water Quality (pH + EC)
    // =================================================
    bool waterOK = false;

    if (waterDevice != nullptr)
        waterOK = connectWaterQualitySensor(waterDevice);
    else
        Serial.println("[WATER] Sensor not found!");

    // Clear scan results after all connect calls are finished.
    scan->clearResults();

    // =================================================
    // Final status
    // =================================================
    Serial.println();
    Serial.println("===== BLE SENSOR SYSTEM STATUS =====");

    Serial.print("Light sensor: ");
    Serial.println(lightOK ? "OK" : "FAILED");

    Serial.print("Temp/Humid sensor: ");
    Serial.println(tempOK ? "OK" : "FAILED");

    Serial.print("Water quality (pH + EC): ");
    Serial.println(waterOK ? "OK" : "FAILED");

    Serial.println("====================================");
}

void printBLEStatus()
{
    Serial.println();
    Serial.println("========== BLE CONNECTION STATUS ==========");

    Serial.print("Light Sensor:        ");
    Serial.println(
        isLightConnected()
            ? "CONNECTED"
            : "DISCONNECTED"
    );

    Serial.print("Temperature Sensor:  ");
    Serial.println(
        isTemperatureConnected()
            ? "CONNECTED"
            : "DISCONNECTED"
    );

    Serial.print("Water Quality:       ");
    Serial.println(
        isWaterQualityConnected()
            ? "CONNECTED"
            : "DISCONNECTED"
    );

    Serial.println("===========================================");
}

void BLEStatusTask(void *pvParameters)
{
    while (1)
    {
        printBLEStatus();

        vTaskDelay(7000);
    }
}

void initBLEStatusTask(){
    xTaskCreate(BLEStatusTask,"BLE Status Task",4096,NULL,1,NULL);
}   
