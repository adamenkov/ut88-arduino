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
#include "ut88_keyboard.h"


namespace rom
{
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
    // 4K bytes of onboard RAM, the rest is kept on the Adafruit DVI Feather
    namespace main
    {
        uint8_t bytes[4096] =
        {
            0xC3, 0x00, 0xF8,   // JP F800 to start the monitor_f (yes, it's ugly)
        };
        enum { start = 0x0000, end = start + sizeof bytes };
    }

    namespace screen
    {
        uint8_t bytes[2048];
        enum { start = 0xE000, end = start + 2 * sizeof bytes };    // E800-EFFF mirrors E000-E7FF
    }

    namespace monitor_f
    {
        uint8_t bytes[1024];
        enum { start = 0xF400, end = start + sizeof bytes };
    }
}   // namespace ram


void setup()
{
    ut88::Z80::Init();

    Wire.begin();
    Wire.setClock(400000);
}


void loop()
{
    uint16_t keyboard_counter = 0;

    uint16_t addr = 0x0000;

    uint8_t prevIORQ_N = IORQ_N;
    uint8_t prevDATA = 0xFF;

    uint8_t portA = 0xFF;

    for (;;)
    {
        ut88::Z80::SetClock();
        
        if (++keyboard_counter == 0)
        {
            keyboard_counter = 0x8000;

            if ((PINH & 0x01) == 0)
            {
                ut88::Z80::Reset();
                continue;
            }

            ut88::Keyboard::Poll();

            //TODO save & load
        }

        addr = ADDR;
        
        if (MREQ_N == 0)
        {
            if (RD_N == 0)
            {
                DATA_DIR = 0xFF;

                if (addr < ram::main::end)      // Assuming main RAM starts at 0x0000
                {
                    DATA_OUT = ram::main::bytes[addr];
                }
                else if (addr < ram::screen::start)
                {
                    // Get the byte from the Adafruit DVI Feather
                    Wire.beginTransmission(0x33);
                    Wire.write(uint8_t(highByte(addr)));
                    Wire.write(uint8_t(lowByte(addr)));
                    Wire.endTransmission();

                    // Wait for the pico, but not more than 1 ms
                    Wire.requestFrom(0x33, 1);
                    uint8_t b = 0xFF;

                    unsigned long start = micros();
                    for (;;)
                    {
                        if (Wire.available() > 0)
                        {
                            while (Wire.available() > 0)
                            {
                                b = Wire.read();
                            }
                            break;
                        }
                        if (micros() - start >= 1000)
                        {
                            break;
                        }
                    }

                    DATA_OUT = b;
                }
                else if (addr < ram::screen::end)   // Assuming screen RAM starts at 0xE000
                {
                    DATA_OUT = ram::screen::bytes[addr & 0x7FF];
                }
                else if ((addr < ram::monitor_f::end) && (ram::monitor_f::start <= addr))  // Assuming Monitor-F's RAM starts at 0xF400
                {
                    DATA_OUT = ram::monitor_f::bytes[addr - ram::monitor_f::start];
                }
                else if (rom::monitor_f::start <= addr)
                {
                    if (addr == 0xFD19)
                    {
                        // Scrolling subroutine

                        Wire.beginTransmission(0x33);   // 0x33 = I2C address I assigned to the Adafruit DVI
                        Wire.write(uint8_t(0x00));      // 0x00 - tell the Adafruit DVI Feather to scroll
                        Wire.endTransmission();
                        
                        // Move the screen RAM accordingly
                        const uint8_t* src = ram::screen::bytes + 0x40; // LD HL,E840
                        uint8_t* dst = ram::screen::bytes;              // LD DE,E800
                        do
                        {
                            *dst = *src;                                // LD A,(HL) : LD (DE),A
                            ++dst;                                      // INC DE
                            ++src;                                      // INC HL
                        } while (src != ram::screen::bytes + 0x0700);   // LD A,H : CP EF : JP NZ,FD1F
                        
                        // Clear the bottom line
                        dst = ram::screen::bytes + 0x06C0;              // LD HL,EEC0
                        do
                        {
                            *dst = 0x20;                                // LD A,20 : LD (HL),A
                            ++dst;                                      // INC L
                        } while (dst != ram::screen::bytes + 0x0700);   // JP NZ,FD2E

                        DATA_OUT = 0xC9;                                // RET
                    }
                    else if (addr == 0xFCD5)
                    {
                        // Clear the screen Monitor subroutine

                        Wire.beginTransmission(0x33);
                        Wire.write(uint8_t(0x01));    // 0x01 - tell the Adafruit DVI Feather to clear the screen
                        Wire.endTransmission();

                        uint8_t* dst = ram::screen::bytes;
                        for (int16_t i = sizeof ram::screen::bytes; --i >= 0; ++dst)
                        {
                            *dst = 0x20;
                        }
                        
                        DATA_OUT = 0xC9;    // RET
                    }
                    else
                    {
                        DATA_OUT = pgm_read_byte_near(rom::monitor_f::bytes + (addr - rom::monitor_f::start));
                    }
                }
                else
                {
                    DATA_OUT = 0xFF;
                }
            }
            else if (WR_N == 0)
            {
                if (addr < ram::main::end)   // Assuming main RAM starts at 0x0000
                {
                    ram::main::bytes[addr] = DATA_IN;
                }
                else if (addr < ram::screen::end)   // Assuming screen RAM starts at 0xE000
                {
                    Wire.beginTransmission(0x33);
                    Wire.write(uint8_t(highByte(addr)));
                    Wire.write(uint8_t(lowByte(addr)));
                    Wire.write(uint8_t(DATA_IN));
                    Wire.endTransmission();
                    
                    if (ram::screen::start <= addr)
                    {
                        ram::screen::bytes[addr & 0x7FF] = DATA_IN;
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
                    if (ADDR_L == 0x05)     // 8852, port C
                    {
                        DATA_OUT = prevDATA = ut88::Keyboard::GetPortC();
                    }
                    else if (ADDR_L == 0x06) // 8852, port B
                    {
                        DATA_OUT = prevDATA = (ut88::Keyboard::GetPortA() == portA) ? ut88::Keyboard::GetPortB() : 0x7F;
                    }
                    else
                    {
                        DATA_OUT = prevDATA = 0xFF;
                    }
                }
            }
            else if (WR_N == 0)
            {
                if (ADDR_L == 0x07) // 8852, port A
                {
                    portA = DATA_IN;
                }
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
