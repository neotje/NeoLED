#ifndef json_h
#define json_h

#include <ArduinoJson.h>
#include <api.hpp>

class Cjson
{
private:
    /* data */
public:
    Cjson(/* args */) {}
    ~Cjson() {}

    StaticJsonDocument<250> document;
    char buffer[250];

    void clear() {
        document.clear();
    }

    void add_key(char* key, float value) {
        document[key] = value;
    }
    void add_key(char* key, int value) {
        document[key] = value;
    }
    void add_key(char* key, char* value) {
        document[key] = value;
    }

    bool has_key(char* key){
        return document.containsKey(key);
    }

    void fromPost() {
        clear();
        deserializeJson(document, APIserver.arg("plain"));
    }

    void send(int code = 200) {
        serializeJson(document, buffer);
        APIserver.send(code, "application/json", buffer);
    }
};

#endif