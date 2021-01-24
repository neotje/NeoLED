#include <config.h>
#include <AnimationManager.hpp>
#include <draw.hpp>
#include <json.hpp>

struct marquee_config
{
    float speed = 5.0f;
    int hueDensity = 8;
    int hueSpeed = 3;
    int length = 5;
    int empty = 2;
    bool smooth = true;
};

namespace marquee
{
    String name = "marquee";
    marquee_config config;      // current config.
    marquee_config emptyConfig; // empty config (used to reset the current config).
    Cjson json;                 // json document for the api responses.

    // animation task will be executed on the second core.
    void animationTask(void *par)
    {
        get_state;
        get_config(marquee_config);

        float scroll = 0.0f;
        float j = 0.0f;
        float k = 0;

        Performance loop;

        while (state.run)
        {
            loop.mark();

            FastLED.clear();

            scroll += config.speed * loop.dt;
            if (scroll > (float)config.length)
                scroll -= (float)config.length;

            j += config.hueSpeed;
            k = j;
            CRGB c;

            for (float i = scroll - config.length - config.empty; i < LED_COUNT; i += (float)config.length)
            {
                for (int j = 0; j < config.length - config.empty; j++)
                {
                    if (config.smooth)
                        DrawPixels(i + (float)j, 1, c.setHue(k += config.hueDensity));
                    else
                        DrawPixels(LED_COUNT - 1 - (int)i + j, 1, c.setHue(k += config.hueDensity));
                }
            }

            FastLED.show();
            delay(5);
        }

        stop_task;
    }

    // function to handle http get request and send back list of options.
    void onAvailableOptionsAPIrequest()
    {
        json.clear();
        json.add_key("speed", "float");
        json.add_key("hueSpeed", "int");
        json.add_key("hueDensity", "int");
        json.add_key("length", "int");
        json.add_key("empty", "int");
        json.add_key("smooth", "bool");
        json.send();
    }

    // function the handle the api request for the current configuration values of the animation.
    void onGetOptionsAPIrequest()
    {
        json.clear();
        json.add_key("speed", config.speed);
        json.add_key("hueSpeed", config.hueSpeed);
        json.add_key("hueDensity", config.hueDensity);
        json.add_key("length", config.length);
        json.add_key("empty", config.empty);
        json.add_key("smooth", config.smooth);
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

            if (json.has_key("speed"))
                config.speed = (float)json.document["speed"];

            if (json.has_key("hueSpeed"))
                config.hueSpeed = (float)json.document["hueSpeed"];

            if (json.has_key("hueDensity"))
                config.hueDensity = (float)json.document["hueDensity"];
            
            if (json.has_key("length"))
                config.length = (int)json.document["length"];
            
            if (json.has_key("empty"))
                config.empty = (int)json.document["empty"];
            
            if (json.has_key("smooth"))
                config.smooth = (bool)json.document["smooth"];
        }

        // don't forget to send back ok status.
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
} // namespace marquee