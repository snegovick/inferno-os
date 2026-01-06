#include <arch/aarch32/armv7e_m/stm32f7/stm32f769xx.h>
#include <arch/sys_io.h>

#include <drivers/clock/stm32f769_clocks.h>
#include <drivers/clock/stm32f769_clock_control.h>

#include <string.h>

#include "uart.h"
#include "stm32f7_uart.h"

#define UART_BRR_MIN    0x10U        /* UART BRR minimum authorized value */
#define UART_BRR_MAX    0x0000FFFFU  /* UART BRR maximum authorized value */

inline static void __uart_disable(struct device *dev) {
  USART_TypeDef *p = dev->devptr;
  p->CR1 &= ~USART_CR1_UE;
}

inline static void __uart_enable(struct device *dev) {
  USART_TypeDef *p = dev->devptr;
  p->CR1 |= USART_CR1_UE;
}

int stm32f7_uart_init(struct device *dev) {
  USART_TypeDef *p = dev->devptr;
  stm32f7_uart_deinit(dev);

  uint32_t clk_rate = 0;
  uint32_t parity = 0;
  uint32_t stop_bits = 0;
  uint32_t word_length = 0;

  struct uart_data *uart = (struct uart_data *)dev->data;

  switch ((uint32_t)p) {
  case USART1_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_USART1, &clk_rate);
    break;
  case USART2_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_USART2, &clk_rate);
    break;
  case USART3_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_USART3, &clk_rate);
    break;
  case UART4_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_UART4, &clk_rate);
    break;
  case UART5_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_UART5, &clk_rate);
    break;
  case USART6_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_USART6, &clk_rate);
    break;
  case UART7_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_UART7, &clk_rate);
    break;
  case UART8_BASE:
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_UART8, &clk_rate);
    break;
  default:
    return -1;
  }

  switch (uart->data_bits) {
  case UART_CFG_DATA_BITS_7:
    word_length = USART_CR1_M1;
    break;
  case UART_CFG_DATA_BITS_8:
    word_length = 0;
    break;
  case UART_CFG_DATA_BITS_9:
    word_length = USART_CR1_M0;
    break;
  case UART_CFG_DATA_BITS_5:
  case UART_CFG_DATA_BITS_6:
  default:
    return -1;
  }

  switch (uart->parity) {
  case UART_CFG_PARITY_NONE:
    parity = 0;
    break;
  case UART_CFG_PARITY_ODD:
    parity = USART_CR1_PCE;
    break;
  case UART_CFG_PARITY_EVEN:
    parity = (USART_CR1_PCE | USART_CR1_PS);
    break;
  case UART_CFG_PARITY_MARK:
  case UART_CFG_PARITY_SPACE:
  default:
    return -1;
  }

  switch (uart->stop_bits) {
  case UART_CFG_STOP_BITS_0_5:
    stop_bits = USART_CR2_STOP_0;
    break;
  case UART_CFG_STOP_BITS_1:
    stop_bits = 0;
    break;
  case UART_CFG_STOP_BITS_1_5:
    stop_bits = (USART_CR2_STOP_0 | USART_CR2_STOP_1);
    break;
  case UART_CFG_STOP_BITS_2:
    stop_bits = USART_CR2_STOP_1;
    break;
  default:
    return -1;
  }

  uint32_t cr1 = word_length | parity | USART_CR1_TE | USART_CR1_RE;
  WRITE_REG(p->CR1, cr1);
  cr1 = READ_REG(p->CR1);

  uint32_t cr2 = stop_bits;
  WRITE_REG(p->CR2, cr2);
  cr2 = READ_REG(p->CR2);

  uint32_t cr3 = 0;
  WRITE_REG(p->CR3, cr3);
  cr3 = READ_REG(p->CR3);

  uint32_t div = (clk_rate + ((uart->baudrate)/2U)) / (uart->baudrate);
  if ((div >= UART_BRR_MIN) && (div <= UART_BRR_MAX)) {
    p->BRR = (uint16_t)div;
  } else {
    return -1;
  }

  CLEAR_BIT(p->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(p->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  __uart_enable(dev);
  return 0;
}

int stm32f7_uart_deinit(struct device *dev) {
  __uart_disable(dev);
  USART_TypeDef *p = dev->devptr;
  p->CR1 = 0x0U;
  p->CR2 = 0x0U;
  p->CR3 = 0x0U;
  return 0;
}

void stm32f7_uart_poll_out8(const struct device *dev, unsigned char ch)
{
  USART_TypeDef *p = dev->devptr;

  while ((p->ISR & USART_ISR_TXE) == 0) {
  }

  p->TDR = ch;
}

void stm32f7_uart_tx_buffer(const struct device *dev, char *buffer, unsigned int len) {
  unsigned int i = 0;
  for (;i < len; i++) {
    stm32f7_uart_poll_out8(dev, buffer[i]);
  }
}

void stm32f7_uart_tx(const struct device *dev, char *buffer) {
  stm32f7_uart_tx_buffer(dev, buffer, strlen(buffer));
}
