#ifndef AnimationManager_h
#define AnimationManager_h

#include <Arduino.h>
#include <FastLED.h>

#define ANIMATION_ARR_LEN 50

#define get_state Animation_state &state = *(Animation_state *)par
#define get_config(type) type &config = *(type *)state.config
#define stop_task    \
    FastLED.clear(); \
    FastLED.show();  \
    vTaskDelete(NULL);

struct Animation
{
    String name;
    TaskFunction_t function;

    void *default_par;

    Animation() {}
    Animation(String _name, TaskFunction_t _func, void *_default_par) : name(_name), function(_func), default_par(_default_par) {}
};

struct Animation_state
{
    String name;
    bool run = false;
    void *config;
};

class AnimationManager
{
private:
    Animation animationsArr[ANIMATION_ARR_LEN];
    TaskHandle_t task0;
    Animation_state current_animation;
    int j = 0;

protected:
public:
    AnimationManager(){};
    ~AnimationManager(){};

    void addToList(Animation anim)
    {
        if (j >= ANIMATION_ARR_LEN)
            return;
        animationsArr[j] = anim;
        j++;
    }

    Animation* getList() {
        return animationsArr;
    }
    int listLength() {
        return j;
    }

    String currentRunning() {
        return current_animation.name;
    }

    void start(String name)
    {
        Animation *anim = getByName(name); // get animation struct
        start(name, anim->default_par);
    }
    void start(String name, void *par)
    {
        if (current_animation.run)
            return;
        
        Animation *anim = getByName(name); // get animation struct

        int len = name.length() + 1;
        char buf[len];
        name.toCharArray(buf, len);

        current_animation.run = true;
        current_animation.config = par;
        current_animation.name = name;

        xTaskCreate(
            anim->function,
            "animationTask",
            4096,
            &current_animation,
            0,
            &task0);
    }

    void stop()
    {
        current_animation.run = false;
        current_animation.name = "";
        delay(5);
    }

    bool keepRunning()
    {
        return current_animation.run;
    }

    Animation *getByName(String name)
    {
        for (int i = 0; i < ANIMATION_ARR_LEN; i++)
            if (animationsArr[i].name == name)
                return &animationsArr[i];
        return NULL;
    }
};

AnimationManager Manager;

#endif