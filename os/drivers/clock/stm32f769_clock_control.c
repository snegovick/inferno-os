#include <arch/sys_io.h>
#include <arch/aarch32/armv7e_m/stm32f7/stm32f769xx.h>
#include <stdint.h>

#include "stm32f769_clock_control.h"
#include "stm32f769_rcc.h"

#include "errno.h"
#include "stm32f769_clocks.h"

#define STM32F769_CLOCK_ID_OFFSET(id) (((id) >> 6U) & 0xFFU)
#define STM32F769_CLOCK_ID_BIT(id)   (id & 0x1FU)

volatile uint64_t uptime_ctr;

void SysTick_Handler(void) {
  uptime_ctr ++;
}

int stm32f769_clock_control_on(uint16_t id) {
  uint32_t addr = RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id);
  uint32_t bit = STM32F769_CLOCK_ID_BIT(id);
  sys_set_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
              STM32F769_CLOCK_ID_BIT(id));
  int ret = sys_test_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
                         STM32F769_CLOCK_ID_BIT(id));
  (void)ret;
  return 0;
}

int stm32f769_clock_control_off(uint16_t id) {
  sys_clear_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
                STM32F769_CLOCK_ID_BIT(id));
  int ret = sys_test_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
                         STM32F769_CLOCK_ID_BIT(id));
  (void)ret;

  return 0;
}

static uint32_t __get_ahb_div(uint32_t val) {
  switch (val) {
  case RCC_CFGR_HPRE_DIV2:
    return 2;
  case RCC_CFGR_HPRE_DIV4:
    return 4;
  case RCC_CFGR_HPRE_DIV8:
    return 8;
  case RCC_CFGR_HPRE_DIV16:
    return 16;
  case RCC_CFGR_HPRE_DIV64:
    return 64;
  case RCC_CFGR_HPRE_DIV128:
    return 128;
  case RCC_CFGR_HPRE_DIV256:
    return 256;
  case RCC_CFGR_HPRE_DIV512:
    return 512;
  case RCC_CFGR_HPRE_DIV1:
  default:
    return 1;
  }
}

static uint32_t __get_apb_div(uint32_t val) {
  switch (val) {
  case RCC_CFGR_PPRE1_DIV2:
    return 2;
  case RCC_CFGR_PPRE1_DIV4:
    return 4;
  case RCC_CFGR_PPRE1_DIV8:
    return 8;
  case RCC_CFGR_PPRE1_DIV16:
    return 16;
  case RCC_CFGR_PPRE1_DIV1:
  default:
    return 1;
  }
}

int stm32f769_clock_control_get_rate(uint16_t id, uint32_t *rate)
{
  uint32_t clk_offt = STM32F769_CLOCK_ID_OFFSET(id);
  uint32_t ahb_div = __get_ahb_div(RCC->CFGR & RCC_CFGR_HPRE_Msk);
  uint32_t apb1_div = __get_apb_div(RCC->CFGR & RCC_CFGR_PPRE1_Msk);
  uint32_t apb2_div = __get_apb_div(RCC->CFGR & RCC_CFGR_PPRE2_Msk);
  uint32_t ahb_rate = CONFIG_BOARD_SYS_CLK / ahb_div;

  switch (clk_offt) {
  case STM32F769_RCC_SYS_SET_OFFSET:
    *rate = CONFIG_BOARD_SYS_CLK;
    break;
  case STM32F769_RCC_AHB1_SET_OFFSET:
    *rate = ahb_rate;
    break;
  case STM32F769_RCC_AHB2_SET_OFFSET:
    *rate = ahb_rate;
    break;
  case STM32F769_RCC_AHB3_SET_OFFSET:
    *rate = ahb_rate;
    break;
  case STM32F769_RCC_APB1_SET_OFFSET:
    *rate = ahb_rate / apb1_div;
    break;
  case STM32F769_RCC_APB2_SET_OFFSET:
    *rate = ahb_rate / apb2_div;
    break;
  default:
    return -ENOTSUP;
  }
  return 0;
}

