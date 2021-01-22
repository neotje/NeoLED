#include <config.hpp>
#include <AnimationManager.hpp>
#include <api.hpp>
#include <json.hpp>

extern CRGB leds[];

struct simple_color_config
{
    CRGB color = CRGB::White;
};

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
        }

        stop_task;
    }

    // function to handle http get request and send back list of options.
    void onGetOptionsAPIrequest()
    {
        json.clear();
        json.add_key("color", "RGB");
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
        APIserver.registerAnimationAPI(name, onGetOptionsAPIrequest, onPostOptionsAPIrequest, onStartAPIrequest);
    }

} // namespace simple_colors