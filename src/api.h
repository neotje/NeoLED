#ifndef api_h
#define api_h

#include <Arduino.h>
#include <config.h>
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


class CAPIServer
{
private:
    WebServer api_server;
    int port = 80;

public:
    CAPIServer(int _port = 80) {
        port = _port;
    }
    ~CAPIServer() {}

    void start()
    {
#ifdef VERBOSE
        Serial.println("Starting API server.");
#endif
        api_server.begin(port);
    }

    void registerAnimation(String name, WebServer::THandlerFunction optionsCallback, WebServer::THandlerFunction startCallback)
    {
        if (name.indexOf(" ") > -1)
        {
            Serial.println("(ERROR) animation string can't contain any spaces when used for api path");
            return;
        }
        
        api_server.on("/options/" + name, optionsCallback);
        api_server.on("/start/" + name, startCallback);
    }
};

CAPIServer APIserver;

#endif