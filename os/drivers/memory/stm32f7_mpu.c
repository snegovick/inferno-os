#include <stdint.h>
#include "stm32f7_mpu.h"
#include <arch/aarch32/armv7e_m/stm32f7/stm32f769xx.h>
#include <arch/sys_io.h>
#include <arch/aarch32/armv7e_m/stm32f769/cortex.h>
#include <drivers/include/device.h>
#include <arch/aarch32/armv7e_m/cmsis/core_cm7.h>

static void __mpu_disable(void)
{
  /* Make sure outstanding transfers are done */
  __DMB();

  /* Disable fault exceptions */
  SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
  
  /* Disable the MPU and clear the control register*/
  MPU->CTRL = 0;
}

static void __mpu_enable(uint32_t ctrl)
{
  /* Enable the MPU */
  MPU->CTRL = ctrl | MPU_CTRL_ENABLE_Msk;
  
  /* Enable fault exceptions */
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
  
  /* Ensure MPU setting take effects */
  __DSB();
  __ISB();
}

int stm32f7_mpu_init(struct device *dev) {
  dev->devptr = (void *)MPU;

  __mpu_disable();
  MPU->RNR = MPU_REGION_NUMBER0;

  /* Disable the Region */
  MODIFY_REG(MPU->RASR, MPU_RASR_ENABLE_Msk, 0);

  /* Apply configuration */
  MPU->RBAR = 0; // Base addr

  uint32_t rasr = MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RASR_XN_Pos;
  rasr |= MPU_REGION_SIZE_4GB << MPU_RASR_SIZE_Pos;
  rasr |= MPU_REGION_NO_ACCESS << MPU_RASR_AP_Pos;
  rasr |= MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos;
  rasr |= MPU_ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos;
  rasr |= MPU_ACCESS_SHAREABLE << MPU_RASR_S_Pos;
  rasr |= MPU_TEX_LEVEL0 << MPU_RASR_TEX_Pos;
  rasr |= 0x87 << MPU_RASR_SRD_Pos; // Sub region disable
  rasr |= MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos;
  
  MPU->RASR = rasr;

  __mpu_enable(MPU_PRIVILEGED_DEFAULT);

  return 0;
}

int stm32f7_mpu_deinit(struct device *dev) {
  return 0;
}
