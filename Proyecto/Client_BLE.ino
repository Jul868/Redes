#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

BLEClient* pClient = nullptr;

#define SERVICE_UUID        "1234"
#define ALTITUDE_UUID       "5678"
#define LATITUDE_UUID       "9012"
#define LONGITUDE_UUID      "3456"

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  pClient = BLEDevice::createClient();
  
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  BLEScanResults scanResults = pBLEScan->start(5);

  bool foundService = false;
  for (int i = 0; i < scanResults.getCount(); i++) {
    BLEAdvertisedDevice advertisedDevice = scanResults.getDevice(i);
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      foundService = true;
      pClient->connect(&advertisedDevice);
      break;
    }
  }
  if (!foundService) {
    Serial.println("No se encontró el dispositivo con el servicio de coordenadas");
    return;
  }
}

void loop() {
    if (pClient->isConnected()) {
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService == nullptr) {
        Serial.println("No se pudo encontrar el servicio de coordenadas");
        return;
    }
    BLERemoteCharacteristic* pAltitudeCharacteristic = pRemoteService->getCharacteristic(ALTITUDE_UUID);
    BLERemoteCharacteristic* pLatitudeCharacteristic = pRemoteService->getCharacteristic(LATITUDE_UUID);
    BLERemoteCharacteristic* pLongitudeCharacteristic = pRemoteService->getCharacteristic(LONGITUDE_UUID);

    if (pAltitudeCharacteristic != nullptr && pLatitudeCharacteristic != nullptr && pLongitudeCharacteristic != nullptr) {
        std::string altitudeValue = pAltitudeCharacteristic->readValue();
        std::string latitudeValue = pLatitudeCharacteristic->readValue();
        std::string longitudeValue = pLongitudeCharacteristic->readValue();
        
        float altitude = *(float*)(altitudeValue.data());
        float latitude = *(float*)(latitudeValue.data());
        float longitude = *(float*)(longitudeValue.data());

        Serial.print("Altitude: ");
        Serial.println(altitude);
        Serial.print("Latitude: ");
        Serial.println(latitude);
        Serial.print("Longitude: ");
        Serial.println(longitude);
    }
  }
  delay(60000); // Espera un minuto antes de leer de nuevo
}
