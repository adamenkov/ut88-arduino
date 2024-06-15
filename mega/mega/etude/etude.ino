#include "lcd_keypad_shield.h"
LiquidCrystal& lcd = ut88::LcdKeypadShield::GetLcd();

#include "ut88_keypad4x4.h"


void setup()
{
    lcd.begin(16, 2);
}

void loop()
{
    const char* names[] = { "select", "left  ", "down  ", "up    ", "right " };
    
    const ut88::LcdKeypadShield::Button shieldButton = ut88::LcdKeypadShield::GetPressedButton();
    if (shieldButton != ut88::LcdKeypadShield::Button::None)
    {
        lcd.setCursor(0, 0);
        lcd.print(names[static_cast<int8_t>(shieldButton)]);
    }

    uint8_t v = ut88::Keypad4x4_portK::GetKeyCode();
    if (v != 0)
    {
        lcd.setCursor(0, 1);
        String s(v);
        s += ' ';
        lcd.print(s.c_str());
    }
}
