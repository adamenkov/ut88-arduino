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


        static void Init()
        {
            DATA_DIR = 0x00;
            ADDR_H_DIR = 0x00;
            ADDR_L_DIR = 0x00;

            pinMode(Pin::PIN_RESET_N, OUTPUT);
            pinMode(Pin::PIN_CLK, OUTPUT);

            pinMode(Pin::PIN_MREQ_N, INPUT);
            pinMode(Pin::PIN_IORQ_N, INPUT);

            pinMode(Pin::PIN_RD_N, INPUT);
            pinMode(Pin::PIN_WR_N, INPUT);

            pinMode(Pin::PIN_INT_N, OUTPUT);
            pinMode(Pin::PIN_NMI_N, OUTPUT);

            Reset();
        }


        inline __attribute__((always_inline))
        static void Reset()
        {
            DATA_DIR = 0x00;

            cli();

            PORTD &= 0x7F;      // digitalWrite(Pin::PIN_RESET_N, LOW);
            PORTB |= 0x0C;      // digitalWrite(Pin::PIN_INT_N, HIGH); digitalWrite(Pin::PIN_NMI_N, HIGH);

            PORTB &= 0xFD;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, LOW);
            delayMicroseconds(1);

            PORTB |= 0x02;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, HIGH);
            delayMicroseconds(1);

            PORTB &= 0xFD;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, LOW);
            delayMicroseconds(1);

            PORTB |= 0x02;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, HIGH);
            delayMicroseconds(1);

            PORTB &= 0xFD;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, LOW);
            delayMicroseconds(1);

            PORTB |= 0x02;      // digitalWrite(ut88::Z80::Pin::PIN_CLK, HIGH);

            PORTD |= 0x80;      // digitalWrite(ut88::Z80::Pin::PIN_RESET_N, HIGH);
            
            sei();
        }
	};
}
