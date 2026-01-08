#include <arch/sys_io.h>
#include <arch/aarch32/armv7e_m/stm32f7/stm32f769xx.h>
#include <stdint.h>

#include "stm32f769_clock_control.h"
#include "stm32f769_rcc.h"

#include "errno.h"
#include "stm32f769_clocks.h"

#define TICK_INT_PRIORITY 0x0FU

#define STM32F769_CLOCK_ID_OFFSET(id) (((id) >> 6U) & 0xFFU)
#define STM32F769_CLOCK_ID_BIT(id)   (id & 0x1FU)

#define HSI_VALUE 16000000

enum CLK_SOURCE {
  CS_UNKNOWN = 0,
  CS_HSI,
  CS_HSE,
  CS_PLL_HSI,
  CS_PLL_HSE,
};

struct rcc_clk_init {
  uint32_t ClockType;
  uint32_t SYSCLKSource;
  uint32_t AHBCLKDivider;
  uint32_t APB1CLKDivider;
  uint32_t APB2CLKDivider;
};

volatile uint64_t uptime_ctr;

void SysTick_Handler(void) {
  uptime_ctr ++;
}

int stm32f769_clock_control_on(uint16_t id) {
  uint32_t addr = RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id);
  uint32_t bit = STM32F769_CLOCK_ID_BIT(id);
  sys_set_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
              STM32F769_CLOCK_ID_BIT(id));
  volatile int ret = sys_test_bit(RCC_BASE + STM32F769_CLOCK_ID_OFFSET(id),
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

static uint32_t __get_apb1_div(uint32_t val) {
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

static uint32_t __get_apb2_div(uint32_t val) {
  switch (val) {
  case RCC_CFGR_PPRE2_DIV2:
    return 2;
  case RCC_CFGR_PPRE2_DIV4:
    return 4;
  case RCC_CFGR_PPRE2_DIV8:
    return 8;
  case RCC_CFGR_PPRE2_DIV16:
    return 16;
  case RCC_CFGR_PPRE2_DIV1:
  default:
    return 1;
  }
}

uint32_t __get_sysclk(void)
{
  uint64_t plln = 0;
  uint32_t pllm = 0, pllvco = 0, pllp = 0;
  uint32_t clk = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (RCC->CFGR & RCC_CFGR_SWS)
  {
    case RCC_SYSCLKSOURCE_STATUS_HSI:  /* HSI used as system clock source */
    {
      clk = HSI_VALUE;
      break;
    }
    case RCC_SYSCLKSOURCE_STATUS_HSE:  /* HSE used as system clock  source */
    {
      clk = CONFIG_BOARD_HSE_CLK;
      break;
    }
    case RCC_SYSCLKSOURCE_STATUS_PLLCLK:  /* PLL used as system clock  source */
    {
      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
      SYSCLK = PLL_VCO / PLLP */
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
      plln = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);
      if (__HAL_RCC_GET_PLL_OSCSOURCE() != RCC_PLLCFGR_PLLSRC_HSI)
      {
        /* HSE used as PLL clock source */
        pllvco = (uint32_t)((((uint64_t) CONFIG_BOARD_HSE_CLK * ((uint64_t)plln))) / (uint64_t)pllm);
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = (uint32_t)((((uint64_t) HSI_VALUE * ((uint64_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos)))) / (uint64_t)pllm);
      }
      pllp = ((((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos) + 1) * 2);

      clk = pllvco / pllp;
      break;
    }
    default:
    {
      clk = HSI_VALUE;
      break;
    }
  }
  return clk;
}

#define __HAL_PWR_OVERDRIVE_ENABLE() (PWR->CR1 |= (uint32_t)PWR_CR1_ODEN)
#define __HAL_PWR_OVERDRIVE_DISABLE() (PWR->CR1 &= (uint32_t)(~PWR_CR1_ODEN))
#define __HAL_PWR_GET_FLAG(__FLAG__) ((PWR->CSR1 & (__FLAG__)) == (__FLAG__))
#define PWR_OVERDRIVE_TIMEOUT_VALUE  1000
#define PWR_UDERDRIVE_TIMEOUT_VALUE  1000
#define __HAL_PWR_OVERDRIVESWITCHING_ENABLE() (PWR->CR1 |= (uint32_t)PWR_CR1_ODSWEN)
#define __HAL_PWR_OVERDRIVESWITCHING_DISABLE() (PWR->CR1 &= (uint32_t)(~PWR_CR1_ODSWEN))
#define PWR_FLAG_ODRDY                  PWR_CSR1_ODRDY
#define PWR_FLAG_ODSWRDY                PWR_CSR1_ODSWRDY
#define PWR_FLAG_UDRDY                  PWR_CSR1_UDRDY

static int __enable_overdrive(void)
{
  uint32_t tickstart = 0;

  stm32f769_clock_control_on(STM32F769_CLOCK_PWR);

  __HAL_PWR_OVERDRIVE_ENABLE();
  tickstart = uptime_ctr;

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_ODRDY))
  {
    if((uptime_ctr - tickstart ) > PWR_OVERDRIVE_TIMEOUT_VALUE)
    {
      return -1;
    }
  }

  __HAL_PWR_OVERDRIVESWITCHING_ENABLE();
  tickstart = uptime_ctr;

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_ODSWRDY))
  {
    if((uptime_ctr - tickstart ) > PWR_OVERDRIVE_TIMEOUT_VALUE)
    {
      return -1;
    }
  }
  return 0;
}