#define __HAL_RCC_GET_SYSCLK_SOURCE() (RCC->CFGR & RCC_CFGR_SWS)
#define RCC_FLAG_MASK  ((uint8_t)0x1F)
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5) == 1)? RCC->CR :((((__FLAG__) >> 5) == 2) ? RCC->BDCR :((((__FLAG__) >> 5) == 3)? RCC->CSR :RCC->CIR))) & ((uint32_t)1 << ((__FLAG__) & RCC_FLAG_MASK)))!= 0)? 1 : 0)

/* #define __HAL_RCC_PLL_CONFIG(__RCC_PLLSource__, __PLLM__, __PLLN__, __PLLP__, __PLLQ__,__PLLR__) \ */
/*   (RCC->PLLCFGR = ((__RCC_PLLSource__) | (__PLLM__)                   | \ */
/*                    ((__PLLN__) << RCC_PLLCFGR_PLLN_Pos)                      | \ */
/*                    ((((__PLLP__) >> 1) -1) << RCC_PLLCFGR_PLLP_Pos)          | \ */
/*                    ((__PLLQ__) << RCC_PLLCFGR_PLLQ_Pos)                      | \ */
/*                    ((__PLLR__) << RCC_PLLCFGR_PLLR_Pos))) */

#define HSE_STARTUP_TIMEOUT    100U   /*!< Time out for HSE start up, in ms */
//#define PLL_TIMEOUT_VALUE 2

int stm32f769_clock_control_init(struct device *dev)
{
  uint32_t tickstart;

  if ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSE) || ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE))) {
    if ((__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) != 0)) {// && (RCC_OscInitStruct->HSEState == RCC_HSE_OFF)) {
      return -1;
    }
  } else {

#if defined(CONFIG_BOARD_HSE_CLK)
#if CONFIG_BOARD_HSE_BYP == 1
    RCC->CR |= RCC_CR_HSEBYP;
    RCC->CR |= RCC_CR_HSEON;
#else
    RCC->CR |= RCC_CR_HSEON;
#endif
    /* Get Start Tick*/
    tickstart = uptime_ctr;

    /* Wait till HSE is ready */
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == 0) {
      if ((uptime_ctr - tickstart) > HSE_STARTUP_TIMEOUT) {
        return -1;
      }
    }
#endif
  }

  /* Check if the PLL is used as system clock or not */
  if (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK) {

    /* Disable the main PLL. */
    __HAL_RCC_PLL_DISABLE();

    /* Get Start Tick*/
    tickstart = uptime_ctr;

    /* Wait till PLL is ready */
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) != 0) {
      if ((uptime_ctr - tickstart) > PLL_TIMEOUT_VALUE) {
        return -1;
      }
    }

    /* Configure the main PLL clock source, multiplication and division factors. */
    __HAL_RCC_PLL_CONFIG(RCC_PLLSOURCE_HSE, 25, 432, RCC_PLLP_DIV2, 9, 7);

    /* Enable the main PLL. */
    __HAL_RCC_PLL_ENABLE();

    /* Get Start Tick*/
    tickstart = uptime_ctr;

    /* Wait till PLL is ready */
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == 0) {
      if ((uptime_ctr - tickstart) > PLL_TIMEOUT_VALUE) {
        return -1;
      }
    }
  }
  return 0;
}

/* static enum clock_control_status */
/* clock_control_mik32_get_status(uint16_t id) */
/* { */
/*    if (sys_test_bit(PM_BASE_ADDRESS + MIK32_CLOCK_ID_OFFSET(id), */
/*         MIK32_CLOCK_ID_BIT(id)) != 0) { */
/*      return CLOCK_CONTROL_STATUS_ON; */
/*    } */

/*    return CLOCK_CONTROL_STATUS_OFF; */
/* } */
