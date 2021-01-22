#include <config.hpp>
#include <AnimationManager.hpp>
#include <draw.hpp>

extern CRGB leds[];

struct comet_config
{
    byte fadeAmt = 24;                                // Fraction fo 256 to fade a pixel by.
    int cometSize = 5;                                // comet width.
    int deltaHue = 5;                                 // how far to step the hue.
    int direction = 1;                                // starting direction.
    float speed = 40;                                  // delay between each cycle.
    int fadeRandom = 2;                               // the chance a pixel gets faded.
    int cometCount = 3;                               // amount of comets.
    int cometDistance = LED_COUNT / (cometCount - 1); // distance between each comet
};

struct comet_t
{
    float pos = 0;
    int dir = 1;
};

void comet(void *par)
{
    get_state;
    get_config(comet_config);

    float hue = HUE_RED; // current color

    Performance loop;

    comet_t comets[config.cometCount];

    // initilize all the comets.
    for (int i = 0; i < config.cometCount; i++)
    {
        comets[i].dir = config.direction;
        comets[i].pos = i * config.cometDistance;
        if (config.direction == -1)
            comets[i].pos = LED_COUNT - (i * config.cometDistance);
    }

    while (state.run)
    {
        loop.mark();

        hue += float(config.deltaHue) * loop.dt;

        for (int i = 0; i < config.cometCount; i++)
        {
            comets[i].pos += comets[i].dir * config.speed * loop.dt;

            // flip direction
            if (comets[i].pos >= (LED_COUNT - config.cometSize))
                comets[i].dir = -1;

            if (comets[i].pos <= 0)
                comets[i].dir = 1;

            // draw comet
            DrawPixels(comets[i].pos, config.cometSize, CRGB().setHue(int(hue)));
        }

        // fade leds
        for (int j = 0; j < LED_COUNT; j++)
            if (random(config.fadeRandom) == 1)
                leds[j] = leds[j].fadeToBlackBy(config.fadeAmt);

        FastLED.show();
        delay(5);
    }

    stop_task;
}