/* HAL_StatusTypeDef __disable_overdrive(void) */
/* { */
/*   uint32_t tickstart = 0; */

/*   __HAL_RCC_PWR_CLK_ENABLE(); */

/*   /\* Disable the Over-drive switch *\/ */
/*   __HAL_PWR_OVERDRIVESWITCHING_DISABLE(); */

/*   /\* Get tick *\/ */
/*   tickstart = HAL_GetTick(); */

/*   while(__HAL_PWR_GET_FLAG(PWR_FLAG_ODSWRDY)) */
/*   { */
/*     if((HAL_GetTick() - tickstart ) > PWR_OVERDRIVE_TIMEOUT_VALUE) */
/*     { */
/*       return HAL_TIMEOUT; */
/*     } */
/*   }  */

/*   /\* Disable the Over-drive *\/ */
/*   __HAL_PWR_OVERDRIVE_DISABLE(); */

/*   /\* Get tick *\/ */
/*   tickstart = HAL_GetTick(); */

/*   while(__HAL_PWR_GET_FLAG(PWR_FLAG_ODRDY)) */
/*   { */
/*     if((HAL_GetTick() - tickstart ) > PWR_OVERDRIVE_TIMEOUT_VALUE) */
/*     { */
/*       return HAL_TIMEOUT; */
/*     } */
/*   } */

/*   return HAL_OK; */
/* } */

