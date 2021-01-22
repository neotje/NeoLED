#include <config.h>
#include <AnimationManager.h>
#include <draw.h>

struct marquee_config
{
    float speed = 5.0f;
    int length = 5;
    int empty = 2;
    bool smooth = true;
};
void marquee(void *par)
{
    get_state;
    get_config(marquee_config);

    float scroll = 0.0f;
    float j = 0.0f;
    byte k = 0;

    Performance loop;

    while (state.run)
    {
        loop.mark();

        FastLED.clear();

        scroll += config.speed * loop.dt;
        if (scroll > (float)config.length)
            scroll -= (float)config.length;

        j += 40.0 * loop.dt;
        k = j;
        CRGB c;

        for (float i = scroll; i < LED_COUNT; i += (float)config.length)
        {
            for (int j = 0; j < config.length - config.empty; j++)
            {
                if (config.smooth)
                    DrawPixels(i + (float)j, 1, c.setHue(k += 800.0 * loop.dt));
                else
                    DrawPixels(LED_COUNT - 1 - (int)i, 1, c.setHue(k += 8));
            }
        }

        FastLED.show();
    }

    stop_task;
}
