#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

float altitude = 100.0; // Altitud de ejemplo
float latitude = 37.7749; // Latitud de ejemplo (San Francisco)
float longitude = -122.4194; // Longitud de ejemplo (San Francisco)

BLEServer *pServer = nullptr;
BLECharacteristic *pAltitudeCharacteristic = nullptr;
BLECharacteristic *pLatitudeCharacteristic = nullptr;
BLECharacteristic *pLongitudeCharacteristic = nullptr;

void setup() {
    BLEDevice::init("BLE_GRUPO_2");
    pServer = BLEDevice::createServer();

    // Crear un servicio para las coordenadas
    BLEService *pCoordinatesService = pServer->createService("1234"); // UUID personalizado para el servicio de coordenadas

    // Característica para la altitud
    pAltitudeCharacteristic = pCoordinatesService->createCharacteristic(
                                                        "5678",
                                                        BLECharacteristic::PROPERTY_READ |
                                                        BLECharacteristic::PROPERTY_NOTIFY
                                                    );
    // Característica para la latitud
    pLatitudeCharacteristic = pCoordinatesService->createCharacteristic(
                                                        "9012",
                                                        BLECharacteristic::PROPERTY_READ |
                                                        BLECharacteristic::PROPERTY_NOTIFY
                                                    );
    // Característica para la longitud
    pLongitudeCharacteristic = pCoordinatesService->createCharacteristic(
                                                        "3456",
                                                        BLECharacteristic::PROPERTY_READ |
                                                        BLECharacteristic::PROPERTY_NOTIFY
                                                    );

    pAltitudeCharacteristic->setValue(altitude);
    pLatitudeCharacteristic->setValue(latitude);
    pLongitudeCharacteristic->setValue(longitude);

    pCoordinatesService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pCoordinatesService->getUUID());
    pAdvertising->start();
}

void loop() {
    // Actualizar las coordenadas
    print("Actualizando coordenadas")
    pAltitudeCharacteristic->setValue(altitude);
    pLatitudeCharacteristic->setValue(latitude);
    pLongitudeCharacteristic->setValue(longitude);
    pAltitudeCharacteristic->notify();
    pLatitudeCharacteristic->notify();
    pLongitudeCharacteristic->notify();
    delay(1000);
}
