#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <string>
#include <stdio.h>

#include "bitmaps.hpp"
#include "disp_func.hpp"
#include "wifi_func.hpp"


static GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

//display.hibernate();

void init_display(u_int8_t rotation, bool flag){
    display.init(115200, flag);
    display.setRotation(rotation);
    
}

void hibernate_display(){
    display.hibernate();
}

void set_color(uint16_t color = GxEPD_BLACK){
    display.setTextColor(color);
}

void set_font(const GFXfont * font){
    display.setFont(font);
}

void print_partial_text(const cord_partial& cord_p, const cord_pos& cord_t, std::string text){
    display.setPartialWindow(cord_p.x, cord_p.y, cord_p.w, cord_p.h);
    display.firstPage();
    do {
        display.setCursor(cord_t.x, cord_t.y);
        display.print(text.c_str());
    } while (display.nextPage());

}

void fill_window(uint16_t color){
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(color);  
    } while (display.nextPage());
}

void fill_part(const cord_partial& cord_p, uint16_t color){
    display.setPartialWindow(cord_p.x, cord_p.y, cord_p.w, cord_p.h);
    display.fillRect(cord_p.x, cord_p.y, cord_p.w, cord_p.h, color);
    display.display(true);
 
}

void draw_bitmap_mod(uint16_t x, uint16_t y, Bitmaps bitmap, bool method){
    if (method){display.setPartialWindow(x, y, bitmap.w, bitmap.h);} else {display.setFullWindow();}
    display.firstPage();
    do {
        if (method) {display.fillRect(x, y, bitmap.w, bitmap.h, GxEPD_WHITE);} else {display.fillScreen(GxEPD_WHITE);}
        display.drawBitmap(x, y, bitmap.bitmap, bitmap.w, bitmap.h, GxEPD_BLACK);
     } while (display.nextPage());
}


void buffer_weather_bar(uint16_t x, uint16_t y, WeatherData data){
    Bitmaps bitmap_select;

    Serial.printf("weatherCode %d temperature %d humidity %d\n", data.weatherCode, data.temperature, data.humidity);

    switch (data.weatherCode){
    case 113:
        bitmap_select = clear;
        break;
    case 116:
    case 119:
    case 122:
        bitmap_select = cloudy;
        break;
    case 143:
    case 248:
    case 260:
        bitmap_select = fog;
        break;
    case 176:   
    case 263:
    case 266:
    case 293:
    case 296:
    case 299: 
    case 302:
    case 305:
    case 308:
    case 353:
    case 356:
    case 359:
        bitmap_select = rain;
        break;
    case 179:  
    case 227:
    case 230:   
    case 323:
    case 326: 
    case 329:
    case 332: 
    case 335:
    case 338: 
    case 368:
    case 371:        
        bitmap_select = snow;
        break;
    case 200:   
    case 386:  
    case 389:  
    case 392:  
    case 395:  
        bitmap_select = thunderstorm;
        break;
    default:
        Serial.println("unknown weather code");
        bitmap_select = clear;
        break;
}

    display.drawBitmap(x, y, bitmap_select.bitmap, bitmap_select.w, bitmap_select.h, GxEPD_BLACK);
    
    std::string text = ""; 

    display.setCursor(x+bitmap_select.w+10, y+18);
    text = std::to_string(data.temperature);
    display.print(text.c_str());

    text = "C";
    display.setCursor(x+bitmap_select.w+40, y+18);
    display.print(text.c_str());
    
    text = std::to_string(data.humidity);
    display.setCursor(x+bitmap_select.w+10, y+38);
    display.print(text.c_str());

    text = "%";
    display.setCursor(x+bitmap_select.w+40, y+38);
    display.print(text.c_str());

}

void buffer_data_bar(uint16_t x, uint16_t y, std::string data_string){
    display.setCursor(x, y);
    display.print(data_string.c_str());
}

void draw_buffer_full(){
    display.setFullWindow();
    display.display(false);
}

void draw_buffer_part(const cord_partial& cord_p){
    display.setPartialWindow(cord_p.x, cord_p.y, cord_p.w, cord_p.h);
    display.display(true);
}


