//
//  I borrowed many ideas on driving Z80 by Arduino Mega 2560 from:
//
////////////////////////////////////////////////////////////////////
// Z80 RetroShield Code
// 2019/02/11
// Version 0.1
// Erturk Kocalar

// The MIT License (MIT)

// Copyright (c) 2019 Erturk Kocalar, 8Bitforce.com

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <EEPROM.h>
#include <Wire.h>

#include "ut88_z80.h"
#include "ut88_keypad4x4.h"
#include "lcd_keypad_shield.h"


LiquidCrystal& lcd = LcdKeypadShield::GetLcd();


namespace rom
{
    namespace monitor_0
    {
        PROGMEM const uint8_t bytes[] =
        {
#include "monitor-0.h"
        };

        enum { start = 0, end = start + sizeof bytes };
    }

    namespace monitor_f
    {
        PROGMEM const uint8_t bytes[] =
        {
#include "monitor-f.h"
        };

        enum { start = 0xF800, end = start + sizeof bytes };
    }
}   // namespace rom


namespace ram
{
    namespace extra
    {
        uint8_t bytes[2048];
        enum { start = 0x3000, end = start + sizeof bytes };
    }

    namespace monitor_0
    {
        uint8_t bytes[1024];
        enum { start = 0xC000, end = start + sizeof bytes };
    }

    namespace screen
    {
        uint8_t bytes[2048];
        enum { start = 0xE000, end = start + 2 * sizeof bytes };
    }

    namespace monitor_f
    {
        uint8_t bytes[1024];
        enum { start = 0xF400, end = start + sizeof bytes };
    }
}   // namespace ram


ISR(TIMER1_COMPA_vect)
{
    ut88::Z80::SetInt();
}


void setup()
{
    ut88::Z80::Init();

    lcd.begin(16, 2);
    lcd.print("FFFF ??   tape=0");

    cli();

    // Timer frequency is 1,024 times lower than the main clock frequency (16 MHz / 1,024).
    // Also, set the CTC mode (clear timer on compare match).
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

    // 15,625 ticks of the timer at frequency (16 MHz / 1,024) is exactly one second
    OCR1A = 15625;

    // Enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Initialize the counter value to 0
    TCNT1  = 0;

    Wire.begin();

    sei();
}


void pause()
{
    // Good old way to spend some time
    volatile int i = 0;
    while (++i != 0)
    {
        volatile int8_t j = 30;
        while (--j != 0)
        {
        }
    }
}


