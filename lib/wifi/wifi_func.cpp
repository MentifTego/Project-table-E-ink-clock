#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "wifi_func.hpp"

bool connect_wifi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin("TP-Link_D8D2", "89499689");
    
    //WiFi.begin("Denis", "987654321!");

    uint8_t count_connect = 0;

    while (WiFi.status() != WL_CONNECTED && count_connect < 20){
        delay(500);
        count_connect++;
    }

    if (WiFi.status() != WL_CONNECTED){
        Serial.println("Connect WIFI - ERR");
        return false;
    }

    Serial.println("Connect WIFI - DONE");

    IPAddress ip = WiFi.localIP();
    Serial.println(ip);

    return true;
}

void disconnect_wifi(){
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}




time_t get_time_wifi(){
    configTime(10800, 0,"pool.ntp.org", "time.cloudflare.com");

    time_t now;
    do {
        delay(250);
        now = time(nullptr);
    } while (now < 100000);
    
    Serial.println(ctime(&now));
    return now + 3 * 3600;
}

bool check_connect(){

    Serial.println(WiFi.status());

    if (WiFi.status() == 4 /*no connect*/){
        return false;
    } 
    return true;
}

bool get_weather(WeatherData &weather){
    String url =
        "https://api.open-meteo.com/v1/forecast?"
        "latitude=" + String(LAT, 4) +
        "&longitude=" + String(LON, 4) +
        "&current=temperature_2m,relative_humidity_2m,weather_code";

    WiFiClientSecure client;
    client.setInsecure(); 

    HTTPClient http;
    Serial.println(url);


    
    if (!http.begin(client, url)){
        Serial.println(1);
        return false;
    }

    int httpCode = http.GET();

    Serial.print("httpCode: ");
    Serial.print(httpCode);
    Serial.print("\n");

    String response = http.getString();

    if (httpCode != HTTP_CODE_OK){
        
        Serial.println(response);
        Serial.println(httpCode);
        Serial.println(2);
        http.end();
        return false;
    }

    
    http.end();

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, response);

    if (error){
        return false;
        Serial.println(3);
    }
        

    JsonObject current = doc["current"];

    weather.temperature = current["temperature_2m"];
    weather.humidity    = current["relative_humidity_2m"];
    weather.weatherCode = current["weather_code"];

    return true;
}
