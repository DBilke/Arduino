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
  //automaticCount();
  count();
  //kitt();
}

void automaticCount() {
  x++;
  draw();
  delay(200);
}

void count() {
  if (!digitalRead(LCD_BUTTON_LEFT)) {
    x--;
    draw();
    delay(200);
  } else if (!digitalRead(LCD_BUTTON_RIGHT)) {
    x++;
    draw();
    delay(200);
  }
}

void kitt() {
  for (byte i=0; i<8; i++) {
    x = 0b00000111<<i;
    draw();
    delay(100);
  }
  for (byte i=0; i<8; i++) {
    x = 0b11100000>>i;
    draw();
    delay(100);
  }
}

void draw() {
  lcd.clearVideoBuffer();
  for (byte i=0; i<8; i++) {
    if (x & (1<<i)) {
      lcd.drawBitmap(120 - (i*16), 20, 8, 8, ledOn);
    } else {
      lcd.drawBitmap(120 - (i*16), 20, 8, 8, ledOff);
    }
  }
  lcd.show();
}




