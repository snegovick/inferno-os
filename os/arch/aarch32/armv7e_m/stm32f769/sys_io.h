#ifndef __SYS_IO_H__
#define __SYS_IO_H__

#include <stdint.h>

#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#ifndef BIT
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#endif

static inline uint8_t sys_read8(uint32_t addr)
{
	return *(volatile uint8_t *)addr;
}

static inline void sys_write8(uint8_t data, uint32_t addr)
{
	*(volatile uint8_t *)addr = data;
}

static inline uint16_t sys_read16(uint32_t addr)
{
	return *(volatile uint16_t *)addr;
}

static inline void sys_write16(uint16_t data, uint32_t addr)
{
	*(volatile uint16_t *)addr = data;
}

static inline uint32_t sys_read32(uint32_t addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void sys_write32(uint32_t data, uint32_t addr)
{
	*(volatile uint32_t *)addr = data;
}

static inline uint64_t sys_read64(uint32_t addr)
{
	return *(volatile uint64_t *)addr;
}

static inline void sys_write64(uint64_t data, uint32_t addr)
{
	*(volatile uint64_t *)addr = data;
}

static inline void sys_set_bit(uint32_t addr, unsigned int bit)
{
	uint32_t temp = *(volatile uint32_t *)addr;

	*(volatile uint32_t *)addr = temp | (1 << bit);
}

static inline void sys_clear_bit(uint32_t addr, unsigned int bit)
{
	uint32_t temp = *(volatile uint32_t *)addr;

	*(volatile uint32_t *)addr = temp & ~(1 << bit);
}

static inline int sys_test_bit(uint32_t addr, unsigned int bit)
{
	uint32_t temp = *(volatile uint32_t *)addr;

	return temp & (1 << bit);
}

static inline void sys_set_bits(uint32_t addr, unsigned int mask)
{
	uint32_t temp = *(volatile uint32_t *)addr;

	*(volatile uint32_t *)addr = temp | mask;
}

static inline void sys_clear_bits(uint32_t addr, unsigned int mask)
{
	uint32_t temp = *(volatile uint32_t *)addr;

	*(volatile uint32_t *)addr = temp & ~mask;
}

#endif/*__SYS_IO_H__*/
