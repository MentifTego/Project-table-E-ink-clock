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

void rtc_alarm() {
    rtc_trigger = true;
}

void setup()
{   
    Serial.begin(115200);

    blink(2, 150, 150);

    attachInterrupt( //настройка прерывания 
        digitalPinToInterrupt(RTC_INT),
        rtc_alarm, //функция выполняющая при прерывании
        FALLING
    );

    if (!connect_wifi()) { //подключение к wifi
        blink(3, 500, 500);
        rp2040.reboot();
    } else {
        blink(2, 150, 150);
    }

    init_display(0, true);
    set_color(GxEPD_BLACK);
    
    //получение данных о погоде 
    if (!get_weather(weather_data)){
        blink(3, 500, 500);
    } else {
        blink(2, 150, 150);
    }

    init_time(get_time_wifi()); //получение времени 
    current_time = get_time();
    disconnect_wifi();

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
    if(current_time.minute() % 30 == 0){
        if (!connect_wifi()) return false;
            
        if (!get_weather(weather_data)){
            Serial.println("weather_data - Err");
            blink(3, 500, 500);
            return false;
           } else {
            Serial.println("weather_data - Done");
            blink(2, 150, 150);
            }

            disconnect_wifi();
        }  
    return true;
}

void loop()
{
    while (!rtc_trigger) {
        __wfi();    
    }
    
    Serial.println("Alarm");
    blink(1, 150, 0);

    init_display(0, false);
    rtc_trigger = false;
    clear_alarm();
    befor_time = current_time;
    current_time = get_time();
     
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
    
    set_next_alarm(); 
    hibernate_display();
}