#include <arch/aarch32/cmsis/core_cm7.h>

#include <drivers/include/device.h>
#include <drivers/memory/stm32f7_mpu.h>
#include <drivers/clock/stm32f769_clocks.h>
#include <drivers/clock/stm32f769_clock_control.h>
#include <drivers/uart/stm32f7_uart.h>

struct device mpu;
struct device clkctrl;
struct device usart1;

struct rcc_clk_init {
  uint32_t ClockType;
  uint32_t SYSCLKSource;
  uint32_t AHBCLKDivider;
  uint32_t APB1CLKDivider;
  uint32_t APB2CLKDivider;
};

extern volatile uint64_t uptime_ctr;

void wait(uint32_t c) {
  uint64_t end = uptime_ctr + c;
  while (uptime_ctr < end) {
  }
}

static void __cpu_cache_enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#define __FLASH_ART_ENABLE() SET_BIT(FLASH->ACR, FLASH_ACR_ARTEN)
#define __FLASH_PREFETCH_BUFFER_ENABLE() (FLASH->ACR |= FLASH_ACR_PRFTEN)
#define TICK_INT_PRIORITY 0x0FU

static void __nvic_setpriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t prioritygroup = 0x00;
  prioritygroup = NVIC_GetPriorityGrouping();
  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
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


static int __init_tick (uint32_t TickPriority)
{
  struct rcc_clk_init clkconfig;
  uint32_t uwTimclock, uwAPB1Prescaler = 0U;
  uint32_t pFLatency;

  /*Configure the TIM6 IRQ priority */
  __nvic_setpriority(SysTick_IRQn, TickPriority, 0U);

  /* Enable the TIM6 global Interrupt */
  NVIC_EnableIRQ(SysTick_IRQn);

  /* Get clock configuration */
  __rcc_get_clock_config(&clkconfig, &flash_latency);

  uint32_t sys_rate = 0;
  stm32f7_clock_control_get_rate(STM32F769_RCC_SYS_SET_OFFSET, &sys_rate);

  sys_write32(((sys_rate / 1000) - 1) & SysTick_LOAD_RELOAD_Msk, SysTick->LOAD);
  sys_clear_bits(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);

  /* Return function status */
  return 0;
}

void arch_init(void) {
  uptime_ctr = 0;
  stm32f7_mpu_init(&mpu);
  __cpu_cache_enable();

#if (CONFIG_CM7_ART_ACCELERATOR_ENABLE != 0)
  __FLASH_ART_ENABLE();
#endif /* ART_ACCELERATOR_ENABLE */

#if (CONFIG_CM7_PREFETCH_ENABLE != 0U)
  __FLASH_PREFETCH_BUFFER_ENABLE();
#endif /* PREFETCH_ENABLE */

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  __init_tick(TICK_INT_PRIORITY);

  stm32f769_clock_control_init(&clkctrl);

  stm32f769_clock_control_on(STM32F769_CLOCK_USART1);

  struct uart_data u1_data;
  u1_data.dev = &usart1;
  u1_data.baudrate = 115200;
  u1_data.parity = UART_CFG_PARITY_NONE;
  u1_data.stop_bits = UART_CFG_STOP_BITS_1;
  u1_data.data_bits = UART_CFG_DATA_BITS_8;

  usart1.devptr = USART1;
  usart1.data = &u1_data;

  stm32f7_uart_init(&usart1);
  stm32f7_uart_tx(&usart1, "Start\r\n");

  while (1) {
    stm32f7_uart_tx(&usart1, "Test\r\n");
    wait(1000);
  }
  return;
}
