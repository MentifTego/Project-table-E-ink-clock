#ifndef _TIME_FUNC_H_
#define _TIME_FUNC_H_

#include <Arduino.h>
#include <Wire.h>  
#include <RTClib.h>

extern RTC_DS3231 rtc;

void init_time(time_t now);

DateTime get_time();

std::string get_data_string(DateTime time);
void set_next_alarm();
void clear_alarm();
#endif 