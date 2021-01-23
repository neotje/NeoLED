#include <config.hpp>
#include <AnimationManager.hpp>
#include <json.hpp>

extern CRGB leds[];

struct rainbow_config
{
    uint8_t initialHue = 0;
    uint8_t deltaHue = 8;
    uint8_t hueDensity = 4; // aka speed of some sort.
};

namespace rainbow
{
    String name = "rainbow";
    rainbow_config config;      // current config.
    rainbow_config emptyConfig; // empty config (used to reset the current config).
    Cjson json;                 // json document for the api responses.

    // animation task will be executed on the second core.
    void animationTask(void *par)
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

    // function to handle http get request and send back list of options.
    void onAvailableOptionsAPIrequest()
    {
        json.clear();
        json.add_key("initialHue", "int");
        json.add_key("deltaHue", "int");
        json.add_key("hueDensity", "int");
        json.send();
    }

    // function the handle the api request for the current configuration values of the animation.
    void onGetOptionsAPIrequest()
    {
        json.clear();
        json.add_key("initialHue", config.initialHue);
        json.add_key("deltaHue", config.deltaHue);
        json.add_key("hueDensity", config.hueDensity);
        json.send();
    }

    // function to handle configuration set and reset
    void onPostOptionsAPIrequest()
    {
        // if request is empty set config to emptyconfig.
        if (!APIserver.hasArg("plain"))
        {
            config = emptyConfig;
        }
        else
        {
            json.fromPost(); // read json from post body.

            if (json.has_key("initialHue"))
                config.initialHue = (int)json.document["initialHue"];
            if (json.has_key("deltaHue"))
                config.deltaHue = (int)json.document["deltaHue"];
            if (json.has_key("hueDensity"))
                config.hueDensity = (int)json.document["hueDensity"];
        }

        APIserver.sendOK();
    }

    // function to handle the animation start request.
    void onStartAPIrequest()
    {
        Manager.start(name); // don't need to set the configuration because we already have a pointer to this configuration in the init function.
        APIserver.sendOK();
    }

    // function called in setup() to register the needed callbacks and variables.
    void init()
    {
        Manager.addToList(Animation(name, animationTask, &config)); // add animation to animationManager.
        // register request handlers.
        APIserver.registerAnimationAPI(
            name,
            onAvailableOptionsAPIrequest,
            onGetOptionsAPIrequest,
            onPostOptionsAPIrequest,
            onStartAPIrequest);
    }
}