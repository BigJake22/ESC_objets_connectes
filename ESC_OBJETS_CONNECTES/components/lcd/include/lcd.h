#pragma once
#include "driver/i2c.h"
#include "esp_err.h"

#ifdef __cplusplus
#endif

void lcd_i2c_init(void);
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(int row, int col);
void lcd_print(const char *str);

#ifdef __cplusplus
#endif
