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
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
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
    Serial.println(http.errorToString(httpCode));

    String response = http.getString();

    WiFiClient tcp;
    IPAddress target(157, 90, 83, 168);

    bool connected = tcp.connect(target, 443);
    Serial.println(connected ? "TCP to IP:443 OK" : "TCP to IP:443 FAIL");
    tcp.stop();

    if (httpCode != HTTP_CODE_OK){
        Serial.print("response: ");
        Serial.print(response);
        Serial.print(" httpCode: ");
        Serial.print(httpCode);
        Serial.print(" HTTP_CODE_OK: ");
        Serial.println(HTTP_CODE_OK);
        http.end();
        return false;
    }

    http.end();

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, response);

    if (error){
        Serial.println(3);
        return false;
    }
        
    JsonObject current = doc["current"];

    weather.temperature = current["temperature_2m"];
    weather.humidity    = current["relative_humidity_2m"];
    weather.weatherCode = current["weather_code"];

    return true;
}
