#ifndef _LED_FUNC_H_
#define _LED_FUNC_H_

#include <Arduino.h>

void init_led_pin();

void blink(u_int8_t count, unsigned long int on_t, unsigned long int off_t);


#endif