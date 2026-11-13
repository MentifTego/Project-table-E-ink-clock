#ifndef _DISP_FUNC_H_
#define _DISP_FUNC_H_

#include <GxEPD2_BW.h>
#include <string>
#include "wifi_func.hpp"
#include <format>

#define EPD_CS 17
#define EPD_DC 16
#define EPD_RST 15
#define EPD_BUSY 14

struct cord_partial
{
   uint16_t x;      
   uint16_t y;
   uint16_t w;
   uint16_t h;
};
struct cord_pos
{
   uint16_t x;
   uint16_t y;
};

struct Weather;
struct Bitmaps;
struct Style;

struct clock_mod {
    uint16_t distance_x_digits; // дистанция между цифрами

    uint16_t size_x; //размер цифры по x 

    uint16_t indent_points_x; //отступ точек от второй цифры для точек
    uint16_t indent_points_y; //отступ первой точки от y
    uint16_t distance_points_y; //расстояние между точками

    uint16_t indent; //отступ от края первой цифры
};

struct time_clock {
    uint8_t hour;
    uint8_t minute;

    uint8_t befor_hour;
    uint8_t befor_minute;
};

enum Segments {
    n, A, B, F, G, E, C, D
};

struct segments_digit{
    Segments segments[7];
};

enum digit_code_name {
    Zero_0 = 0,
    One_1,
    Two_2,
    Three_3,
    Four_4,
    Five_5,
    Six_6,
    Seven_7,
    Eight_8,
    Nine_9,
    Hex_A,
    Hex_B,
    Hex_C,
    Hex_D,
    Hex_E,
    Hex_F,
    Char_R
};

const segments_digit digits[] = {
  //{A, B, C, D, E, F, G}
    {A, B, C, D, E, F, n},    // 0
    {n, B, C, n, n, n, n},     // 1
    {A, B, n, D, E, n, G},       // 2
    {A, B, C, D, n, n, G},       // 3
    {n, B, C, n, n, F, G},          // 4
    {A, n, C, D, n, F, G},       // 5
    {A, n, C, D, E, F, G},    // 6
    {A, B, C, n, n, n, n},             // 7
    {A, B, C, D, E, F, G}, // 8
    {A, B, C, D, n, F, G},    // 9
    {A, B, C, n, E, F, G},    // A
    {n, n, C, D, E, F, G},       // b 
    {A, n, n, D, E, F, n},          // C
    {n, B, C, D, E, n, G},       // d 
    {A, n, n, D, E, F, G},       // E
    {A, n, n, n, E, F, G},           // F
    {A, n, n, n, E, F, n}           // r
};


void init_display(u_int8_t rotation, bool flag);
void fill_window(uint16_t color);
void fill_part(const cord_partial& cord_p, uint16_t color);
void print_partial_text(const cord_partial& cord_p, const cord_pos& cord_t, std::string text);
void set_color(uint16_t color);
void set_font(const GFXfont * font);

void draw_bitmap_mod(uint16_t x, uint16_t y, Bitmaps bitmap, bool method);
void buffer_weather_bar(uint16_t x, uint16_t y, WeatherData data);
void buffer_data_bar(uint16_t x, uint16_t y, std::string data_string);

void draw_buffer_full();
void draw_buffer_part(const cord_partial& cord_p);

void draw_buffer_segment(uint16_t x, uint16_t y, Style style, segments_digit segments_now, segments_digit segments_befor, bool first_draw);
void draw_buffer_clock(uint16_t x, uint16_t y, time_clock time, Style style, clock_mod * mod, void (*draw_buffer_digit) (uint16_t x, uint16_t y, Style style, segments_digit segments_now, segments_digit segments_befor, bool first_draw), bool first_draw);
void hibernate_display();

#endif