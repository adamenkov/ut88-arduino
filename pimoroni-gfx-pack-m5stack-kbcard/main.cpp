#include "pico/stdlib.h"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/gfx_pack/gfx_pack.hpp"
#include "pimoroni_i2c.hpp"

#include "ut88_minimal.h"

using namespace pimoroni;

UT88 ut88;

ST7567 st7567(128, 64, GfxPack::gfx_pack_pins);
PicoGraphics_Pen1Bit graphics(st7567.width, st7567.height, nullptr);
RGBLED backlight_rgb(GfxPack::BL_R, GfxPack::BL_G, GfxPack::BL_B, Polarity::ACTIVE_HIGH);
Button button_a(GfxPack::A);
Button button_b(GfxPack::B);
Button button_c(GfxPack::C);
Button button_d(GfxPack::D);
Button button_e(GfxPack::E);

// Create I2C instance for i2c0 on GPIO 4 (SDA), GPIO 5 (SCL)
I2C i2c(4, 5, 100000);  // 100kHz is fine for CardKB
constexpr uint8_t CARDKB_ADDR = 0x5F;


int main()
{
    sleep_ms(100);
    st7567.set_backlight(128); // 0 to 255
    graphics.set_font("bitmap8");

    word kb_counter = 0;

    ut88.mmio_display.bytes[0] = 0xFF;
    ut88.mmio_display.bytes[1] = 0xFF;
    ut88.mmio_display.bytes[2] = 0xFF;

    ut88.redraw = true;


    for (;;)
    {
        if (++kb_counter == 0)
        {
            if (ut88.kb_byte == 0)
            {
                byte b = 0;
                int result = i2c.read_blocking(CARDKB_ADDR, &b, size_t(1), false);
                ut88.kb_byte = result == 1 ? b : 0;
            }
        }

        if (ut88.tick())
        {
            if (ut88.redraw)
            {
                ut88.redraw = false;

                graphics.set_pen(0);
                graphics.clear();
                graphics.set_pen(15);

                static int offset[]{45, 20, 0};
                for (int i = 0; i < 3; ++i)
                {
                    graphics.text(hex_byte[ut88.mmio_display.bytes[i]], {offset[i], 0}, 128);
                }

                st7567.update(&graphics);
            }
        }
        else
        {
            graphics.text("Unimplemented instruction", {0, 20}, 128, 1);
            
            graphics.text(ut88.disassemble(ut88.pc), {0, 40}, 128, 1);

            st7567.update(&graphics);

            while (!button_a.raw())
            {
            }
            
            break;
        }
    }
}
