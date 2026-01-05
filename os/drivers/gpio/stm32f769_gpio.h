#ifndef __STM32F769_GPIO_H__
#define __STM32F769_GPIO_H__

#include <stdint.h>

GPIO_TypeDef *stm32f769_get_port(unsigned int port_idx);
uint16_t stm32f769_get_port_clkid(unsigned int port_idx);
int stm32f769_gpio_configure(struct device *dev, uint32_t pin, uint32_t flags);
int stm32f769_gpio_configure_analog(struct device *dev, uint32_t pin, uint32_t flags);
int stm32f769_gpio_configure_af(struct device *dev, uint32_t pin, uint32_t flags);
int stm32f769_gpio_init(struct device *dev);
int stm32f769_gpio_read(struct device *dev, uint32_t pin);
void stm32f769_gpio_write(struct device *dev, uint32_t pin, int val);

#endif/*__STM32F769_GPIO_H__*/
