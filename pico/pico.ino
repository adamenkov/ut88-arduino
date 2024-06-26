#include <PicoDVI.h>
#include <Wire.h>


DVIGFX1 display(DVI_RES_640x480p60, false, adafruit_feather_dvi_cfg);
//DVIGFX1 display(DVI_RES_400x240p60, false, adafruit_feather_dvi_cfg);

uint16_t display_left;
uint16_t display_top;


enum : uint16_t
{
    text_display_width = 64,
    text_display_height = 30,

    character_width = 6,
    character_height = 8,
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

    display_left = display.width() / 2 - text_display_width * character_width / 2;
    display_top = display.height() / 2 - text_display_height * character_height / 2;

    Wire.begin(0x33);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
}


void loop()
{
    digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN) == LOW) ? HIGH : LOW);
    delay(1000);
}


void onReceive(int)
{
    for (int n = Wire.available(); n >= 3; n = Wire.available())
    {
        uint16_t b = static_cast<uint16_t>(Wire.read());
        if ((0xE0 <= b) && (b < 0xF0))
        {
            uint16_t addr = ((b & 0x0007) << 8) | (static_cast<uint16_t>(Wire.read()) & 0x00FF);
            uint8_t ch = static_cast<uint16_t>(Wire.read()) & 0x00FF;
            display.drawBitmap(
                display_left + character_width * (addr % text_display_width),
                display_top + character_height * (addr / text_display_width),
                font + 8 * ch,
                character_width,
                character_height,
                0xFFFF,
                0x0000
            );
        }
    }
}


void onRequest()
{

}
