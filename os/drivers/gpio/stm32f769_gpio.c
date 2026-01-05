#include "gpio.h"
#include <arch/sys_io.h>
/* #include "memory_map.h" */
/* #include "soc_gpio.h" */
/* #include "soc.h" */
/* #include "clocks.h" */
/* #include "clock_control.h" */

int stm32f769_gpio_configure_analog(struct device *dev, uint32_t pin, uint32_t flags)
{
//	const struct gpio_stm32f769_config *config = port->config;
  uint32_t port_idx = STM32F769_PORT_GET(pin);
  GPIO_TypeDef *p = NULL;
  if (dev == NULL) {
    p = stm32f769_get_port(port_idx);
  } else {
    p = dev->devptr;
  }
  
  uint32_t moder = sys_read32(p->MODER);
  uint32_t ospeedr = sys_read32(p->OSPEEDR);
  uint32_t pupdr = sys_read32(p->PUPDR);
  uint32_t otyper = sys_read32(p->OTYPER);

  moder |= ((0x3) << (pin * 2));
  sys_write32(moder, p->MODER);

  // Low speed
  ospeedr &= ~((0x3) << (pin * 2));
  sys_write32(ospeedr, p->OSPEEDR);

  if ((flags & GPIO_OPEN_DRAIN) != 0U) {
    otyper |= (1 << pin);
  } else {
    otyper &= ~(1 << pin);
  }
  sys_write32(otyper, p->OTYPER);

	if ((flags & GPIO_PULL_UP) != 0U) {
		pupdr &= ~(0x3 << (pin * 2));
		pupdr |= (0x1 << (pin * 2));
	} else if ((flags & GPIO_PULL_DOWN) != 0U) {
		pupdr &= ~(0x3 << (pin * 2));
		pupdr |= (0x2 << (pin * 2));
	} else {
		pupdr &= ~(0x3 << (pin * 2));
	}

  sys_write32(pupdr, p->PUPDR);

	return 0;
}

int stm32f769_gpio_configure_af(struct device *dev, uint32_t pin, uint32_t flags)
{
//	const struct gpio_stm32f769_config *config = port->config;
  uint32_t port_idx = STM32F769_PORT_GET(pin);
  GPIO_TypeDef *p = NULL;
  if (dev == NULL) {
    p = stm32f769_get_port(port_idx);
  } else {
    p = dev->devptr;
  }
  
  uint32_t moder = sys_read32(p->MODER);
  uint32_t ospeedr = sys_read32(p->OSPEEDR);
  uint32_t pupdr = sys_read32(p->PUPDR);
  uint32_t otyper = sys_read32(p->OTYPER);

  moder &= ~((0x3) << (pin * 2));
  moder |= ((0x2) << (pin * 2));
  sys_write32(moder, p->MODER);

  // Low speed
  ospeedr &= ~((0x3) << (pin * 2));
  sys_write32(ospeedr, p->OSPEEDR);

  if ((flags & GPIO_OPEN_DRAIN) != 0U) {
    otyper |= (1 << pin);
  } else {
    otyper &= ~(1 << pin);
  }
  sys_write32(otyper, p->OTYPER);

	if ((flags & GPIO_PULL_UP) != 0U) {
		pupdr &= ~(0x3 << (pin * 2));
		pupdr |= (0x1 << (pin * 2));
	} else if ((flags & GPIO_PULL_DOWN) != 0U) {
		pupdr &= ~(0x3 << (pin * 2));
		pupdr |= (0x2 << (pin * 2));
	} else {
		pupdr &= ~(0x3 << (pin * 2));
	}

  sys_write32(pupdr, p->PUPDR);

	return 0;
}