void loop()
{
    LcdKeypadShield::Button shieldButton = LcdKeypadShield::Button::None;
    uint16_t lcd_counter = 0;

    uint16_t addr = 0x0000;

    uint8_t prevIORQ_N = IORQ_N;
    uint8_t prevDATA = 0xFF;

    for (;;)
    {
        ut88::Z80::SetClock();
        
        if (++lcd_counter == 0)
        {
            shieldButton = LcdKeypadShield::GetPressedButton();

            switch (shieldButton)
            {
            case LcdKeypadShield::Button::Up:
                shieldButton = LcdKeypadShield::Button::None;
                lcd.setCursor(0, 1);
                lcd.print("Uploading...");

                EEPROM.put(0, ram::extra::bytes);
                EEPROM.put(sizeof ram::extra::bytes, ram::monitor_0::bytes);
                EEPROM.put(sizeof ram::extra::bytes + sizeof ram::monitor_0::bytes, ram::monitor_f::bytes);
                
                pause();
                lcd.setCursor(0, 1);
                lcd.print("            ");
                break;

            case LcdKeypadShield::Button::Down:
                shieldButton = LcdKeypadShield::Button::None;
                lcd.setCursor(0, 1);
                lcd.print("Downloading...");
                
                EEPROM.get(0, ram::extra::bytes);
                EEPROM.get(sizeof ram::extra::bytes, ram::monitor_0::bytes);
                EEPROM.get(sizeof ram::extra::bytes + sizeof ram::monitor_0::bytes, ram::monitor_f::bytes);
                
                pause();
                lcd.setCursor(0, 1);
                lcd.print("              ");
                break;

            case LcdKeypadShield::Button::Right:
                shieldButton = LcdKeypadShield::Button::None;
                ut88::Z80::Reset();
                continue;
            }
        }

        addr = ADDR;
        
        if (MREQ_N == 0)
        {
            if (RD_N == 0)
            {
                DATA_DIR = 0xFF;

                // Assuming ROM starts at 0x0000 for optimization
                if (addr < rom::monitor_0::end)
                {
                    DATA_OUT = pgm_read_byte_near(rom::monitor_0::bytes + addr);
                }
                else if (addr < ram::extra::end)   // Assuming extra RAM starts at 0x3000
                {
                    if (ram::extra::start <= addr)
                    {
                        DATA_OUT = ram::extra::bytes[addr - ram::extra::start];
                    }
                }
                else if (addr < ram::monitor_0::end)   // Assuming Monitor-0's RAM starts at 0xC000
                {
                    if (ram::monitor_0::start <= addr)
                    {
                        DATA_OUT = ram::monitor_0::bytes[addr - ram::monitor_0::start];
                    }
                }
                else if (addr < ram::screen::end)   // Assuming screen RAM starts at 0xE000
                {
                    if (ram::screen::start <= addr)
                    {
                        DATA_OUT = ram::screen::bytes[(addr - ram::screen::start) & 0x7FF];
                    }
                }
                else if (addr < ram::monitor_f::end)   // Assuming Monitor-F's RAM starts at 0xF400
                {
                    if (ram::monitor_f::start <= addr)
                    {
                        DATA_OUT = ram::monitor_f::bytes[addr - ram::monitor_f::start];
                    }
                }
                else if (rom::monitor_f::start <= addr)
                {
                    DATA_OUT = pgm_read_byte_near(rom::monitor_f::bytes + (addr - rom::monitor_f::start));
                }
                else
                {
                    DATA_OUT = 0xFF;
                }
            }
            else if (WR_N == 0)
            {
                if (addr < ram::extra::end)   // Assuming extra RAM starts at 0x3000
                {
                    if (ram::extra::start <= addr)
                    {
                        ram::extra::bytes[addr - ram::extra::start] = DATA_IN;
                    }
                }
                else if (addr < 0x9003)
                {
                    if (0x9000 <= addr)
                    {
                        static uint8_t xOffset[3] = { 5, 2, 0 };
                        lcd.setCursor(xOffset[addr - 0x9000], 0);

                        static const char* digits[256] =
                        {
                            "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
                            "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
                            "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
                            "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",

                            "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
                            "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
                            "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
                            "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",

                            "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
                            "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
                            "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
                            "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",

                            "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
                            "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
                            "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
                            "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
                        };
                        lcd.print(digits[DATA_IN]);
                    }
                }
                else if (addr < ram::monitor_0::end)   // Assuming Monitor-0's RAM starts at 0xC000
                {
                    if (ram::monitor_0::start <= addr)
                    {
                        ram::monitor_0::bytes[addr - ram::monitor_0::start] = DATA_IN;
                    }
                }
                else if (addr < ram::screen::end)   // Assuming screen RAM starts at 0xE000
                {
                    if (ram::screen::start <= addr)
                    {
                        ram::screen::bytes[(addr - ram::screen::start) & 0x7FF] = DATA_IN;

                        Wire.beginTransmission(0x33);
                        Wire.write(uint8_t(highByte(addr)));
                        Wire.write(uint8_t(lowByte(addr)));
                        Wire.write(uint8_t(DATA_IN));
                        Wire.endTransmission();
                    }
                }
                else if (addr < ram::monitor_f::end)   // Assuming Monitor-F's RAM starts at 0xF400
                {
                    if (ram::monitor_f::start <= addr)
                    {
                        ram::monitor_f::bytes[addr - ram::monitor_f::start] = DATA_IN;
                    }
                }
            }
        }
        else if (IORQ_N == 0)
        {   
            if (RD_N == 0)
            {
                DATA_DIR = 0xFF;

                if (prevIORQ_N == 0)
                {
                    DATA_OUT = prevDATA;
                }
                else
                {
                    if (ADDR_L == 0xA0)
                    {
                        DATA_OUT = prevDATA = (shieldButton == LcdKeypadShield::Button::Select)
                            ? 0x80
                            : ut88::Keypad4x4_portK::GetKeyCode();

                        shieldButton = LcdKeypadShield::Button::None;
                    }
                    else
                    {
                        DATA_OUT = 0xFF;
                    }
                }
            }
            else if (WR_N == 0)
            {
            }
            else
            {
                // No access to pin M1, probably an interrupt
                ut88::Z80::ResetInt();

                DATA_DIR = 0xFF;
                DATA_OUT = 0xFF;    // RST 38 (RST 7)
            }
        }

        ut88::Z80::ResetClock();

        DATA_DIR = 0x00;
        
        prevIORQ_N = IORQ_N;
    }
}
