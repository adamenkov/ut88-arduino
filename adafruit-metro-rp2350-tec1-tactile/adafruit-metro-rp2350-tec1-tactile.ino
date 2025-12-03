// 16-bit Adafruit_GFX-compatible framebuffer for RP2350 HSTX

#include <Adafruit_dvhstx.h>

#if defined(ADAFRUIT_FEATHER_RP2350_HSTX)
DVHSTXPinout pinConfig = ADAFRUIT_FEATHER_RP2350_CFG;
#elif defined(ADAFRUIT_METRO_RP2350)
DVHSTXPinout pinConfig = ADAFRUIT_METRO_RP2350_CFG;
#elif (defined(ARDUINO_RASPBERRY_PI_PICO_2) || defined(ARDUINO_RASPBERRY_PI_PICO_2W))
DVHSTXPinout pinConfig = ADAFRUIT_HSTXDVIBELL_CFG;
#else
// If your board definition has PIN_CKP and related defines,
// DVHSTX_PINOUT_DEFAULT is available
DVHSTXPinout pinConfig = DVHSTX_PINOUT_DEFAULT;
#endif


DVHSTX8 display(pinConfig, DVHSTX_RESOLUTION_640x480);
// If you get the message "error: 'DVHSTX_PINOUT_DEFAULTx' was not declared"
// then you need to give the pins numbers explicitly, like the example below.
// The order is: {CKP, D0P, D1P, D2P} DVHSTX16 display({12, 14, 16, 18},
// DVHSTX_RESOLUTION_320x240);

const uint8_t font[2048*6] =
{
//#define INV_BIT_TO_BYTE(x, bit) ((x & (1 << bit)) ? 0x00 : 0xDF)
//#define U(x)    INV_BIT_TO_BYTE(x, 5), INV_BIT_TO_BYTE(x, 4), INV_BIT_TO_BYTE(x, 3),\
                INV_BIT_TO_BYTE(x, 1), INV_BIT_TO_BYTE(x, 1), INV_BIT_TO_BYTE(x, 0)
#define U(x)    uint8_t((~x) << 2)
#include "ut88font.h"
};


int display_left;
int display_top;


enum
{
    TEXT_DISPLAY_WIDTH = 64,
    TEXT_DISPLAY_HEIGHT = 32,   // Change this to 30 if you want to try to use a 400x240 mode

    CHARACTER_WIDTH = 6,
    CHARACTER_HEIGHT = 8,
};




#include "ut88.h"
UT88 ut88(display);


void setup() {
  gpio_init_mask(0xC000'01FF);
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
  PULLUP(8);
  PULLUP(30); PULLUP(31); PULLUP(41); PULLUP(42); PULLUP(43); PULLUP(44); PULLUP(45); PULLUP(46);
#undef PULLUP

  Serial.begin(115200);
  // while(!Serial);
  if (!display.begin()) { // Blink LED if insufficient RAM
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) {
      digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
      Serial.println("display initialization error");
    }
  }
  Serial.println("display initialized");

  display_left = display.width() / 2 - TEXT_DISPLAY_WIDTH * CHARACTER_WIDTH / 2;
  display_top = display.height() / 2 - TEXT_DISPLAY_HEIGHT * CHARACTER_HEIGHT / 2;

  ut88.reset();
}


void loop() {
  //static uint16_t counter;
  if (ut88.tick())
  {
    /*
    if (++counter % 10000 == 0) {
      Serial.println(ut88.disassemble(ut88.pc).c_str());
    }
    */
  }
  else
  {
    for (;;)
    {
      Serial.print("Unimplemented instruction: ");
      Serial.println(ut88.disassemble(ut88.pc).c_str());

      sleep_ms(1'000);
    }
  }
}
