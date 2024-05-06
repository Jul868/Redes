#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic *pLatitudeCharacteristic = NULL;
BLECharacteristic *pLongitudeCharacteristic = NULL;
BLECharacteristic *pAltitudeCharacteristic = NULL;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define LATITUDE_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define LONGITUDE_UUID "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"
#define ALTITUDE_UUID "5ec11f33-dc1b-4e31-b9fb-2f9a274c9cdf"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Client Connected");
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Client Disconnected");
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32 GPS Server");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(SERVICE_UUID);

  pLatitudeCharacteristic = pService->createCharacteristic(
                               LATITUDE_UUID,
                               BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                             );
  pLatitudeCharacteristic->addDescriptor(new BLE2902());
  pLatitudeCharacteristic->setValue("34.0522");

  pLongitudeCharacteristic = pService->createCharacteristic(
                                LONGITUDE_UUID,
                                BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                              );
  pLongitudeCharacteristic->addDescriptor(new BLE2902());
  pLongitudeCharacteristic->setValue("-118.2437");

  pAltitudeCharacteristic = pService->createCharacteristic(
                               ALTITUDE_UUID,
                               BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                             );
  pAltitudeCharacteristic->addDescriptor(new BLE2902());
  pAltitudeCharacteristic->setValue("90m");

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Service Advertising Started");
}

void loop() {
  // This is left empty intentionally.
}
