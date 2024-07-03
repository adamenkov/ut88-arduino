#include <PicoDVI.h>
#include <Wire.h>

// I'd rather have this display mode, but I couldn't make it work, even with QSPI /4
//DVIGFX1 display(DVI_RES_400x240p30, false, adafruit_feather_dvi_cfg);
DVIGFX1 display(DVI_RES_640x480p60, false, adafruit_feather_dvi_cfg);

int display_left;
int display_top;


enum
{
    TEXT_DISPLAY_WIDTH = 64,
    TEXT_DISPLAY_HEIGHT = 32,   // Change this to 30 if you want to try to use a 400x240 mode

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


uint8_t __attribute__((aligned(4))) ram_for_mega[0xF800];

uint16_t requested_addr;    // requested by Arduino

void onReceive(int cb)
{
    if (cb == 3)
    {
        int addr = (Wire.read() & 0xFF);
        addr <<= 8;
        addr |= (Wire.read() & 0xFF);

        if (addr < 0xE000)
        {
            ram_for_mega[addr] = Wire.read();
        }
        else if (addr < 0xF000)
        {
            addr &= 0x7FF;
            int ch = Wire.read() & 0xFF;

            uint16_t fg = (ch <= 0x7f) ? 0xFFFF : 0x0000;

            display.drawBitmap(
                display_left + CHARACTER_WIDTH * (addr % TEXT_DISPLAY_WIDTH),
                display_top + CHARACTER_HEIGHT * (addr / TEXT_DISPLAY_WIDTH),
                font + 8 * (ch & 0x7F),
                CHARACTER_WIDTH,
                CHARACTER_HEIGHT,
                fg,
                ~fg
            );
        }
    }
    else if (cb == 2)
    {
        int addr = (Wire.read() & 0xFF);
        addr <<= 8;
        addr |= (Wire.read() & 0xFF);

        requested_addr = addr;
    }
    else
    {
        // cb == 1

        int command = Wire.read();
        if (command == 0)
        {
            // Move lines up
            uint8_t* ut88_buffer = display.getBuffer() + 640 / 8 * display_top;
            memcpy(ut88_buffer, ut88_buffer + 640 / 8 * CHARACTER_HEIGHT, 640 / 8 * CHARACTER_HEIGHT * 27);     // 27 lines - that's how it is in the UT-88's Monitor

            // Clear the 28th line (line 27)
            memset(ut88_buffer + 640 / 8 * CHARACTER_HEIGHT * 27, 0x0000, 640 / 8 * CHARACTER_HEIGHT);
        }
        else
        {
            // Clear screen
            display.fillScreen(0x0000);
        }
    }
}

void onRequest()
{
    Wire.write((requested_addr < sizeof ram_for_mega) ? ram_for_mega[requested_addr] : 0xFF);
}
