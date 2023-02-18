################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Control_ECU_main.c \
../buzzer.c \
../control_system.c \
../dc_motor.c \
../external_eeprom.c \
../gpio.c \
../pwm.c \
../timer.c \
../twi.c \
../uart.c 

OBJS += \
./Control_ECU_main.o \
./buzzer.o \
./control_system.o \
./dc_motor.o \
./external_eeprom.o \
./gpio.o \
./pwm.o \
./timer.o \
./twi.o \
./uart.o 

C_DEPS += \
./Control_ECU_main.d \
./buzzer.d \
./control_system.d \
./dc_motor.d \
./external_eeprom.d \
./gpio.d \
./pwm.d \
./timer.d \
./twi.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega16 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


