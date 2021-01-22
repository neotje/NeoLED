#include <config.h>
#include <AnimationManager.h>

extern CRGB leds[];

struct twinkle_config
{
    int speed = 200;
    int on = LED_COUNT / 4;
};
void twinkle(void *par)
{
#define TWINKLE_NUM_COLORS 7

    get_state;
    get_config(twinkle_config);

    static const CRGB colors[TWINKLE_NUM_COLORS] = {
        CRGB::Red,
        CRGB::Blue,
        CRGB::Green,
        CRGB::Purple,
        CRGB::Gold,
        CRGB::Orange,
        CRGB::Silver};

    while (state.run)
    {
        FastLED.clear(false);

        for (int i = 0; i < config.on; i++)
        {
            leds[random(LED_COUNT)] = colors[random(TWINKLE_NUM_COLORS)];
            FastLED.show();
            delay(config.speed);
        }
    }

    stop_task;
}
