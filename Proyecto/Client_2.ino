#include <Arduino.h>
#include "BLEDevice.h"
#include <wire.h>
//#include "BLEScan.h"

//Pines pantalla LCD1602 I2C

#define LCD_ADDRESS 0x27 // Asegúrate de usar la dirección correcta
#define BACKLIGHT 0x08
#define En 0x04  // Enable bit
#define Rs 0x01  // Register select bit

// Define UUIDs:
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID    charUUID_1("beb5483e-36e1-4688-b7f5-ea07361b26a8");
static BLEUUID    charUUID_2("1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e");
static BLEUUID    charUUID_3("1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87f");

// Some variables to keep track on device connected
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;



// Define pointer for the BLE connection
static BLEAdvertisedDevice* myDevice;
BLERemoteCharacteristic* pRemoteChar_1;
BLERemoteCharacteristic* pRemoteChar_2;
BLERemoteCharacteristic* pRemoteChar_3;

// Callback function that is called whenever a client is connected or disconnected
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

// Function that is run whenever the server is connected
bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());
  
  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  connected = true;
  pRemoteChar_1 = pRemoteService->getCharacteristic(charUUID_1);
  pRemoteChar_2 = pRemoteService->getCharacteristic(charUUID_2);
  pRemoteChar_3 = pRemoteService->getCharacteristic(charUUID_3);
  if(connectCharacteristic(pRemoteService, pRemoteChar_1) == false)
    connected = false;
  else if(connectCharacteristic(pRemoteService, pRemoteChar_2) == false)
    connected = false;

  else if(connectCharacteristic(pRemoteService, pRemoteChar_3) == false)
    connected = false;  

  if(connected == false) {
    pClient-> disconnect();
    Serial.println("At least one characteristic UUID not found");
    return false;
  }
  return true;
}

// Function to chech Characteristic
bool connectCharacteristic(BLERemoteService* pRemoteService, BLERemoteCharacteristic* l_BLERemoteChar) {
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  if (l_BLERemoteChar == nullptr) {
    Serial.print("Failed to find one of the characteristics");
    Serial.print(l_BLERemoteChar->getUUID().toString().c_str());
    return false;
  }
  Serial.println(" - Found characteristic: " + String(l_BLERemoteChar->getUUID().toString().c_str()));
  return true;
}

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  //Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
  
    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
  
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
  
    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  initializeLCD();
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    std::string rxValue = pRemoteChar_1->readValue();
    Serial.print("Latitude (readValue): ");
    Serial.println(rxValue.c_str());

    std::string rxValue2 = pRemoteChar_2->readValue();
    Serial.print("Altitude (readValue): ");
    Serial.println(rxValue2.c_str());

    std::string rxValue3 = pRemoteChar_3->readValue();
    Serial.print("Longitude (readValue): ");
    Serial.println(rxValue3.c_str());
    Serial.println("-----------------------------------");

    displayGPS(atof(rxValue.c_str()), atof(rxValue2.c_str()), atof(rxValue3.c_str()));
    
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

  // In this example "delay" is used to delay with one second. This is of course a very basic 
  // implementation to keep things simple. I recommend to use millis() for any production code
  delay(1000);
}

// Función para enviar comandos al LCD
void lcdCommand(uint8_t command) {
  Wire.beginTransmission(LCD_ADDRESS);
  Wire.write((command & 0xF0) | BACKLIGHT); // Envía los 4 bits superiores
  Wire.write((command & 0xF0) | En | BACKLIGHT); // Pulso de Enable
  Wire.write((command & 0xF0) | BACKLIGHT);
  
  Wire.write(((command << 4) & 0xF0) | BACKLIGHT); // Envía los 4 bits inferiores
  Wire.write(((command << 4) & 0xF0) | En | BACKLIGHT); // Pulso de Enable
  Wire.write(((command << 4) & 0xF0) | BACKLIGHT);
  Wire.endTransmission();
  delayMicroseconds(50); // Tiempo necesario para la mayoría de los comandos
}

// Función para enviar datos (caracteres) al LCD
void lcdWrite(uint8_t value) {
  Wire.beginTransmission(LCD_ADDRESS);
  Wire.write((value & 0xF0) | Rs | BACKLIGHT); // Envía los 4 bits superiores
  Wire.write((value & 0xF0) | Rs | En | BACKLIGHT); // Pulso de Enable
  Wire.write((value & 0xF0) | Rs | BACKLIGHT);
  
  Wire.write(((value << 4) & 0xF0) | Rs | BACKLIGHT); // Envía los 4 bits inferiores
  Wire.write(((value << 4) & 0xF0) | Rs | En | BACKLIGHT); // Pulso de Enable
  Wire.write(((value << 4) & 0xF0) | Rs | BACKLIGHT);
  Wire.endTransmission();
  delayMicroseconds(50); // Tiempo necesario para enviar datos
}

// Función para configurar la posición del cursor
void lcdSetCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  lcdCommand(0x80 | (col + row_offsets[row]));
}

void initializeLCD() {
  Wire.begin();  // Inicia la comunicación I2C

  // Inicialización de la pantalla en modo de 4 bits
  lcdCommand(0x03);
  delayMicroseconds(4500); // Espera más de 4.1ms
  lcdCommand(0x03);
  delayMicroseconds(4500); // Espera más
  lcdCommand(0x03);
  delayMicroseconds(150);
  lcdCommand(0x02); // Configuración a 4 bits
  
  lcdCommand(0x28); // Configuración de 4 bits, 2 líneas, fuente de 5x8
  lcdCommand(0x0C); // Enciende la pantalla, cursor apagado
  lcdCommand(0x06); // Modo de entrada
  lcdCommand(0x01); // Limpia la pantalla
  delay(2); // Este comando necesita al menos 2ms, no microsegundos
}

void displayGPS(float Lat, float Alt, float Long) {
  lcdCommand(0x01);  // Clear display
  lcdSetCursor(0, 0);  // Set cursor at the beginning of the first line
  lcdWriteString("Latitude: ");
  lcdWriteFloat(Lat);

  lcdSetCursor(0, 1);  // Move cursor to the second line
  lcdWriteString("Altitude: ");
  lcdWriteFloat(Alt);

  lcdSetCursor(0, 2);  // Move cursor to the third line
  lcdWriteString("Longitude: ");
  lcdWriteFloat(Long);

}