int stm32f769_clock_control_get_rate(uint16_t id, uint32_t *rate)
{
  uint32_t clk_offt = STM32F769_CLOCK_ID_OFFSET(id);
  uint32_t ahb_div = __get_ahb_div(RCC->CFGR & RCC_CFGR_HPRE_Msk);
  uint32_t apb1_div = __get_apb1_div(RCC->CFGR & RCC_CFGR_PPRE1_Msk);
  uint32_t apb2_div = __get_apb2_div(RCC->CFGR & RCC_CFGR_PPRE2_Msk);
  uint32_t ahb_rate = __get_sysclk() / ahb_div;

  switch (clk_offt) {
  case STM32F769_RCC_SYS_SET_OFFSET:
    *rate = __get_sysclk();
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
#define __HAL_FLASH_GET_LATENCY()     (READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY))
#define __HAL_FLASH_SET_LATENCY(__LATENCY__) \
                  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, (uint32_t)(__LATENCY__))

static int __deinit_clk(void)
{
  uint32_t tickstart;

  tickstart = uptime_ctr;

  /* Set HSION bit to the reset value */
  SET_BIT(RCC->CR, RCC_CR_HSION);

  /* Wait till HSI is ready */
  while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0)
  {
    if ((uptime_ctr - tickstart) > HSI_TIMEOUT_VALUE)
    {
      return -ETIMEDOUT;
    }
  }

  /* Set HSITRIM[4:0] bits to the reset value */
  SET_BIT(RCC->CR, RCC_CR_HSITRIM_4);

  /* Get Start Tick */
  tickstart = uptime_ctr;

  /* Reset CFGR register */
  CLEAR_REG(RCC->CFGR);

  /* Wait till clock switch is ready */
  while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != 0)
  {
    if ((uptime_ctr - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
    {
      return -ETIMEDOUT;
    }
  }

  /* Get Start Tick */
  tickstart = uptime_ctr;

  /* Clear HSEON, HSEBYP and CSSON bits */
  CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON);

  /* Wait till HSE is disabled */
  while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != 0)
  {
    if ((uptime_ctr - tickstart) > HSE_TIMEOUT_VALUE)
    {
      return -ETIMEDOUT;
    }
  }

  /* Get Start Tick */
  tickstart = uptime_ctr;

  /* Clear PLLON bit */
  CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

  /* Wait till PLL is disabled */
  while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != 0)
  {
    if ((uptime_ctr - tickstart) > PLL_TIMEOUT_VALUE)
    {
      return -ETIMEDOUT;
    }
  }

  /* Get Start Tick */
  tickstart = uptime_ctr;

  /* Reset PLLI2SON bit */
  CLEAR_BIT(RCC->CR, RCC_CR_PLLI2SON);

  /* Wait till PLLI2S is disabled */
  while (READ_BIT(RCC->CR, RCC_CR_PLLI2SRDY) != 0)
  {
    if ((uptime_ctr - tickstart) > PLLI2S_TIMEOUT_VALUE)
    {
      return -ETIMEDOUT;
    }
  }

  /* Get Start Tick */
  tickstart = uptime_ctr;

  /* Reset PLLSAI bit */
  CLEAR_BIT(RCC->CR, RCC_CR_PLLSAION);

  /* Wait till PLLSAI is disabled */
  while (READ_BIT(RCC->CR, RCC_CR_PLLSAIRDY) != 0)
  {
    if ((uptime_ctr - tickstart) > PLLSAI_TIMEOUT_VALUE)
    {
      return -ETIMEDOUT;
    }
  }

  /* Once PLL, PLLI2S and PLLSAI are OFF, reset PLLCFGR register to default value */
  RCC->PLLCFGR = RCC_PLLCFGR_PLLM_4 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLQ_2 | 0x20000000U;

  /* Reset PLLI2SCFGR register to default value */
  RCC->PLLI2SCFGR = RCC_PLLI2SCFGR_PLLI2SN_6 | RCC_PLLI2SCFGR_PLLI2SN_7 | RCC_PLLI2SCFGR_PLLI2SQ_2 | RCC_PLLI2SCFGR_PLLI2SR_1;

  /* Reset PLLSAICFGR register to default value */
  RCC->PLLSAICFGR = RCC_PLLSAICFGR_PLLSAIN_6 | RCC_PLLSAICFGR_PLLSAIN_7 | RCC_PLLSAICFGR_PLLSAIQ_2 | 0x20000000U;

  /* Disable all interrupts */
  CLEAR_BIT(RCC->CIR, RCC_CIR_LSIRDYIE | RCC_CIR_LSERDYIE | RCC_CIR_HSIRDYIE | RCC_CIR_HSERDYIE | RCC_CIR_PLLRDYIE | RCC_CIR_PLLI2SRDYIE | RCC_CIR_PLLSAIRDYIE);

  /* Clear all interrupt flags */
  SET_BIT(RCC->CIR, RCC_CIR_LSIRDYC | RCC_CIR_LSERDYC | RCC_CIR_HSIRDYC | RCC_CIR_HSERDYC | RCC_CIR_PLLRDYC | RCC_CIR_PLLI2SRDYC | RCC_CIR_PLLSAIRDYC | RCC_CIR_CSSC);

  /* Clear LSION bit */
  CLEAR_BIT(RCC->CSR, RCC_CSR_LSION);

  /* Reset all CSR flags */
  SET_BIT(RCC->CSR, RCC_CSR_RMVF);

  /* Update the SystemCoreClock global variable */
  SystemCoreClock = HSI_VALUE;

  /* Adapt Systick interrupt period */
  return stm32f769_init_tick();
}

