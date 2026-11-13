#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Wire.h>
#include <RTClib.h>
#include <string>

#include "time_func.hpp"
#include "disp_func.hpp"
#include "wifi_func.hpp"
#include "bitmaps.hpp"
#include "led_func.hpp"

#define RTC_INT 1



DateTime current_time;
DateTime befor_time;

clock_mod mode = {
    14,
    80,
    6,
    35,
    53,
    14
};

WeatherData weather_data;

volatile bool rtc_trigger = false;

void rtc_alarm()
{
    rtc_trigger = true;
}

void setup()
{
    blink(2, 500, 200);
        
    Serial.begin(115200);

    delay(100);

    

    attachInterrupt(
        digitalPinToInterrupt(RTC_INT),
        rtc_alarm,
        FALLING
    );

    connect_wifi();

    init_time(get_time_wifi());

    current_time = get_time();

    init_display(0, true);

    set_color(GxEPD_BLACK);

    Serial.print("check_connect: ");
    check_connect();
    Serial.print("\n");


    bool flag = get_weather(weather_data);
    flag = true;
    while(!flag)
    {   
        
        flag = get_weather(weather_data);
    }

    disconnect_wifi();


    digitalWrite(LED_BUILTIN, LOW);

    fill_window(GxEPD_WHITE);

    set_font(&FreeMonoBold12pt7b);

    buffer_weather_bar(300,260,weather_data);

    set_font(&FreeMonoBold24pt7b);

    buffer_data_bar(0,296,get_data_string(current_time));

    draw_buffer_clock(0,60,{current_time.hour(), current_time.minute()},style_1,&mode,&draw_buffer_segment,true);

    draw_buffer_full();

    set_next_alarm();

    hibernate_display();
}


bool get_weather_func(){
    if(current_time.minute() % 20 == 0){
        if (!connect_wifi()){
            return false;
            }
        for (uint8_t i = 0; i < 3; i++){
            if (!get_weather(weather_data)){
                Serial.println("weather_data - Err");
                continue;
            } else {
                Serial.println("weather_data - Done");
                break;
            }
        }
        
    Serial.println("Connect - loop");
    disconnect_wifi();
    Serial.println("Disconnect - loop");
    } 
    return true;
}

void loop()
{

    if(!rtc_trigger){
        delay(100);
        return;
    }

    init_display(0, false);
    rtc_trigger = false;
    clear_alarm();
    befor_time = current_time;
    current_time = get_time();
    
    digitalWrite(LED_BUILTIN, HIGH);
     
    get_weather_func();

    if(current_time.minute() % 5 == 0){   
        Serial.println("draw_full");
        fill_window(GxEPD_WHITE);

        draw_buffer_clock(0,60,{current_time.hour(), current_time.minute(), befor_time.hour(), befor_time.minute()}, style_1,&mode,&draw_buffer_segment, true);

        set_font(&FreeMonoBold24pt7b);
        buffer_data_bar(0,296,get_data_string(current_time));
        set_font(&FreeMonoBold12pt7b);     
        buffer_weather_bar(300,260,weather_data);

        draw_buffer_full();

        }  else {
            Serial.println("draw_part");
            draw_buffer_clock(0,60,{current_time.hour(), current_time.minute(), befor_time.hour(), befor_time.minute()}, style_1,&mode,&draw_buffer_segment,false);  
            draw_buffer_part({0,0,400,300});
        }

    digitalWrite(LED_BUILTIN, LOW);    

    set_next_alarm();
    hibernate_display();

}


