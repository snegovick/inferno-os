#include <arch/sys_io.h>
#include <arch/aarch32/armv7e_m/stm32f7/stm32f769xx.h>
#include <drivers/clock/stm32f769_clocks.h>
#include <drivers/clock/stm32f769_clock_control.h>
#include <drivers/gpio/stm32f769_gpio.h>

#include <stddef.h>
#include "stm32f769_pinctrl.h"

static const GPIO_TypeDef *stm32f769_port_addrs[] = {
  GPIOA,
  GPIOB,
  GPIOC,
  GPIOD,
  GPIOE,
  GPIOF,
  GPIOG,
  GPIOH,
  GPIOI,
  GPIOJ,
  GPIOK,
};

static const uint16_t stm32f769_port_clkids[] = {
  STM32F769_CLOCK_GPIOA,
  STM32F769_CLOCK_GPIOB,
  STM32F769_CLOCK_GPIOC,
  STM32F769_CLOCK_GPIOD,
  STM32F769_CLOCK_GPIOE,
  STM32F769_CLOCK_GPIOF,
  STM32F769_CLOCK_GPIOG,
  STM32F769_CLOCK_GPIOH,
  STM32F769_CLOCK_GPIOI,
  STM32F769_CLOCK_GPIOJ,
  STM32F769_CLOCK_GPIOK,
};

GPIO_TypeDef *stm32f769_get_port(unsigned int port_idx) {
  GPIO_TypeDef *p = stm32f769_port_addrs[port_idx];
  return p;
}

uint16_t stm32f769_get_port_clkid(unsigned int port_idx) {
  return stm32f769_port_clkids[port_idx];
}

void stm32f769_pinctrl_configure_pin(uint32_t pin)
{
  uint8_t port_idx;
  uint32_t port;
  uint32_t pin_num;
  uint32_t af;
  uint16_t clkid;
  //uint16_t strength;

  port_idx = STM32F769_PORT_GET(pin);
  GPIO_TypeDef *p = stm32f769_get_port(port_idx);

  clkid = stm32f769_get_port_clkid(port_idx);

  pin_num = STM32F769_PIN_GET(pin);
  af = STM32F769_AF_GET(pin);

  stm32f769_clock_control_on(clkid);

  if (af != STM32F769_ANALOG) {
    stm32f769_gpio_configure_af(NULL, pin, pin);
    if (pin_num < 8) {
      MODIFY_REG(p->AFR[0], (0xf << (pin_num * 4)), (af << (pin_num * 4)));
    } else {
      MODIFY_REG(p->AFR[1], (0xf << ((pin_num - 8) * 4)), (af << ((pin_num - 8) * 4)));
    }
  } else {
    // set mode ANALOG
    stm32f769_gpio_configure_analog(NULL, pin, pin);
  }
}

/* int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins, uint8_t pin_cnt, */
/*         uintptr_t reg) */
/* { */
/*  ARG_UNUSED(reg); */

/*  for (uint8_t i = 0U; i < pin_cnt; i++) { */
/*    pinctrl_configure_pin(pins[i]); */
/*  } */

/*  return 0; */
/* } */

