/*
  Display.cpp - Library to control a ST7565 based 12864 LCD.
 Thomas Baum, th.baum@online.de, 10.05.2014
 */

#include <SPI.h>
#include "Display.h"
#include "Font.h"

const int LCD_A0 = 8;
const int LCD_RST = 9;
const int LCD_CS = 10;
const int LCD_SDA = 11;
const int LCD_SCL = 13;

void Display::init(byte contrast) {
  isSoftSPI = false;
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(LCD_A0, OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);

  digitalWrite(LCD_RST, LOW);
  digitalWrite(LCD_CS, LOW);
  delay(50);
  digitalWrite(LCD_RST, HIGH);

  writeCommand(0xa0);
  writeCommand(0xc8);
  writeCommand(0xa6);
  writeCommand(0xa2);
  writeCommand(0x2f);
  writeCommand(0xf8);
  writeCommand(0x00);
  writeCommand(0x27);
  writeCommand(0x81);
  writeCommand(contrast);
  writeCommand(0xac);
  writeCommand(0x00);
  writeCommand(0xa4);
  writeCommand(0xaf);
}

void Display::initSoftSPI(byte contrast) {
  isSoftSPI = true;
  pinMode(LCD_SDA, OUTPUT);
  pinMode(LCD_SCL, OUTPUT);
  pinMode(LCD_A0, OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);

  digitalWrite(LCD_RST, LOW);
  digitalWrite(LCD_CS, LOW);
  delay(50);
  digitalWrite(LCD_RST, HIGH);

  writeCommand(0xa0);
  writeCommand(0xc8);
  writeCommand(0xa6);
  writeCommand(0xa2);
  writeCommand(0x2f);
  writeCommand(0xf8);
  writeCommand(0x00);
  writeCommand(0x27);
  writeCommand(0x81);
  writeCommand(contrast);
  writeCommand(0xac);
  writeCommand(0x00);
  writeCommand(0xa4);
  writeCommand(0xaf);
}

void Display::writeCommand(byte command) {
  digitalWrite(LCD_A0, LOW);
  if (isSoftSPI) {
    shiftOut(LCD_SDA, LCD_SCL, MSBFIRST, command);
  } else {
    SPI.transfer(command);
  }
  digitalWrite(LCD_A0, HIGH);
}

void Display::writeData(byte data) {
  if (isSoftSPI) {
    shiftOut(LCD_SDA, LCD_SCL, MSBFIRST, data);
  } else {
    SPI.transfer(data);
  } 
}

void Display::resetRamAddress(void) {
  writeCommand(0x10);
  writeCommand(0x00);
  writeCommand(0xb0);
}

void Display::setPageAddress(byte pageAddress) {
  writeCommand((byte) pageAddress | 0xb0);
}

void Display::setColumnAddress(byte columnAddress) {
  byte highNibble = (byte)columnAddress >> 4;
  writeCommand(0x10 | highNibble);
  byte lowNibble = (byte)columnAddress & 0x0f;
  writeCommand(lowNibble);
}

void Display::clearDisplayRAM() {
  for(int i = 0; i < 8; i++) {
    setPageAddress(i);
    setColumnAddress(0);
    for(int j = 0; j < 128; j++) {
      writeData(0x00);
    }
  }
}

void Display::clearVideoBuffer() {
  for(word i=0; i<1024; i++) {
    videoBuffer[i] = 0;
  }
}

void Display::drawPixel(byte x, byte y) {
  if ((x >= 128) || (y >= 64)) {
    return;
  }
  videoBuffer[x + (y/8) * 128] |= _BV((y%8));
}

/*
 * Bresenham's line algorithm
 * en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
void Display::drawLine(byte x0, byte y0, byte x1, byte y1) {
  short dx = abs(x1-x0), sx = (x0<x1) ? 1 : -1;
  short dy = abs(y1-y0), sy = (y0<y1) ? 1 : -1; 
  short err = (dx>dy ? dx : -dy)/2, e2;
  while(true) {
    drawPixel(x0, y0);
    if (x0==x1 && y0==y1) {
      break;
    }
    e2 = err;
    if (e2 >-dx) { 
      err -= dy; 
      x0 += sx; 
    }
    if (e2 < dy) { 
      err += dx; 
      y0 += sy; 
    }
  }
}

void Display::drawString(byte x, byte y, const char *c) {
  if(y > HEIGHT) {
    return;
  }
  byte page = y/8;
  do {
    char character = c[0];
    if (! character)
      return;

    for (byte i =0; i<CHARACTER_WIDTH; i++ ) {
      word charBuffer = pgm_read_byte(font7x5+(character*6)+i) << y%8;
      videoBuffer[x+(page*WIDTH)] |= charBuffer;
      videoBuffer[x+(WIDTH*(page+1))] |= charBuffer >> 8;
      x++;
    }
  } 
  while (c++);
}

void Display::drawBitmap(byte x, byte y, byte width, byte height, const byte *bitmap) {
  for (byte j=0; j<height; j++) {
    for (byte i=0; i<width; i++ ) {
      if (pgm_read_byte(bitmap + i + (j/8)*width) & _BV(j%8)) {
        drawPixel(x+i, y+j);
      }
    }
  }
}

/*
 * Bresenham's ellipse algorithm
 * de.wikipedia.org/wiki/Bresenham-Algorithmus
 */
void Display::drawEllipse(byte x, byte y, byte a, byte b) {
  int dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
  long a2 = a*a, b2 = b*b;
  long err = b2-(2*b-1)*a2, e2; /* Fehler im 1. Schritt */

  do {
    drawPixel(x+dx, y+dy); /* I. Quadrant */
    drawPixel(x-dx, y+dy); /* II. Quadrant */
    drawPixel(x-dx, y-dy); /* III. Quadrant */
    drawPixel(x+dx, y-dy); /* IV. Quadrant */

    e2 = 2*err;
    if (e2 <  (2*dx+1)*b2) { 
      dx++; 
      err += (2*dx+1)*b2; 
    }
    if (e2 > -(2*dy-1)*a2) { 
      dy--; 
      err -= (2*dy-1)*a2; 
    }
  } 
  while (dy >= 0);

  while (dx++ < a) { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */
    drawPixel(x+dx, y); /* -> Spitze der Ellipse vollenden */
    drawPixel(x-dx, y);
  }
}

void Display::invertRectangle(byte x1, byte y1, byte x2, byte y2) {
  for (byte x=x1; x<=x2; x++) {
    for (byte y=y1; y<=y2; y++) {
      videoBuffer[x + (y/8) * 128] ^= _BV((y%8));
    }
  }
}

void Display::show() {
  short position=0;
  for (byte page=0; page<8; page++) {
    setPageAddress(page);
    setColumnAddress(0);
    for (byte column=0; column<128; column++) {
      writeData(videoBuffer[position++]);
    }
  }
}





