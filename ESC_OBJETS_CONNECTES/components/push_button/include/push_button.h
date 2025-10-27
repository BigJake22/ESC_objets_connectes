#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H
#include "driver/gpio.h"

void button_init();
int button_poll(void);

#endif