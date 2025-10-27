#ifndef LED_H
#define LED_H
#include "driver/gpio.h"
typedef struct {
    gpio_num_t gpio;
    uint8_t state;
} Led;
void leds_init(void);
void led_toggle(Led *led);
void led_on(Led *led);
void led_off(Led *led);
Led* get_led_ep1(void);
Led* get_led_ep2(void);
Led* get_led_err(void);
void leds_morse_sequence(const char *message);
#endif