int stm32f769_clock_control_init(struct device *dev)
{
  uint32_t tickstart;
  enum CLK_SOURCE cs = CS_UNKNOWN;
  int ret = 0;

  uint32_t clk_source = __HAL_RCC_GET_SYSCLK_SOURCE();
  uint32_t pllcfgr = READ_REG(RCC->PLLCFGR);
  uint32_t rcccr = READ_REG(RCC->CR);

  switch (clk_source) {
  case RCC_SYSCLKSOURCE_STATUS_HSE:
    cs = CS_HSE;
    break;
  case RCC_SYSCLKSOURCE_STATUS_HSI:
    cs = CS_HSI;
    break;
  case RCC_SYSCLKSOURCE_STATUS_PLLCLK:
    if ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE) {
      cs = CS_PLL_HSE;
    } else if ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSI) {
      cs = CS_PLL_HSI;
    } else {
      while (1) {};
    }
    break;
  default:
    while (1) {};
    break;
  }

  ret = __deinit_clk();
  if (ret != 0) {
    while (1) {};
  }

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

  if (__enable_overdrive() != 0) {
    while (1) {};
  }

  if (FLASH_ACR_LATENCY_7WS > __HAL_FLASH_GET_LATENCY())
  {
    /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLASH_ACR_LATENCY_7WS);

    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if (__HAL_FLASH_GET_LATENCY() != FLASH_ACR_LATENCY_7WS)
    {
      while (1) {};
      return -1;
    }
  }

  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_HCLK_DIV16);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, (RCC_HCLK_DIV16 << 3));
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_SYSCLK_DIV1);

  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == 0)
  {
    while (1) {};
    return -1;
  }

  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

  /* Get Start Tick*/
  tickstart = uptime_ctr;

  while (__HAL_RCC_GET_SYSCLK_SOURCE() != (RCC_SYSCLKSOURCE_PLLCLK << RCC_CFGR_SWS_Pos))
  {
    if ((uptime_ctr - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
    {
      while (1) {};
      return -1;
    }
  }

  /* Decreasing the number of wait states because of lower CPU frequency */
  if (FLASH_ACR_LATENCY_7WS < __HAL_FLASH_GET_LATENCY())
  {
    /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLASH_ACR_LATENCY_7WS);

    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if (__HAL_FLASH_GET_LATENCY() != FLASH_ACR_LATENCY_7WS)
    {
      return -1;
    }
  }

  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_HCLK_DIV4);

  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, ((RCC_HCLK_DIV2) << 3));

  SystemCoreClock = __get_sysclk() >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos];

  stm32f769_init_tick();
  return 0;
}


static void __rcc_get_clock_config(struct rcc_clk_init  *clkinit, uint32_t *flash_latency)
{
  /* Set all possible values for the Clock type parameter --------------------*/
  clkinit->ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

  /* Get the SYSCLK configuration --------------------------------------------*/
  clkinit->SYSCLKSource = (uint32_t)(RCC->CFGR & RCC_CFGR_SW);

  /* Get the HCLK configuration ----------------------------------------------*/
  clkinit->AHBCLKDivider = (uint32_t)(RCC->CFGR & RCC_CFGR_HPRE);

  /* Get the APB1 configuration ----------------------------------------------*/
  clkinit->APB1CLKDivider = (uint32_t)(RCC->CFGR & RCC_CFGR_PPRE1);

  /* Get the APB2 configuration ----------------------------------------------*/
  clkinit->APB2CLKDivider = (uint32_t)((RCC->CFGR & RCC_CFGR_PPRE2) >> 3);

  /* Get the Flash Wait State (Latency) configuration ------------------------*/
  *flash_latency = (uint32_t)(FLASH->ACR & FLASH_ACR_LATENCY);
}

int stm32f769_init_tick(void)
{
  uint32_t TickPriority = TICK_INT_PRIORITY;
  struct rcc_clk_init clkconfig;
  uint32_t flash_latency;

  uint32_t prioritygroup = 0x00;
  prioritygroup = NVIC_GetPriorityGrouping();
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, TickPriority, 0));
  //__nvic_setpriority(SysTick_IRQn, TickPriority, 0U);

  /* Enable the TIM6 global Interrupt */
  NVIC_EnableIRQ(SysTick_IRQn);

  /* Get clock configuration */
  __rcc_get_clock_config(&clkconfig, &flash_latency);

  uint32_t ahb1_rate = 0;
  stm32f769_clock_control_get_rate(STM32F769_CLOCK_GPIOA, &ahb1_rate);
  uint32_t sys_rate = 0;
  stm32f769_clock_control_get_rate(STM32F769_CLOCK_SYS, &sys_rate);

  MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, ((ahb1_rate) / 1000) - 1);
  CLEAR_BIT(SysTick->VAL, SysTick_VAL_CURRENT_Msk);
  SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);

  /* Return function status */
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
