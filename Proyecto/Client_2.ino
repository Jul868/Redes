#include "BLEDevice.h"

// Define UUIDs for service and characteristics
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID latitudeUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");
static BLEUUID longitudeUUID("1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e");
static BLEUUID altitudeUUID("5ec11f33-dc1b-4e31-b9fb-2f9a274c9cdf");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLEAdvertisedDevice* myDevice;

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    connected = true;
    Serial.println("Connected to the server");
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("Disconnected from the server");
    doScan = true;  // Ensure we start scanning again
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(myDevice);
  Serial.println(" - Connected to server");

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }

  // Retrieve each characteristic and set notification callback
  BLERemoteCharacteristic* pRemoteCharacteristic;
  pRemoteCharacteristic = pRemoteService->getCharacteristic(latitudeUUID);
  if (pRemoteCharacteristic != nullptr && pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  pRemoteCharacteristic = pRemoteService->getCharacteristic(longitudeUUID);
  if (pRemoteCharacteristic != nullptr && pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  pRemoteCharacteristic = pRemoteService->getCharacteristic(altitudeUUID);
  if (pRemoteCharacteristic != nullptr && pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  // Print received data based on UUID
  if (pBLERemoteCharacteristic->getUUID().equals(latitudeUUID)) {
    Serial.print("Received Latitude: ");
  } else if (pBLERemoteCharacteristic->getUUID().equals(longitudeUUID)) {
    Serial.print("Received Longitude: ");
  } else if (pBLERemoteCharacteristic->getUUID().equals(altitudeUUID)) {
    Serial.print("Received Altitude: ");
  } else {
    Serial.print("Received Unknown Characteristic (");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print("): ");
  }
  Serial.println((char*)pData);
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  if (doScan) {
    BLEDevice::getScan()->start(0); // 0 = scan forever, this is just an example
    doScan = false;
  }
}
