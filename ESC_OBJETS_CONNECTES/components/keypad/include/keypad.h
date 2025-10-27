#ifndef KEYPAD_H
#define KEYPAD_H
#include "driver/gpio.h"

void keypad_init(void);
char keypad_scan(void);

#endif