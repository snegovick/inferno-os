#include <arch/sys_io.h>
#include <arch/aarch32/armv7e_m/stm32f7/stm32f769xx.h>
#include <drivers/pinctrl/stm32f769_pinctrl.h>
#include <drivers/clock/stm32f769_clocks.h>
#include <drivers/clock/stm32f769_clock_control.h>

#include <stddef.h>

#include "gpio.h"
/* #include "memory_map.h" */
/* #include "soc_gpio.h" */
/* #include "soc.h" */
/* #include "clocks.h" */
/* #include "clock_control.h" */

int stm32f769_gpio_configure_analog(struct device *dev, uint32_t pin, uint32_t flags)
{
//  const struct gpio_stm32f769_config *config = port->config;
  GPIO_TypeDef *p = NULL;
  if (dev == NULL) {
    uint32_t port_idx = STM32F769_PORT_GET(pin);
    p = stm32f769_get_port(port_idx);
  } else {
    p = dev->devptr;
  }
  
  uint32_t moder = READ_REG(p->MODER);
  uint32_t ospeedr = READ_REG(p->OSPEEDR);
  uint32_t pupdr = READ_REG(p->PUPDR);
  uint32_t otyper = READ_REG(p->OTYPER);

  uint32_t pin_num = pin & 0xf;

  moder |= ((0x3) << (pin_num * 2));
  WRITE_REG(p->MODER, moder);

  // Low speed
  ospeedr &= ~((0x3) << (pin_num * 2));
  WRITE_REG(p->OSPEEDR, ospeedr);

  if ((flags & GPIO_OPEN_DRAIN) != 0U) {
    otyper |= (1 << pin_num);
  } else {
    otyper &= ~(1 << pin_num);
  }
  WRITE_REG(p->OTYPER, otyper);

  if ((flags & GPIO_PULL_UP) != 0U) {
    pupdr &= ~(0x3 << (pin_num * 2));
    pupdr |= (0x1 << (pin_num * 2));
  } else if ((flags & GPIO_PULL_DOWN) != 0U) {
    pupdr &= ~(0x3 << (pin_num * 2));
    pupdr |= (0x2 << (pin_num * 2));
  } else {
    pupdr &= ~(0x3 << (pin_num * 2));
  }

  WRITE_REG(p->PUPDR, pupdr);

  return 0;
}

int stm32f769_gpio_configure_af(struct device *dev, uint32_t pin, uint32_t flags)
{
//  const struct gpio_stm32f769_config *config = port->config;
  GPIO_TypeDef *p = NULL;
  if (dev == NULL) {
    uint32_t port_idx = STM32F769_PORT_GET(pin);
    p = stm32f769_get_port(port_idx);
  } else {
    p = dev->devptr;
  }

  uint32_t pin_num = pin & 0xf;
  uint32_t moder = READ_REG(p->MODER);
  uint32_t ospeedr = READ_REG(p->OSPEEDR);
  uint32_t pupdr = READ_REG(p->PUPDR);
  uint32_t otyper = READ_REG(p->OTYPER);

  moder &= ~((0x3) << (pin_num * 2));
  moder |= ((0x2) << (pin_num * 2));
  WRITE_REG(p->MODER, moder);

  // Low speed
  ospeedr &= ~((0x3) << (pin_num * 2));
  WRITE_REG(p->OSPEEDR, ospeedr);

  if ((flags & GPIO_OPEN_DRAIN) != 0U) {
    otyper |= (1 << pin_num);
  } else {
    otyper &= ~(1 << pin_num);
  }
  WRITE_REG(p->OTYPER, otyper);

  if ((flags & GPIO_PULL_UP) != 0U) {
    pupdr &= ~(0x3 << (pin_num * 2));
    pupdr |= (0x1 << (pin_num * 2));
  } else if ((flags & GPIO_PULL_DOWN) != 0U) {
    pupdr &= ~(0x3 << (pin_num * 2));
    pupdr |= (0x2 << (pin_num * 2));
  } else {
    pupdr &= ~(0x3 << (pin_num * 2));
  }

  WRITE_REG(p->PUPDR, pupdr);

  return 0;
}

