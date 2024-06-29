#include <PicoDVI.h>
#include <Wire.h>


DVIGFX1 display(DVI_RES_640x480p60, false, adafruit_feather_dvi_cfg);
//DVIGFX1 display(DVI_RES_400x240p60, false, adafruit_feather_dvi_cfg, VREG_VOLTAGE_1_30);

int display_left;
int display_top;


enum
{
    TEXT_DISPLAY_WIDTH = 64,
    TEXT_DISPLAY_HEIGHT = 32,

    CHARACTER_WIDTH = 6,
    CHARACTER_HEIGHT = 8,
};


PROGMEM const uint8_t font[2048] =
{
#define U(x)    uint8_t((~x) << 2)
#include "ut88font.h"
};


void setup()
{
    if (!display.begin())
    {
        pinMode(LED_BUILTIN, OUTPUT);
        
        for (;;)
        {
            digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN) == LOW) ? HIGH : LOW);
            delay(500);
        }
    }

    display_left = display.width() / 2 - TEXT_DISPLAY_WIDTH * CHARACTER_WIDTH / 2;
    display_top = display.height() / 2 - TEXT_DISPLAY_HEIGHT * CHARACTER_HEIGHT / 2;

    Wire.begin(0x33);
    Wire.setClock(400000);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
}


void loop()
{
    digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN) == LOW) ? HIGH : LOW);
    delay(1000);
}


static uint8_t __attribute__((aligned(4))) screen[0x0800];


void onReceive(int cb)
{
    if (cb == 3)
    {
        int b = Wire.read();
        if ((0xE0 <= b) && (b < 0xF0))
        {
            int addr = ((b & 0x7) << 8) | (Wire.read() & 0xFF);
            int ch = Wire.read() & 0xFF;

            screen[addr] = ch;  // for scrolling

            display.drawBitmap(
                display_left + CHARACTER_WIDTH * (addr % TEXT_DISPLAY_WIDTH),
                display_top + CHARACTER_HEIGHT * (addr / TEXT_DISPLAY_WIDTH),
                font + 8 * (ch & 0x7F),
                CHARACTER_WIDTH,
                CHARACTER_HEIGHT,
                (ch <= 0x7f) ? 0xFFFF : 0x0000,
                (ch <= 0x7f) ? 0x0000 : 0xFFFF
            );
        }
    }
    else
    {
        int command = Wire.read();
        if (command == 0)
        {
            // Move lines up

            memcpy(screen, screen + 0x40, 0x06C0);

            uint32_t* dst = reinterpret_cast<uint32_t*>(screen + 0x06C0);
            for (int i = 16; --i >= 0; ++dst)
            {
                *dst = 0x20202020;
            }

            for (int addr = 0x0700; --addr >= 0;)
            {
                unsigned int ch = screen[addr];
                uint16_t fg = (ch <= 0x7f) ? 0xFFFF : 0x0000;
                uint16_t bg = ~fg;

                display.drawBitmap(
                    display_left + CHARACTER_WIDTH * (addr % TEXT_DISPLAY_WIDTH),
                    display_top + CHARACTER_HEIGHT * (addr / TEXT_DISPLAY_WIDTH),
                    font + 8 * (ch & 0x7F),
                    CHARACTER_WIDTH,
                    CHARACTER_HEIGHT,
                    fg,
                    bg
                );
            }
            Wire.write(uint8_t(0xFF));
        }
        else
        {
            // Clear screen

            memset(screen, 0x20, sizeof screen);
            display.fillScreen(0x0000);
        }
    }
}

void onRequest()
{

}
