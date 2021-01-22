#ifndef api_h
#define api_h

#include <Arduino.h>
#include <config.hpp>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

void ConnectToWiFi()
{
#ifdef VERBOSE
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
#endif

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
#ifdef VERBOSE
        Serial.print(".");
#endif
        delay(500);
    }

#ifdef VERBOSE
    Serial.print("\nConnected. IP: ");
    Serial.println(WiFi.localIP());
#endif
}

class CAPIServer : public WebServer
{
public:
    CAPIServer(int p = 80) : WebServer(p) {}
    ~CAPIServer() {}

    void registerAnimationAPI(String name, THandlerFunction optionsGetCallback, THandlerFunction optionsPostCallback, THandlerFunction startCallback)
    {
        if (name.indexOf(" ") > -1)
        {
            Serial.println("(ERROR) animation string can't contain any spaces when used for api path");
            return;
        }

        on(String("/options/get/") + name, optionsGetCallback);
        on(String("/options/set/") + name, HTTP_POST, optionsPostCallback);
        on(String("/start/") + name, startCallback);

#ifdef VERBOSE
        Serial.println(String("API server registered: ") + name);
#endif
    }

    void onList(THandlerFunction onGetList)
    {
        on("/list", onGetList);
    }

    void onStop(THandlerFunction onStopCurrent)
    {
        on("/stop", onStopCurrent);
    }

    void sendOK()
    {
        send(200, "application/json", "{status:ok}");
    }
};

CAPIServer APIserver(80);

#endif
