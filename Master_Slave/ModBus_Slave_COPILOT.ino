#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiAP.h>
#include <stdio.h>
#include <stdlib.h>

char codigo = '0';
String x = "0";
String trama = "0";
String respuesta = "0";
char led = '0';
char accion = '0';

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long startTime = 0;
int interval1 = 100;
int interval2 = 100;
int interval3 = 100;
int ledState1 = LOW;
int ledState2 = LOW;
int ledState3 = LOW;

int TRANSACTION_ID = 0;
char hexa_freq [5];
char hexa_transaction [5];
char hexa_led1 [5];
char hexa_led2 [5];
char hexa_led3 [5];
char hexa_contador1 [5];
char hexa_contador2 [5];
String PROTOCOL_ID = "0000";

String TAM_1 = "0006";
String TAM_2 = "000C";
String TAM_3 = "";
String UNIT_ID = "01";

//PIN LED
const int ledPin = 13;
const int ledPin_2 = 14;
const int ledPin_3 = 27;
int PULSADOR_1 = 33;
int PULSADOR_2 = 32;

//CONTADORES
int contador = 0;
int contador_2 = 0;
int freq = 0;

WiFiServer servidor(502);  // Puerto en el que el ESP32 está escuchando
WiFiClient cliente;


bool led1_encendido = false;
bool led2_encendido = false;
bool led3_encendido = false;

void setup() {
  startTime = millis();
  Serial.begin(9600);
  WiFi.begin("Motorola", "Qwe12345");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a la red WiFi");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  // Iniciar el servidor Modbus TCP/IP
  servidor.begin();
  Serial.println("Servidor Modbus iniciado");

  //mb.server();

  pinMode(ledPin, OUTPUT);
  pinMode(ledPin_2, OUTPUT);
  pinMode(ledPin_3, OUTPUT);
  pinMode(PULSADOR_1, INPUT_PULLUP);
  pinMode(PULSADOR_2, INPUT_PULLUP);



  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin_2, LOW);
  digitalWrite(ledPin_3, LOW);

}

void loop() {


  cliente = servidor.available();
  if (cliente) {
    Serial.println("Cliente conectado");

    // Leer datos del cliente
    while (cliente.connected()) {
      unsigned long currentMillis = millis();
      unsigned long currentMillis2 = millis();
      unsigned long currentMillis3 = millis();
      codigo = trama[15];
      sprintf(hexa_led1, "%04X", int(led1_encendido));
      sprintf(hexa_led2, "%04X", int(led2_encendido));
      sprintf(hexa_led3, "%04X", int(led3_encendido));
      sprintf(hexa_contador1, "%04X", int(contador));
      sprintf(hexa_contador2, "%04X", int(contador_2));
      sprintf(hexa_freq, "%04X", int(freq));

      if (cliente.available()) {
        // Leer la trama Modbus recibida
        trama = cliente.readStringUntil('\n');
        Serial.println("Trama Modbus recibida: " + trama);
        TRANSACTION_ID ++;
        sprintf(hexa_transaction, "%04X", TRANSACTION_ID);
      }


      // Interpretar la trama Modbus
      if (trama.length() >= 8) {



        // Leer Holding Register (código 03)
        if (codigo == '3') {
          // Construir la respuesta con los valores de los registros solicitados
          respuesta = hexa_transaction + PROTOCOL_ID + TAM_2 + UNIT_ID + x + codigo + hexa_led1 + hexa_led2 + hexa_led3 + hexa_contador1 + hexa_contador2;

          // Enviar la respuesta al cliente
          cliente.println(respuesta);
          Serial.println("Respuesta Modbus enviada: " + respuesta);
        }
        // Write Single Register (código 06)
        else if (codigo == '6' && trama.length() >= 12) {
          led = trama[19];
          accion = trama[23];

          if (led == '0') {
            if (accion == '1') {
              led1_encendido = true;
              //digitalWrite(ledPin,HIGH);
              if (currentMillis - previousMillis1 >= interval1) {
                previousMillis1 = currentMillis;

                if (ledState1 == LOW) {
                  ledState1 = HIGH;
                } else {
                  ledState1 = LOW;
                }
                digitalWrite(ledPin, ledState1);  // Establece el estado del LED
              }
              respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0000") +  hexa_led1;
              cliente.println(respuesta);
              Serial.println("Respuesta Modbus enviada: " + respuesta);
            }

            else if (accion == '0') {
              led1_encendido = false;
              digitalWrite(ledPin, LOW);
            }



          }

          else if (led == '1') {
            if (accion == '1') {
              led2_encendido = true;
              //digitalWrite(ledPin_2, HIGH);
              if (currentMillis2 - previousMillis2 >= interval1) {
                previousMillis2 = currentMillis2;

                if (ledState2 == LOW) {
                  ledState2 = HIGH;
                } else {
                  ledState2 = LOW;
                }
                digitalWrite(ledPin_2, ledState2);  // Establece el estado del LED
              }

            }
            else if (accion == '0') {
              led2_encendido = false;
              digitalWrite(ledPin_2, LOW);
            }

            respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0001") +  hexa_led2;
            cliente.println(respuesta);
            Serial.println("Respuesta Modbus enviada: " + respuesta);
          }

          else if (led == '2') {
            if (accion == '1') {
              led3_encendido = true;
              //digitalWrite(ledPin_3, HIGH);
              if (currentMillis3 - previousMillis3 >= interval1) {
                previousMillis3 = currentMillis3;

                if (ledState3 == LOW) {
                  ledState3 = HIGH;
                } else {
                  ledState3 = LOW;
                }
                digitalWrite(ledPin_3, ledState3);  // Establece el estado del LED
              }

            }
            else if (accion == '0') {
              led3_encendido = false;
              digitalWrite(ledPin_3, LOW);
            }

            respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0002") +  hexa_led3;
            cliente.println(respuesta);
            Serial.println("Respuesta Modbus enviada: " + respuesta);
          }

          else if (led == '3') {
            contador = atoi(&accion);
            sprintf(hexa_contador1, "%04X", char(contador));
            respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0003") +  hexa_contador1;
            cliente.println(respuesta);
            Serial.println("Respuesta Modbus enviada: " + respuesta);
          }
          else if (led == '4') {
            contador_2 = atoi(&accion);;
            sprintf(hexa_contador2, "%04X", char(contador_2));
            respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0004") +  hexa_contador2;
            cliente.println(respuesta);
            Serial.println("Respuesta Modbus enviada: " + respuesta);
          }
          else if(led = '5'){
            digitalWrite(ledPin, LOW);
            digitalWrite(ledPin_2, LOW);
            digitalWrite(ledPin_3, LOW);

            if (accion == 1){

              interval1 = 500;
              freq = 1;
            }
            else if(accion == 2){
              interval1 = 1000;
              freq = 2;
            }
            else if(accion == 3){
              interval1 = 1500;
              freq = 3;
            }
            respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0005") +  hexa_freq;
            cliente.println(respuesta);
            Serial.println("Respuesta Modbus enviada: " + respuesta);
          }

        }
      }

      if (digitalRead(PULSADOR_1) == LOW) {
        contador++;
        Serial.print("Contador 1: ");
        Serial.println(contador);

      }
      if (digitalRead(PULSADOR_2) == LOW) {
        contador_2++;
        Serial.print("Contador 2: ");
        Serial.println(contador_2);
      }

    }

    // Cerrar la conexión con el cliente
    cliente.stop();
    Serial.println("Cliente desconectado");


  }


}