#pragma once


namespace ut88
{
	class Keyboard
	{
	public:
        inline __attribute__((always_inline))
        static void Poll()
        {
            portA = 0xFF;
            portB = 0x7F;
            portC = 0x07;

            for (int row = 0; row < 8; ++row)
            {
                static uint8_t portF[] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };
                PORTF = portF[row];
                delayMicroseconds(1);
                uint8_t k = PINK;
                if (k == 0xFF)
                {
                    continue;
                }

                /*
                static const char* keys[] =
                {
                    "US", "RI", "LE", "DO", "UP", ".", ",", "SS",   "B", "\\", "SP", ";",   "", "_", "J", "RUS",
                    "4", "3", "2", "1",     "0", "/", "@", ":",     "", "V", "", "9",       "8", "7", "6", "5",
                    "W", "S", "I", "F",     "", "", "", "-",        "D", "L", "O", "]",     "R", "P", "A", "K",
                    "N", "Y", "E", "C",     "H", "Z", "T", "X",     "CR", "", "Q", "G",     "^", "U", "M", "[",
                };
                */
                
                if (row == 0)
                {
                    if ((k & 0x80) == 0)
                    {
                        portC &= 0x03;
                        k |= 0x80;
                    }
                    if ((k & 0x01) == 0)
                    {
                        portC &= 0x05;
                        ++k;
                    }
                }
                else if ((row == 1) && ((k & 0x80) == 0))
                {
                    portC &= 0x06;
                    k |= 0x80;
                }

                int8_t log_k = log_of_inv(k);
                if (log_k < 0)
                {
                    continue;

                }
                uint8_t key = (row << 3) | log_k;

                static const uint8_t port55a[] =
                {
    #define A(bus_wire)   (~(1 << (bus_wire - 1)))
                    0xFF, A(8), A(8), A(8),     A(8), A(3), A(2), 0xFF,     A(3), A(7), A(7), A(2),     0xFF, A(7), A(4), 0xFF,
                    A(1), A(1), A(1), A(1),     A(1), A(3), A(3), A(2),     0xFF, A(6), 0xFF, A(2),     A(2), A(2), A(1), A(1),
                    A(6), A(6), A(4), A(4),     0xFF, 0xFF, 0xFF, A(2),     A(3), A(5), A(5), A(7),     A(5), A(5), A(3), A(4),
                    A(5), A(6), A(4), A(3),     A(4), A(7), A(6), A(6),     A(8), 0xFF, A(5), A(4),     A(7), A(6), A(5), A(7),
                };
                portA = port55a[key];
                
                static const uint8_t port55b[] =
                {
    #define B(bus_wire) (~(1 << (bus_wire - 9)))
                     0xFF, B( 9), B(10), B(12),     B(11), B( 9), B(14),  0xFF,     B(13), B(11), B(15), B(13),      0xFF, B(14), B(14),  0xFF,
                    B(13), B(12), B(11), B(10),     B( 9), B(10), B(11), B(12),      0xFF, B(12),  0xFF, B(11),     B(10), B( 9), B(15), B(14),
                    B(13), B( 9), B(13), B(10),      0xFF,  0xFF,  0xFF, B(15),     B(15), B( 9), B(12), B(12),     B(15), B(13), B(12), B(15),
                    B(11), B(15), B( 9), B(14),     B(12), B( 9), B(10), B(14),     B(13),  0xFF, B(14), B(11),     B(13), B(11), B(10), B(10),
                };
                portB = port55b[key];
            }
        }

        inline __attribute__((always_inline))
        static uint8_t GetPortA()
        {
            return portA;
        }
        
        inline __attribute__((always_inline))
        static uint8_t GetPortB()
        {
            return portB;
        }
        
        inline __attribute__((always_inline))
        static uint8_t GetPortC()
        {
            return portC;
        }
        
    private:
        static struct S
        {
            S()
            {
                DDRF = 0xFF;

                DDRK = 0x00;
                PORTK = 0xFF;
            }
        } s;


        inline __attribute__((always_inline))
        static uint8_t log_of_inv(uint8_t x)
        {
            static uint8_t log_inv[256] =
            {
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,

                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0x07,

                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0x06,

                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0x05,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0x04,
                0xFF, 0xFF, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF, 0x03,   0xFF, 0xFF, 0xFF, 0x02,  0xFF, 0x01, 0x00, 0xFF
            };

            return log_inv[x];
        }

        static uint8_t portA, portB, portC;
	};

    Keyboard::S Keyboard::s;

    uint8_t Keyboard::portA, Keyboard::portB, Keyboard::portC;
}   // namespace ut88
