#include <config.h>
#include <Arduino.h>
#include <esp32-hal-cpu.h>
#include <FastLED.h>
#include <AnimationManager.hpp>
#include <api.hpp>
#include <json.hpp>

#include <simple_colors.hpp>
#include <rainbow.hpp>
#include <marquee.hpp>
#include <comet.hpp>

CRGB leds[LED_COUNT];

void test(void *par)
{
  FastLED.setBrightness(255);
  while (true)
  {
    FastLED.showColor(CRGB::White);
    delay(1000);
    FastLED.showColor(CRGB::Black);
    delay(1000);
  }
}

struct acceleration_config
{
  float a = 0.5;
  float vmax = 2.0;
  float xmax = (float)(LED_COUNT / LED_PER_M);
  float xmin = 0.0;
  int trail = 5;
  float length = (float)(LED_COUNT / LED_PER_M);
  CRGB color = CRGB::White;
  bool startDir = true;
};
void acceleration(void *par)
{
  acceleration_config config = *(acceleration_config *)par;
  float v = 0.0;
  float x;

  if (config.startDir)
  {
    x = config.xmin;
  }
  else
  {
    x = config.xmax;
  }

  float dt;
  bool dir = config.startDir;
  int i = 0;

  unsigned long start = millis();
  unsigned long end = millis();

  for (;;)
  {
    config = *(acceleration_config *)par;
    end = millis();
    dt = (end - start) / 1000.0;
    start = millis();

    if (x < config.xmin)
    {
      dir = true;
    }
    if (x > config.xmax)
    {
      dir = false;
    }

    if (dir)
    {
      float dx = config.xmax - x;
      float s = (v / 2.0) * (v / config.a);

      if (s > dx)
      {
        v -= config.a * dt;
      }
      else if (v < config.vmax)
      {
        v += config.a * dt;
      }
    }
    else
    {
      float s = (v / 2.0) * (v / config.a);

      if (s > x - config.xmin)
      {
        v += config.a * dt;
      }
      else if (v > -config.vmax)
      {
        v -= config.a * dt;
      }
    }

    x += v * dt;

#ifdef VERBOSE
    Serial.print(x, 8);
    Serial.print(" ");
    Serial.println(v, 8);
#endif

    i = round((x / config.length) * (float)LED_COUNT);

    //FastLED.clear();
    if (i >= 0 && i <= LED_COUNT)
    {
      leds[i] = config.color;

      int tl = constrain((abs(v) / config.vmax) * config.trail, 0, config.trail); // tail length based on speed.

      if (dir)
      {
        leds[i - tl - 1] = CRGB::Black;
        leds[i - tl - 2] = CRGB::Black;
      }
      else
      {
        leds[i + tl + 1] = CRGB::Black;
        leds[i + tl + 2] = CRGB::Black;
      }

      for (size_t t = 1; t < tl + 1; t++)
      {
        int c_scale = (float(t) / float(tl)) * 255;
        if (dir)
        {
          leds[i - t] = config.color;
          leds[i - t].fadeToBlackBy(c_scale);
        }
        else
        {
          leds[i + t] = config.color;
          leds[i + t].fadeToBlackBy(c_scale);
        }
      }
    }
    FastLED.show();

    delay(5);
  }
}

struct colors_config
{
  int speed = 50;
};
void colors(void *par)
{
  colors_config config = *(colors_config *)par;

  int theta = 0;
  bool dir = true;

  while (true)
  {
    int r = sin8(theta);
    //int g = sin8(theta + 85);
    //int b = sin8(theta - 85);
    int g = sin8(theta - 127);
    int b = sin8(theta + 127);

    if (theta == 255)
    {
      dir = false;
    }
    if (theta == 0)
    {
      dir = true;
    }

    if (dir)
    {
      theta++;
    }
    else
    {
      theta--;
    }

    FastLED.showColor(CRGB(r, g, b));

#ifdef VERBOSE
    Serial.print(r);
    Serial.print(" ");
    Serial.print(g);
    Serial.print(" ");
    Serial.println(b);
#endif

    delay(config.speed);
  }
}

Cjson json;
void onGetList()
{
  Animation *list = Manager.getList();

  json.clear();

  for (int i = 0; i < Manager.listLength(); i++)
  {
    json.document["animations"][i] = list[i].name;
  }

  json.send();
}

void onGetCurrent()
{
  json.clear();
  json.document["name"] = Manager.currentRunning();
  json.send();
}

void onStop()
{
  Manager.stop();
  APIserver.sendOK();
}

void onSetBrightness()
{
  if (!APIserver.hasArg("plain"))
  {
    json.clear();
    json.add_key("brightness", FastLED.getBrightness());
    json.send();
  }
  else
  {
    json.fromPost();

    if (json.has_key("brightness") == true)
    {
      FastLED.setBrightness(constrain(json.document["brightness"], 0, 255));
    }

    APIserver.sendOK();
  }
}

void setup()
{
  Serial.begin(115200);

  Serial.print("CPU freq: ");
  Serial.println(getCpuFrequencyMhz());

  FastLED.addLeds<WS2813, LED_PIN, RGB>(leds, LED_COUNT);
  FastLED.showColor(CRGB::Black);
  FastLED.setBrightness(255);

  /*
  register animations;
  */
  simple_colors::init();
  rainbow::init();
  marquee::init();
  comet::init();

  ConnectToWiFi();

  // general api commands
  APIserver.onList(onGetList);
  APIserver.onStop(onStop);
  APIserver.onCurrent(onGetCurrent);
  APIserver.onBrightness(onSetBrightness);

  // start server
  APIserver.begin();
}

void loop()
{
  APIserver.handleClient();
}
