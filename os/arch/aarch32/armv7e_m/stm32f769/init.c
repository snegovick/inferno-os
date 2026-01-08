#include <arch/aarch32/armv7e_m/stm32f7/stm32f769xx.h>
#include <arch/sys_io.h>

#include <drivers/clock/stm32f769_rcc.h>
#include <drivers/include/device.h>
#include <drivers/memory/stm32f7_mpu.h>
#include <drivers/clock/stm32f769_clocks.h>
#include <drivers/clock/stm32f769_clock_control.h>
#include <drivers/pinctrl/stm32f769_pinctrl.h>
#include <drivers/uart/stm32f7_uart.h>
#include <drivers/uart/uart.h>
#include <drivers/gpio/gpio.h>
#include <drivers/gpio/stm32f769_gpio.h>

#include "cortex.h"

struct device mpu;
struct device clkctrl;
struct device usart1;
struct device gpioj;

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

void WWDG_IRQHandler(void)
{
  while (1) {
  }
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

#define __FLASH_ART_ENABLE() SET_BIT(FLASH->ACR, FLASH_ACR_ARTEN)
#define __FLASH_PREFETCH_BUFFER_ENABLE() SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN)

static void __nvic_setpriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t prioritygroup = 0x00;
  prioritygroup = NVIC_GetPriorityGrouping();
  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
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
  stm32f769_init_tick();

  if (stm32f769_clock_control_init(&clkctrl) < 0) {
    while (1) {}
  }

  stm32f769_clock_control_on(STM32F769_CLOCK_GPIOA);
  stm32f769_clock_control_on(STM32F769_CLOCK_USART1);

  struct uart_data u1_data;
  u1_data.dev = &usart1;
  u1_data.baudrate = 115200;
  u1_data.parity = UART_CFG_PARITY_NONE;
  u1_data.stop_bits = UART_CFG_STOP_BITS_1;
  u1_data.data_bits = UART_CFG_DATA_BITS_8;

  usart1.devptr = USART1;
  usart1.data = &u1_data;

  stm32f769_pinctrl_configure_pin(STM32F769_PINMUX_AF(STM32F769_PORTA, 9, AF7));
  stm32f769_pinctrl_configure_pin(STM32F769_PINMUX_AF(STM32F769_PORTA, 10, AF7));

  if (stm32f7_uart_init(&usart1) < 0) {
    while (1) {
    }
  }

  stm32f7_uart_tx(&usart1, "Start\r\n");

  gpioj.devptr = GPIOJ;
  stm32f769_gpio_init(&gpioj);
  stm32f769_gpio_configure(&gpioj, 13, GPIO_OUTPUT);
  int stat = 0;
  uint32_t sys_rate = 0;

  while (1) {
    if (stat) {
      stm32f769_gpio_write(&gpioj, 13, stat);
      stat = 0;
    } else {
      stm32f769_gpio_write(&gpioj, 13, stat);
      stat = 1;
    }
    stm32f7_uart_tx(&usart1, "Test\r\n");
    wait(1000);
    stm32f769_clock_control_get_rate(STM32F769_CLOCK_SYS, &sys_rate);
  }
  return;
}
