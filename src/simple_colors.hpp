#include <config.hpp>
#include <AnimationManager.hpp>
#include <api.hpp>
#include <json.hpp>

extern CRGB leds[];

struct simple_color_config
{
    CRGB color = CRGB::White;
    int temp = 0;
};

const int temps[9] = {
    UncorrectedTemperature,
    ClearBlueSky,
    OvercastSky,
    DirectSunlight,
    HighNoonSun,
    CarbonArc,
    Halogen,
    Tungsten100W,
    Tungsten40W};

namespace simple_colors
{
    String name = "simple_colors";
    simple_color_config config;
    simple_color_config emptyConfig;
    Cjson json;

    void animationTask(void *par)
    {
        get_state;
        get_config(simple_color_config);

        while (state.run)
        {
            FastLED.showColor(config.color);
            FastLED.setCorrection(TypicalLEDStrip);
            FastLED.setTemperature(temps[config.temp]);
        }

        stop_task;
    }

    // function to handle http get request and send back list of options.
    void onAvailableOptionsAPIrequest()
    {
        json.clear();
        json.add_key("color", "RGB");
        json.add_key("temp", "int");
        json.send();
    }

    void onGetOptionsAPIrequest()
    {
        json.clear();
        json.document["color"]["r"] = config.color.r;
        json.document["color"]["g"] = config.color.g;
        json.document["color"]["b"] = config.color.b;
        json.document["temp"] = config.temp;
        json.send();
    }

    // function to handle
    void onPostOptionsAPIrequest()
    {
        if (!APIserver.hasArg("plain"))
        {
            config = emptyConfig;
        }
        else
        {
            json.fromPost();

            if (json.has_key("color"))
            {
                CRGB c(
                    (int)json.document["color"]["r"],
                    (int)json.document["color"]["g"],
                    (int)json.document["color"]["b"]);
                config.color = c;
            }

            if (json.has_key("temp"))
            {
                config.temp = (int)json.document["temp"];
            }
        }

        APIserver.sendOK();
    }

    void onStartAPIrequest()
    {
        Manager.start("simple_colors");
        APIserver.sendOK();
    }

    void init()
    {
        Manager.addToList(Animation(name, animationTask, &config));
        APIserver.registerAnimationAPI(name, onAvailableOptionsAPIrequest, onGetOptionsAPIrequest, onPostOptionsAPIrequest, onStartAPIrequest);
    }

} // namespace simple_colors
