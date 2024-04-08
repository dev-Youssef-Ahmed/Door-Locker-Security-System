################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/application.c \
../src/buzzer.c \
../src/external_eeprom.c \
../src/gpio.c \
../src/int2.c \
../src/main.c \
../src/motor.c \
../src/pwm.c \
../src/timer1.c \
../src/twi.c \
../src/uart.c 

OBJS += \
./src/application.o \
./src/buzzer.o \
./src/external_eeprom.o \
./src/gpio.o \
./src/int2.o \
./src/main.o \
./src/motor.o \
./src/pwm.o \
./src/timer1.o \
./src/twi.o \
./src/uart.o 

C_DEPS += \
./src/application.d \
./src/buzzer.d \
./src/external_eeprom.d \
./src/gpio.d \
./src/int2.d \
./src/main.d \
./src/motor.d \
./src/pwm.d \
./src/timer1.d \
./src/twi.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\Users\Youssef\Desktop\Youssef_Ahmed_85_Final_Project\Youssef_Ahmed_85_Final_Project_CONTROL_ECU\inc" -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


