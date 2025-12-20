#ifndef DD_GPIO_H
#define DD_GPIO_H

struct dd_gpio_bank;
typedef struct dd_gpio_bank *dd_gpio_bank_t;
int dd_gpio_bank_init(char bank, dd_gpio_bank_t *out);
void dd_gpio_bank_destroy(dd_gpio_bank_t *out);

struct dd_gpio_pin;
typedef struct dd_gpio_pin *dd_gpio_pin_t;
int dd_gpio_pin_init(int pin, dd_gpio_bank_t bank, dd_gpio_pin_t *out);
void dd_gpio_pin_destroy(dd_gpio_pin_t *out);

#endif // DD_GPIO_H
