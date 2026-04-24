################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/GFSK/modulator.c 

C_DEPS += \
./User/GFSK/modulator.d 

OBJS += \
./User/GFSK/modulator.o 


EXPANDS += \
./User/GFSK/modulator.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/GFSK/%.o: ../User/GFSK/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"/home/ivan/mounriver-studio-projects/SSB_driver/Debug" -I"/home/ivan/mounriver-studio-projects/SSB_driver/Core" -I"/home/ivan/mounriver-studio-projects/SSB_driver/User" -I"/home/ivan/mounriver-studio-projects/SSB_driver/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

