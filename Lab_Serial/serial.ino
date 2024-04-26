#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
 
BLEClient* pClient = nullptr;
 
#define SERVICE_UUID        "180F"
#define CHARACTERISTIC_UUID "2A19"  // UUID estándar de la característica de nivel de batería
 
 
void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
 
  // Crear cliente BLE
  pClient = BLEDevice::createClient();
  Serial.println("Cliente BLE iniciado");
 
  // Iniciar escaneo BLE
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  Serial.println("Escaneando...");
  BLEScanResults scanResults = pBLEScan->start(15);  // Escanear durante 5 segundos
 
  // Conectar con el servidor que tiene el servicio de batería
  bool foundService = false;
  for (int i = 0; i < scanResults.getCount(); i++) {
    Serial.println("# de encontrados: ");
    Serial.println(scanResults.getCount());
    BLEAdvertisedDevice advertisedDevice = scanResults.getDevice(i);
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      Serial.println("Dispositivo con servicio de batería encontrado");
      foundService = true;
      pClient->connect(&advertisedDevice);
      break;
    }
  }
 
  if (!foundService) {
    Serial.println("No se encontró el dispositivo con el servicio de batería");
    return;
  }
}
 
void loop() {
  if (pClient->isConnected()) {
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService == nullptr) {
      Serial.println("No se pudo encontrar el servicio de batería");
      return;
    }
    BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
    if (pRemoteCharacteristic != nullptr) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("Nivel de Batería: ");
      Serial.println((int)value[0]);
    }
  }
  delay(60000); // Espera un minuto antes de leer de nuevo
}