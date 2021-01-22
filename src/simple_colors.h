#include <config.h>
#include <AnimationManager.h>

extern CRGB leds[];

struct simple_color_config
{
    CRGB color = CRGB::White;
};
void simple_color(void *par)
{
    get_state;
    get_config(simple_color_config);
    
    while (state.run)
    {
        FastLED.showColor(config.color);
    }
    
    stop_task;
}
