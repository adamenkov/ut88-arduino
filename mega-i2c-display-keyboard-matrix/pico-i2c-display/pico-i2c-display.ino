#include <PicoDVI.h>
#include <Wire.h>


DVIGFX1 display(DVI_RES_640x480p60, false, adafruit_feather_dvi_cfg);
//DVIGFX1 display(DVI_RES_400x240p30, false, adafruit_feather_dvi_cfg);

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

    //Serial.begin(115200);
}


void loop()
{
    digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN) == LOW) ? HIGH : LOW);
    delay(1000);
}


//static uint8_t __attribute__((aligned(4))) screen[0x0800];

uint8_t __attribute__((aligned(4))) ram_for_mega[0xF800];

uint16_t saved_addr;

void onReceive(int cb)
{
    if (cb == 3)
    {
        int addr = (Wire.read() & 0xFF);
        addr <<= 8;
        addr |= (Wire.read() & 0xFF);

        if ((addr < 0xE000) || ((0xF000 <= addr) && (addr < 0xF400)))
        {
            ram_for_mega[addr] = Wire.read();
        }
        else if (addr < 0xF000)
        {
            //Serial.print("Character ");

            addr &= 0x7FF;
            int ch = Wire.read() & 0xFF;

            //Serial.print(uint8_t(ch), HEX);
            //Serial.print(" at address ");
            //Serial.println(addr, HEX);

            //screen[addr] = ch;  // for scrolling

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

        saved_addr = addr;
    }
    else
    {
        int command = Wire.read();
        if (command == 0)
        {
            // Move lines up
            //Serial.println("Move lines up.");

            /*
            memcpy(screen, screen + 0x40, 0x06C0);  // 27 lines

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
            */
            uint8_t* ut88_buffer = display.getBuffer() + 640 / 8 * display_top;
            memcpy(ut88_buffer, ut88_buffer + 640 / 8 * CHARACTER_HEIGHT, 640 / 8 * CHARACTER_HEIGHT * 27);

            //display.fillRect(display_left, display_top + CHARACTER_HEIGHT * 27, TEXT_DISPLAY_WIDTH * CHARACTER_WIDTH, CHARACTER_HEIGHT, 0x0000);
            memset(ut88_buffer + 640 / 8 * CHARACTER_HEIGHT * 27, 0x0000, 640 / 8 * CHARACTER_HEIGHT);

            //Wire.write(uint8_t(0xFF));

            //Serial.println("Done moving lines up.");
        }
        else
        {
            // Clear screen
            //Serial.println("Clear screen.");

            //memset(screen, 0x20, sizeof screen);
            display.fillScreen(0x0000);
        }
    }
}

void onRequest()
{
    //Serial.print("Request for byte at address ");
    //Serial.println(saved_addr, HEX);

    Wire.write((saved_addr < sizeof ram_for_mega) ? ram_for_mega[saved_addr] : 0xFF);
}
