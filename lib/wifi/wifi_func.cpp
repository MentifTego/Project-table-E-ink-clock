#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "wifi_func.hpp"

bool connect_wifi(){
    WiFi.defaultLowPowerMode(); //важно
    WiFi.mode(WIFI_STA);
    WiFi.begin("TP-Link_D8D2", "89499689");
    //WiFi.begin("Denis", "987654321!");
    uint8_t count_connect = 0;
    Serial.print("Connecting WIFI");
    while (WiFi.status() != WL_CONNECTED && count_connect < 20){
        delay(500);
        Serial.print(".");
        count_connect++;
    }

    if (WiFi.status() != WL_CONNECTED){
        Serial.println("\nConnect WIFI - ERR");
        return false;
    }

    Serial.println("\nConnect WIFI - DONE");

    IPAddress ip = WiFi.localIP();
    Serial.print("IP: ");
    Serial.println(ip);

    return true;
}

void disconnect_wifi(){
    WiFi.disconnect(true);
    WiFi.end();
    delay(10);
    Serial.println("disconnect WIFI");
}


time_t get_time_wifi(){
    configTime(10800, 0,"pool.ntp.org", "time.cloudflare.com");

    time_t now;
    do {
        delay(100);
        now = time(nullptr);
    } while (now < 100000);
    
    now = now + 3 * 3600;

    struct tm timeinfo;
    localtime_r(&now, &timeinfo);   
    char timeStringBuff[50];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%d.%m.%Y %H:%M:%S", &timeinfo);
    Serial.println(timeStringBuff);
    
    return now;
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
        "https://wttr.in/" + 
        String(LAT, 4) +
        "," + 
        String(LON, 4) +
        "?format=j2";

    WiFiClientSecure client;
    client.setInsecure(); 

    HTTPClient http;
    Serial.println(url);

    if (!http.begin(client, url)){
        Serial.println(1);
        return false;
    }

    int httpCode = http.GET();
    Serial.println(http.errorToString(httpCode));

    if (httpCode != HTTP_CODE_OK){
        Serial.println("HTTP_CODE_OK - False");
        http.end();
        return false;
    }
    
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, http.getStream());

    if (error){
        Serial.println(error.c_str());
        http.end();
        return false;
    }
        
    JsonObject current = doc["current_condition"][0];

    weather.temperature = current["temp_C"];
    weather.humidity    = current["humidity"];
    weather.weatherCode = current["weatherCode"];
    return true;
}
