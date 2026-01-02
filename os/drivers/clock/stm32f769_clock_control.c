#include <arch/sys_io.h>
#include "stm32f769_clock_control.h"
#include <stdint.h>

#include "errno.h"
#include "stm32f769_clocks.h"

#define STM32F769_CLOCK_ID_OFFSET(id) (((id) >> 6U) & 0xFFU)
#define STM32F769_CLOCK_ID_BIT(id)	 ((id)&0x1FU)

int stm32f7_clock_control_on(uint16_t id) {
	sys_set_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
              STM32F769_CLOCK_ID_BIT(id));
  int ret = sys_test_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
               STM32F769_CLOCK_ID_BIT(id));
  (void)ret;
	return 0;
}

int stm32f7_clock_control_off(uint16_t id) {
	sys_clear_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
              STM32F769_CLOCK_ID_BIT(id));
  int ret = sys_test_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
               STM32F769_CLOCK_ID_BIT(id));
  (void)ret;

	return 0;
}

int stm32f7_clock_control_get_rate(uint16_t id,
                                 uint32_t *rate)
{
	return 0;
}

/* static enum clock_control_status */
/* clock_control_mik32_get_status(uint16_t id) */
/* { */
/* 	if (sys_test_bit(PM_BASE_ADDRESS + MIK32_CLOCK_ID_OFFSET(id), */
/* 			 MIK32_CLOCK_ID_BIT(id)) != 0) { */
/* 		return CLOCK_CONTROL_STATUS_ON; */
/* 	} */

/* 	return CLOCK_CONTROL_STATUS_OFF; */
/* } */
