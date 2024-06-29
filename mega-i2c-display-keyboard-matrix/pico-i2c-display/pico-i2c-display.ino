#include <PicoDVI.h>
#include <Wire.h>


DVIGFX1 display(DVI_RES_640x480p60, false, adafruit_feather_dvi_cfg);
//DVIGFX1 display(DVI_RES_400x240p60, false, adafruit_feather_dvi_cfg, VREG_VOLTAGE_1_30);

uint16_t display_left;
uint16_t display_top;


enum : uint16_t
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

    Serial.begin(115200);
}


void loop()
{
    digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN) == LOW) ? HIGH : LOW);
    delay(1000);
}


static uint8_t screen[0x0800];


void onReceive(int cb)
{
    if (cb == 3)
    {
        uint16_t b = static_cast<uint16_t>(Wire.read());
        if ((0xE0 <= b) && (b < 0xF0))
        {
            uint16_t addr = ((b & 0x0007) << 8) | (static_cast<uint16_t>(Wire.read()) & 0x00FF);
            uint8_t ch = static_cast<uint16_t>(Wire.read()) & 0x00FF;

            screen[addr] = ch;  // for scrolling
            Serial.print(addr, HEX);
            Serial.print(": ");
            Serial.println(ch, HEX);

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
        Serial.println("Scrolling!");
        // Scrolling!
        const uint8_t* src = screen + 0x40;                   // LD HL,E840
        uint8_t* dst = screen;                          // LD DE,E800
        do
        {
            *dst = *src;                                // LD A,(HL)    LD (DE),A
            ++dst;                                      // INC DE
            ++src;                                      // INC HL
        } while (src != screen + 0x0700);               // LD A,H   CP EF   JP NZ,FD1F
        Serial.println("Done copying.");
        dst = screen + 0x06C0;                          // LD HL,EEC0
        do
        {                                               // LD A,20
            *dst = 0x20;                                // LD (HL),A
            ++dst;                                      // INC L
        } while (dst != screen + 0x0700);               // JP NZ,FD2E
        Serial.println("Done cleaning.  Now rendering.");

        for (int16_t addr = 0x0700; --addr >= 0;)
        {
            uint8_t ch = screen[addr];
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
        Serial.println("Done rendering.");
        Wire.write(uint8_t(0xFF));
    }
}


void onRequest()
{

}
