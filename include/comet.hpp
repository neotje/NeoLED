#include <config.hpp>
#include <AnimationManager.hpp>
#include <draw.hpp>
#include <json.hpp>

extern CRGB leds[];

struct comet_config
{
    byte fadeAmt = 42;                                // Fraction fo 256 to fade a pixel by.
    int cometSize = 5;                                // comet width.
    int deltaHue = 8;                                 // how far to step the hue.
    int direction = 1;                                // starting direction.
    float speed = 40;                                 // delay between each cycle.
    int fadeRandom = 2;                               // the chance a pixel gets faded.
    int cometCount = 3;                               // amount of comets.
    int cometDistance = LED_COUNT / (cometCount - 1); // distance between each comet
};

struct comet_t
{
    float pos = 0;
    int dir = 1;
};

namespace comet
{
    String name = "comet";
    comet_config config;      // current config.
    comet_config emptyConfig; // empty config (used to reset the current config).
    Cjson json;               // json document for the api responses.

    // animation task will be executed on the second core.
    void animationTask(void *par)
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

    // function to handle http get request and send back list of options.
    void onAvailableOptionsAPIrequest()
    {
        json.clear();
        json.add_key("fadeAmt", "int");
        json.add_key("cometSize", "int");
        json.add_key("deltaHue", "int");
        json.add_key("direction", "int");
        json.add_key("speed", "float");
        json.add_key("fadeRandom", "int");
        json.add_key("cometCount", "int");
        json.add_key("cometDistance", "int");
        json.send();
    }

    // function the handle the api request for the current configuration values of the animation.
    void onGetOptionsAPIrequest()
    {
        json.clear();
        json.add_key("fadeAmt", config.fadeAmt);
        json.add_key("cometSize", config.cometSize);
        json.add_key("deltaHue", config.deltaHue);
        json.add_key("direction", config.direction);
        json.add_key("speed", config.speed);
        json.add_key("fadeRandom", config.fadeRandom);
        json.add_key("cometCount", config.cometCount);
        json.add_key("cometDistance", config.cometDistance);
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

            if (json.has_key("fadeAmt"))
                config.fadeAmt = (byte)json.document["fadeAmt"];

            if (json.has_key("cometSize"))
                config.cometSize = (int)json.document["cometSize"];

            if (json.has_key("deltaHue"))
                config.deltaHue = (int)json.document["deltaHue"];

            if (json.has_key("direction"))
                config.direction = (int)json.document["direction"];

            if (json.has_key("speed"))
                config.speed = (float)json.document["speed"];

            if (json.has_key("fadeRandom"))
                config.fadeRandom = (int)json.document["fadeRandom"];

            if (json.has_key("cometCount"))
                config.cometCount = (int)json.document["cometCount"];

            if (json.has_key("cometDistance"))
                config.cometDistance = (int)json.document["cometDistance"];
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
} // namespace comet