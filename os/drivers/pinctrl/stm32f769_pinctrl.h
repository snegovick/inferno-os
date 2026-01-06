#ifndef __DT_BINDINGS_PINCTRL_STM32F769_AF_H__
#define __DT_BINDINGS_PINCTRL_STM32F769_AF_H__

#define STM32F769_AF0 0U
#define STM32F769_AF1 1U
#define STM32F769_AF2 2U
#define STM32F769_AF3 3U
#define STM32F769_AF4 4U
#define STM32F769_AF5 5U
#define STM32F769_AF6 6U
#define STM32F769_AF7 7U
#define STM32F769_AF8 8U
#define STM32F769_AF9 9U
#define STM32F769_AF10 10U
#define STM32F769_AF11 11U
#define STM32F769_AF12 12U
#define STM32F769_AF13 13U
#define STM32F769_AF14 14U
#define STM32F769_AF15 15U
#define STM32F769_ANALOG 16U

#define STM32F769_PIN_MSK 0xFU
#define STM32F769_PIN_POS 0U
#define STM32F769_PORT_MSK 0xFU
#define STM32F769_PORT_POS 4U
#define STM32F769_AF_MSK 0x1FU
#define STM32F769_AF_POS 8U

#define STM32F769_PORTA 0
#define STM32F769_PORTB 1
#define STM32F769_PORTC 2
#define STM32F769_PORTD 3
#define STM32F769_PORTE 4
#define STM32F769_PORTF 5
#define STM32F769_PORTG 6
#define STM32F769_PORTH 7
#define STM32F769_PORTI 8
#define STM32F769_PORTJ 9
#define STM32F769_PORTK 10

#define STM32F769_PORT_GET(pinmux)                        \
  (((pinmux) >> STM32F769_PORT_POS) & STM32F769_PORT_MSK)

#define STM32F769_PIN_GET(pinmux)                       \
  (((pinmux) >> STM32F769_PIN_POS) & STM32F769_PIN_MSK)

#define STM32F769_AF_GET(pinmux)                      \
  (((pinmux) >> STM32F769_AF_POS) & STM32F769_AF_MSK)

/**
 * - 0..3: pin
 * - 4..7: port
 * - 8..13: af
 * 
 * port: Port ('A'..'K')
 * pin: Pin (0..15)
 * af: Alternate function (ANALOG, AFx, x=0..15)
 */

#define STM32F769_PINMUX_AF(port, pin, af)                        \
  ((((port) & STM32F769_PORT_MSK) << STM32F769_PORT_POS) |        \
   (((pin) & STM32F769_PIN_MSK) << STM32F769_PIN_POS) |           \
   (((STM32F769_ ## af) & STM32F769_AF_MSK) << STM32F769_AF_POS))

void stm32f769_pinctrl_configure_pin(uint32_t pin);
GPIO_TypeDef *stm32f769_get_port(unsigned int port_idx);
uint16_t stm32f769_get_port_clkid(unsigned int port_idx);

#endif/*__DT_BINDINGS_PINCTRL_STM32F769_AF_H_ */