int stm32f769_gpio_configure(struct device *dev, uint32_t pin, uint32_t flags)
{
//  const struct gpio_stm32f769_config *config = port->config;
  GPIO_TypeDef *p = NULL;
  if (dev == NULL) {
    uint32_t port_idx = STM32F769_PORT_GET(pin);
    p = stm32f769_get_port(port_idx);
  } else {
    p = dev->devptr;
  }

  uint32_t pin_num = pin & 0xf;
  uint32_t moder = READ_REG(p->MODER);
  uint32_t ospeedr = READ_REG(p->OSPEEDR);
  uint32_t pupdr = READ_REG(p->PUPDR);
  uint32_t otyper = READ_REG(p->OTYPER);

  if ((flags & GPIO_OUTPUT) != 0U) {
    // GPIO mode
    moder &= ~((0x3) << (pin_num * 2));
    moder |= (0x01 << (pin_num * 2));
    WRITE_REG(p->MODER, moder);

    // Low speed
    ospeedr &= ~((0x3) << (pin_num * 2));
    WRITE_REG(p->OSPEEDR, ospeedr);

    if ((flags & GPIO_OUTPUT_INIT_HIGH) != 0U) {
      WRITE_REG(p->BSRR, BIT(pin_num));
    } else if ((flags & GPIO_OUTPUT_INIT_LOW) != 0U) {
      WRITE_REG(p->BSRR, BIT(pin_num + 16));
    }
  } else if ((flags & GPIO_INPUT) != 0U) {
    // GPIO mode
    moder &= ~((0x3) << (pin_num * 2));
    WRITE_REG(p->MODER, moder);

    // Low speed
    ospeedr &= ~((0x3) << (pin_num * 2));
    WRITE_REG(p->OSPEEDR, ospeedr);
  } else { // set mode ANALOG
    // GPIO mode
    moder |= ((0x3) << (pin_num * 2));
    WRITE_REG(p->MODER, moder);

    // Low speed
    ospeedr &= ~((0x3) << (pin_num * 2));
    WRITE_REG(p->OSPEEDR, ospeedr);
  }

  if ((flags & GPIO_OPEN_DRAIN) != 0U) {
    otyper |= (1 << pin_num);
  } else {
    otyper &= ~(1 << pin_num);
  }
  WRITE_REG(p->OTYPER, otyper);

  if ((flags & GPIO_PULL_UP) != 0U) {
    pupdr &= ~(0x3 << (pin_num * 2));
    pupdr |= (0x1 << (pin_num * 2));
  } else if ((flags & GPIO_PULL_DOWN) != 0U) {
    pupdr &= ~(0x3 << (pin_num * 2));
    pupdr |= (0x2 << (pin_num * 2));
  } else {
    pupdr &= ~(0x3 << (pin_num * 2));
  }

  WRITE_REG(p->PUPDR, pupdr);

  return 0;
}

int stm32f769_gpio_init(struct device *dev)
{
  uint32_t clkid = 0;
  switch ((uint32_t)dev->devptr) {
  case GPIOA_BASE:
    clkid = STM32F769_CLOCK_GPIOA;
    break;
  case GPIOB_BASE:
    clkid = STM32F769_CLOCK_GPIOB;
    break;
  case GPIOC_BASE:
    clkid = STM32F769_CLOCK_GPIOC;
    break;
  case GPIOD_BASE:
    clkid = STM32F769_CLOCK_GPIOD;
    break;
  case GPIOE_BASE:
    clkid = STM32F769_CLOCK_GPIOE;
    break;
  case GPIOF_BASE:
    clkid = STM32F769_CLOCK_GPIOF;
    break;
  case GPIOG_BASE:
    clkid = STM32F769_CLOCK_GPIOG;
    break;
  case GPIOH_BASE:
    clkid = STM32F769_CLOCK_GPIOH;
    break;
  case GPIOI_BASE:
    clkid = STM32F769_CLOCK_GPIOI;
    break;
  case GPIOJ_BASE:
    clkid = STM32F769_CLOCK_GPIOJ;
    break;
  case GPIOK_BASE:
    clkid = STM32F769_CLOCK_GPIOK;
    break;
  default:
    return -1;
  }

  stm32f769_clock_control_on(clkid);

  return 0;
}

int stm32f769_gpio_read(struct device *dev, uint32_t pin) {
  GPIO_TypeDef *p = dev->devptr;
  uint32_t state = READ_REG(p->IDR);
  if (state & BIT(pin)) {
    return 1;
  }
  return 0;
}

void stm32f769_gpio_write(struct device *dev, uint32_t pin, int val) {
  GPIO_TypeDef *p = dev->devptr;
  if (val == 0) {
    WRITE_REG(p->BSRR, BIT(pin + 16));
  } else {
    WRITE_REG(p->BSRR, BIT(pin));
  }
}
