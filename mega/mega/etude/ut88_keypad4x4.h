#pragma once


namespace ut88
{
	class Keypad4x4_portK
	{
	public:
		static uint8_t getKeyCode()
		{
			PORTK = 0xF7;
			uint8_t PORTK_value = PINK;

			if (PORTK_value == 0xF7)
			{
				PORTK = 0xFB;
				PORTK_value = PINK;
			}

			if (PORTK_value == 0xFB)
			{
				PORTK = 0xFD;
				PORTK_value = PINK;
			}

			if (PORTK_value == 0xFD)
			{
				PORTK = 0xFE;
				PORTK_value = PINK;
			}

			PORTK = 0xFF;   // to prevent occasional key misreads

			static uint8_t keypadLU[256] =
			{
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,

				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  3,    0,  0,  0,  7,    0, 11, 15,  0,

				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  2,    0,  0,  0,  6,    0, 10, 14,  0,

				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,
				 0,  0,  0,  0,    0,  0,  0,  1,    0,  0,  0,  5,    0,  9, 13,  0,
				 0,  0,  0,  0,    0,  0,  0, 16,    0,  0,  0,  4,    0,  8, 12,  0,
				 0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0
			};

			return keypadLU[PORTK_value];
		}

    private:
        static struct S
        {
            S() { DDRK = 0x0F; }
        } s;
	};

    Keypad4x4_portK::S Keypad4x4_portK::s;
}
