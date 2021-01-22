#include <config.h>
#include <AnimationManager.h>

extern CRGB leds[];

struct rainbow_config
{
    uint8_t initialHue = 0;
    uint8_t deltaHue = 8;
    uint8_t hueDensity = 4; // aka speed of some sort.
};
void rainbow(void *par)
{
    get_state;
    get_config(rainbow_config);

    uint8_t initialHue = config.initialHue;

    while (state.run)
    {
        fill_rainbow(leds, LED_COUNT, initialHue += config.hueDensity, config.deltaHue);
        FastLED.show();
    }

    stop_task;
}