#include "Display.h"
#include "SPI.h"

#define LCD_BUTTON_LEFT A4
#define LCD_BUTTON_RIGHT A5

const PROGMEM byte ledOn [] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
const PROGMEM byte ledOff [] = {
  0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF,
};

Display lcd = Display();
byte x = 0;

void setup() {
  lcd.init(20);
  pinMode(LCD_BUTTON_LEFT, INPUT_PULLUP);
  pinMode(LCD_BUTTON_RIGHT, INPUT_PULLUP);
  draw();
}

void loop() {
  if (!digitalRead(LCD_BUTTON_LEFT)) {
    x = x<<1;
    x = x | 0b00000001;
    draw();
    delay(200);
  } else if (!digitalRead(LCD_BUTTON_RIGHT)) {
    x = x<<1;
    draw();
    delay(200);
  }
}

void draw() {
  lcd.clearVideoBuffer();
  for (byte i=0; i<8; i++) {
    if (x & (1<<i)) {
      lcd.drawBitmap(120 - (i*16), 8, 8, 8, ledOn);
    } else {
      lcd.drawBitmap(120 - (i*16), 8, 8, 8, ledOff);
    }
  }
  char buffer[50]; 
  sprintf(buffer, "Dec: %03d", x);
  lcd.drawString(8, 30, buffer);
  sprintf(buffer, "Hex: 0x%02X", x);
  lcd.drawString(8, 40, buffer); 
  sprintf(buffer, "Bin: ");
  lcd.drawString(8, 50, buffer); 
  utoa(x, buffer, 2);
  lcd.drawString(38, 50, buffer); 
  sprintf(buffer, "li->1");
  lcd.drawString(90, 30, buffer); 
  sprintf(buffer, "re->0");
  lcd.drawString(90, 40, buffer); 
  lcd.show();
}




