#include <drivers/include/device.h>
#include <drivers/memory/stm32f7_mpu.h>
#include <arch/aarch32/cmsis/core_cm7.h>

struct device mpu;

static void __cpu_cache_enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#define __FLASH_ART_ENABLE()  SET_BIT(FLASH->ACR, FLASH_ACR_ARTEN)
#define __FLASH_PREFETCH_BUFFER_ENABLE()  (FLASH->ACR |= FLASH_ACR_PRFTEN)

static void __nvic_setpriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{ 
  uint32_t prioritygroup = 0x00;
    
  prioritygroup = NVIC_GetPriorityGrouping();
  
  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

static int __init_tick (uint32_t TickPriority)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
  uint32_t              uwPrescalerValue = 0U;
  uint32_t              pFLatency;
  
    /*Configure the TIM6 IRQ priority */
  __nvic_setpriority(TIM6_DAC_IRQn, TickPriority ,0U);
  
  /* Enable the TIM6 global Interrupt */
  NVIC_EnableIRQ(TIM6_DAC_IRQn);
  
  /* Enable TIM6 clock */
  __HAL_RCC_TIM6_CLK_ENABLE();
  
  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  
  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;
  
  /* Compute TIM6 clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1) 
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
  }
  
  /* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);
  
  /* Initialize TIM6 */
  TimHandle.Instance = TIM6;
  
  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  TimHandle.Init.Period = (1000000U / 1000U) - 1U;
  TimHandle.Init.Prescaler = uwPrescalerValue;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&TimHandle) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    return HAL_TIM_Base_Start_IT(&TimHandle);
  }
  
  /* Return function status */
  return HAL_ERROR;
}

void arch_init(void) {
  stm32f7_mpu_init(&mpu);

  __cpu_cache_enable();

  /* Configure Instruction cache through ART accelerator */ 
#if (CONFIG_CM7_ART_ACCELERATOR_ENABLE != 0)
  __FLASH_ART_ENABLE();
#endif /* ART_ACCELERATOR_ENABLE */

  /* Configure Flash prefetch */
#if (CONFIG_CM7_PREFETCH_ENABLE != 0U)
  __FLASH_PREFETCH_BUFFER_ENABLE();
#endif /* PREFETCH_ENABLE */

  /* Set Interrupt Group Priority */
  //HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  HAL_InitTick(TICK_INT_PRIORITY);

  /* Init the low level hardware */
  HAL_MspInit();

  /* Return function status */
  return HAL_OK;
}
