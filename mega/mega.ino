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

#include "ut88_z80.h"
#include "ut88_keypad4x4.h"
#include "lcd_keypad_shield.h"


LiquidCrystal& lcd = LcdKeypadShield::GetLcd();


namespace rom
{
    PROGMEM const uint8_t bytes[] =
    {
    #include "monitor.h"
    };

    enum { start = 0, end = start + sizeof bytes };
}


namespace ram
{
    uint8_t bytes[1024];
    enum { start = 0xC000, end = start + sizeof bytes };
}


ISR(TIMER1_COMPA_vect)
{ 
    PORTB &= 0xF7;      // digitalWrite(ut88::Z80::Pin::PIN_INT_N, LOW);
}


void setup()
{
    lcd.begin(16, 2);

    ut88::Z80::Init();

    cli();

    // Timer frequency is 1,024 times lower than the main clock frequency (16 MHz / 1,024).
    // Also, set the CTC mode (clear timer on compare match).
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

    // 15,625 ticks of the timer frequency (16 MHz / 1,024) is exactly one second
    OCR1A = 15625;

    // Enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Initialize the counter value to 0
    TCNT1  = 0;

    sei();
}


void loop()
{
    LcdKeypadShield::Button shieldButton;
    uint16_t addr;

    for (;;)
    {
        PORTB |= 0x02;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, HIGH);
        
        shieldButton = LcdKeypadShield::GetPressedButton();

        addr = ADDR;
        
        if (MREQ_N == 0)
        {
            if (RD_N == 0)
            {
                DATA_DIR = 0xFF;

                // Assuming ROM starts at 0x0000 for optimization
                if (addr < rom::end)
                {
                    if (rom::start <= addr)
                    {
                        DATA_OUT = pgm_read_byte_near(rom::bytes + addr);
                    }
                }
                else if (addr < ram::end)   // Assuming RAM (0xC000) goes after the LCD (0x9000)
                {
                    if (ram::start <= addr)
                    {
                        DATA_OUT = ram::bytes[addr - ram::start];
                    }
                }
            }
            else if (WR_N == 0)
            {
                if (addr < 0x9003)
                {
                    if (0x9000 <= addr)
                    {
                        static uint8_t xOffset[3] = { 0, 2, 5 };
                        lcd.setCursor(xOffset[addr - 0x9000], 0);
                        lcd.print(static_cast<uint8_t>(DATA_IN), HEX);
                    }
                }
                else if (addr < ram::end)
                {
                    if (ram::start <= addr)
                    {
                        ram::bytes[addr - ram::start] = DATA_IN;
                    }
                }
            }
        }
        else if (IORQ_N == 0)
        {    
            if (RD_N == 0)
            {
                DATA_DIR = 0xFF;

                if (ADDR_L == 0xA0)
                {
                    uint8_t keyCode = ut88::Keypad4x4_portK::GetKeyCode();
                    if (keyCode != 0x00)
                    {
                        DATA_OUT = keyCode;
                    }
                    else if (shieldButton == LcdKeypadShield::Button::Select)
                    {
                        DATA_OUT = 0x80;
                    }
                }

            }
            else if (WR_N == 0)
            {
            }
            else
            {
                // No access to pin M1, probably an interrupt
                PORTB |= 0x08;      // digitalWrite(ut88::Z80::Pin::PIN_INT_N, HIGH);

                DATA_DIR = 0xFF;
                DATA_OUT = 0xFF;    // RST 38 (RST 7)
            }
        }

        if (shieldButton == LcdKeypadShield::Button::Right)
        {
            ut88::Z80::Reset();
            continue;
        }

        PORTB &= 0xFD;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, LOW);
        DATA_DIR = 0x00;
        delayMicroseconds(1);
    }
}
