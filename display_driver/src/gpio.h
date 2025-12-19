#ifndef DISP_DRVER_GPIO_H
#define DISP_DRVER_GPIO_H

struct gpio_bank;
typedef struct gpio_bank *gpio_bank_t;
int gpio_bank_init(char bank, gpio_bank_t *out);
void gpio_bank_destroy(gpio_bank_t *out);

struct gpio_pin;
typedef struct gpio_pin *gpio_pin_t;
int gpio_pin_init(int pin, gpio_bank_t bank, gpio_pin_t *out);
void gpio_pin_destroy(gpio_pin_t *out);



#endif // DISP_DRVER_GPIO_H
