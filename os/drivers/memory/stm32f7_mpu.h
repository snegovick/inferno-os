#ifndef __STM32F7_MPU_H__
#define __STM32F7_MPU_H__

#include <drivers/include/device.h>

int stm32f7_mpu_init(struct device *dev);
int stm32f7_mpu_deinit(struct device *dev);

#endif/*__STM32F7_MPU_H__*/
