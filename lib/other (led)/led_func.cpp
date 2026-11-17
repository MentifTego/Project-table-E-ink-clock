#include "led_func.hpp"

#include <Arduino.h>

#define LED LED_BUILTIN 

void init_led_pin(){
    pinMode(LED, OUTPUT);
}

void blink(u_int8_t count, unsigned long int on_t, unsigned long int off_t){
    for (u_int8_t i = 0; i < count; ++i){
        digitalWrite(LED, HIGH);
        delay(on_t);
        digitalWrite(LED, LOW);
        delay(off_t);
    }
}

