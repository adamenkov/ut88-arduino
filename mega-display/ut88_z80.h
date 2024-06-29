#include "Arduino.h"
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

#pragma once

#define DATA_OUT (PORTL)
#define DATA_IN  (PINL)
#define ADDR_H   (PINC)
#define ADDR_L   (PINA)
#define ADDR     ((ADDR_H << 8) | ADDR_L)

#define DATA_DIR   DDRL
#define ADDR_H_DIR DDRC
#define ADDR_L_DIR DDRA

#define RD_N    (PINB & 0x01)
#define WR_N    (PING & 0x02)
#define MREQ_N  (PING & 0x01)
#define IORQ_N  (PING & 0x04)

namespace ut88
{
	class Z80
	{
	public:
        enum Pin : uint8_t
        {
            PIN_RESET_N = 38,
            PIN_CLK     = 52,

            PIN_MREQ_N  = 41,
            PIN_IORQ_N  = 39,

            PIN_RD_N    = 53,
            PIN_WR_N    = 40,

            PIN_INT_N   = 50,
            PIN_NMI_N   = 51,
        };


        inline __attribute__((always_inline))
        static void Init()
        {
            DATA_DIR = 0x00;
            ADDR_H_DIR = 0x00;
            ADDR_L_DIR = 0x00;

            // pinMode(Pin::PIN_RD_N, INPUT);   // PB0
            // pinMode(Pin::PIN_CLK, OUTPUT);   // PB1
            // pinMode(Pin::PIN_NMI_N, OUTPUT); // PB2
            // pinMode(Pin::PIN_INT_N, OUTPUT); // PB3
            DDRB &= 0xFE;
            DDRB |= 0x0E;
            
            // pinMode(Pin::PIN_RESET_N, OUTPUT);   // PD7
            DDRD |= 0x80;

            // pinMode(Pin::PIN_MREQ_N, INPUT);     // PG0
            // pinMode(Pin::PIN_WR_N, INPUT);       // PG1
            // pinMode(Pin::PIN_IORQ_N, INPUT);     // PG2
            DDRG &= 0xF8;

            // Also, I use D17 (PH0) to request Z80 reset
            DDRH &= 0xFE;
            PORTH |= 0x01;

            pinMode(4, INPUT_PULLUP);

            Reset();
        }


        inline __attribute__((always_inline))
        static void Reset()
        {
            DATA_DIR = 0x00;

            cli();

            SetReset();
            ResetInt();
            ResetNmi();

            ResetClock();
            delayMicroseconds(1);

            SetClock();
            delayMicroseconds(1);

            ResetClock();
            delayMicroseconds(1);

            SetClock();
            delayMicroseconds(1);

            ResetClock();
            delayMicroseconds(1);

            ResetReset();

            SetClock();
            
            sei();
        }


        inline __attribute__((always_inline))
        static void SetClock()
        {
            PORTB |= 0x02;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, HIGH);
        }

        inline __attribute__((always_inline))
        static void ResetClock()
        {
            PORTB &= 0xFD;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, LOW);
        }


        inline __attribute__((always_inline))
        static void SetReset()
        {
            PORTD &= 0x7F;      // digitalWrite(Pin::PIN_RESET_N, LOW);
        }

        inline __attribute__((always_inline))
        static void ResetReset()
        {
            PORTD |= 0x80;      // digitalWrite(ut88::Z80::Pin::PIN_RESET_N, HIGH);
        }


        inline __attribute__((always_inline))
        static void SetInt()
        {
            PORTB &= 0xF7;      // digitalWrite(ut88::Z80::Pin::PIN_INT_N, LOW);
        }

        inline __attribute__((always_inline))
        static void ResetInt()
        {
            PORTB |= 0x08;      // digitalWrite(ut88::Z80::Pin::PIN_INT_N, HIGH);
        }


        inline __attribute__((always_inline))
        static void SetNmi()
        {
            PORTB &= 0xFB;      // digitalWrite(ut88::Z80::Pin::PIN_NMI_N, LOW);
        }

        inline __attribute__((always_inline))
        static void ResetNmi()
        {
            PORTB |= 0x04;      // digitalWrite(ut88::Z80::Pin::PIN_NMI_N, HIGH);
        }
	};
}
