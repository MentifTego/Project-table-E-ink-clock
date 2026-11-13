#ifndef _DATA_FUNC_H_
#define _DATA_FUNC_H_

bool connect_wifi();
bool check_connect();
time_t get_time_wifi();

constexpr float LAT = 44.86495;
constexpr float LON = 38.15781;


struct WeatherData
{
    uint8_t temperature;
    uint8_t humidity;
    uint8_t weatherCode;
};

bool get_weather(WeatherData &weather);
void disconnect_wifi();
#endif 