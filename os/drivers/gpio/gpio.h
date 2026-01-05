#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

#define GPIO_INPUT              (1U << 16)
#define GPIO_OUTPUT             (1U << 17)
#define GPIO_DISCONNECTED	0

#define GPIO_OUTPUT_INIT_LOW    (1U << 18)
#define GPIO_OUTPUT_INIT_HIGH   (1U << 19)
#define GPIO_OUTPUT_INIT_LOGICAL (1U << 20)

#define GPIO_OUTPUT_LOW         (GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW)
#define GPIO_OUTPUT_HIGH        (GPIO_OUTPUT | GPIO_OUTPUT_INIT_HIGH)
#define GPIO_OUTPUT_INACTIVE    (GPIO_OUTPUT |			\
				 GPIO_OUTPUT_INIT_LOW |		\
				 GPIO_OUTPUT_INIT_LOGICAL)
#define GPIO_OUTPUT_ACTIVE      (GPIO_OUTPUT |			\
				 GPIO_OUTPUT_INIT_HIGH |	\
				 GPIO_OUTPUT_INIT_LOGICAL)

#define GPIO_INT_DISABLE               (1U << 21)
#define GPIO_INT_ENABLE                (1U << 22)
#define GPIO_INT_LEVELS_LOGICAL        (1U << 23)
#define GPIO_INT_EDGE                  (1U << 24)
#define GPIO_INT_LOW_0                 (1U << 25)
#define GPIO_INT_HIGH_1                (1U << 26)

#define GPIO_INT_MASK                  (GPIO_INT_DISABLE | \
					GPIO_INT_ENABLE | \
					GPIO_INT_LEVELS_LOGICAL | \
					GPIO_INT_EDGE | \
					GPIO_INT_LOW_0 | \
					GPIO_INT_HIGH_1)
#define GPIO_INT_EDGE_RISING           (GPIO_INT_ENABLE | \
					GPIO_INT_EDGE | \
					GPIO_INT_HIGH_1)
#define GPIO_INT_EDGE_FALLING          (GPIO_INT_ENABLE | \
					GPIO_INT_EDGE | \
					GPIO_INT_LOW_0)
#define GPIO_INT_EDGE_BOTH             (GPIO_INT_ENABLE | \
					GPIO_INT_EDGE | \
					GPIO_INT_LOW_0 | \
					GPIO_INT_HIGH_1)
#define GPIO_INT_LEVEL_LOW             (GPIO_INT_ENABLE | \
					GPIO_INT_LOW_0)
#define GPIO_INT_LEVEL_HIGH            (GPIO_INT_ENABLE | \
					GPIO_INT_HIGH_1)
#define GPIO_INT_EDGE_TO_INACTIVE      (GPIO_INT_ENABLE | \
					GPIO_INT_LEVELS_LOGICAL | \
					GPIO_INT_EDGE | \
					GPIO_INT_LOW_0)
#define GPIO_INT_EDGE_TO_ACTIVE        (GPIO_INT_ENABLE | \
					GPIO_INT_LEVELS_LOGICAL | \
					GPIO_INT_EDGE | \
					GPIO_INT_HIGH_1)
#define GPIO_INT_LEVEL_INACTIVE        (GPIO_INT_ENABLE | \
					GPIO_INT_LEVELS_LOGICAL | \
					GPIO_INT_LOW_0)
#define GPIO_INT_LEVEL_ACTIVE          (GPIO_INT_ENABLE | \
					GPIO_INT_LEVELS_LOGICAL | \
					GPIO_INT_HIGH_1)
#define GPIO_DIR_MASK		(GPIO_INPUT | GPIO_OUTPUT)

#define GPIO_OPEN_DRAIN         (1 << 27)
#define GPIO_OPEN_SOURCE        (1 << 28)
#define GPIO_PULL_UP            (1 << 29)
#define GPIO_PULL_DOWN          (1 << 30)
#define GPIO_INT_WAKEUP         (1 << 31)

#endif/*__GPIO_H__*/
