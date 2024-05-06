#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Initialize all pointers
BLEServer* pServer = nullptr;                        
BLECharacteristic* pLatitude = nullptr;      
BLECharacteristic* pAltitude = nullptr;
BLECharacteristic* pLongitude = nullptr;


// Some variables to keep track on device connected
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Variable that will continuously be increased and written to the client
uint32_t value = 0;

#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_2 "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"
#define CHARACTERISTIC_UUID_3 "12345678-1234-1234-1234-123456789abc"

#define Latitude 37.7749
#define Altitude 122.4194
#define Longitude -122.4194

// Callback function that is called whenever a client is connected or disconnected
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("BLE_GRUPO_2");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic 
  pLatitude = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_1,
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_READ
                    );                   

  pAltitude = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_2,
                      BLECharacteristic::PROPERTY_READ   |                      
                      BLECharacteristic::PROPERTY_NOTIFY
                    ); 
  pLongitude = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_3,
                      BLECharacteristic::PROPERTY_READ   |                      
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    // notify changed value
    char LatBuffer[32];
    char AltBuffer[32];
    char LongBuffer[32];

    sprintf(LatBuffer, "%f", Latitude);
    sprintf(AltBuffer, "%f", Altitude);
    sprintf(LongBuffer, "%f", Longitude);

    if (deviceConnected) {
      // pCharacteristic_1 is an integer that is increased with every second
      // in the code below we send the value over to the client and increase the integer counter
      pLatitude->setValue((uint8_t*)LatBuffer, strlen(LatBuffer));
      pLatitude->notify();
      Serial.println("Characteristic 1 (notify): " + String(LatBuffer));

      pAltitude->setValue((uint8_t*)AltBuffer, strlen(AltBuffer));
      pAltitude->notify();
      Serial.println("Characteristic 2 (notify): " + String(AltBuffer));

      pLongitude->setValue((uint8_t*)LongBuffer, strlen(LongBuffer));
      pLongitude->notify();
      Serial.println("Characteristic 3 (notify): " + String(LongBuffer));
      delay(1000);
    }
    // The code below keeps the connection status uptodate:
    // Disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // Connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}