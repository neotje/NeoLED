Open with the platformio-ide extension in visual studio code.

# NeoLED

LED strip animation manager and API server for ESP32.

## snippets

Animation

```c++
struct name_of_animation_config
{
    CRGB color = CRGB::White;
}

namespace name_of_animation
{
    String name = "animation name";
    simple_color_config config; // current config.
    simple_color_config emptyConfig; // empty config (used to reset the current config).
    Cjson json; // json document for the api responses.

    // animation task will be executed on the second core.
    void animationTask(void *par)
    {
        get_state;
        get_config(name_of_animation_config);

        while (state.run)
        {
            // animation loop
        }

        stop_task;
    }

    // function to handle http get request and send back list of options.
    void onAvailableOptionsAPIrequest()
    {
        json.clear();
        //             key  value type
        json.add_key("color", "RGB");
        json.send();
        /* response
        {
            "color": "RGB"
        }
        */
    }

    // function the handle the api request for the current configuration values of the animation.
    void onGetOptionsAPIrequest() {
        json.clear();
        json.document["color"]["r"] = config.color.r;
        json.document["color"]["g"] = config.color.g;
        json.document["color"]["b"] = config.color.b;
        json.send();
        /*
        {
            "color": {
                "r": 255,
                "g": 255,
                "b": 255
            }
        }
        */
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

            if (json.has_key("color"))
            {
                CRGB c(
                    (int)json.document["color"]["r"],
                    (int)json.document["color"]["g"],
                    (int)json.document["color"]["b"]
                );
                config.color = c;
            }
        }

        // don't forget to send back ok status.
        APIserver.sendOK();
    }

    // function to handle the animation start request.
    void onStartAPIrequest()
    {
        Manager.start("simple_colors"); // don't need to set the configuration because we already have a pointer to this configuration in the init function.
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
            onStartAPIrequest
        );
    }
}
```
