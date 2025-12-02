void setup()
{
    gpio_init_mask(0xC000'00FF);
    gpio_init(41);
    gpio_init(42);
    gpio_init(43);
    gpio_init(44);
    gpio_init(45);
    gpio_init(46);

    //DDRF = 0xFF;
    gpio_set_dir_out_masked(0xFF);

    //DDRK = 0x00;
    //PORTK = 0xFF;
#define PULLUP(pin) gpio_set_dir(pin, false); gpio_pull_up(pin)
    PULLUP(30); PULLUP(31); PULLUP(41); PULLUP(42); PULLUP(43); PULLUP(44); PULLUP(45); PULLUP(46);
#undef PULLUP

    Serial.begin(115200);
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
        static uint8_t portF[] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };

        //PORTF = ~(1 << row);
        gpio_put_masked(0xFF, portF[row]);

        delay(1);

        //uint8_t k = ~PINK;
        uint8_t k = ~(gpio_get(30) | (gpio_get(31) << 1) | ((gpio_get_all64() >> 39) & 0xFC));

        if (k != 0x00)
        {
            Serial.print("~k=");
            Serial.print(uint8_t(~k), HEX);
            Serial.print(", log(k)=");
            Serial.print(log(k));
            Serial.print(", row=");
            Serial.print(row);
            Serial.print(", name=");

            uint8_t key = (row << 3) | log(k);

            static const char* keys[] =
            {
                    "US", "RI", "LE", "DO", "UP", ".", ",", "SS",   "B", "\\", "SP", ";",   "", "_", "J", "RUS",
                    "4", "3", "2", "1",     "0", "/", "@", ":",     "", "V", "", "9",       "8", "7", "6", "5",
                    "W", "S", "I", "F",     "", "", "", "-",        "D", "L", "O", "]",     "R", "P", "A", "K",
                    "N", "Y", "E", "C",     "H", "Z", "T", "X",     "CR", "", "Q", "G",     "^", "U", "M", "[",
            };
            Serial.print(keys[key]);
            Serial.print(", A = ");
            
            static const uint8_t port55a[] =
            {
#define A(bus_wire)   uint8_t(~(1 << (bus_wire - 1)))
                    0xFF, A(8), A(8), A(8),     A(8), A(3), A(2), 0xFF,     A(3), A(7), A(7), A(2),     0xFF, A(7), A(4), 0xFF,
                    A(1), A(1), A(1), A(1),     A(1), A(3), A(3), A(2),     0xFF, A(6), 0xFF, A(2),     A(2), A(2), A(1), A(1),
                    A(6), A(6), A(4), A(4),     0xFF, 0xFF, 0xFF, A(2),     A(3), A(5), A(5), A(7),     A(5), A(5), A(3), A(4),
                    A(5), A(6), A(4), A(3),     A(4), A(7), A(6), A(6),     A(8), 0xFF, A(5), A(4),     A(7), A(6), A(5), A(7),
            };
            Serial.print(port55a[key], HEX);
            Serial.print(", B = ");
            
            static const uint8_t port55b[] =
            {
#define B(bus_wire) uint8_t(~(1 << (bus_wire - 9)))
                     0xFF, B( 9), B(10), B(12),     B(11), B( 9), B(14),  0xFF,     B(13), B(11), B(15), B(13),      0xFF, B(14), B(14),  0xFF,
                    B(13), B(12), B(11), B(10),     B( 9), B(10), B(11), B(12),      0xFF, B(12),  0xFF, B(11),     B(10), B( 9), B(15), B(14),
                    B(13), B( 9), B(13), B(10),      0xFF,  0xFF,  0xFF, B(15),     B(15), B( 9), B(12), B(12),     B(15), B(13), B(12), B(15),
                    B(11), B(15), B( 9), B(14),     B(12), B( 9), B(10), B(14),     B(13),  0xFF, B(14), B(11),     B(13), B(11), B(10), B(10),

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

    delay(1000);
}
