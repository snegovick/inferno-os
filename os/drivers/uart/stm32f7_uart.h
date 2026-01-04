#ifndef __STM32F7_UART_H__
#define __STM32F7_UART_H__

#include <drivers/include/device.h>

int stm32f7_uart_init(struct device *dev);
int stm32f7_uart_deinit(struct device *dev);
void stm32f7_uart_poll_out8(const struct device *dev, unsigned char ch);
void stm32f7_uart_tx_buffer(const struct device *dev, char *buffer, unsigned int len);
void stm32f7_uart_tx(const struct device *dev, char *buffer);

#endif/*__STM32F7_UART_H__*/
