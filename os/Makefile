DEBUG = 1
REVISION = $(shell git rev-list HEAD --count)
DIRTY = $(shell git diff-index --quiet HEAD; echo $$?)
ifeq ($(DIRTY),1)
	SUFFIX = "-rc"
else
	SUFFIX = ""
endif

PROJECT = test
TARGET = $(PROJECT).elf
CROSS_COMPILE=~/zephyr-sdk-0.17.1/arm-zephyr-eabi/bin/arm-zephyr-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

SOURCES = main.c
SOURCES+= arch/aarch32/armv7e_m/stm32f769/init.c
SOURCES+= arch/aarch32/armv7e_m/stm32f7/system_stm32f7xx.c
SOURCES+= drivers/clock/stm32f769_clock_control.c
SOURCES+= drivers/pinctrl/stm32f769_pinctrl.c
SOURCES+= drivers/gpio/stm32f769_gpio.c
SOURCES+= drivers/memory/stm32f7_mpu.c
SOURCES+= drivers/uart/stm32f7_uart.c

LDFLAGS =  -nostartfiles -nostdlib -nodefaultlibs -lgcc
LDFLAGS+= -Wl,-Map=$(PROJECT).map,--gc-sections
LDFLAGS+= -Tarch/aarch32/armv7e_m/stm32f769/link.ld -mfloat-abi=hard -mfpu=fpv5-sp-d16 -march=armv8-m.main+dsp
#LDFLAGS += -flto
#LDFLAGS += -Tld/spifi.ld -march=rv32imc_zicsr_zifencei

CFLAGS = -Wall -Wextra -std=gnu99 -funsigned-char -funsigned-bitfields
CFLAGS+= -mfloat-abi=hard -mfpu=fpv5-sp-d16 -march=armv8-m.main+dsp
ifneq ($(DEBUG),1)
#CFLAGS += -flto -O2
else
#CFLAGS += -flto -O2
CFLAGS+= -ggdb3
endif
CFLAGS+= -fshort-enums -ffunction-sections -fdata-sections -fno-delete-null-pointer-checks
CFLAGS+= -fno-builtin
CFLAGS+= -I./

# CFLAGS += -DEN_I2C1
# CFLAGS += -DEN_I2C0
# CFLAGS += -DEN_USART1
# CFLAGS += -DEN_EEPROM
#CFLAGS += -DIRQ_TRACE
#CFLAGS += -DEN_TSENS
#CFLAGS += -DCOMPACT

ASSOURCES = arch/aarch32/armv7e_m/stm32f7/startup_stm32f769xx.s

ASFLAGS = $(COMMON)
ASFLAGS += $(CFLAGS)
ASFLAGS += -x assembler-with-cpp

OBJECTS = $(SOURCES:.c=.o)
ASOBJECTS = $(ASSOURCES:.s=.o)

DEPS=$(patsubst %.o, %.o.d, $(notdir $(OBJECTS)))
DEPS+=$(patsubst %.o, %.o.d, $(notdir $(ASOBJECTS)))

all: $(TARGET) $(PROJECT).bin $(PROJECT).lss size

$(PROJECT).bin: $(TARGET)
	$(OBJCOPY) -O binary  $< $@
	cp $(PROJECT).bin $(PROJECT)-$(REVISION)-$(BOARD)$(SUFFIX).bin

$(PROJECT).lss: $(TARGET)
	$(OBJDUMP) -h -S $< > $@

$(TARGET): $(OBJECTS) $(ASOBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(ASOBJECTS) -o $@

$(OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ASOBJECTS): %.o: %.s
	$(CC) $(ASFLAGS) -c $< -o $@

.PHONY: size
size: ${TARGET}
	@echo
	@$(SIZE) ${TARGET}

## Clean target
.PHONY: clean
clean:
	rm $(OBJECTS) $(ASOBJECTS) $(PROJECT).bin $(PROJECT).elf $(PROJECT).map $(PROJECT).lss $(DEPS) *.bin *.elf *.map *.lss
