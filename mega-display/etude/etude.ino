void setup()
{
    DDRF = 0xFF;

    DDRK = 0x00;
    PORTK = 0xFF;

    Serial.begin(9600);
}


uint8_t log(uint8_t power_of_2)
{
    for (int ret = 0; power_of_2 != 0; ++ret, power_of_2 >>= 1)
    {
        if (power_of_2 & 1)
        {
            return ret;
        }
    }

    return 0xFF;
}


void loop()
{
    for (int row = 0; row < 8; ++row)
    {
        PORTF = ~(1 << row);
        uint8_t k = ~PINK;
        if (k != 0x00)
        {
            uint8_t key = (row << 3) | log(k);

            static const char* keys[] =
            {
                "SS", ",", ".", "UP",  "DOWN", "LEFT", "RIGHT", "US",   "RUS", "J", "_", "",  ";", "SPACE", "\\", "B",
                ":", "@", "/", "0",  "1", "2", "3", "4",   "5", "6", "7", "8",  "9", "", "V", "",
                "-", "", "", "",  "F", "I", "S", "W",   "K", "A", "P", "R",  "]", "O", "L", "D",
                "X", "T", "Z", "H",  "C", "E", "Y", "N",   "[", "M", "U", "^",  "G", "Q", "", "CR",
            };
            Serial.print(keys[key]);
            Serial.print(", A = ");
            
            static const uint8_t port55a[] =
            {
#define A(bus_wire)   (~(1 << (bus_wire - 1)))
                0xFF, A(2), A(3), A(8),  A(8), A(8), A(8), 0xFF,   0xFF, A(4), A(7), 0xFF,  A(2), A(7), A(7), A(3),
                A(2), A(3), A(3), A(1),  A(1), A(1), A(1), A(1),   A(1), A(1), A(2), A(2),  A(2), 0xFF, A(6), 0xFF,
                A(2), 0xFF, 0xFF, 0xFF,  A(4), A(4), A(6), A(6),   A(4), A(3), A(5), A(5),  A(7), A(5), A(5), A(3),
                A(6), A(6), A(7), A(4),  A(3), A(4), A(6), A(5),   A(7), A(5), A(6), A(7),  A(4), A(5), 0xFF, A(8),
            };
            Serial.print(port55a[key]);
            Serial.print(", B = ");
            
            static const uint8_t port55b[] =
            {
#define B(bus_wire) (~(1 << (bus_wire - 9)))
                 0xFF, B(14), B( 9), B(11),  B(12), B(10), B( 9),  0xFF,    0xFF, B(14), B(14),  0xFF,  B(13), B(15), B(11), B(13),
                B(12), B(11), B(10), B( 9),  B(10), B(11), B(12), B(13),   B(14), B(15), B( 9), B(10),  B(11),  0xFF, B(12),  0xFF,
                B(15),  0xFF,  0xFF,  0xFF,  B(10), B(13), B( 9), B(13),   B(15), B(12), B(13), B(15),  B(12), B(12), B( 9), B(15),
                B(14), B(10), B( 9), B(12),  B(14), B( 9), B(15), B(11),   B(10), B(10), B(11), B(13),  B(11), B(14),  0xFF, B(13),
            };
            Serial.print(port55b[key]);
            Serial.print(", C = ");
            
            static const uint8_t port55c[] =
            {
                3, 7, 7, 7,  7, 7, 7, 5,   6, 7, 7, 7,  7, 7, 7, 7,
                7, 7, 7, 7,  7, 7, 7, 7,   7, 7, 7, 7,  7, 7, 7, 7,
                7, 7, 7, 7,  7, 7, 7, 7,   7, 7, 7, 7,  7, 7, 7, 7,
                7, 7, 7, 7,  7, 7, 7, 7,   7, 7, 7, 7,  7, 7, 7, 7,
            };
            Serial.println(port55c[key]);
        }
    }

    delay(150);
}
