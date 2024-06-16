#pragma once

#include <LiquidCrystal.h>


class LcdKeypadShield
{
public:
    enum class Button : uint8_t { Select, Left, Down, Up, Right, None };


    inline __attribute__((always_inline))
    static Button GetPressedButton()
    {
        static const int analogKeys[] = { 760, 535, 360, 300, 30 };
        
        const int analogKey = analogRead(A0);
        
        for (int8_t i = sizeof analogKeys / sizeof analogKeys[0]; --i >= 0;)
        {
            if (analogKey <= analogKeys[i])
            {
                return static_cast<Button>(i);
            }
        }

        return Button::None;
    }


    inline __attribute__((always_inline))
    static LiquidCrystal& GetLcd()
    {
        return lcd;
    }


private:
    static LiquidCrystal lcd;
};

const uint8_t rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal LcdKeypadShield::lcd(rs, en, d4, d5, d6, d7);
