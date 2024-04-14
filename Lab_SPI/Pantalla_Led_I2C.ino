//codigo pantalla lsd sin liquid cristal: 
#include <Wire.h>

#define LCD_ADDRESS 0x27 // Asegúrate de usar la dirección correcta
#define BACKLIGHT 0x08
#define En 0x04  // Enable bit
#define Rs 0x01  // Register select bit

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

void setup() {
  Wire.begin();  // Inicia la comunicación I2C
  delay(50);

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
  
  // Enviar mensajes
  lcdSetCursor(0, 0); // Primera línea
  for (const char *p = "Hola mundo!"; *p; p++) { lcdWrite(*p); }
  
  lcdSetCursor(0, 1); // Segunda línea
  for (const char *p = "vas a caer"; *p; p++) { lcdWrite(*p); }
  
  lcdSetCursor(0, 2); // Tercera línea
  for (const char *p = "gege"; *p; p++) { lcdWrite(*p); }

  lcdSetCursor(0, 3); // Cuarta línea
  for (const char *p = "y kakuna"; *p; p++) { lcdWrite(*p); }
}

void loop() {
  // No es necesario actualizar el LCD constantemente
}