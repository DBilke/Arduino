/*
  Display.h - Library to control a ST7565 based 12864 LCD.
  Thomas Baum, th.baum@online.de, 10.05.2014
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Display {
  public:
    static const byte WIDTH = 128;
    static const byte HEIGHT = 64;
    Display() {}

    void init(byte contrast);
    void initSoftSPI(byte contrast);
    void writeCommand(byte command);
    void writeData(byte data);
    void resetRamAddress(void);
    void setPageAddress(byte pageAddress);
    void setColumnAddress(byte columnAddress);
    void clearDisplayRAM(void);
    void clearVideoBuffer(void);
    void drawPixel(byte x, byte y);
    void drawLine(byte x1, byte y1, byte x2, byte y2);
    void drawString(byte x, byte y, const char string[]);
    void drawBitmap(byte x, byte y, byte width, byte height, const byte bitmap[]);
    void drawEllipse(byte x, byte y, byte a, byte b);
    void invertRectangle(byte x1, byte y1, byte x2, byte y2);
    void show();
    
  private:
    byte videoBuffer [1024];
    boolean isSoftSPI;
};

#endif

