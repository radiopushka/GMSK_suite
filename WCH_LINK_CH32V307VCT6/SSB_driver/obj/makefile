################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/GFSK/IIR/subdir.mk
-include User/GFSK/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
SSB_driver.hex \

SECONDARY_LIST += \
SSB_driver.lst \

SECONDARY_SIZE += \
SSB_driver.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: SSB_driver.elf secondary-outputs

# Tool invocations
SSB_driver.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "/home/ivan/mounriver-studio-projects/SSB_driver/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Xlinker --print-memory-usage -Wl,-Map,"SSB_driver.map" --specs=nano.specs --specs=nosys.specs -o "SSB_driver.elf" $(OBJS)  $(LIBS)
SSB_driver.hex: SSB_driver.elf
	@	riscv-none-embed-objcopy -O ihex "SSB_driver.elf" "SSB_driver.hex"
SSB_driver.lst: SSB_driver.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "SSB_driver.elf" > "SSB_driver.lst"
SSB_driver.siz: SSB_driver.elf
	riscv-none-embed-size --format=berkeley "SSB_driver.elf"

# Other Targets
clean:
	-$(RM) $(OBJS) $(EXPANDS) $(CALLGRAPH_DOT) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) SSB_driver.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets