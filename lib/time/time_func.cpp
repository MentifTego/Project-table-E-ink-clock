#include <Arduino.h>
#include <Wire.h>  
#include <RTClib.h>
#include <string>
#include <time.h>

RTC_DS3231 rtc;

void init_time(time_t now){  
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();

  if (!rtc.begin()){
    Serial.println("ERROR rtc.begin");
    return;
  }
    delay(100);
    rtc.adjust(DateTime(now));
    Serial.println(now);
}

DateTime get_time(){
  DateTime now = rtc.now();
  return now;
}

std::string get_data_string(DateTime time){
    std::string data;
    uint8_t day = time.day();
    uint8_t month = time.month();
    if (day < 10){
        data += "0";
    }
    data += std::to_string(day);
    data += ".";
    if (month < 10){
        data += "0";
    }
    data += std::to_string(month);

    return(data);
}

void set_next_alarm() {
    DateTime now = rtc.now();

    rtc.clearAlarm(1);

    DateTime now_aligned(now.year(), now.month(), now.day(), now.hour(), now.minute(), 0);
    DateTime alarm = now_aligned + TimeSpan(0, 0, 1, 0);

    rtc.setAlarm1(
        alarm,
        DS3231_A1_Second
    );
}

void clear_alarm(){
    rtc.clearAlarm(1);
}