void draw_buffer_segment(uint16_t x, uint16_t y, Style style, segments_digit segments_now, segments_digit segments_befor, bool first_draw){
    for (uint8_t i = 0; i < 7; i++){
        if (first_draw){
            if (segments_now.segments[i] == n)
                continue;

            else {
                switch (segments_now.segments[i]){
                case A: display.drawBitmap(x+11, y, style.H.bitmap, style.H.w, style.H.h, GxEPD_BLACK); break;
                case B: display.drawBitmap(x+67, y+11, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case C: display.drawBitmap(x+67, y+78, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case D: display.drawBitmap(x+11, y+134, style.H.bitmap, style.H.w, style.H.h, GxEPD_BLACK); break;
                case E: display.drawBitmap(x, y+78, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case F: display.drawBitmap(x, y+11, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case G: display.drawBitmap(x+11, y+67, style.H.bitmap, style.H.w, style.H.h, GxEPD_BLACK); break;
            }
            }
        }
    
        else if (segments_now.segments[i] == segments_befor.segments[i]) continue;
        else if (segments_now.segments[i] != segments_befor.segments[i] && segments_now.segments[i] != n){
            switch (segments_now.segments[i]){
                case A: display.drawBitmap(x+11, y, style.H.bitmap, style.H.w, style.H.h, GxEPD_BLACK); break;
                case B: display.drawBitmap(x+67, y+11, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case C: display.drawBitmap(x+67, y+78, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case D: display.drawBitmap(x+11, y+134, style.H.bitmap, style.H.w, style.H.h, GxEPD_BLACK); break;
                case E: display.drawBitmap(x, y+78, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case F: display.drawBitmap(x, y+11, style.V.bitmap, style.V.w, style.V.h, GxEPD_BLACK); break;
                case G: display.drawBitmap(x+11, y+67, style.H.bitmap, style.H.w, style.H.h, GxEPD_BLACK); break;
            }
        }
        else if (segments_now.segments[i] != segments_befor.segments[i] && segments_now.segments[i] == n){
            switch (segments_befor.segments[i]){
                case A: display.drawBitmap(x+11, y, style.H.bitmap, style.H.w, style.H.h, GxEPD_WHITE); break;
                case B: display.drawBitmap(x+67, y+11, style.V.bitmap, style.V.w, style.V.h, GxEPD_WHITE); break;
                case C: display.drawBitmap(x+67, y+78, style.V.bitmap, style.V.w, style.V.h, GxEPD_WHITE); break;
                case D: display.drawBitmap(x+11, y+134, style.H.bitmap, style.H.w, style.H.h, GxEPD_WHITE); break;
                case E: display.drawBitmap(x, y+78, style.V.bitmap, style.V.w, style.V.h, GxEPD_WHITE); break;
                case F: display.drawBitmap(x, y+11, style.V.bitmap, style.V.w, style.V.h, GxEPD_WHITE); break;
                case G: display.drawBitmap(x+11, y+67, style.H.bitmap, style.H.w, style.H.h, GxEPD_WHITE); break;
            }
        }
    } 
}

void draw_buffer_clock(uint16_t x, uint16_t y, time_clock time, Style style, clock_mod * mod, void (*draw_buffer_digit) (uint16_t x, uint16_t y, Style style, segments_digit segments_now, segments_digit segments_befor, bool first_draw), bool first_draw){
    uint16_t x_new = x;
    
    x_new += mod->indent;
    draw_buffer_digit(x_new, y, style, digits[time.hour/10], digits[time.befor_hour/10], first_draw); 
    x_new +=( mod->distance_x_digits + mod->size_x);
    draw_buffer_digit(x_new, y, style, digits[time.hour%10], digits[time.befor_hour%10], first_draw); 
    x_new +=(mod->size_x);

    x_new+=mod->indent_points_x;


    display.drawBitmap(x_new, y+mod->indent_points_y, style.P.bitmap, style.P.w, style.P.h, GxEPD_BLACK);
    display.drawBitmap(x_new, y+(mod->indent_points_y+style.P.w+mod->distance_points_y), style.P.bitmap, style.P.w, style.P.h, GxEPD_BLACK);
   
    x_new+=style.P.w;
    x_new += mod->indent_points_x;
    draw_buffer_digit(x_new, y, style, digits[time.minute/10], digits[time.befor_minute/10], first_draw); 
    x_new +=( mod->distance_x_digits + mod->size_x);
    draw_buffer_digit(x_new, y, style, digits[time.minute%10], digits[time.befor_minute%10], first_draw); 
}