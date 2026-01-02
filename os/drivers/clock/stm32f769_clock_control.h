#ifndef __STM32F769_CLOCK_CONTROL_H__
#define __STM32F769_CLOCK_CONTROL_H__

#include <stdint.h>

int stm32f769_clock_control_on(uint16_t id);
int stm32f769_clock_control_off(uint16_t id);
int stm32f769_clock_control_get_rate(uint16_t id, uint32_t *rate);

#endif/*__STM32F769_CLOCK_CONTROL_H__*/