int stm32f769_gpio_configure(struct device *dev, uint32_t pin, uint32_t flags)
{
//	const struct gpio_stm32f769_config *config = port->config;
  uint32_t port_idx = STM32F769_PORT_GET(pin);
  GPIO_TypeDef *p = NULL;
  if (dev == NULL) {
    p = stm32f769_get_port(port_idx);
  } else {
    p = dev->devptr;
  }
  
  uint32_t moder = sys_read32(p->MODER);
  uint32_t ospeedr = sys_read32(p->OSPEEDR);
  uint32_t pupdr = sys_read32(p->PUPDR);
  uint32_t otyper = sys_read32(p->OTYPER);

	if ((flags & GPIO_OUTPUT) != 0U) {
    // GPIO mode
    moder &= ~((0x3) << (pin * 2));
    moder |= (0x01 << (pin * 2));
    sys_write32(moder, p->MODER);

    // Low speed
    ospeedr &= ~((0x3) << (pin * 2));
    sys_write32(ospeedr, p->OSPEEDR);

		if ((flags & GPIO_OUTPUT_INIT_HIGH) != 0U) {
      sys_write32(BIT(pin), p->BSRR);
		} else if ((flags & GPIO_OUTPUT_INIT_LOW) != 0U) {
      sys_write32(BIT(pin + 16), p->BSRR);
		}
	} else if ((flags & GPIO_INPUT) != 0U) {
    // GPIO mode
    moder &= ~((0x3) << (pin * 2));
    sys_write32(moder, p->MODER);

    // Low speed
    ospeedr &= ~((0x3) << (pin * 2));
    sys_write32(ospeedr, p->OSPEEDR);
	} else { // set mode ANALOG
    // GPIO mode
    moder |= ((0x3) << (pin * 2));
    sys_write32(moder, p->MODER);

    // Low speed
    ospeedr &= ~((0x3) << (pin * 2));
    sys_write32(ospeedr, p->OSPEEDR);
	}

  if ((flags & GPIO_OPEN_DRAIN) != 0U) {
    otyper |= (1 << pin);
  } else {
    otyper &= ~(1 << pin);
  }
  sys_write32(otyper, p->OTYPER);

	if ((flags & GPIO_PULL_UP) != 0U) {
		pupdr &= ~(0x3 << (pin * 2));
		pupdr |= (0x1 << (pin * 2));
	} else if ((flags & GPIO_PULL_DOWN) != 0U) {
		pupdr &= ~(0x3 << (pin * 2));
		pupdr |= (0x2 << (pin * 2));
	} else {
		pupdr &= ~(0x3 << (pin * 2));
	}

  sys_write32(pupdr, p->PUPDR);

	return 0;
}

int stm32f769_gpio_init(struct device *dev)
{
  uint32_t clkid = 0;
  switch (dev->devptr) {
  case GPIOA:
    clkid = STM32F769_CLOCK_GPIOA;
    break;
  case GPIOB:
    clkid = STM32F769_CLOCK_GPIOB;
    break;
  case GPIOC:
    clkid = STM32F769_CLOCK_GPIOC;
    break;
  case GPIOD:
    clkid = STM32F769_CLOCK_GPIOD;
    break;
  case GPIOE:
    clkid = STM32F769_CLOCK_GPIOE;
    break;
  case GPIOF:
    clkid = STM32F769_CLOCK_GPIOF;
    break;
  case GPIOG:
    clkid = STM32F769_CLOCK_GPIOG;
    break;
  case GPIOH:
    clkid = STM32F769_CLOCK_GPIOH;
    break;
  case GPIOI:
    clkid = STM32F769_CLOCK_GPIOI;
    break;
  case GPIOJ:
    clkid = STM32F769_CLOCK_GPIOJ;
    break;
  case GPIOK:
    clkid = STM32F769_CLOCK_GPIOK;
    break;
  }

	stm32f769_clock_control_on(clkid);

	return 0;
}

int stm32f769_gpio_read(struct device *dev, uint32_t pin) {
  GPIO_TypeDef *p = dev->devptr;
  uint32_t state = sys_read32(p->IDR);
  if (state & BIT(pin)) {
    return 1;
  }
  return 0;
}

void stm32f769_gpio_write(struct device *dev, uint32_t pin, int val) {
  GPIO_TypeDef *p = dev->devptr;
  if (val == 0) {
    sys_write32(BIT(pin + 16), p->BSRR);
  } else {
    sys_write32(BIT(pin), p->BSRR);
  }
}
