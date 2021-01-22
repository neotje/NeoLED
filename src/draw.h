#ifndef draw_h
#define draw_h

#include <config.h>
#include <AnimationManager.h>

extern CRGB leds[];

CRGB ColorFraction(CRGB c, float f)
{
    f = min(1.0f, f);
    return CRGB(c).fadeToBlackBy(255 * (1.0f - f));
}

void DrawPixels(float pos, float length, CRGB color)
{
    float availFirstPixel = 1.0f - (pos - (long)(pos));
    float amtFirstPixel = min(availFirstPixel, length); // amount to fade pixel.
    float remaining = min(length, LED_COUNT - pos);
    int iPos = pos;

    // draw first pixel
    if (remaining > 0.0f)
    {
        leds[iPos++] += ColorFraction(color, amtFirstPixel);
        remaining -= amtFirstPixel;
    }

    // draw all full pixels
    while (remaining > 1.0f)
    {
        leds[iPos++] += color;
        remaining--;
    }

    // draw last pixel
    if (remaining > 0.0f)
    {
        leds[iPos] += ColorFraction(color, remaining);
    }
}

void clearPixels() {
    FastLED.clear();
    FastLED.showColor(CRGB::Black);
}

class Performance
{
private:
    /* data */
public:
    unsigned long start = millis();
    unsigned long end = millis();
    float dt = 0.0f;

    Performance() {}
    ~Performance() {}

    float mark()
    {
        end = millis();
        dt = (end - start) / 1000.0;
        start = millis();

        return dt;
    }
};

#endif